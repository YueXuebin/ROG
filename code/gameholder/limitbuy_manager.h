/*
 *	玩家的限购商城
 */
#ifndef limitbuy_manager_h__
#define limitbuy_manager_h__

class Player;
class LimitBuy_Manager
{
public:
    LimitBuy_Manager(Player* player);
    virtual ~LimitBuy_Manager(void);

    typedef std::vector<LIMIT_BUY>  LimitBuyList;

    void Clear();

    void LoadLimitBuyInfo(const LIMIT_BUY_LIST& limitbuy_info);
    void FillLimitBuyInfo(LIMIT_BUY_LIST& limitbuy_info);

    void OnRecv(uint32 msgID, void * data);

    void CalDailyRefresh();
    bool JudgeLimitNum(uint32 productid, int32 productnum,int32 type);                          // 判断限制次数
    bool JudgeLimitNumToNum(uint32 productid, int32 productnum,int32 type, int32 maxnum);       // 根据参数判断限制次数

private:
    bool FindProduct(uint32 id ,int32 type);
    int32 FindProductNum(uint32 id ,int32 type);

    bool AddLimitProduct(LIMIT_BUY& buylimt,int32 type);
    bool BMaxLimitNum(uint32 productid, int32 productnum,int32 type);
    void AddLimitProductNum(uint32 productid, int32 productnum,int32 type);

    void SendLimitList(LIMIT_BUY_INFO_ACK& sendmsg);

private:
    Player* m_player;

    LIMIT_BUY_LIST m_buylimtList;
    
};

#endif // limitbuy_manager_h__