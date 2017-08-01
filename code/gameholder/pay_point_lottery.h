//充值积分抽奖
#ifndef __PAY_POINT_LOTTERY_H__
#define __PAY_POINT_LOTTERY_H__

class Player;

class PayPointLotteryManager
{
public:
    void ActiveLotteryCostItemExchange();

    void LoadPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info);

    void FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info);

    void OnRecv(uint32 msgID, void* data);

    void CalDailyRefresh();             // 每日刷新

    void Clear();

    PayPointLotteryManager(Player *player);

    ~PayPointLotteryManager();

private:
    typedef std::vector<PAY_POINT_LOTTERY_GET_RECORD> PAY_POINT_LOTTERY_GET_LIST;
    typedef std::vector<int> LOTTERY_OUTPUT_ITEM_IDX_LIST;

    void OnMsgPayPointInfoReq(uint32 msgID, void *data);

    void OnMsgGetPayPointLotteryCostItemReq(uint32 msgID, void *data);

    void OnMsgPayPointLotteryOnceStartReq(uint32 msgID, void *data);

    void OnMsgPayPointLotteryAllStartReq(uint32 msgID, void *data);

    void OnMsgGetPayPointLotteryOutputReq(uint32 msgID, void *data);

    void OnMsgPayPointLotteryExchangeItemReq(uint32 msgID, void *data);

    void PayPointLotteryStart(int count);

    void PushPayPointLotteryInfoAck(unsigned short err);

    void AddLotteryOutputItem();

    bool IsEnableTime(uint32 time) const;

    Player *m_player;
    PAY_POINT_LOTTERY_INFO m_payPointLotteryInfo;
    PAY_POINT_LOTTERY_GET_LIST m_worldGetRecord;
    PAY_POINT_LOTTERY_GET_LIST m_localGetRecord;
    LOTTERY_OUTPUT_ITEM_IDX_LIST m_lotteryOutputItemIdxList;
};

#endif