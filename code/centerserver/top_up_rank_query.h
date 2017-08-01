/*
* ���в�ѯ������
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
    void CreateNewQuery(uint32 sessionID, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg); // ��Ӳ�ѯ
    bool IsQueryMatch(RANK_QUERY_CREATE_INFO req, TOP_UP_RANK_QUERY_CREATE_REQ *recvMsg);                // �ж���Ϣ�Ƿ�ƥ��

    void DoQueryFirst(RANK_QUERY_CREATE_INFO* req);                              // ��һ�ν����ѯ������Ϣ
    void SendDBRankList(RANK_QUERY_CREATE_INFO* req);                            // ��ѯ������Ϣ
    void DoRankList(RANK_QUERY_CREATE_INFO* req);                                // ��ѯ������Ϣ
    void QueryRankList(RANK_QUERY_CREATE_INFO* req);                                                       // ˢ�²�ѯ�б�
    void UpdateQueryTime();                                                       // ˢ�²�ѯ�б�

private:

    // new 
    typedef std::vector<RANK_QUERY_CREATE_INFO> QueryInfoList;
    QueryInfoList m_queryInfoList;                                // ��ѯ�б�
};

#endif