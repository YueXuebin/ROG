/*
    圣光生存战管理器
 */
#ifndef city_survival_manager_h__
#define city_survival_manager_h__

class Player;

typedef std::map<uint64, uint32> SurvivalPlayerMap;
class CitySurvivalManager
{
public:
    CitySurvivalManager();
    ~CitySurvivalManager();

    DECLARE_SINGLETON(CitySurvivalManager)

    void Clear();

    void Update();

    void OnRecv(uint32 msgID, void* data, Player* player=0);

    void OnCSRecv(uint64 playerID, uint32 msgID, void* data);

    // 玩家进入
    void PlayerEnter(Player* player);

    // 玩家离开
    void PlayerLeave(Player* player);

    bool IsOpen() { return m_open; }

    // 毫秒
    uint32 GetCurrTime() { return m_cur_time; }

    void SendEnterAck(Player* player, uint16 errCode);

private:
    // ts通知开启
    void OnStart(void* data);

    // ts通知结束
    void OnEnd(void* data);

    // 真正开启
    void OnReallyStart();

    // 真正结束
    void OnReallyEnd();

    // 收到信息广播
    void OnInfoNtf(void* data);

    void OnInfoReq(void* data, Player* player);

    void OnEnterReq(void* data, Player* player);

    bool IsActivityServer();

    uint16 CanEnter(Player* player);

    // 广播圣光生存战信息到其他gs
    void SendCitySurvivalInfoNtf();

private:
    // 是否开启中
    bool m_open;

    // 是否活动所在服务器
    bool m_activity_server;

    // 当前时间
    uint32 m_cur_time;

    // 参加活动的玩家列表
    SurvivalPlayerMap m_player_map;

    // 信息变更,是否广播标记位
    bool m_city_survival_info_modify;
};


#endif
// city_survival_manager_h__
