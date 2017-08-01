//充值积分抽奖配置文件
#ifndef __PAY_POINT_LOTTERY_CONFIG_H__
#define __PAY_POINT_LOTTERY_CONFIG_H__

class PayPointLotteryConfig
{
public:
    typedef std::vector<PAY_POINT_LOTTERY_ITEM> LOTTERY_ITEM_LIST;
    typedef std::vector<PAY_POINT_LOTTERY_PACKAGE> LOTTERY_PACKAGE_LIST;

    DECLARE_SINGLETON(PayPointLotteryConfig)

    int32 GetMaxGetCount() const { return mMaxGetCount; }

    uint32 GetLotteryCostItemTID() const { return mLotteryCostItemTid; }

    uint32 GetLotteryPackageTID(int32 vipLevel) const;

    int32 GetLotteryItemIdxByTID(uint32 tid) const;

    const PAY_POINT_LOTTERY_ITEM* GetLotteryItemByIdx(int32 idx) const;

    const PAY_POINT_LOTTERY_ITEM* GetLotteryItemByTID(uint32 tid) const;

    int32 GetExchangeItemIdxByTID(uint32 tid) const;

    const PAY_POINT_LOTTERY_ITEM* GetExchangeItemByIdx(int32 idx) const;

    const PAY_POINT_LOTTERY_ITEM* GetExchangeItemByTID(uint32 tid) const;

    bool isWorldRadioItem(uint32 tid) const;

    void Clear();

    bool LoadConfig(const char* path);

    PayPointLotteryConfig();

    virtual ~PayPointLotteryConfig();

private:
    uint32 mStartTimeStamp;
    uint32 mEndTimeStamp;
    int32 mExchangeRate;
    int32 mMaxGetCount;
    uint32 mLotteryCostItemTid;
    LOTTERY_ITEM_LIST mLotteryItemList;
    LOTTERY_ITEM_LIST mExchangeItemList;
    LOTTERY_PACKAGE_LIST mLotteryPackageList;
    std::vector<uint32> mLotteryWorldRadioItemTIDFilterList;
};

#endif