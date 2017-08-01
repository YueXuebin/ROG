#include "centerserver_pch.h"
#include "gmt_executor.h"
#include "config_loader.h"
#include "game_handler.h"
#include "mail_manager.h"
#include "player_manager.h"
#include "player.h"
#include "json_helper.h"
#include "festival_job.h"
#include "gate_handler.h"
#include "gmt_event.h"
#include "gmt_http_async.h"
#include "guild_manager.h"
#include "guild_job.h"
#include "announce_manager.h"
#include "chat_manager.h"
#include "mall_manager.h"
#include "server_monitor.h"
#include "game_setting_job.h"

IMPLEMENT_SINGLETON(GmtExecutor)


// ==============================
// 构造函数
// ==============================
GmtExecutor::GmtExecutor()
{
    m_CheckCounter = 0;
    
    uint32 pid = Crown::SDGetCurrentProcessId();
    std::string SemaName = "GmtExecutor_" + Crown::ToString(pid);

    m_GmtSema = Crown::CreateNamedSemaphore(SemaName.c_str(), 0);
    CnAssert(m_GmtSema);
    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_gmtAsync = NULL;
}

// ==============================
// 析构函数
// ==============================
GmtExecutor::~GmtExecutor()
{
    Unint();
}

// ==============================
// 初始化
// ==============================
bool GmtExecutor::Init(const char* addrIP, uint32 addrPort)
{
    if(m_gmtAsync)
    {
        CnDelete m_gmtAsync;
        m_gmtAsync = 0;
    }

    m_gmtAsync = CnNew GmtHttpAsync;

    // 初始化异步执行
    m_gmtAsync->SetSemaphore(m_GmtSema);
        
    m_gmtAsync->SetExecutor(this);

    m_gmtAsync->Init(addrIP, addrPort);

    m_gmtAsync->Start();

    return true;
}

// ==============================
// 销毁
// ==============================
void GmtExecutor::Unint()
{
    if(m_gmtAsync)
    {
        CnDelete m_gmtAsync;
        m_gmtAsync = NULL;
    }

    Crown::DestoryNamedSemaphore(m_GmtSema);
    m_GmtSema = NULL;
}

// ==============================
// 更新
// ==============================
void GmtExecutor::Update()
{
    for(GmtEvent* gmtEvent = CallbackPopFront(); gmtEvent; gmtEvent = CallbackPopFront())
    {
        if(!gmtEvent)
        {
            CnAssert(false);
            continue;
        }

        CnDbg("gmt counter=%d\n", gmtEvent->m_Counter);
        ParseGmtCmd(gmtEvent->m_cmdjson);

        delete gmtEvent;
    }
}

GmtEvent* GmtExecutor::QueryPopFront()
{
    GmtEvent* pEvent = NULL;
    QueryLock();
    if(!m_queryList.empty())
    {
        pEvent = m_queryList.front();
        m_queryList.pop_front();
    }
    QueryUnLock();
    return pEvent;
}

void GmtExecutor::QueryPushBack(GmtEvent* pEvent)
{
    QueryLock();
    m_queryList.push_back(pEvent);
    QueryUnLock();

    m_GmtSema->Post();      // 通知gmt http线程处理结果
}

GmtEvent* GmtExecutor::CallbackPopFront()
{
    GmtEvent* pEvent = NULL;
    CallbackLock();
    if(!m_callbackList.empty())
    {
        pEvent = m_callbackList.front();
        m_callbackList.pop_front();
    }
    CallbackUnLock();
    return pEvent;
}

void GmtExecutor::CallbackPushBack(GmtEvent* pEvent)
{
    CallbackLock();
    m_callbackList.push_back(pEvent);
    CallbackUnLock();
}

void GmtExecutor::OnGmtAskAck(void* data)
{
   // GMT_MSG* gmtAskAck = (GMT_MSG*)data;
  //  OnGmtGameAsking(gmtAskAck);

     GMT_MSG_SET_GAME_LIST* gmtAskAck = (GMT_MSG_SET_GAME_LIST*)data;
     if(!gmtAskAck) return;
    OnGmtGameAsking(gmtAskAck);
}

