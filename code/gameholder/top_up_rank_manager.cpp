#include "gameholder_pch.h"
#include "top_up_rank_manager.h"
#include "player.h"
#include "center_handler.h"
#include "task_festive_config.h"
#include "player_manager.h"
#include "task_event_define.h"

IMPLEMENT_SINGLETON(TopUpRankManager)

TopUpRankManager::TopUpRankManager()
{
    m_rechargeMaxCount = 0;
}

TopUpRankManager::~TopUpRankManager()
{
}

void TopUpRankManager::Init(uint32 centerID)
{
    m_centerID = centerID;

    const TaskFestiveMap* dbFestivalMap = TaskFestiveConfig::Instance()->GetFestivalMap(centerID);
    if(!dbFestivalMap)
    {
        return;
    }
    // 遍历支线任务（这里的前提是map中的first是任务编号，而任务的顺序一定是由小到大的）
    for(TaskFestiveMap::const_iterator activityIt = dbFestivalMap->begin(); activityIt != dbFestivalMap->end(); ++activityIt)
    {
        for(std::vector<TASK_TEMPLATE>::const_iterator taskIt = activityIt->second.tasks.begin(); taskIt != activityIt->second.tasks.end(); ++taskIt)
        {
            for (std::vector<TASK_TARGET>::const_iterator targetIte = taskIt->targets.begin(); targetIte != taskIt->targets.end(); ++targetIte)
            {
                switch(targetIte->type)
                {
                case TASK_TARGET_109:
                case TASK_TARGET_112:
                case TASK_TARGET_113:
                case TASK_TARGET_114:
                case TASK_TARGET_115:
                case TASK_TARGET_116:
                case TASK_TARGET_130:
                case TASK_TARGET_124:
                    {
                        SendRankQueryInfo(&activityIt->second, targetIte->type);
                        break;
                    }
                    break;
                }
            }
        }
    }
}

void TopUpRankManager::SendRankQueryInfo( const TASK_FESTIVAL_TEMPLATE *festivalTmpl, uint32 type)
{
    uint32 topnCount = 0;
    //统计最大的topn排名
    std::vector<TASK_TEMPLATE>::const_iterator taskIte, taskIteEnd = festivalTmpl->tasks.end();
    for (taskIte = festivalTmpl->tasks.begin(); taskIte != taskIteEnd; ++taskIte)
    {
        std::vector<TASK_TARGET>::const_iterator targetIte, targetIteEnd = (*taskIte).targets.end();
        for (targetIte = (*taskIte).targets.begin(); targetIte != targetIteEnd; ++targetIte)
        {
            const TASK_TARGET &target = (*targetIte);
            if (target.type == type)
            {
                topnCount = Crown::SDMax(topnCount, target.num);
            }
        }
    }

    uint32 beginTime = festivalTmpl->begin_time;
    uint32 triggerTime = festivalTmpl->trigger_time;
    uint32 queryTotalRechargeLine = 0;
    if(type == TASK_TARGET_109)
    {
        queryTotalRechargeLine = TOP_UP_RANK_QUERY_TOTAL_RECHARGE_LINE;
        m_rechargeMaxCount = topnCount;
    }
    else
    {
        queryTotalRechargeLine = topnCount;
    }
    //uint32 queryUpdateInterval = TOP_UP_RANK_QUERY_UPDATE_INTERVAL;
    uint32 queryUpdateInterval = 60000;

    TOP_UP_RANK_QUERY_CREATE_REQ req;
    req.festival_id = festivalTmpl->festival_id;
    req.rank_type = type;
    req.begin_time = beginTime;
    req.end_time = triggerTime;
    req.total_recharge_line = queryTotalRechargeLine;
    req.update_interval = queryUpdateInterval;
    if(type == TASK_TARGET_109)
    {
        req.loop = 1;
    }
    else
    {
        req.loop = 0;
    }
    CenterHandler::Instance()->SendToMasterCenter(MSG_RANKING_TOP_UP_RANK_QUERY_CREATE_REQ, &req);
}


uint32 TopUpRankManager::GetTopUpRank(uint64 playerID, uint32 type)
{
    RankListRegister::iterator regIte = m_rankListRegister.find(type);
    if (regIte == m_rankListRegister.end())
    {
        return 0;
    }
    else
    {
        TopUpRankRegister topUpRankRegister = regIte->second;
        TopUpRankRegister::iterator i = topUpRankRegister.find(playerID);
        if (i != topUpRankRegister.end())
        {
            return i->second;
        }
    }
    return 0;
}

