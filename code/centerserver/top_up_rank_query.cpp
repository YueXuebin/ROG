#include "centerserver_pch.h"
#include "top_up_rank_query.h"
#include "ranking_list_manager.h"
#include "top_up_rank_job.h"
#include "game_handler.h"
#include "special_activity_job.h"

TopUpRankQuery::TopUpRankQuery(RankListManager *rankMgr) : RankQuery(rankMgr)
{

}

TopUpRankQuery::~TopUpRankQuery()
{
}

void TopUpRankQuery::Update()
{
    UpdateQueryTime();
}

void TopUpRankQuery::DailyRefresh()
{

    for(QueryInfoList::iterator it = m_queryInfoList.begin(); it != m_queryInfoList.end(); ++it)
    {
        QueryRankList(&(*it));
    }
}

void TopUpRankQuery::GMDailyRefresh()
{

    for(QueryInfoList::iterator it = m_queryInfoList.begin(); it != m_queryInfoList.end(); ++it)
    {
        //QueryRankList(&(*it));
        DoRankList(&(*it));
    }
}

void TopUpRankQuery::CreateNewQuery( uint32 sessionID, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg )
{

    bool isNew = true;
    for(QueryInfoList::iterator it = m_queryInfoList.begin(); it != m_queryInfoList.end(); ++it)
    {
        if(it->sessionID == sessionID && it->rank_req.festival_id == recvMsg->festival_id)
        {
            // 已有查询变更查询信息
            isNew = false;
            bool flag = IsQueryMatch((*it), recvMsg);
            if(!flag)
            {
                it->rank_req.begin_time = recvMsg->begin_time;
                it->rank_req.end_time = recvMsg->end_time;
                it->rank_req.total_recharge_line = recvMsg->total_recharge_line;
                it->rank_req.update_interval = recvMsg->update_interval;
                it->rank_req.loop = recvMsg->loop;
                DoQueryFirst(&(*it));
            }
           break;
        }
    }

    if(isNew)
    {
        // 没有查询新建查询信息
        RANK_QUERY_CREATE_INFO info;
        info.sessionID = sessionID;
        info.interval = 0;
        info.is_query = 0;
        info.rank_req.festival_id = recvMsg->festival_id;
        info.rank_req.rank_type = recvMsg->rank_type;
        info.rank_req.begin_time = recvMsg->begin_time;
        info.rank_req.end_time = recvMsg->end_time;
        info.rank_req.total_recharge_line = recvMsg->total_recharge_line;
        info.rank_req.update_interval = recvMsg->update_interval;
        info.rank_req.loop = recvMsg->loop;
        m_queryInfoList.push_back(info);
        DoQueryFirst(&info);
    }
}

bool TopUpRankQuery::IsQueryMatch( RANK_QUERY_CREATE_INFO req, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg )
{
    if (req.rank_req.begin_time != recvMsg->begin_time)
    {
        return false;
    }
    if (req.rank_req.end_time != recvMsg->end_time)
    {
        return false;
    }
    if (req.rank_req.total_recharge_line != recvMsg->total_recharge_line)
    {
        return false;
    }
    if (req.rank_req.update_interval != recvMsg->update_interval)
    {
        return false;
    }
    return true;
}

void TopUpRankQuery::DoQueryFirst( RANK_QUERY_CREATE_INFO* req )
{
    if((req->is_query == 1) && !(req->rank_req.loop == 1) )
    {
        if(req->rank_req.rank_type == 0)return;
        uint32 indexID = req->rank_req.festival_id * 1000 +  req->rank_req.rank_type;
        // 已经查询过 发送查询
        uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(req->sessionID);
        //GameHandler::Instance()->SendByID(gameSvrID, MSG_RANKING_TOP_UP_RANK_QUERY_ACK, &m_sendMsgInfo);
        GameHandler::Instance()->SendByID(regionID, MSG_RANKING_TOP_UP_RANK_QUERY_ACK, &req->rank_ack);
    }
    else
    {
        //新成员加入时查询一次
        uint32 curt = (uint32)Crown::SDNow().GetTimeValue();

        uint32 gameSvrID = 0;
        uint32 sessionID = 0;

        if(req->rank_req.loop == 1)
        {
            //可以循环查询
            if(curt > (uint32)req->rank_req.end_time)
            {
                SendDBRankList(req);
            }
            else
            {
                DoRankList(req);
            }
        }
        else
        {
            //一次触发查询
            if(curt > (uint32)req->rank_req.end_time)
            {
                SendDBRankList(req);
            }
        }
    }
}