void GmtExecutor::OnGmtSetAck(void *data)
{
    GMT_MSG* gmtSetAck = (GMT_MSG*)data;
    OnGmtGameSetting(gmtSetAck);
}
// ==============================
// 解析 GM 命令
// ==============================
void  GmtExecutor::ParseGmtCmd(std::string& cmdstr)
{
    Json::Reader    reader;
    Json::Value     cmdjson;
    
    std::string     cmd;

    m_CheckCounter++;

    Json::Value     ackjson;

    if(!reader.parse(cmdstr, cmdjson, false))
    {
        CnError("Parse gmt command failed:'%s'\n", cmdstr.c_str());
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "Parse gmt command failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 0;
        PushGmtCmdResultInfo(ackjson, false);
        return;
    }

    if(!cmdjson.isObject())
    {
        CnError("cmdjson is not object:'%s'\n", cmdstr.c_str());
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "gmt command net invalid";
        ackjson[HTTP_PARAM_ERRORCODE] = 0;
        PushGmtCmdResultInfo(ackjson, false);
        return;
    }

    cmd = toString(cmdjson["cmd"]);

    CnDbg("gmt cmd=%s\n", cmd.c_str());

    // 异步http响应
    if(cmd == "gamesettinginit")
    {   // 游戏参数查询
        GmtGameAsking(cmdjson);
        return;
    }

    //跨服数据
    if(cmd == "askcrossserver")
    {
        //跨服sever列表数据
       GmtAskCrossCenterIP(cmdjson);
       return; 
    }


    // 直接http响应
    bool ret = true;
    

    if(cmd == "pcu")
    {   // pcu查询
        ret = GmtPcu(cmdjson, ackjson);
    }
    else if(cmd == "marquee")
    {   // 公告
        ret = GmtMarquee(cmdjson, ackjson);
    }
    else if(cmd == "send_mail")
    {   // 邮件
        ret = GmtSendMail(cmdjson, ackjson);
    }
    else if(cmd == "close_chat")
    {   //设置取消禁言
        ret = GmtChat(cmdjson, ackjson);
    }
    else if(cmd == "kick_player")
    {   // 踢玩家下线
        ret = GmtKickPlayer(cmdjson, ackjson);
    }
    else if(cmd == "synchronous_festival")
    {   // 同步活动数据
        ret = GmtSynchronousFestivel(cmdjson, ackjson);
    }
    else if(cmd == "synchronous_mall")
    {
        //同步商城数据
        ret = GmtSynchronounMallInfo(cmdjson, ackjson);
    }
    else if(cmd == "proclamation")
    {   // 更新警示公告
        ret = GmtProclamation(cmdjson, ackjson);
    }
    else if(cmd == "gamesetting")
    {   // 游戏配置设置
        ret = GmtGameSetting(cmdjson, ackjson);
    }
    else if(cmd == "guild_bulletin")
    {   // 修改公会公告
        ret = GmtGuildBulletin(cmdjson, ackjson);
    }
    else if(cmd == "guild_office")
    {   // 修改公会职位
        ret = GmtGuildOffice(cmdjson, ackjson);
    }
    else if(cmd == "guild_info")
    {   // 公会信息
        ret = GmtGuildInfo(cmdjson, ackjson);
    }
    else if(cmd == "askannouce")
    {   // 查询循环公告
        ret = GmtAnnouceAsk(ackjson);
    }
    else if(cmd == "deleteannnouce")
    {   // 删除循环公告
        ret = GmtDeleteAnnouceAsk(cmdjson);
    }
    else if(cmd == "add_chat_block_ip")
    {   // 添加聊天屏蔽ip
        ret = GmtAddChatBlockIP(cmdjson, ackjson);
    }
    else if(cmd == "remove_chat_block_ip")
    {   // 删除聊天屏蔽ip
        ret = GmtRemoveChatBlockIP(cmdjson, ackjson);
    }
    else if(cmd == "ask_chat_block_ip")
    {   // 查询聊天屏蔽ip
        ret = GmtAskChatBlockIP(cmdjson, ackjson);
    }
    else if(cmd == "add_cross_server")
    {   // 添加跨服CenterIP
       ret = GmtAddCrossCenterIP(cmdjson, ackjson);
    }
    else if(cmd == "delete_cross_server")
    {   // 删除跨服CenterID
        ret = GmtRemoveCrossCenterIP(cmdjson, ackjson);
    }
    else if(cmd == "ask_server_state")
    {   // 查询服务器运行状态
        ret = GmtAskServerRunningState(cmdjson, ackjson);
    }
    else if(cmd == "commonsetting")
    {   // 同步设置数据
        ret = GmtSynchronousGameSetting(cmdjson, ackjson);
    }
    else if(cmd == "limitbuyclear")
    {   // 清限购表
        ret = GmtClearLimitBuyInfo(cmdjson, ackjson);
    }
    else if(cmd == "like")
    {   // FB点赞奖励请求
        ret = GmtLikeInfo(cmdjson, ackjson);
    }
    else if(cmd == "subtractitem")
    {   // 删除物品
        ret = GmtSubtractItem(cmdjson, ackjson);
    }
    else
    {   // 无效的命令
        CnWarn("unknown gmt cmd %s\n", cmd.c_str());
        ackjson["msg"] = "unknown gmt cmd";
        PushGmtCmdResultInfo(ackjson, false);
        return;
    }

    if(ret)
    {
        //PushGmtCmdResult(ackjson.toStyledString());
        PushGmtCmdResultInfo(ackjson);
    }
    else
    {
        //PushGmtCmdResult(ackjson.toStyledString(), false);
        PushGmtCmdResultInfo(ackjson, false);
    }

}

