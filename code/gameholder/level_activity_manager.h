/*
*    等级活动配置
*/
#ifndef level_activity_manager_h__
#define level_activity_manager_h__

class LevelActivityManager
{
public:
    LevelActivityManager(Player* player);
    ~LevelActivityManager();

    void LoadLevelActivityInfo(LEVEL_ACTIVITY_INFO& levelActivityInfo);
    void FillLevelActivityInfo(LEVEL_ACTIVITY_INFO& levelActivityInfo);

    void OnRecv(uint32 msgID, void * data);

    void SaveLevelActivity(uint32 level);                           // 向gc请求立即存档等级投资信息
    void CBSaveLevelActivity(void* data);                         // gc存档等级投资信息回调

private:
    void UpdateFlag( SPECIAL_ACTIVITY_INFO_LIST& infoList, int32 level, int32 flag );

    void OnLevelActivityInfoReq(void *data);
    void OnLevelActivityAcceptRewardReq(void *data);

    void SetLevelAcceptFlag(uint32 level);
    bool IsLevelAccept(uint32 level);

    void SetIndexAcceptFlag(uint32 index);
    bool IsIndexAccept(uint32 index);

    void SendAcceptRewardAck(uint32 errCode, int32 index);
private:
    uint32 m_acceptFlag;        // 领奖标记
    uint32 m_activityId;

    Player* m_player;
};

#endif