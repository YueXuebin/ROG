
#ifndef buy_cheap_manager_h__
#define buy_cheap_manager_h__

class Player;

class BuyCheapMgr
{
public:
    BuyCheapMgr(Player* player);
    ~BuyCheapMgr();

    void LoadInfo(const BUY_CHEAP_INFO& info);
    void FillInfo(BUY_CHEAP_INFO& info);

    void OnRecv(uint32 msgID, void* data);

    void CalDailyRefresh();                                 //< Ã¿ÈÕË¢ÐÂ

    void onBuyCheapInfoReq();
    void onBuyCheapRewardReq();

    void onBuyCheapBuyReq(void* data);
    void CBBuyCheapBuy(const std::string& str);
private:
    BUY_CHEAP_STATE* GetCheapState(int32 index);
    int32 GetDiscount(const BUY_CHEAPS_ITEM* buyCheapsTmpl, int32 index);

    void sendBuyCheapStateAck(int32 err_code, int buy_index);
private:
    Player* m_player;

    BUY_CHEAP_INFO m_buyCheapInfo;
};


#endif // buy_cheap_manager_h__