bool GmtExecutor::GmtDeleteAnnouceAsk(Json::Value& cmdjson)
{
    int32 id = toInt32(cmdjson["id"]);
    return AnnouceManager::Instance()->DeleteAnnouceId(id);
}

void GmtExecutor::PushGmtCmdResult(const std::string& ack_string, bool retOK)
{
    GmtEvent* gmtEvent = new GmtEvent;
    gmtEvent->m_HttpResult = retOK ? HTTP_OK : HTTP_INTERNAL;
    gmtEvent->m_cmdjson = ack_string;

    m_CheckCounter--;

    CnDbg("gmt result %d\n", m_CheckCounter);

    QueryPushBack(gmtEvent);
}

void GmtExecutor::PushGmtCmdResultInfo(Json::Value& ackjson, bool retOK)
{
    Json::Value     ackStatusJson;
    GmtEvent* gmtEvent = new GmtEvent;
    gmtEvent->m_HttpResult = HTTP_OK;

    if(ackjson.isMember(HTTP_PARAM_STATUS))
    {
        ackStatusJson[HTTP_PARAM_STATUS] = ackjson[HTTP_PARAM_STATUS];
        ackjson.removeMember(HTTP_PARAM_STATUS);
    }
    else
    {
        if(retOK)
        {
            ackStatusJson[HTTP_PARAM_STATUS] = "success";
        }
        else
        {
            ackStatusJson[HTTP_PARAM_STATUS] = "failed";
        }
    }
    if(ackjson.isMember(HTTP_PARAM_MSG))
    {
        ackStatusJson[HTTP_PARAM_MSG] = ackjson[HTTP_PARAM_MSG];
        ackjson.removeMember(HTTP_PARAM_MSG);
    }
    else
    {
        ackStatusJson[HTTP_PARAM_MSG] = "";
    }

    if(retOK)
    {
        ackStatusJson[HTTP_PARAM_CONTENT] = ackjson;
    }
    else
    {
        if(ackjson.isMember(HTTP_PARAM_ERRORCODE))
        {
            ackStatusJson[HTTP_PARAM_ERRORCODE] = ackjson[HTTP_PARAM_ERRORCODE];
            ackjson.removeMember(HTTP_PARAM_ERRORCODE);
            
        }
        else
        {
            ackStatusJson[HTTP_PARAM_ERRORCODE] = "";
        }
    }
    gmtEvent->m_cmdjson = ackStatusJson.toStyledString();
    m_CheckCounter--;

    CnDbg("gmt result %d\n", m_CheckCounter);

    QueryPushBack(gmtEvent);
}

