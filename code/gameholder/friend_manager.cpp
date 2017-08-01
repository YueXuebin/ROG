#include "gameholder_pch.h"
#include "friend_manager.h"
#include "player_manager.h"
#include "player.h"
#include "message_encoder.h"
#include "center_handler.h"
#include "event_define.h"
#include "task_manager.h"

// ==================================================
// 构造函数
// ==================================================
FriendManager::FriendManager(Player* player) :
m_player(player)
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
FriendManager::~FriendManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void FriendManager::Clear()
{
    m_friend_info.Clear();
}

// ==================================================
// 接收网络消息
// ==================================================
void FriendManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FRIEND_INFO_REQ:           // 好友信息请求
        OnFriendInfoReq();
        break;
    case MSG_FRIEND_INFO_NTF:           // 好友信息修改通知
        OnFriendInfoNtf(data);
        break;
    case MSG_FRIEND_ADD_REQ:            // 添加好友
        OnFriendAddReq(data);
        break;
    case MSG_FRIEND_ADD_ANS:            // 添加好友应答
        OnFriendAddAns(data);
        break;
    case MSG_FRIEND_DEL_REQ:            // 删除好友
        OnFriendDelReq(data);
        break;
    case MSG_FRIEND_QUICK_ADD_REQ:      // 快速加入好友请求
        OnFriendQuickAddReq(data);
        break;
    default:
        break;
    }
}

// ==================================================
// 设置好友信息
// ==================================================
void FriendManager::LoadInfo(FRIEND_LIST& info)
{
    m_friend_info = info;
    // 数据合法性检查
#ifdef DEBUG
    for(std::vector<FRIEND_INFO>::iterator it = m_friend_info.list.begin();
        it != m_friend_info.list.end(); ++it)
    {
        CnAssert(it->sex != 0);
    }
#endif
}

// ==================================================
// 填充好友信息
// ==================================================
void FriendManager::FillInfo(FRIEND_LIST& info)
{
    info = m_friend_info;
}

// ==================================================
// 好友信息请求
// ==================================================
void FriendManager::OnFriendInfoReq()
{
    m_friend_info.opt_flag = OPT_INIT_FLAG;
    m_player->SendToGate(MSG_FRIEND_INFO_NTF, &m_friend_info);
}

// ==================================================
// 好友信息通知
// ==================================================
void FriendManager::OnFriendInfoNtf(void* data)
{
    FRIEND_LIST* recvMsg = (FRIEND_LIST*)data;
    //FRIEND_LIST sendMsg;
    uint64 playerID = 0;
    if(!recvMsg) return;

    for(std::vector<FRIEND_INFO>::iterator friendIt = recvMsg->list.begin(); 
        friendIt != recvMsg->list.end(); ++friendIt)
    {
        if(OPT_ADD_FLAG == recvMsg->opt_flag)
        {
            m_friend_info.list.push_back(*friendIt);
        }
        else if(OPT_DEL_FLAG == recvMsg->opt_flag)
        {
            bool isExist = false;
            for(std::vector<FRIEND_INFO>::iterator it = m_friend_info.list.begin();
                    it != m_friend_info.list.end(); ++it)
            {
                if((it->player_id_l == friendIt->player_id_l) && 
                    (it->player_id_h == friendIt->player_id_h))
                {
                    m_friend_info.list.erase(it);
                    isExist = true;
                    break;
                }
            }
            CnAssert(isExist);
        }
        else if(OPT_UPDATE_FLAG == recvMsg->opt_flag)
        {
            bool isExist = false;
            for(std::vector<FRIEND_INFO>::iterator it = m_friend_info.list.begin();
                it != m_friend_info.list.end(); ++it)
            {
                if((it->player_id_l == friendIt->player_id_l) && 
                    (it->player_id_h == friendIt->player_id_h))
                {
                    //CnAssert( (it->gatesvr_id != 0 && it->unionmap_id != 0) 
                    //    || (it->gatesvr_id == 0 && it->unionmap_id == 0) );
                    //CnAssert( (friendIt->gatesvr_id != 0 && friendIt->unionmap_id != 0) 
                    //    || (friendIt->gatesvr_id == 0 && friendIt->unionmap_id == 0) );
                    //CnAssert(it->career == friendIt->career);
                    CnAssert(it->sex == friendIt->sex);
                    *it = *friendIt;
                    isExist = true;
                    break;
                }
            }
            CnAssert(isExist);
        }
        else
            CnAssert(false);
    }

    if(recvMsg->opt_flag == OPT_ADD_FLAG)
    {
        m_player->SendTaskEvent(TASK_EVENT_ADD_FRIEND, 0, 0);
    }
}

