#ifndef fcm_manager_h__
#define fcm_manager_h__

class Player;

// ==================================================
/// 防沉迷管理器
// ==================================================
class FCMManager
{
public:
    FCMManager(Player* player);
    ~FCMManager();

    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const FCM_INFO& info);
    void FillInfo(FCM_INFO& info);

    FCM_INFO& GetFcmInfo() { return m_fcm_info; }

    void OnFcmInfoReq(void* data);

    void CalFcmGainRate();                                          ///< 计算防沉迷奖励率

    int32 GetFcmGainValue(int32 val);

    void GetFcmRewardList(const std::vector<REWARD_ITEM>& 
        inList, std::vector<REWARD_ITEM>& outList);                                 ///< 获得长沉迷后的奖励列表

private:
    Player*             m_player;
    FCM_INFO            m_fcm_info;
};


#endif // fcm_manager_h__
