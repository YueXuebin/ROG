#include "gameholder_pch.h"
#include "title_manager.h"
#include "player.h"
#include "title_config.h"
#include "battle_recorder.h"
#include "event_define.h"
#include "vip_config.h"
#include "scene.h"
#include "property_set_calculater.h"

bool SortTitleId(const TITLE_INFO& title1, const TITLE_INFO& title2) 
{
    return (title1.titlid < title2.titlid);
}

TitleManager::TitleManager(Player* player) :
m_player(player)
{
    Clear();
}

TitleManager::~TitleManager(void)
{

}

void TitleManager::LoadTitleInfo(TITLE_LIST& info)
{
    m_titleList = info.list;
    JudgeVipTitle(true); //找到最大vip 称号，移除 兼容老数据
}

void TitleManager::FillTitleInfo(TITLE_LIST& info)
{
    info.list = m_titleList;
}

void TitleManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case  MSG_INST_TITLE_REQ:
        {
            InitTitle(data);
        }
        break;
    case MSG_EQUIP_REQ:
        {
            EquipTitle(data);
        }
        break;
    case MSG_UPDATE_TITLE:
        {
            CalDailyRefresh();
        }
    default:
        break;
    }
}

void TitleManager::AddTitle(uint32 titileid)
{
    TITLE_INFO* titlepoint = GetTitleInfo(titileid);
    if(titlepoint)
        return;

    int32 b_state = ERR_FAILED;

    int32 type  = TitleConfig::Instance()->GetTitleType(titileid);

    if(!type)
        return;

    TITLE_INFO titleInfo;
    titleInfo.titlid = titileid;
    titleInfo.b_state = HAVE_NO_TITLE;
    if(type == ACTIVITY_TITLE)
    {
        uint32 curtime = 0;
        curtime = TitleConfig::Instance()->GetTitleTime(titileid);
        titleInfo.starttime = (uint32)Crown::SDNow().GetTimeValue() + curtime;
        titleInfo.time = curtime;
    }
    else
    {
        titleInfo.starttime = 0;
        titleInfo.time = 0;
    }
    titleInfo.num = 1;
    m_titleList.push_back(titleInfo);
    TITLE_INFO* curtitle = 0;
    curtitle = &titleInfo;
    b_state = OPT_ADD_FLAG;

    m_player->CalcProperty();
    //得到某一个称号，得到解锁宠物条件
    m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, titileid, ACTIVATION_TITLE, 0);

    m_player->SendPlaySystemLog(PLAYSYSTEM_TITLE_ADD, titileid);

    if(b_state != OPT_UPDATE_FLAG)
    {
        SendInitTitleAck(b_state,curtitle);
    }
    else
    {
        SendOneUpate(b_state,curtitle);
    }
}

void TitleManager::Clear()
{
}

void TitleManager::InitTitle(void* data)
{
    TITLE_INFO* curtitle = NULL;
    SendInitTitleAck(OPT_INIT_FLAG, curtitle);

    // 请求排行榜刷新
    //SendRankReqList();
}

void TitleManager::onEnterScene( Scene* pScene )
{
    //NotifyOtherPlayer();
}

void TitleManager::CalDailyRefresh()
{
    TitleList::iterator titleIt;
    Crown::CSDDateTime now = Crown::SDNow();
    int32 titleType = 0;

    bool needNotify = false;

    m_updateList.clear();

    for(titleIt = m_titleList.begin(); titleIt != m_titleList.end(); )
    {
        titleType = TitleConfig::Instance()->GetTitleType(titleIt->titlid);
        if (titleType == ACTIVITY_TITLE)
        {
            //时间到了
            if((uint32)now.GetTimeValue() >=  titleIt->starttime)
            {
                uint32 titleId = titleIt->titlid;
                titleIt->time = 0;
                titleIt->starttime = 0;

                if(titleIt->b_state == HAVE_TITLE)
                {
                    titleIt->b_state = HAVE_NO_TITLE;
                }

                needNotify = true ;//通知客户端
                m_updateList.push_back(*titleIt);
                titleIt = m_titleList.erase(titleIt);
                m_player->SendPlaySystemLog(PLAYSYSTEM_TITLE_DEL, titleId);
                m_player->CalcProperty();
            }
            else
            {
                needNotify = true;
                //时间没到
                titleIt->time = titleIt->starttime - (uint32)now.GetTimeValue();
                m_updateList.push_back(*titleIt);
                titleIt++;
            }
        }
        else
        {
            titleIt++;
        }
    }


    if(needNotify)
    {
        NotifyOtherPlayer();
        SendInitTitleAck(OPT_UPDATE_FLAG);
    }
}

