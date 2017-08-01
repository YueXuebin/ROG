/*
 *	活动排行榜管理
 */
#ifndef __TOP_UP_RANK_MANAGER_H__
#define __TOP_UP_RANK_MANAGER_H__

class Player;

class TopUpRankManager
{
public:
    DECLARE_SINGLETON(TopUpRankManager)

    TopUpRankManager();

    ~TopUpRankManager();

    void Init(uint32 centerID);                                                         // 初始排行活动信息

    uint32 GetTopUpRank(uint64 playerID, uint32 type);                                  // 获取排行

    void OnTopUpRankQueryAck(uint64 playerID, uint32 msgID, void* data);                // 排行信息处理

    void SendTopUpRankInfoAck(Player *player, void* data);                              // 发送排行信息

    bool IsInReceiveRewardTime(uint32 time, uint32 id);                                 // 活动是否开启

    void GmActivityRankReq(uint32 type);                                                           // GM更新活动排行请求

private:
    void UpdateTask(uint32 indexID);

    void SendRankQueryInfo(const TASK_FESTIVAL_TEMPLATE *festivalTmpl, uint32 type);

private:

    typedef std::map<uint32, TOP_UP_RANK_INFO_ACK> TopUpRankAckMap;
    TopUpRankAckMap m_topUpRankAck;                                                     // 排行信息列表  center server返回

    typedef std::map<uint64, uint32> TopUpRankRegister;
    TopUpRankRegister m_topUpRankRegister;                                              // 排行信息类表

    typedef std::map<uint32, TopUpRankRegister> RankListRegister;
    RankListRegister m_rankListRegister;                                                // 排行信息

    uint32 m_centerID;                                                                  // center ID
    uint32 m_rechargeMaxCount;                                                          // 充值排行计数
};

#endif