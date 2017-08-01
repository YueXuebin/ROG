#ifndef DUNGEON_SCORE_MANAGER_H
#define DUNGEON_SCORE_MANAGER_H

class Player;

// ==================================================
/// 副本分数管理系统
// ==================================================
class DungeonScoreManager
{
public:
    DungeonScoreManager();
    ~DungeonScoreManager();

    DECLARE_SINGLETON(DungeonScoreManager)

    void Clear();

    bool Init();

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

private:
    void OnSaveDungeonInfo(uint64 playerID, void* data);            // 保存副本信息

    void OnDungeonScoreInfoReq(uint64 playerID, void* data);        // 请求副本分数信息

    void OnCrossTeamDungeonScoreInfoReq(uint32 sessionID, void* data);   // 请求跨服组队副本通关记录

    // 副本分数
    typedef std::map<uint32, DUNGEON_SCORE_INFO> DungeonScoreMap;   // <副本id, 分数信息>
    DungeonScoreMap m_dungeonScoreMap;
};

#endif