void TitleManager::SendInitTitleAck(uint16 b_state, TITLE_INFO* titleInfo)
{
    TITLE_STATE_LIST sendMsg;

    switch(b_state)
    {
    case OPT_ADD_FLAG:
        {
            if(titleInfo)
            {
                sendMsg.opt_flag = OPT_ADD_FLAG;
                sendMsg.list.push_back(*titleInfo);
            }
        }
        break;
    case OPT_UPDATE_FLAG:
        {
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            sendMsg.list = m_updateList;
        }
        break;

    case OPT_DEL_FLAG:
        {
            sendMsg.opt_flag = OPT_DEL_FLAG;
            sendMsg.list = m_updateList;
        }
        break;
    case OPT_INIT_FLAG:
        {
            sendMsg.opt_flag = OPT_INIT_FLAG;
            sendMsg.list = m_titleList;
        }
        break;
    }
    m_player->SendToGate(MSG_INST_TITLE_ACK, &sendMsg);
}

void TitleManager::SendOneUpate(uint16 b_state, TITLE_INFO* titleInfo)
{
    TITLE_STATE_LIST sendMsg;
    sendMsg.Clear();
    switch(b_state)
    {
    case OPT_UPDATE_FLAG:
        {
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            if(titleInfo)
            {
                sendMsg.list.push_back(*titleInfo);
                m_player->SendToGate(MSG_INST_TITLE_ACK, &sendMsg);
            }
        }
        break;
    }
}

void TitleManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case EVENT_WORLD_BOSS_DAMAGE_RANK:  // 活动完成后
        {
            // 活动输出排名
            int32 rank = m_player->GetBattleRecorder()->GetWorldBossRank();
            RankTitle(WORLD_BOSS_TITLE, rank);
        }
        break;
    case EVENT_ASHUAR_TITLE:
        {
            // 修罗场排名
            int32 dead_num = arg1; // 杀死还是被杀
            int32 kill_num = arg2; // 个数
            if(dead_num > 0)
            {
                AshuarTitle(DEAD_FLAG,dead_num);
            }
            if(kill_num > 0)
            {
                AshuarTitle(KILL_FLAG,kill_num);
            }
        }
        break;
    case EVENT_ANSWER_RANK:
        {
            //勇者问答
            int32 rank = arg1;
            RankTitle(ANSWING_TITLE,rank);
        }
        break;
        //所有成就的这样写
    case EVENT_ACHIVE_TITLE:
        {
            int32 achiveid = arg1;//要发送成就条件id
            //JudeAddTitle(achiveid,ACHIVE_TITLE);
        }
        break;
        //离线竞技排名
        //勇者争霸
    case EVENT_OFFLINE_RANK:
        {
            ////勇者争霸的称号大于20级才给
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(OFF_LINE_TITLE,rank);
            //}
        }
        break;
    case EVENT_HOSERUNING_RANK:
        {
            int32 rank = arg1;
            RankTitle(HOSERUNING_TITLE,rank);
        }
        break;
         //case EVENT_TASK_TITLE:
         //{
         //uint32 taskid = arg1;
         //TaskTitle(TASK_TITLE,taskid);
         //}
         //break;
    case EVENT_FOLLOW_TITLE:
        {
            int32 num = arg1;
            GeneRalTitle(FOLLOW_TITLE,num);
        }
        break;
        //暮光之战
    case EVENT_TWIGHT_WAR:
        {
            //排名第一
            int32 rank = arg1;
            RankTitle(GUID_BOSS_TITLE,rank);
        }
        break;
        //魔物讨伐令
    case EVENT_GUIDBOSS_RANK:
        {
            //所有人员
            DealAllPlayer(TOWER_LIGHT_TITLE);
        }
        break;
    case EVENT_USE_ITEM:
        {
            int32 titleid = arg1;
            int32 type = 0;
            type = TitleConfig::Instance()->GetTitleType(titleid);

            if(!type)
                return;
            AddTitle(titleid);
        }
        break;
    case EVENT_CHANGE_VIP:
        {
            MoreThanCondiiton(VIP_TITLE, m_player->GetVipLevel());
        }
        break;
    case EVENT_GUILD_RANK_TITLE:
        {
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(GUILD_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_BATTLE_RANK_TITLE: 
        {
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(BATTLE_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_HOROSCOPE_RANK_TITLE:
        {
            //星空图排行称号
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(HOROSCOPE_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_ASHURA_RANK_TITLE:
        {
            ////修罗场称号
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(ASHURA_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_GET_VIP_REWARD_TITLE:
        {
            int32 level = arg1;
            GeneRalTitle(GET_VIP_REWARD_TITLE,level);
        }
        break;
    case EVENT_VIP_OUT_OF_DATE:     //vip 过期
        {
           // int32 level = arg1;
            DeleteOutDateVip();
        }
        break;
    case EVENT_WING_RANK_TITLE:
        {
            //翅膀排行称号
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(WING_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_PET_RANK_TITLE:
        {
            ////宠物排行称号
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(PET_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_RIDE_RANK_TITLE:
        {
            //坐骑排行称号
            //if(GameParamConfig::Instance()->GetOpenRankState())
            //{
            //    int32 rank = arg1;
            //    RankTitle(RIDE_RANK_TITLE,rank);
            //}
        }
        break;
    case EVENT_FB_LIKE_TITLE:
        {
            int32 num = arg1;
            GeneRalTitle(FB_LIKE_TITLE,num);
        }
        break;
    default:
        break;
    }
}

void TitleManager::GeneRalTitle(int32 type,int32 reason,int32 num)
{
    uint32 titleid = 0;
    titleid = TitleConfig::Instance()->GetGeneralTitle(type,reason,num);
    if(!titleid)
        return;
    AddTitle(titleid);
}

void TitleManager::DealAllPlayer(int32 type)
{
    uint32 titleid = 0;
    titleid = TitleConfig::Instance()->DealAllPlayer(type);
    if(!titleid)
        return;
    AddTitle(titleid);
}

void TitleManager::TaskTitle(int32 type,uint32 taskid)
{
    uint32 titleid = 0;
    titleid = TitleConfig::Instance()->GetTaskTitle(type,taskid);
    if(!titleid)
        return;
    AddTitle(titleid);
}

void TitleManager::AshuarTitle(int32 flag,int32 num)
{
    TitleVec::iterator vec_title;

    TitleVec* curtitle = 0;
    curtitle = TitleConfig::Instance()->GetAshuraTitle(ASHURA_TITLE,flag,num);
    if(!curtitle)
        return;
    for (vec_title = curtitle->begin(); vec_title != curtitle->end(); vec_title++)
    {
        AddTitle(*vec_title);
    }
}


void TitleManager::MoreThanCondiiton(int32 type,int32 level)
{
    int32 curtitleid = 0;
    curtitleid = TitleConfig::Instance()->GetGreaterCondition(type,level);
    if(!curtitleid)
        return;

    int32 upOrDown = JudgeVipUpOrDown(level);

    AddTitle(curtitleid);

    bool b_upOrDown = false;
    if(upOrDown == VIP_UP)
        b_upOrDown = true;
    else if (upOrDown == VIP_DOWN)
        b_upOrDown = false;
    else
        return;

    JudgeVipTitle(b_upOrDown);
}

//删除过期的的vip等级
void TitleManager::DeleteOutDateVip()
{
    uint32 deleteid = 0;
    TitleList::iterator cutrTitle;
    for (cutrTitle = m_titleList.begin(); cutrTitle != m_titleList.end(); cutrTitle++)
    {
        uint32 type = TitleConfig::Instance()->GetTitleType(cutrTitle->titlid);
        if(type == ORDINARY_TITLE)
        {
            int32 vipmodle= TitleConfig::Instance()->GetTitleModelType(cutrTitle->titlid);
            if(vipmodle == VIP_TITLE)
            {
                deleteid = cutrTitle->titlid;
                break;
            }
        }
    }

    if(deleteid)
    {
        DeleteTitle(deleteid);
        NotifyOtherPlayer();
    }
}
int32 TitleManager::JudgeVipUpOrDown(int32 level)
{
    std::sort(m_titleList.begin(), m_titleList.end(), SortTitleId);
    TitleList::iterator cutrTitle;
    for (cutrTitle = m_titleList.begin(); cutrTitle != m_titleList.end(); cutrTitle++)
    {
        uint32 type = TitleConfig::Instance()->GetTitleType(cutrTitle->titlid);
        if(type == ORDINARY_TITLE)
        {
            int32 vipmodle= TitleConfig::Instance()->GetTitleModelType(cutrTitle->titlid);
            if(vipmodle == VIP_TITLE)
            {
                int32 conditonid = TitleConfig::Instance()->GetVipConditonReason(cutrTitle->titlid);
                if(level >= conditonid)
                {
                    return VIP_UP;
                }
                else
                {
                    return VIP_DOWN;
                }
            }
        }
    }

    return VIP_ERROR;
}
void TitleManager::RankTitle(int32 type,int32 rank)
{
    TitleVec::iterator vec_title;
    TitleVec* curtitle = 0;
    curtitle = TitleConfig::Instance()->GetRankTitle(type,rank);
    if(!curtitle)
        return;
    for (vec_title = curtitle->begin(); vec_title != curtitle->end(); vec_title++)
    {
        AddTitle(*vec_title);
    }
}

int32 TitleManager::GetHaveNum()
{
    int32 have_num  = 0;
    TitleList::iterator iterTitle;
    for (iterTitle = m_titleList.begin(); iterTitle!=m_titleList.end(); iterTitle++)
    {
        if(iterTitle->b_state == HAVE_TITLE)
        {
            have_num = have_num + 1;
        }
    }

    return have_num;
}

TITLE_INFO* TitleManager::GetTitleInfo(uint32 titleid)
{
    TITLE_INFO* titleinfo = NULL;
    for (TitleList::iterator iterTitle = m_titleList.begin(); iterTitle!=m_titleList.end(); iterTitle++)
    {
        if(titleid == iterTitle->titlid)
        {
            titleinfo = &(*iterTitle);
            break;
        }
    }

    return titleinfo;
}


void TitleManager::SetTitleState(uint32 titleid,int32 state)
{
    for (TitleList::iterator iterTitle = m_titleList.begin(); iterTitle!=m_titleList.end(); iterTitle++)
    {
        if(titleid == iterTitle->titlid)
        {
            iterTitle->b_state = state;
            break;
        }
    }

}


void TitleManager::EquipTitle(void* data)
{
    EQUIP_TITLE* recvMsg = (EQUIP_TITLE*)data;
    uint32 titlid = recvMsg->title_id;
    uint8 b_have =  recvMsg->b_have;

    TITLE_INFO* iterTitle = GetTitleInfo(titlid);
    if(!iterTitle)
        return;

    int32 type  = TitleConfig::Instance()->GetTitleType(titlid);
    if(!type)
        return;

    bool  b_exe = false;
    bool b_titlefind  =  false;
    if(HAVE_TITLE == b_have)
    {
        if(TITLE_MAX_NUM <= 0)
        {
            CnAssert(false);
            return;
        }
        if(GetHaveNum()>= TITLE_MAX_NUM)
            return;

        if(type == ACTIVITY_TITLE)
        {
            if(iterTitle->starttime <= 0)
                return;
        }

        //属性值发生改变
        iterTitle->b_state = HAVE_TITLE;
        m_player->CalcProperty();
        b_exe = true;
    }
    else if(HAVE_NO_TITLE == b_have)
    {
        iterTitle->b_state = HAVE_NO_TITLE;
        m_player->CalcProperty();
        b_exe = true;
    }

    if(b_exe)
    {
        NotifyOtherPlayer();
    }

    TITLE_INFO* curtitle = 0;
    curtitle = &(*iterTitle);
    SendOneUpate(OPT_UPDATE_FLAG, curtitle);
}

void TitleManager::NotifyOtherPlayer()
{
    PLAYER_TITLE_LIST info;
    info.Clear();

    TitleList::iterator cutitleiter;
    for (cutitleiter = m_titleList.begin(); cutitleiter!=m_titleList.end();cutitleiter++)
    {
        if(cutitleiter->b_state == HAVE_TITLE)
        {
            info.list.push_back(*cutitleiter);
        }
    }
    //通知其他玩家看我的称号消息
    m_player->NotifyTitleInfo(info);
}

void TitleManager::SendUpdateTitleAck(TITLE_INFO* titleInfo,bool Instbool)
{
    TITLE_STATE_LIST sendMsg;
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_UPDATE_FLAG;
    sendMsg.list = m_titleList;
    m_player->SendToGate(MSG_INST_TITLE_ACK, &sendMsg);
}

void TitleManager::CalProp()
{
    TitleList::iterator cutrTitle;
    PropertySet propSet;

    for (cutrTitle = m_titleList.begin(); cutrTitle != m_titleList.end(); cutrTitle++)
    {
        uint32 type = 0;
        type = TitleConfig::Instance()->GetTitleType(cutrTitle->titlid);
        if(type == ACTIVITY_TITLE)
        {
            if(cutrTitle->time >0)
            {
                TitleConfig::Instance()->AddTitleReward(cutrTitle->titlid, propSet);
            }
        }
        else
        {
            TitleConfig::Instance()->AddTitleReward(cutrTitle->titlid, propSet);
        }
    }

    //m_player->GetPropertySetCalculater()->CalcBattlePowerByTitle(propSet);
}

void TitleManager::JudgeVipTitle(bool upOrDown)
{
    int32 viplevel = 0;
    int32 vipid = 0;//称号是vip的话，取最大的vip属性，其他属性不加

    std::vector<int32> titlevec;
    titlevec.clear();

    std::sort(m_titleList.begin(), m_titleList.end(), SortTitleId); //从小到大排序
    TitleList::iterator cutrTitle;
    for (cutrTitle = m_titleList.begin(); cutrTitle != m_titleList.end(); cutrTitle++)
    {
        uint32 type = 0;
        type = TitleConfig::Instance()->GetTitleType(cutrTitle->titlid);
        GetDeleteVipLevel(upOrDown, type, cutrTitle->titlid, viplevel, vipid, titlevec);
    }

    if(titlevec.size() <= 0 || vipid <= 0)
        return;

    bool b_havetitle = false;
    if(titlevec.size() > 0)
    {
        int32 curtitlid = titlevec.at(0);
        TITLE_INFO* titleinfo = GetTitleInfo(curtitlid);
        if(titleinfo)
        {
            if(titleinfo->b_state == HAVE_TITLE)
            {
                b_havetitle = true;
            }
        }
    }

    bool b_exe = false;
    std::vector<int32>::iterator curtitleVec;
    for (curtitleVec = titlevec.begin(); curtitleVec != titlevec.end(); curtitleVec++)
    {
        DeleteTitle(*curtitleVec);
        b_exe = true;
    }

    if(b_havetitle)
        SetTitleState(vipid, HAVE_TITLE);

    m_player->CalcProperty();

    if(b_exe)
    NotifyOtherPlayer();
}

void TitleManager::DeleteTitle(int32 titleid)
{
    m_updateList.clear();

    TitleList::iterator cutrTitle;
    for (cutrTitle = m_titleList.begin(); cutrTitle != m_titleList.end();)
    {
        if(titleid == cutrTitle->titlid)
        {
            m_updateList.push_back(*cutrTitle);
            cutrTitle = m_titleList.erase(cutrTitle);
            m_player->SendPlaySystemLog(PLAYSYSTEM_TITLE_DEL, titleid);
        }
        else
        {
            cutrTitle++;
        }
    }

    SendInitTitleAck(OPT_DEL_FLAG);
}
//upOrDown为真得到最大vip的称号id,或upOrDown为false最小的id titlevec要删除的vip等级
void TitleManager::GetDeleteVipLevel(bool upOrDown, int32 type,int32 titleid, int32& viplevel,int32& vipid, std::vector<int32>& titlevec)
{
    if(type != ORDINARY_TITLE) return;

    int32 vipmodle= TitleConfig::Instance()->GetTitleModelType(titleid);
    if(vipmodle != VIP_TITLE) return;

    int32 conditonid = TitleConfig::Instance()->GetVipConditonReason(titleid);
    if(upOrDown)//升级
    {
        if(conditonid >= viplevel)
        {
            if(vipid != 0)
                titlevec.push_back(vipid);

            viplevel = conditonid;
            vipid = titleid;
        }
    }
    else
    {
        //降级
        if(viplevel == 0)
        {
            viplevel = conditonid;
            vipid = titleid;
        }
        else
        {
            if(conditonid > viplevel)
            {
                titlevec.push_back(titleid);
            }

        }
    }

}
void TitleManager::OnRankingHourRefresh()
{
    //向center发消息请求排行榜的刷新
    //初始化的时候也要请求刷新
    SendRankReqList();
}

void TitleManager::SendRankReqList()
{
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_HOROSCOPE, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_HOROSCOPE);
    //}

    OnAskRankReq(RANKING_TYPE_BATTLE_POWER);

    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_ASHURA, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_ASHURA);
    //}

    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_GUILD, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_GUILD);
    //}

    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_WING, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_WING);
    //}

    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_PET_INFO, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_FOLLOW_PET);
    //}

    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_RIDE_INFO, m_player, false) == ERR_SUCCEED)
    //{
    //    OnAskRankReq(RANKING_TYPE_RIDE_PET);
    //}
}

void TitleManager::OnAskRankReq(int32 ranktype)
{
    RANKING_RANK_REQ sendMsg;
    sendMsg.reason = RANKING_TITLE_RANK_INFO;
    sendMsg.rank_type = ranktype;

    uint64 playerid = m_player->GetPlayerID();
    BIGINT_ID Id;
    Id.id_h = U64ID_HIGH(playerid);
    Id.id_l = U64ID_LOW(playerid);
    sendMsg.playerList.push_back(Id);

    m_player->SendToCenter(MSG_RANKING_RANK_REQ, &sendMsg);
}
