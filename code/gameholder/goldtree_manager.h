/*
 *	金像管理
 */
#ifndef goldtree_manager_h__
#define goldtree_manager_h__

class Player;

class GoldtreeManager
{
public:
    GoldtreeManager(Player* player);
    ~GoldtreeManager();

    void Clear();

    void LoadGoldtreeInfo(GOLDTREE_INFO& info);                     // 加载金像信息
    void FillGoldtreeInfo(GOLDTREE_INFO& info);                     // 填充金像信息

    void OnRecv(uint32 msgID, void* data);                          // 消息接收

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2);   // 接收事件

    void CalDailyRefresh();

    int16 CanEnterGoldtree();       // 是否能跳地图

    void OnGoldtreeCD();
private:
    void onGoldtreeInfoReq();                                       // 请求活动信息
    void onGoldtreeCDReq();                                         // 请求秒CD
    void onGoldtreeBuyReq();                                        // 请求购买次数

    int32 GetMaxEnterCount();                                       // 获得最大进入次数

    void SaveYesterdayData();                                       // 保存昨天进入次数

    void EnterGoldtree();                                           // 每日进入次数加一
    void Notify();
    void SendGameNotify();                                          // 发送进入次数变化通知
    void SendReward();                                              // 发送奖励给玩家

    void SubBossHP(int32 bossHP, int32 time);                       // boss血量减少10%
private:
    bool    m_Dirty;                    // 是否发送信息给客户端
    Player* m_player;

    uint32  m_enterCount;               // 进入次数
    uint32  m_enterCDTime;              // 进入冷却时间
    uint32  m_enterBuyCount;            // 已购买次数

    uint32  m_bossHP;                   // Boss血量 万分比
    uint32  m_gamePoint;                // 累积金钱数
};

#endif // goldtree_manager_h__
