//砸金蛋活动
#ifndef __EGG_MANAGER_H__
#define __EGG_MANAGER_H__

class Player;

class EggManager
{
public:
    EggManager(Player *player);
    ~EggManager(void); 

    void LoadEggInfo(EGG_ACTIVITY_INFO &info);

    void FillEggInfo(EGG_ACTIVITY_INFO &info);

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );//接受事件

    void GmAddEgg( uint32 type, uint32 num );

private:

    void OnEggInfReq(uint32 msgID, void *data);             // 金蛋信息请求

    void OnSmashEggInfReq(uint32 msgID, void *data);        // 砸金蛋信息

    void SendEggListNotify();                               // 砸蛋信息通知
    
    void InitConfig();                                      // 初始砸蛋信息

    void AddEggNum( uint32 type, uint32 num);               // 添加蛋的数量
    void ReduceEggNum(uint32 type, uint32 num);             // 减少蛋的数量
    EGG_INFO* GetEggInfo(uint32 type);                      // 获取蛋的信息
    void OnOneKeyEggHandle(uint32 type);                    // 一键砸蛋

    void SendSmashEggNotify(uint32 err);                    // 一键砸蛋错误

    void RandomRewardHandle(int type, int goodf, std::vector<REWARD_ITEM>* itemList); // 随机物品

    void EggMarquee(int32 type, int32 id, int32 num);

    bool ResetTaskFestival( uint32 centerID, uint32 festivalID );                     // 重置任务

    Player *m_player;

    EGG_ACTIVITY_INFO m_eggListInfo;
};
#endif