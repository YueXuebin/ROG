/*
 *	宝库管理
 */
#ifndef treasury_manager_h__
#define treasury_manager_h__

class Player;

class TreasuryManager
{
public:
    TreasuryManager(Player* player);
    ~TreasuryManager();

    void Clear();

    void LoadTreasuryInfo(TREASURY_INFO& info);                     // 加载宝库信息
    void FillTreasuryInfo(TREASURY_INFO& info);                     // 填充宝库信息

    void Update();                                  // 心跳

    void OnRecv(uint32 msgID, void* data);                          // 消息接收

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);   // 接收事件

    void CalDailyRefresh();

    int16 CanEnterTreasury();      // 是否能跳地图

    void EnterTreasury();                                           // 每日进入次数加一

    uint32 GetKillMonsterCount() {return m_killMonsterCout;}        // 杀死怪物数量

    void onTreasuryCD();                                            // 花完钱秒cd

private:
    void onTreasuryInfoReq();                                       // 请求活动信息
    void onTreasuryCDReq();                                         // 宝库秒CD
    void onTreasuryBuyCount();                                      // 宝库购买次数

    int32 GetMaxEnterCount();                                       // 得到可进入的最大次数

    void Notify();
    void SendGameNotify();                                          // 发送进入次数变化通知

    void AddKillCount();                                            // 添加击杀怪物

    void SendReward();                                              // 发送奖励给玩家
    void SaveYesterdayData();

private:
    bool m_Dirty;                       // 是否发送信息给客户端
    Player* m_player;

    uint32 m_enterCount;                // 进入次数
    uint32 m_enterCDTime;               // 进入冷却时间
    uint32 m_enterBuyCount;             // 已购买进入次数

    uint32 m_killMonsterCout;           // 杀死的怪物数量
    uint32 m_gamePoint;                 // 累积金钱数
};

#endif // treasury_manager_h__
