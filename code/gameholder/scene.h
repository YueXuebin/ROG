/*
*	场景对象
*/
#ifndef scene_h__
#define scene_h__

class BattleUnit;
class Creature;
class Player;
class Monster;
class Boss;
class Npc;
class Pet;
class OfflinePlayer;
class Trigger;
class Checkpoint;
class Spawn;
class CoWorld;
class GridWorld;
class NavMesh;
class Regular;
class Dungeon;
class SceneNotifier;
class SceneObject;
class EventDispatcher;
class BattleUnitConnecter;
class Touch;
class DropObject;
class AOIManager;
class b2World;


class Teleport
{
public:
    void Clear()
    {
        m_sourceTriggerID = 0;
        m_targetSceneID = 0;
        m_targetTriggerID = 0;
    }

    enum
    {
        kEXIT_SCENE_ID   = 0xFF,     // 退出副本ID
    };

    uint32  m_sourceTriggerID;
    uint32  m_targetSceneID;
    uint32  m_targetTriggerID;
};

// 离A单位距离
class BattleUnitDistance
{
public:
    void Clear()
    {
        m_battleUnit = NULL;
        m_distance = 0;
    }

    BattleUnit*  m_battleUnit;          // B单位
    real    m_distance;                 // 距离
};
typedef std::vector<BattleUnitDistance> BUDistanceList; // 离A单位置的距离列表
 

class Scene : public b2ContactListener
{
public:
    Scene(Dungeon* pParent, uint32 sceneId, const std::list<Teleport>& triggers, 
            CoWorld* pCoWorld, b2World* pB2World, const std::string& designFileName, bool bClose);
    virtual ~Scene();

    typedef std::map<uint32, BattleUnit*>  BattleUnitMap;
    typedef std::map<uint32, Player*> PlayerMap;
    typedef std::map<uint32, Monster*>  MonsterMap;
    typedef std::map<uint32, Boss*> BossMap;
    typedef std::map<uint32, Pet*>  PetMap;
    typedef std::map<uint32, OfflinePlayer*>    OfflinePlayerMap;
    typedef std::map<uint32, Npc*> NpcMap;
    
    typedef std::map<uint32, SceneObject*>  SceneObjectMap;
    typedef std::map<uint32, Trigger*> TriggerMap;
    typedef std::map<uint32, Checkpoint*> CheckPointMap;
    typedef std::map<uint32, Spawn*> SpawnMap;
    typedef std::map<uint32, Touch*> TouchMap;
    typedef std::map<uint32, DropObject*> DropObjectMap;

    typedef std::list<Teleport> TeleportList;

    void Update(uint32 frame_count);

    uint32 GetSceneID() { return m_scene_id; }
    uint32 GetDungeonID() {return m_dungeon_id;}
    uint32 GetChannelID() {return m_channel_id;}
    bool GetCanClose() { return m_bClose; }
    Dungeon* GetDungeon() {return m_pDungeon;}
    uint32 GetUnionID();

    BattleUnitConnecter*   GetBattleUnitConnecter();

    b2World* GetB2World() {return m_pB2World;}
    CoWorld* GetCoWorld() {return m_pCoWorld;}
    GridWorld* GetGridWorld() {return m_pGridWorld;}
    Regular* GetRegular() {return m_pRegular;}
    EventDispatcher*    GetEventDispatcher() {return m_EventDispatcher;}

    void SendEvent(uint32 receiverId, uint32 eventId, int32 arg1, int32 arg2, int32 arg3, uint32 delayTime = 0); // (接受者, 事件id, ...)
    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void AddPlayer(Player* player);                         // 加入player对象
    void DelPlayer(uint32 id);

    void AddMonster(Monster* monster);                      // 加入monster对象
    void DelMonster(uint32 id);

    void AddBoss(Boss* boss);                               // 加入boss对象
    void DelBoss(uint32 id);

    void AddPet(Pet* pet);                      // 加入战斗宠物
    void DelPet(uint32 id);

    void AddOfflinePlayer(OfflinePlayer* pOfflinePlayer);   // 加入离线玩家
    void DelOfflinePlayer(uint32 id);

