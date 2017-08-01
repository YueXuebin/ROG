/*
* 排行查询操作器
*/
#ifndef __TOP_UP_RANK_QUERY__
#define __TOP_UP_RANK_QUERY__

#include "rank_query.h"

class TopUpRankQueryExecutor;

class TopUpRankQuery : public RankQuery
{
public:

    virtual void Update();

    virtual void DailyRefresh();
    virtual void GMDailyRefresh();

    TopUpRankQuery(RankListManager *rankMgr);

    virtual ~TopUpRankQuery();

    // new
    void CreateNewQuery(uint32 sessionID, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg); // 添加查询
    bool IsQueryMatch(RANK_QUERY_CREATE_INFO req, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg);                // 判断信息是否匹配

    void DoQueryFirst(RANK_QUERY_CREATE_INFO* req);                              // 第一次进入查询排行信息
    void SendDBRankList(RANK_QUERY_CREATE_INFO* req);                            // 查询排行信息
    void DoRankList(RANK_QUERY_CREATE_INFO* req);                                // 查询排行信息
    void QueryRankList(RANK_QUERY_CREATE_INFO* req);                                                       // 刷新查询列表
    void UpdateQueryTime();                                                       // 刷新查询列表

private:

    // new 
    typedef std::vector<RANK_QUERY_CREATE_INFO> QueryInfoList;
    QueryInfoList m_queryInfoList;                                // 查询列表
};

#endif