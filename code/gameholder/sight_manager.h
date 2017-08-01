#ifndef sight_manager_h__
#define sight_manager_h__

//class SceneObject;
//class Player;
//class BattleUnit;
//
//class SightManager
//{
//public:
//
//    enum 
//    {
//        SIGHT_BATTLE_UNIT_SIZE  = 50,           //视野内最大生物数量
//        SIGHT_RECT_LENGHT       = 2000,         //视野高度
//        SIGHT_RECT_WIDTH        = 2000,         //视野宽度
//        UPDATE_SIGHT_INTERVAL   = 1000,         //更新频率(毫秒)
//    };
//
//    typedef std::map<uint32, Player*> PlayerMap;
//    typedef std::map<uint32, BattleUnit*>  BattleUnitMap;
//    typedef std::map<uint32, SceneObject*>  SceneObjectMap;
//
//    SightManager(Player *own);
//
//    ~SightManager();
//
//    //更新视野
//    void Update();
//
//    //初始化视野内所有生物和对象
//    void Init();
//
//    const PlayerMap& GetPlayers() const { return m_Players; }
//    
//    const BattleUnitMap& GetBattleUnits() const { return *m_BattleUnits; }
//    const BattleUnitMap& GetNewBattleUnits() const { return *m_NewBattleUnits; }
//    const BattleUnitMap& GetOldBattleUnits() const { return *m_OldBattleUnits; }
//
//    const SceneObjectMap& GetObjects() const { return *m_SceneObjects; }
//    const SceneObjectMap& GetNewObjects() const { return *m_NewSceneObjects; }
//    const SceneObjectMap& GetOldObjects() const { return *m_OldSceneObjects; }
//
//    void AddPlayer(Player *player);
//    void AddBattleUnit(BattleUnit *pBattleUnit); 
//    void AddSceneObject(SceneObject *pSceneObject);
//
//
//private:
//    void InitSightBattleUnit();
//
//    void InitSightSceneObject();
//
//
//private:
//    Player              *m_pOwn;
//
//    PlayerMap           m_Players;
//
//    BattleUnitMap       *m_BattleUnits;
//    BattleUnitMap       *m_NewBattleUnits;
//    BattleUnitMap       *m_OldBattleUnits;
//
//    SceneObjectMap      *m_SceneObjects; 
//    SceneObjectMap      *m_NewSceneObjects;
//    SceneObjectMap      *m_OldSceneObjects;
//  
//    uint32               m_updateSightTime;
//};

#endif //sight_manager_h__