    void AddTrigger(Trigger* trigger);
    void DelTrigger(uint32 id);

    void AddCheckpoint(Checkpoint* checkpoint);
    void DelCheckpoint(uint32 id);

    void AddSpawn(Spawn* spawn);
    void DelSpawn(uint32 id);

    void AddTouch(Touch* patch);                            // 加入采集点
    void DelTouch(uint32 id);

    void AddDropItem(DropObject* dropItem);                 //加入掉落物品
    void DelDropItem(uint32 id);

    void SetCanUseAI(bool canUse) {m_CanUseAI = canUse;}
    bool IsCanUseAI();

    uint32      GetSceneTime() {return m_SceneTime;}

    SceneNotifier* GetNotifier() { return m_pNotifier; }
    bool        IsAOINotifier() {return m_useAOI;}
    bool        IsPlayerSee(SceneObject* pSceneObject);
    bool        IsPlayerCanSee(Player* player, SceneObject* pSceneObject);

    // 查找对象
    BattleUnitMap& GetBattleUnits() {return m_BattleUnits;}
    PlayerMap& GetPlayers() {return m_Players;}
    MonsterMap& GetMonsters() {return m_Monsters;}
    BossMap& GetBosses() {return m_Bosses;}
    NpcMap& GetNpcs() {return m_Npcs;}
    PetMap& GetPets() {return m_Pets;}
    OfflinePlayerMap&   GetOfflinePlayer() {return m_OfflinePlayer;}

    SceneObjectMap& GetObjects() {return m_SceneObjects;}
    TriggerMap& GetTriggers() { return m_Triggers; }
    CheckPointMap& GetCheckpoints() { return m_Checkpoints; }
    SpawnMap& GetSpawn() { return m_Spawns; }
    TouchMap& GetTouch() { return m_Touchs; }
    DropObjectMap& GetDropItemsMap() {return m_DropItems;}

    BattleUnit* GetBattleUnit(uint32 objectID);
    Player* FindPlayer(uint32 objectId);
    Player* GetPlayerByPlayerId(uint64 playerID);
    Player* GetPlayerByPlayerName(const std::string& playerName);
    Player* GetPlayerByObjectId(uint32 objectID) {return FindPlayer(objectID);}
    SceneObject* GetSceneObject(uint32 objectID);
    Trigger* GetTriggerByID(uint32 triggerID);
    Trigger* GetTriggerByName(const std::string &name) const;
    Trigger* GetFirstTrigger();
    Teleport* GetTeleport(uint32 triggerID);
    Checkpoint* GetCheckPoint(uint32 checkpointID);
    Spawn* GetSpawn(uint32 spawnID);                               // 通过spawnID得到刷怪点信息
    Touch* GetTouch(uint32 touchID);
    DropObject* GetDropItem(uint32 objectID);

    uint32 GetPlayerCount() {return (uint32)m_Players.size();}     // 此场景中玩家数

    // 消息通知
    void NotifyMove(uint32 objectID, int32 angle, const CoVec2& position, const CoVec2& velocity , bool isMoving);  // 通知移动
    void NotifyMessageToAll(uint32 msgId, void* msg);                           // 通知所有人
    void NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender = false);        // 某对象范围通知

    // 寻路
    bool    FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath);

    // 范围搜索
    int32   FindBattleUnitInRect(const CoVec2& center, int32 length, int32 width, int32 angle, BattleUnit** outFind, int32 outLen);
    int32   FindBattleUnitInFan(const CoVec2& center, int32 radius, int32 angleCenter, int32 angleSeperation, BattleUnit** outFind, int32 outLen);
    int32   FindBattleUnitInCircle(const CoVec2& center, int32 radius, BattleUnit** outFind, int32 outLen, uint32 exclude_id=0);
    int32   FindBattleUnitInCircle(const CoVec2& center, int32 radius, BattleUnitMap& outFind, uint32 exclude_id=0);        // 寻找范围内的战斗单位,(坐标点,范围,范围内的战斗单位,排除的战斗单位ID)
    BattleUnit*  FindNearestEnemyBattleUnitInCircle(BattleUnit* battleUnit, const CoVec2& center, int32 radius, uint32 exclude_id=0);              // 寻找范围内最近的敌人
    CoVec2  FindValidPositionInCircle(const CoVec2& center, int32 radius, int32 collider_radius);       // 寻找圆范围内一个随机的可用位置(中心,搜寻半径,碰撞半径)
    CoVec2  FindValidPositionInRing(const CoVec2& center, int32 max_radius, int32 min_radius, int32 collider_radius);       // 寻找圆环范围内一个随机的可用位置(中心,外半径,内半径,碰撞半径)
    CoVec2  FindValidDropItemPostion(const CoVec2& center, int32 radius);             // 寻找一个掉落物品的位置(确保掉落物品不重叠,且再可行走区域)

    bool    TestCircleWithTriangles(const CoVec2& center, int32 radius);
    bool    TestCircleWithCell(const CoVec2& center, int32 radius);
    bool    TestCircleWithBound(const CoVec2& center, int32 radius);

    static bool SortDistance(const BattleUnitDistance& a, const BattleUnitDistance& b);

    // box2D碰撞处理
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);