void TopUpRankQuery::SendDBRankList( RANK_QUERY_CREATE_INFO* req )
{
    uint32 regionID = 0;
    uint32 sessionID = 0;

    TOP_UP_RANK_INFO_ACK sendMsg;
    uint32 indexID = req->rank_req.festival_id * 1000 +  req->rank_req.rank_type;
    SpecialActivityJob::Instance()->GetRankListInfo(indexID, sendMsg);

    if(req->rank_ack.rank_list.size() < 1)
    {
        DoRankList(req);
    }
    else
    {
        req->rank_ack = sendMsg;
        regionID = GameHandler::Instance()->GetRegionIDBySessionID(req->sessionID);
        GameHandler::Instance()->SendByID(regionID, MSG_RANKING_TOP_UP_RANK_QUERY_ACK, &sendMsg);

        req->is_query = 1;
    }
}

void TopUpRankQuery::DoRankList( RANK_QUERY_CREATE_INFO* req )
{
    TOP_UP_RANK_INFO_ACK sendMsg;

    TOP_UP_RANK_QUERY_CREATE_REQ queryMsg;
    queryMsg.begin_time = req->rank_req.begin_time;
    queryMsg.end_time = req->rank_req.end_time;
    queryMsg.total_recharge_line = req->rank_req.total_recharge_line;

    switch(req->rank_req.rank_type)
    {
    case TASK_TARGET_109:
        {
            TopUpRankJob::Instance()->QueryRechargeRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_112:
        {
            TopUpRankJob::Instance()->QueryWingRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_113:
        {
            TopUpRankJob::Instance()->QueryPetRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_114:
        {
            TopUpRankJob::Instance()->QueryRideRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_115:
        {
            TopUpRankJob::Instance()->QueryBattleRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_116:
        {
            TopUpRankJob::Instance()->QueryLevelRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_130:
        {
            // qxw
            TopUpRankJob::Instance()->QueryFashionRank(queryMsg, sendMsg);
        }
        break;
    case TASK_TARGET_124:
        {
            // qxw
            TopUpRankJob::Instance()->QueryOfflinePlayerRank(queryMsg, sendMsg);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    sendMsg.rank_type = req->rank_req.rank_type;
    sendMsg.festival_id = req->rank_req.festival_id;

    req->rank_ack = sendMsg;
    
   

    uint32 indexID = req->rank_req.festival_id * 1000 +  req->rank_req.rank_type;
    if(req->rank_req.rank_type == TASK_TARGET_109)
    {
        if((uint32)Crown::SDNow().GetTimeValue() > (uint32)req->rank_req.end_time)
        {
            SpecialActivityJob::Instance()->SaveRankListInfo(indexID, sendMsg);
        }
    }
    else
    {
        SpecialActivityJob::Instance()->SaveRankListInfo(indexID, sendMsg);
    }

    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(req->sessionID);
    uint32 errSend = GameHandler::Instance()->SendByID(regionID, MSG_RANKING_TOP_UP_RANK_QUERY_ACK, &sendMsg);

    if(!(req->rank_req.loop == 1))
        req->is_query = 1;
}

void TopUpRankQuery::QueryRankList(RANK_QUERY_CREATE_INFO* req)
{
    // 已经查询过了
    if(req->is_query == 1)
        return;

    uint32 curt = (uint32)Crown::SDNow().GetTimeValue();
    if(req->rank_req.loop == 1)
    {
        //可以循环查询
        if(curt > (uint32)req->rank_req.end_time)
        {
            req->rank_req.loop = 0;
            DoRankList(req);
        }
        else
        {
            DoRankList(req);
        }
    } 
    else
    {
        //一次触发查询
        if(curt > (uint32)req->rank_req.end_time)
        {
            DoRankList(req);
        }
    }

}

void TopUpRankQuery::UpdateQueryTime()
{
    for(QueryInfoList::iterator it = m_queryInfoList.begin(); it != m_queryInfoList.end(); ++it)
    {
        it->interval += g_ServerUpdateTime;

        if((uint32)Crown::SDNow().GetTimeValue() > (uint32)it->rank_req.end_time)
        {
            it->interval = 0;
            QueryRankList(&(*it));
        }

        if (it->interval > it->rank_req.update_interval)
        {
            it->interval = 0;
            QueryRankList(&(*it));
        }
    }
}


