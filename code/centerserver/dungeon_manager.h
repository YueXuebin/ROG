/*
*	副本管理器
*/
#ifndef dungeon_manager_h__
#define dungeon_manager_h__

class Player;

// 副本模板
class DungeonTmpl
{
public:
    void Clear()
    {
        m_dungeon_id    = 0;
        m_dungeon_type  = 0;
    }

    uint32  m_dungeon_id;
    int32   m_dungeon_type;
};

// 副本数据
class DungeonInfo
{
public:
    void Clear()
    {
        m_cent_game_id    = 0;
        m_gamesvr_type  = 0;
        m_dungeon_id    = 0;
        m_max_channel   = 0;
        m_max_player    = 0;
        m_player_count  = 0;
    }

    uint32  m_cent_game_id;
    uint32  m_gamesvr_type;
    uint32  m_dungeon_id;
    int32   m_max_channel;
    int32   m_max_player;
    int32   m_player_count;     // 玩家数
};


class DungeonManager
{
public:
    DungeonManager();
    ~DungeonManager();

    DECLARE_SINGLETON(DungeonManager)

    typedef std::map<uint32, DungeonTmpl> DungeonTmplMap;           // <副本id, 模板>
    typedef std::map<uint32, DungeonInfo> DungeonInfoMap;           // <副本id, 数据>

    bool    Init();

    bool    LoadDungeonConfig();

    void    RecvServerDungeonListNtf(uint32 sessionID, void* data);

    uint32  RequestJumpDungeonId(uint32 dungeonID, uint32 centGameID, Player* player); // 请求副本是否可以跳转

    uint32  GetServerIdByUnionId(uint32 unionID);

    uint32  GetServerIdByDungeonId(uint32 dungeonID);

    uint32  GetDungeonType(uint32 dungeonId);       // 根据副本Id获得副本类型

    bool    HasDungeonId(uint32 dungeonID);

    bool    HasDungeonIdOnGame(uint32 dungeonID, uint32 centGameID, bool justCrossGame);

    bool    HasUnionId(uint32 unionId);             // 某频道的副本是否存在

    void    IncreasePlayer(uint32 dungeonId, Player* player);
    void    DecreasePlayer(uint32 dungeonId, Player* player);

    uint32  GetPlayerNum();

    void    OnCloseGameServer(uint32 centGameID);

    const std::vector<uint32>* GetGameListByDungeon(uint32 dungeonID, bool isCross);         // 查找副本所在的Game列表

private:
    DungeonInfo* GetDungeonInfoWithServerID(uint32 dungeonID, uint32 centGameID, bool justCrossGame);

private:
    DungeonTmplMap              m_dungeonTmplMap;       // 副本模板

    std::map<uint32, DungeonInfoMap>        m_dungeonInfoMap;       // 非跨服GS副本数据 <CenterID+GameID, DungeonInfoMap>
    std::map<uint32, DungeonInfoMap>        m_dungeonInfoMapCross;  // 跨服GS的副本数据 <CenterID+GameID, DungeonInfoMap>

    typedef std::map<uint32, std::vector<uint32> > DungeonInGameMap;
    DungeonInGameMap   m_dungeonInGameMap;      // 各副本所在的Game列表 <dungeonID, list<CenterID+GameID>>
    DungeonInGameMap   m_dungeonInGameMapCross; // 各副本所在的跨服Game列表

    std::map<Player*, Player*>  m_CheckPlayers;
};


#endif // dungeon_manager_h__