// ==================================================
// 添加好友请求
// ==================================================
void FriendManager::OnFriendAddReq(void* data)
{
    FRIEND_ADD_REQ* recvMsg = (FRIEND_ADD_REQ*)data;
    uint16 errcode = ERR_SUCCEED;
    uint64 player_id = 0;
    uint32 union_id = 0;
    Player* player = NULL;

    CnAssert(recvMsg);
    if(!recvMsg) return;

    // 检查好友为不同人
    if(recvMsg->dst_name == m_player->GetName())
    {
        errcode = ERR_FAILED;
        goto _ERROR;
    }

    // 检查是否已有同名好友
    for(std::vector<FRIEND_INFO>::iterator it = m_friend_info.list.begin();
            it != m_friend_info.list.end(); ++it)
    {
        if(it->name == recvMsg->dst_name)
        {
            errcode = ERR_SAME_NAME_PLAYER;
            goto _ERROR;
        }
    }

    recvMsg->player_id_l = U64ID_LOW(m_player->GetPlayerID());
    recvMsg->player_id_h = U64ID_HIGH(m_player->GetPlayerID());
    recvMsg->src_name = m_player->GetName();
    recvMsg->src_level = m_player->GetLevel();

    player = PlayerManager::Instance()->GetPlayerByName(recvMsg->dst_name);
    if(player && (player->GetNetState() == Player::kStateInGame))
    {
        // 不是同服的玩家不能加好友
        if(player->GetCenterSvrID() != m_player->GetCenterSvrID())
        {
            errcode = ERR_FAILED;
            goto _ERROR;
        }

        //player->SendToGate(MSG_FRIEND_ADD_REQ, recvMsg);  // 给 B 玩家
        
        //player->SendToGate(MSG_FRIEND_ADD_CFM, recvMsg);  // 给 B 玩家
    }

    // 直接改为可单向加为好友(就是好友收藏夹功能)
    // 全部让center做处理
    CenterHandler::Instance()->SendByPlayer(m_player, MSG_FRIEND_ADD_REQ, recvMsg, m_player->GetPlayerID());
    
    return;

_ERROR:
    SendFriendAddAck(errcode, recvMsg->src_name, recvMsg->dst_name);
}

// ==================================================
// 添加好友应答
// ==================================================
void FriendManager::OnFriendAddAns(void* data)
{
    FRIEND_ADD_ACK* recvMsg = (FRIEND_ADD_ACK*)data;
    //uint16 errcode = ERR_SUCCEED;
    //uint64 playerID = 0;

    if(!recvMsg) return;

    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_FRIEND, m_player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    // 检查好友数量
    //if(m_friend_info.list.size() >= GameParamConfig::Instance()->GetMaxFriendNum(m_player->GetVipLevel()))
    //{
    //    errcode = ERR_MAX_FRIEND_NUM;
    //    goto _ERROR;
    //}

    recvMsg->dst_name = m_player->GetName();

    CenterHandler::Instance()->SendByPlayer(m_player, MSG_FRIEND_ADD_ANS, recvMsg, m_player->GetPlayerID());

//_ERROR:
//    m_player->NotifyErrorCode(errcode);
}

// ==================================================
// 删除好友
// ==================================================
void FriendManager::OnFriendDelReq(void* data)
{
    FRIEND_DEL_REQ* recvMsg = (FRIEND_DEL_REQ*)data;

    if(!recvMsg) return;

    CenterHandler::Instance()->SendByPlayer(m_player, MSG_FRIEND_DEL_REQ, recvMsg, m_player->GetPlayerID());
}

// ==================================================
// 快速加入好友请求
// ==================================================
void FriendManager::OnFriendQuickAddReq(void* data)
{
    FRIEND_QUICK_ADD_REQ sendMsg;

    sendMsg.Clear();

    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_FRIEND, m_player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    TaskManager* taskMgr = m_player->GetTaskManager();
    if(taskMgr->HasFriendTask())
    {
        sendMsg.add_robot = 1;
    }

    CenterHandler::Instance()->SendByPlayer(m_player, MSG_FRIEND_QUICK_ADD_REQ, &sendMsg, m_player->GetPlayerID());
}

// ==================================================
// 发送添加好友应答
// ==================================================
void FriendManager::SendFriendAddAck(uint16 errcode, std::string src_name, std::string dst_name)
{
    FRIEND_ADD_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.src_name = src_name;
    sendMsg.dst_name = dst_name;
    m_player->SendToGate(MSG_FRIEND_ADD_ACK, &sendMsg);
}

// ==================================================
// 发送删除好友应答
// ==================================================
void FriendManager::SendFriendDelAck(uint16 errcode)
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = errcode;
    m_player->SendToGate(MSG_FRIEND_DEL_ACK, &sendMsg);
}
