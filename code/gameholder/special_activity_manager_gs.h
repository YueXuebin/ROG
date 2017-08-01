/*
*    特殊活动配置
*/
#ifndef special_activity_manager_gs_h__
#define special_activity_manager_gs_h__

class SpecialActivityManager
{
public:
    SpecialActivityManager(Player* player);
    ~SpecialActivityManager();

    void OnRecv(uint32 msgID, void * data);

private:
    void SetAcceptedReward(int32& status, int32 index);
    bool IsAcceptedReward(int32 status, int32 index);

    void OnSpecialActivityInfoReq(uint32 msgID, void *data);

    void OnSpecialActivityAcceptRewardReq(uint32 msgID, void *data);
    void OnSpecialActivityAcceptRewardAck(uint32 msgID, void *data);

    void SendSpecialActivitySaveInfoReq(int32 activityId, int32 status, int32 param);
    void OnSpecialActivitySaveInfoAck(uint32 msgID, void *data);

    void OnSpecialActivityLoadInfoReq(uint32 msgID, void *data);
    void OnSpecialActivityLoadInfoAck(uint32 msgID, void *data);

    void SendSpecialActivityInfoToPlayer(int32 activityId, int32 param, int32 status);
    void SendSpecialActivityRewardToPlayer(int32 errCode, int32 activityId, int32 index, int32 param);

    void OnFirstTimeRechargeInfoReq(void *data);
    void OnFirstTimeRechargeAcceptRewardReq(void *data);
    void OnFirstTimeRechargeAcceptRewardAck(void *data);
    void SendFirstTimeRechargeAcceptRewardAck(uint32 errCode);

    void OnUpgradeWingActivityAcceptRewardReq(SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req);
    void OnMicroClientActivityAcceptRewardReq(SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ* req);

private:
    Player* m_player;

    int32 m_upgrade_wing_status;
    int32 m_micro_client_status;
};

#endif