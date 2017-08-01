#ifndef liveness_manager_h__
#define liveness_manager_h__


class Player;

typedef std::map<int32, LIVENESS_TARGET_INFO> LivenessTargetStatus;
typedef std::map<int32, int32> LivenessRewardStatus;

class LivenessManager
{
public:

    LivenessManager(Player* player);
    ~LivenessManager();

    void Clear();
    
    void LoadLivenessInfo(const LIVENESS_INFO& liveness_info);
    void FillLivenessInfo(LIVENESS_INFO& liveness_info);

    void CalDailyRefresh();     // 日常刷新

    void OnRecv(uint32 msgID, void * data);

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3); // 接收事件
  
private:
    void Update();
    void SendLivenessInfoAck();
    void SendLivenessAcceptRewardAck(int32 errCode, int32 index);

    void OnLivenessInfoReq(void *data);
    void OnLivenessAcceptRewardReq(void *data);

    void OnUpdateTarget(int32 target, int32 num);

private:
    Player* m_player;
    int32 m_cur_liveness;
    LivenessTargetStatus m_liveness_target_map;
    LivenessRewardStatus m_liveness_reward_map;

};

#endif