private:
    // 生物类进出场景
    void AddBattleUnit(BattleUnit* pBattleUnit);
    void DelBattleUnit(uint32 id);
    // 物体类进出场景
    void AddSceneObject(SceneObject* pSceneObject);
    void DelSceneObject(uint32 id);

    void _UpdateDelObject(SceneObject* pObject);

    void UpdateCraetureEvil(uint32 curr_tick);

    // 根据副本类型判断是否使用aoi
    bool IsEnableAoi();

public:
    // 判断物体是否需要通知给客户端
    bool IsSceneObjectNotify(SceneObject* pObject);

private:
    uint32              m_scene_id;         // 场景id
    uint32              m_dungeon_id;       // 副本id
    uint32              m_channel_id;       // 频道id
    Dungeon*            m_pDungeon;         // 拥有此场景的副本
    bool                m_bClose;           // 是否结算场景
    bool                m_InUnitUpdate;     // 是否在Update中

    uint32              m_HeartBeat;        // 地图心跳
    uint32              m_SceneTime;        // 场景时间

    SceneNotifier*      m_pNotifier;        // 通知器
    CoWorld*            m_pCoWorld;         // 物理场景
    NavMesh*            m_pNavMesh;         // 导航网格
    GridWorld*          m_pGridWorld;       // 网格场景
    b2World*            m_pB2World;         // 物理世界
    Regular*            m_pRegular;         // 游戏规则
    EventDispatcher*    m_EventDispatcher;  // 事件分发器

    std::vector<SceneObject*>  m_TempObjectList; // 在update中加入的对象，先放入此列表中

    BattleUnitMap       m_BattleUnits;      // 所有战斗对象<id, 生物>(包含如下的 角色、怪物、NPC、塔等)
    PlayerMap           m_Players;          // 所有角色<角色id, 角色>
    MonsterMap          m_Monsters;         // 所有怪物<怪物id, 怪物>
    BossMap             m_Bosses;           // 所有BOSS<BOSSid, BOSS>
    PetMap              m_Pets;             // 所有宠物<宠物id, 宠物>
    OfflinePlayerMap    m_OfflinePlayer;    // 所有离线玩家<离线玩家id, 离线玩家>
    NpcMap              m_Npcs;             // 所有NPC<NPCid, NPC>

    SceneObjectMap      m_SceneObjects;     // 所有物件类对象<id, 物件>(包含触发器, 路点，出生点等)
    TriggerMap          m_Triggers;         // 所有的触发器
    CheckPointMap       m_Checkpoints;      // 所有的点
    SpawnMap            m_Spawns;           // 所有出生点
    TouchMap            m_Touchs;           // 所有采集点
    DropObjectMap       m_DropItems;        // 所有掉落物品

    TeleportList        m_TeleportList;     // 传送点列表

    bool                m_CanUseAI;         // 是否开启AI

    bool                m_useAOI;           // 是否使用视野系统

};


#endif // scene_h__
