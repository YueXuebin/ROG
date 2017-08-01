#ifndef multiserver_battle_manager_h__
#define multiserver_battle_manager_h__


class Player;
/*
 *  跨服战斗匹配管理器
 */
class MultiserverBattleManager
{
public:
    DECLARE_SINGLETON(MultiserverBattleManager)

    typedef std::vector<MULTISERVER_BATTLE_APPLY_REQ> ApplyList;

    MultiserverBattleManager(void);
    ~MultiserverBattleManager(void);

    void OnRecvFromPlayer(uint32 msgID, void* data);
    void OnRecvFromCenter(uint32 msgID, void* data);

    void Update(uint32 frame_count);

    void OnSeasonNtf(void* data);

    uint32 GetSeasonTime() {return m_seasonInfo.season_time;}

    uint32 GetChannelIDByThreeApplyList(uint64 playerID);
    ApplyList& GetThreeApplyList() {return m_ThreeApplyList;}

    void ThreeApplyListErase(uint64 playerID);                                              // 去除3V3指定玩家

private:
    void OnMultiserverBattleApplyHandle(void* data);                                        // 跨服战斗报名申请
    void SendApplyAck(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType);  // 申请消息回应
    
    void OneApplyReq(MULTISERVER_BATTLE_APPLY_REQ* recvMsg);                                // 1V1匹配请求
    void ThreeApplyReq(MULTISERVER_BATTLE_APPLY_REQ* recvMsg);                              // 3V3匹配请求

    void OnMultiserverBattleQuitHandle(void* data);                                         // 跨服战斗退出申请
    void SendQuitAck(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType);   // 退出消息回应

    void SendApplyNtf(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType, uint32 channel_id, uint32 ackState); // 请求应答

    void SendWaitAck(uint32 ph, uint32 pl, uint32 type, uint32 gameID);

    bool JudgeMatchCount(uint32 mh, uint32 ml, uint32 ph, uint32 pl, uint32 maxCount);      // 判断匹配次数
    void SaveMatchCount(uint32 ph, uint32 pl, uint32 oh, uint32 ol);                        // 保存匹配次数
    void OnClearMatchCount();                                                               // 匹配次数重置

private:
    uint32 cTick;
    ApplyList m_OneApplyList;                                                               // 1V1战斗申请列表
    ApplyList m_ThreeApplyList;                                                             // 3V3战斗申请列表

    // 匹配数据
    typedef std::vector<MULTISERVER_MATCH_COUNT> MatchList;
    typedef std::map<uint64, MatchList>  MatchMap;
    MatchMap m_matchMap;                                                                    // 匹配列表

    int32 m_last_time;                                                                      // 匹配次数重置时间

    MULTISERVER_BATTLE_SEASON_NTF m_seasonInfo;                                             // 赛季信息
};

#endif 