void TopUpRankManager::OnTopUpRankQueryAck(uint64 playerID, uint32 msgID, void* data)
{
    TOP_UP_RANK_INFO_ACK* topUpRankInfoAck = (TOP_UP_RANK_INFO_ACK*)data;

    TopUpRankRegister topUpRankRegister;
    uint64 guid = 0;
    for (uint32 i = 0; i < topUpRankInfoAck->rank_list.size(); ++i)
    {
        const TOP_UP_RANK_INFO &info = topUpRankInfoAck->rank_list[i];
        guid = U64ID(info.player_id.id_h, info.player_id.id_l);
        topUpRankRegister[guid] = i + 1;
    }

     //m_rankListRegister[topUpRankInfoAck->rank_type] = topUpRankRegister;
    uint32 indexId = topUpRankInfoAck->festival_id * 1000 + topUpRankInfoAck->rank_type;
    m_rankListRegister[indexId] = topUpRankRegister;
    // 保存排行列表
    //m_topUpRankAck[topUpRankInfoAck->rank_type] = *(topUpRankInfoAck);
     m_topUpRankAck[indexId] = *(topUpRankInfoAck);
    //UpdateTask(topUpRankInfoAck->rank_type);
     UpdateTask(indexId);
}

void TopUpRankManager::SendTopUpRankInfoAck(Player *player, void* data)
{
    SERVER_TIME* msg = (SERVER_TIME*)data;
    TopUpRankAckMap::iterator regIte = m_topUpRankAck.find(msg->time);
    if (regIte == m_topUpRankAck.end())
    {
        return;
    }
    else
    {
        player->SendToGate(MSG_TOP_UP_RANK_INFO_ACK, &regIte->second);
    }
}

bool TopUpRankManager::IsInReceiveRewardTime(uint32 time, uint32 id)
{
    const TASK_FESTIVAL_TEMPLATE *festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_centerID, id);
    if(!festivalTmpl)
    {
        // 没有充值排行活动
        return false;
    }

    return time >= festivalTmpl->begin_time && time <= festivalTmpl->end_time;
}

void TopUpRankManager::UpdateTask(uint32 indexID)
{
    TopUpRankRegister topUpRankRegister;

    RankListRegister::iterator regIte = m_rankListRegister.find(indexID);
    if (regIte == m_rankListRegister.end())
    {
        return;
    }
    else
    {
        topUpRankRegister = regIte->second;
    }
    uint32 festival_id = indexID / 1000;
    uint32 type = indexID % (festival_id * 1000);

    TopUpRankRegister::iterator i, iEnd = topUpRankRegister.end();
    for (i = topUpRankRegister.begin(); i != iEnd; ++i)
    {
        Player *player = PlayerManager::Instance()->GetPlayerByPlayerID(i->first);

        if (!player)
        {
            continue;
        }
        
        switch(type)
        {
        case TASK_TARGET_109:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_ENTER, festival_id, 0);

                if (i->second <= m_rechargeMaxCount)
                {
                    player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_TOPN, festival_id, 0);
                }
            }
            break;
        case TASK_TARGET_112:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_WING, festival_id, 0);
            }
            break;
        case TASK_TARGET_113:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_PET, festival_id, 0);
            } 
            break;
        case TASK_TARGET_114:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_RIDE, festival_id, 0);
            }
            break;
        case TASK_TARGET_115:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_BATTLE, festival_id, 0);
            }
            break;
        case TASK_TARGET_116:
            {
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_LEVEL, festival_id, 0);
            }
            break;
        case TASK_TARGET_130:
            {
                // qxw
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_FASHION, festival_id, 0);
            }
            break;
        case TASK_TARGET_124:
            {
                // qxw
                player->SendTaskEvent(TASK_EVENT_TOP_UP_RANK_OFFLINE_PLAYER, festival_id, 0);
            }
            break;
        default:
            CnAssert(false);
            break;
        }
    }
}

void TopUpRankManager::GmActivityRankReq(uint32 type)
{
    TOP_UP_RANK_QUERY_CREATE_REQ req;
    req.rank_type = type;
    CenterHandler::Instance()->SendToMasterCenter(MSG_RANKING_TOP_UP_RANK_QUERY_CREATE_REQ, &req);
}


 