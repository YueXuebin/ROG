//充值排行查询
#ifndef __TOP_UP_RANK_JOB__
#define __TOP_UP_RANK_JOB__

class DBExecutor;
class DBRecorder;

class TopUpRankJob
{
public:
    DECLARE_SINGLETON(TopUpRankJob)

    //设置SQL执行
    void SetDBExecutor(DBExecutor *val) { m_dbExecutor = val; }

    void QueryRechargeRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);

    void QueryWingRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryPetRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryRideRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryBattleRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryLevelRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryFashionRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);
    void QueryOfflinePlayerRank(TOP_UP_RANK_QUERY_CREATE_REQ &req, TOP_UP_RANK_INFO_ACK &ack);

    TopUpRankJob();

    ~TopUpRankJob();

private:
    TopUpRankJob(const TopUpRankJob&);
    TopUpRankJob& operator=(const TopUpRankJob&);
    TopUpRankJob* operator&();
    const TopUpRankJob* operator&() const;

    DBExecutor *m_dbExecutor;
};

#endif