bool GmtExecutor::GmtSynchronousFestivel(Json::Value& cmdjson, Json::Value& ackjson)
{
    uint32 update_time = toUint32(cmdjson["update_time"]);

    // TASK_TYPE_FESTIVAL
    if(FestivalJob::Instance()->InsertGmtSyncRecorder(GM_UPDATE_DB_FESTIVAL, update_time))
    {
        FestivalJob::Instance()->HandleGmtFestivalList();
        ackjson[HTTP_PARAM_MSG] = "successful operation";
        return true;
    }
    else
    {
        CnError("update fesitveldb failed %u\n", update_time);
        ackjson[HTTP_PARAM_MSG] = "update fesitveldb failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }
}

bool GmtExecutor::GmtSynchronounMallInfo(Json::Value& cmdjson, Json::Value& ackjson)
{
    MallManager::Instance()->RecvReloadMallDBInfo();
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtClearLimitBuyInfo(Json::Value& cmdjson, Json::Value& ackjson)
{
    std::map<uint64, Player*> playerMap = PlayerManager::Instance()->GetPlayerMap();
    int32 playerNum = playerMap.size();
    if(playerNum <= 0)
    {
        ackjson[HTTP_PARAM_MSG] = "no player";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return true;
    }

    NONE sendMsg;
    for(std::map<uint64, Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        playerIt->second->SendToGame(MSG_LIMIT_BUY_INFO_CLEAR_REQ, &sendMsg);
    }
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

// 查询PCU
bool GmtExecutor::GmtPcu(Json::Value& cmdjson, Json::Value& ackjson)
{
    uint32  playerNum = PlayerManager::Instance()->GetPlayerNum();

    ackjson["player_num"] = playerNum;
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}
// ==============================
// 发送公告信息
// ==============================
bool GmtExecutor::GmtMarquee(Json::Value& cmdjson, Json::Value& ackjson)
{
    // json合法性检查
    if(!cmdjson.isMember("chat")
        || !cmdjson.isMember("refershtime")
        || !cmdjson.isMember("check")
        || !cmdjson.isMember("starttime")
        || !cmdjson.isMember("endtime")
        )
    {
        CnWarn("GmtMarquee json error\n");
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "GmtMarquee json error";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }

    // 参数不为null
    if(cmdjson["chat"].isNull()
        || cmdjson["refershtime"].isNull()
        || cmdjson["check"].isNull()
        || cmdjson["starttime"].isNull()
        || cmdjson["endtime"].isNull()
        )
    {
        CnWarn("GmtMarquee json has null\n");
        CnAssert(false);
        ackjson["error"] = "GmtMarquee json has null";
        ackjson[HTTP_PARAM_ERRORCODE] = 3;
        return false;
    }

	CHAT_MSG sendMsg;

	sendMsg.channel = CHAT_CHANNEL_MARQUEE;
	sendMsg.chat = toString(cmdjson["chat"]);

	if(sendMsg.chat == "")
	{
		ackjson["error"] = "chat length=0";
        ackjson[HTTP_PARAM_ERRORCODE] = 4;
		return false;
	}

	if(sendMsg.chat.length() > CHAT_MAX_LENGTH)
	{
		ackjson["error"] = "chat too long";
        ackjson[HTTP_PARAM_ERRORCODE] = 4;
		return false;
	}

	CnInfo("GMT: chat marquee %s\n", sendMsg.chat.c_str());


	int32 refershtime = 0;

	if(cmdjson["refershtime"].isString())
	{
		refershtime = toInt32(cmdjson["refershtime"]);
	}
    sendMsg.starttime = toInt32(cmdjson["starttime"]);

    sendMsg.endtime = toInt32(cmdjson["endtime"]);

	int32 check = toInt32(cmdjson["check"]);

	sendMsg.looptime = refershtime;
	sendMsg.isloop = check;

	GameHandler::Instance()->RecvChatReq(0, 0, &sendMsg);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
	return true;
}

// ====================================================================================================
// 发送邮件信息
//=====================================================================================================
bool GmtExecutor::GmtSendMail(Json::Value& cmdjson, Json::Value& ackjson)
{
    int32 mailType = 0;
    int32 val = 0;
    uint32 itemID = 0;
    std::string playerName;
    std::vector<REWARD_ITEM> list;
    REWARD_ITEM attach;
    list.clear();

    // json合法性检查
    if(!cmdjson.isMember("mail_receiver")
        || !cmdjson.isMember("mail_title")
        || !cmdjson.isMember("mail_content")
        )
    {
        CnWarn("GmSendMail json error\n");
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "GmSendMail json error";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }
    
    // 参数不为null
    if(cmdjson["mail_receiver"].isNull()
        || cmdjson["mail_title"].isNull()
        || cmdjson["mail_content"].isNull()
        )
    {
        CnWarn("GmSendMail json has null\n");
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "GmSendMail json has null";
        ackjson[HTTP_PARAM_ERRORCODE] = 3;
        return false;
    }


    // 接收者名称
    playerName = toString(cmdjson["mail_receiver"]);

    std::string title = toString(cmdjson["mail_title"]);
    std::string message = toString(cmdjson["mail_content"]);

    std::string condition = toString(cmdjson["condition"]);

    // 邮件类型
    if(cmdjson["mail_type"].isString())
        mailType = SDAtoi(cmdjson["mail_type"].asCString());
    else
    {
        CnDbg("GmSendMail type error");
        ackjson[HTTP_PARAM_MSG] = "GmSendMail type error";
        ackjson[HTTP_PARAM_ERRORCODE] = 4;

        return false;
    }

    switch(mailType)
    {
    case MAIL_TYPE_PLAYER:      // 私人邮件
    case MAIL_TYPE_PLAYER_SYS:  // 个人系统邮件
        {
            if(playerName == "" || playerName == "all")
            {
                CnDbg("GmSendMail no player name");
                ackjson[HTTP_PARAM_MSG] = "GmSendMail no player name";
                ackjson[HTTP_PARAM_ERRORCODE] = 5;
                return false;
            }
        }
        break;
    case MAIL_TYPE_SYSTEM:      // 全服系统邮件
    case MAIL_TYPE_GROUP:       // 公会邮件
        break;
    default:
        CnAssert(false);
        CnDbg("GmSendMail type error");
        ackjson[HTTP_PARAM_MSG] = "GmSendMail default type error";
        ackjson[HTTP_PARAM_ERRORCODE] = 6;
        return false;
        break;
    }

    Json::Value itemList ="";
    if(cmdjson["attachment_list"].isArray())
        itemList = cmdjson["attachment_list"];

    std::vector<REWARD_ITEM> rewardList;
    REWARD_ITEM rewardItem;
    for(uint32 i = 0; i < itemList.size(); ++i)
    {
        rewardItem.Clear();
        read_REWARD_ITEM(rewardItem, itemList[i]);
        rewardList.push_back(rewardItem);
    }

    CnInfo("GmtSendMail type=%d title=%s player=%s\n", mailType, title.c_str(), playerName.c_str());

    MailManager::Instance()->SendMail(0, playerName, title, message, mailType, rewardList, 0, condition);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

// ====================================================================================================
// 开启或取消禁言
//=====================================================================================================
bool GmtExecutor::GmtChat(Json::Value& cmdjson, Json::Value& ackjson)
{
    CHAT_MSG sendMsg;

    std::string _name = toString(cmdjson["account"]);
    int32 flag= toInt32(cmdjson["b_flag"]);
    sendMsg.src_name = _name;
    sendMsg.src_flag = flag;
    GameHandler::Instance()->RecvModifyRightFlag(0, 0, &sendMsg);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

// ====================================================================================================
// 踢玩家下线
//=====================================================================================================
bool GmtExecutor::GmtKickPlayer(Json::Value& cmdjson, Json::Value& ackjson)
{
    std::string playerName = toString(cmdjson["account"]);;

    Player* pPlayer = PlayerManager::Instance()->GetPlayerByName(playerName);
    if(pPlayer)
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_REPEAT_LOGIN, pPlayer->GetCentGateID(), pPlayer->GetLoginID());
    }
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

// ==============================
// 发送警示公告信息
// ==============================
bool GmtExecutor::GmtProclamation(Json::Value& cmdjson, Json::Value& ackjson)
{
    Json::Reader    reader;
    Json::Value     infojson;

    GLOBAL_PROCLAMATION_BAG sendMsg;
    sendMsg.Clear();

    PROCLAMATION_INFO msg_info;

    std::string listjson = toString(cmdjson["list"]);

    if(!reader.parse(listjson, infojson, false))
    {
        CnError("Parse gmt command GmProclamation failed:'%s'\n", listjson.c_str());
        CnAssert(false);
        ackjson[HTTP_PARAM_MSG] = "Parse gmt command GmProclamation failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }

    uint32 count = infojson.size();
    for(uint32 i = 0; i < count; ++i)
    {
        Json::Value& val = infojson[i];
        msg_info.id     = toUint32(val["id"]);
        msg_info.type   = toUint32(val["type"]);
        msg_info.state  = toUint32(val["state"]);
        msg_info.time   = toUint32(val["time"]);
        msg_info.msg    = toString(val["msg"]);
        sendMsg.chat_list.push_back(msg_info);

        if(msg_info.msg.length() > 1000)
        {
            CnInfo("GMT: command msg max length %s\n", "proclamation");
            ackjson[HTTP_PARAM_MSG] = "command msg max length";
            ackjson[HTTP_PARAM_ERRORCODE] = 3;
            return false;
        }
    }
    
    CnInfo("GMT: chat command %s\n", "proclamation");

    GameHandler::Instance()->RecvProclamationReq(0, 0, &sendMsg);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtAnnouceAsk(Json::Value& ackjson)
{
    ackjson[HTTP_PARAM_MSG] = "successful operation";
	return AnnouceManager::Instance()->GetAnnouceList(ackjson);
}

void GmtExecutor::GmtGameAsking(Json::Value& cmdjson)
{
    int32 type = toInt32(cmdjson["type"]);
    
    GMT_MSG sendMsg;
    sendMsg.type = type;
    CnDbg("GmGameAsking type=%d\n", type);
    GameHandler::Instance()->SendOneServer(MSG_GMT_ASK_REQ, &sendMsg);
}

/*void GmtExecutor::OnGmtGameAsking(GMT_MSG* data)
{
    Json::Value ackjson;
    ackjson["type"] = data->type;
    ackjson["value"] = data->value;

    if (data->val_list.size() > 0)
    {
        Json::Value valjson;

        if (data->type == GAME_SETTING_TYPE_VIP_LIMITED_PURCHASING_TIME)
        {
            valjson["start_timestamp"] = data->val_list[0].val;
            valjson["end_timestamp"] = data->val_list[1].val;
            ackjson["timestamps"].append(valjson);
        }
    }

    CnDbg("OnGmGameAsking type=%d value=%d\n", data->type, data->value);
    PushGmtCmdResult(ackjson.toStyledString());
}*/

void GmtExecutor::OnGmtGameAsking(GMT_MSG_SET_GAME_LIST* data)
{
    if(!data) return;

   Json::Value jsongmtist;

    //ackjson["type"] = data->type;
   // ackjson["value"] = data->value;
    Json::Value ackjson;
    for(std::vector<GMT_MSG_SET_GAME>::iterator iter = data->gmtsetgame_list.begin(); iter != data->gmtsetgame_list.end(); iter++)
    {
        ackjson.clear();
        ackjson["value"]= iter->value;
        ackjson["type"] = iter->type;

        CnDbg("OnGmGameAsking type=%d value=%d\n", iter->type, iter->value);
        jsongmtist["gmtsetgame_list"].append(ackjson);
    }


   // Json::Value jsongack;
    if(data->val_list.size() >= 2)
    {
         ackjson.clear();
         ackjson["start_timestamp"] = data->val_list[0].val;
         ackjson["end_timestamp"] = data->val_list[1].val;
         ackjson["type"]  = data->type;
         CnDbg("OnGmGameAsking type=%d start_timestamp=%d\n", data->type,  data->val_list[0].val);
         jsongmtist["timestamps"].append(ackjson);
   }


    // Json::Value jsongack;
    //PushGmtCmdResult(jsongmtist.toStyledString());
    PushGmtCmdResultInfo(jsongmtist);
}
bool GmtExecutor::GmtGameSetting( Json::Value& cmdjson, Json::Value& ackjson )
{
    int32 type = toInt32(cmdjson["type"]);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    if(type == GAME_SETTING_TYPE_CHAT_WORLD_LEVEL)
    {

        GMT_MSG sendMsg;
        sendMsg.type = type;
        sendMsg.value = toInt32(cmdjson["chat_world"]);

        CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        sendMsg.Clear();
        sendMsg.type = GAME_SETTING_TYPE_CHAT_TEAM_LEVEL;
        sendMsg.value = toInt32(cmdjson["chat_team"]);

        CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        sendMsg.Clear();
        sendMsg.type = GAME_SETTING_TYPE_CHAT_PRIVATE_LEVEL;
        sendMsg.value = toInt32(cmdjson["chat_private"]);

        CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        sendMsg.Clear();
        sendMsg.type = GAME_SETTING_TYPE_CHAT_HORN_LEVEL;
        sendMsg.value = toInt32(cmdjson["chat_hron"]);

        CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        sendMsg.Clear();
        sendMsg.type = GAME_SETTING_TYPE_CHAT_GUILD_LEVEL;
        sendMsg.value = toInt32(cmdjson["chat_guild"]);

        CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        return true;
    }
    else if (type == GAME_SETTING_TYPE_VIP_LIMITED_PURCHASING_TIME)
    {
        GMT_MSG sendMsg;
        sendMsg.type = type;
        GMT_VALUE gmtVal;
        gmtVal.val = toInt32(cmdjson["start_timestamp"]);
        sendMsg.val_list.push_back(gmtVal);
        gmtVal.val = toInt32(cmdjson["end_timestamp"]);
        sendMsg.val_list.push_back(gmtVal);

        GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

        return true;
    }

    GMT_MSG sendMsg;
    sendMsg.type = type;
    sendMsg.value = toInt32(cmdjson["value"]);

    CnDbg("GmGameSetting type=%d value=%d\n", type, sendMsg.value);

    GameHandler::Instance()->SendToAll(MSG_GMT_SET_REQ, &sendMsg);

    return true;
}

void GmtExecutor::OnGmtGameSetting(GMT_MSG* data)
{
    Json::Value ackjson;
    ackjson["type"] = data->type;
    ackjson["value"] = data->value;

    CnDbg("OnGmGameSetting type=%d value=%d\n", data->type, data->value);
}

bool GmtExecutor::GmtGuildBulletin( Json::Value& cmdjson, Json::Value& ackjson )
{
    uint64 id = Crown::SDAtou64(toString(cmdjson["id"]).c_str());
    std::string bulletinStr = toString(cmdjson["bulletin"]);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return GuildManager::Instance()->OnGuildBulletinUpdate(id, bulletinStr);
}

bool GmtExecutor::GmtGuildOffice( Json::Value& cmdjson, Json::Value& ackjson )
{
    uint64 gid = Crown::SDAtou64(toString(cmdjson["gid"]).c_str());
    uint64 pid = Crown::SDAtou64(toString(cmdjson["pid"]).c_str());
    uint32 officeid = toUint32(cmdjson["officeid"]);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return GuildManager::Instance()->OnGuildOfficeUpdate(gid, pid, officeid);
}

bool GmtExecutor::GmtGuildInfo( Json::Value& cmdjson, Json::Value& ackjson )
{
    uint64 id = Crown::SDAtou64(toString(cmdjson["id"]).c_str());
    // 检查公会信息是否已加载，防止玩家加入的是一个全都离线的公会
    if(!GuildManager::Instance()->GetGuildInfo(id))
    {
        GuildJob::Instance()->LoadGuildInfoForMgr(id);
    }
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtAskChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson)
{
    std::vector<uint32> chatBlockIPs = ChatManager::Instance()->GetChatBlockIPs();

    for(uint32 i=0; i<chatBlockIPs.size(); i++)
    {
        uint32 ip = chatBlockIPs[i];
        ackjson["ip"].append( Crown::SDInetNtoa(ip));
    }
    
    return true;
}

bool GmtExecutor::GmtAddChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson)
{
    int32 ip = Crown::SDInetAddr(toString(cmdjson["ip"]).c_str());

    ChatManager::Instance()->AddChatBlockIP(ip);
    CnDbg("GmtAddChatBlockIP value=%d\n", ip);
    return true;
}

bool GmtExecutor::GmtRemoveChatBlockIP(Json::Value& cmdjson, Json::Value& ackjson)
{
    int32 ip = Crown::SDInetAddr(toString(cmdjson["ip"]).c_str());

    ChatManager::Instance()->RemoveChatBlockIP(ip);
    CnDbg("GmtRemoveChatBlockIP value=%d\n", ip);
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtAddCrossCenterIP(Json::Value& cmdjson, Json::Value& ackjson)
{
    std::string ip = toString(cmdjson["ip"]);
    uint32 port = toUint32(cmdjson["port"]);

    if(ip =="" || port == 0)
    {
        ackjson[HTTP_PARAM_MSG] = "ip or port error";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false; 
    }

    std::list<ServerConfig>* list = ConfigLoader::Instance()->GetServerConfig(SERVER_TYPE_GAME);
    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        // 发送给已激活的跨服GameServer
        if(!it->m_is_config && it->m_activate)
        {
            SERVER_INFO info;
            info.server_ip = ip;
            info.server_port = port;
            GameHandler::Instance()->SendByID(it->m_id, MSG_CROSS_SERVER_ADD, &info);

        }
    }
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtRemoveCrossCenterIP(Json::Value& cmdjson, Json::Value& ackjson)
{
    std::string ip = toString(cmdjson["ip"]);
    uint32 port = toUint32(cmdjson["port"]);

    if(ip =="" || port == 0)
    {
        ackjson[HTTP_PARAM_MSG] = "ip or port error";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false; 
    }

    std::list<ServerConfig>* list = ConfigLoader::Instance()->GetServerConfig(SERVER_TYPE_GAME);
    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        // 发送给已激活的跨服GameServer
        if(!it->m_is_config && it->m_activate)
        {
            SERVER_INFO info;
            info.server_ip = ip;
            info.server_port = port;
            GameHandler::Instance()->SendByID(it->m_id, MSG_CROSS_SERVER_REMOVE, &info);

        }
    }
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

void GmtExecutor::GmtAskCrossCenterIP(Json::Value& cmdjson)
{
    ServerConfig* gsInfo = NULL;
    std::list<ServerConfig>* list = ConfigLoader::Instance()->GetServerConfig(SERVER_TYPE_GAME);
    for(std::list<ServerConfig>::iterator it=list->begin(); it != list->end(); ++it)
    {
        // 发送给已激活的跨服GameServer
        if(!it->m_is_config && it->m_activate)
        {
            gsInfo = &(*it);
            break;
        }
    }

    // 询问某一个跨服的GameServer
    if(!gsInfo)
    {
        // 非跨服的区无数据
        CnDbg("on cross center\n");
        Json::Value ackjson;
        //PushGmtCmdResult(ackjson.toStyledString());
        ackjson[""] = "on cross center";
        PushGmtCmdResultInfo(ackjson);
        return;
    }

    NONE none;
    GameHandler::Instance()->SendByID(gsInfo->m_id, MSG_CROSS_SERVER_LIST_REQ, &none);
}

void GmtExecutor::OnGmtAskCrossCenterIpListAck(void* data)
{
    SERVER_LIST_ACK* curdata = (SERVER_LIST_ACK*)data;
    Json::Value ackjson;

    if(!curdata)
         PushGmtCmdResultInfo(ackjson);

    uint32 crossCount = 0;
    for(uint32 i=0; i<curdata->list.size(); i++)
    {
        SERVER_INFO& info = curdata->list[i];
        Json::Value js_info;
        js_info["ip"] = info.server_ip;
        js_info["port"] = info.server_port;
        js_info["center_id"] = info.center_id;

        if(info.center_id > 0)
            crossCount++;

        ackjson["ip"].append( js_info );
    }

    CnDbg("OnGmtAskCrossCenterIP %d cross=%d\n", curdata->list.size(), crossCount);
    PushGmtCmdResultInfo(ackjson);
}

bool GmtExecutor::GmtAskServerRunningState(Json::Value& cmdjson, Json::Value& ackjson)
{
    uint32 state = ServerMonitor::Instance()->GetState();

    ackjson["state"] = state;
    ackjson["player_num"] = PlayerManager::Instance()->GetPlayerNum();
	ackjson["version"] = VERSION_PROTOCOL;
    ackjson[HTTP_PARAM_MSG] = "successful operation";
    return true;
}

bool GmtExecutor::GmtSynchronousGameSetting( Json::Value& cmdjson, Json::Value& ackjson )
{
    uint32 type = toUint32(cmdjson["type"]);

    if(type == 1)
    {
        int32 curTime = (int32)Crown::SDNow().GetTimeValue();
        CnDbg("update GameSettingJob success time=%d \n", curTime);
        GameSettingJob::Instance()->HandleGmtGameSetting();
        ackjson[HTTP_PARAM_MSG] = "successful operation";
        return true;
    }
    else
    {
        //CnError("update fesitveldb failed %u\n", update_time);
        CnDbg("update GameSettingJob failed type=%d \n", type);
        ackjson[HTTP_PARAM_MSG] = "update GameSettingJob failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }
}

bool GmtExecutor::GmtLikeInfo( Json::Value& cmdjson, Json::Value& ackjson )
{
    std::string user_id = toString(cmdjson["user_id"]);

    uint32 flag = PlayerManager::Instance()->OnLikeState(user_id);

    if(flag == 1)
    {
        int32 curTime = (int32)Crown::SDNow().GetTimeValue();
        CnDbg("Get Like Reward success account=%s  time=%d\n", user_id.c_str(), curTime);
        ackjson[HTTP_PARAM_STATUS] = "success";
        ackjson[HTTP_PARAM_MSG] = "Get like reward success";
        return true;
    }
    else
    {
        CnDbg("Get Like Reward Failed type=%s\n", user_id.c_str());
        ackjson[HTTP_PARAM_STATUS] = "failed";
        ackjson[HTTP_PARAM_MSG] = "Get like reward failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }
}

bool GmtExecutor::GmtSubtractItem( Json::Value& cmdjson, Json::Value& ackjson )
{
    //std::string account = toString(cmdjson["account"]);
    uint64 pid = Crown::SDAtou64(toString(cmdjson["account"]).c_str());
    uint32 itemid = toUint32(cmdjson["item_id"]);
    uint32 itemnum = toUint32(cmdjson["item_num"]);

    bool flag = PlayerManager::Instance()->OnSubItem(pid, itemid, itemnum);

    if(flag)
    {
        int32 curTime = (int32)Crown::SDNow().GetTimeValue();
        CnDbg("Gmt Sub Item  account=%d id=%d num=%d  time=%d\n", pid, itemid, itemnum, curTime);
        ackjson[HTTP_PARAM_STATUS] = "success";
        ackjson[HTTP_PARAM_MSG] = "Gmt Sub Item success";
        return true;
    }
    else
    {
        CnDbg("Gmt Sub Item Failed type=%d\n", pid);
        ackjson[HTTP_PARAM_STATUS] = "failed";
        ackjson[HTTP_PARAM_MSG] = "Gmt Sub Item failed";
        ackjson[HTTP_PARAM_ERRORCODE] = 2;
        return false;
    }
}
