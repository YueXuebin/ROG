/*
 *  副本对象
 */
#ifndef dungeon_h__
#define dungeon_h__

class BattleUnit;
class Scene;
class DungeonSet;
class Player;
class DungeonRule;

class Dungeon
{
    friend class DungeonSet;
public:
    Dungeon(DungeonSet* pSet, uint32 channelID);
    ~Dungeon();

    bool Init();
    void Update(uint32 frame_count);
    void OneSecondUpdate();

    Scene*  GetScene(uint32 sceneID);
    Scene*  GetDefaultScene() {return m_pDefaultScene;}     // 该副本的初始场景
    std::map<uint32, Scene*>& GetScenes() { return m_scenes; }

    DungeonSet* GetDungeonSet() {return m_pDungeonSet;}
    DungeonRule* GetDungeonRule() {return m_DungeonRule;}

    void    DungeonClear(bool success, std::vector<std::string>* list = NULL);    // 副本完成

    // 设置关卡的各状态时间(小心使用)
    void    SetAllStateTime(int32 readyTime, int32 runTime, int32 endTime, int32 curTime);
    // 设置指定场景的各状态时间
    void    SetSceneAllStateTime(int32 sceneID, int32 readyTime, int32 runTime, int32 endTime, int32 curTime);

    void    SetRegularTime(int32 time);
    void    StopAllRegular(bool success);

    uint32  GetDungeonTime() {return m_DungeonTime;}

    uint32  GetType();
    uint32  GetDungeonID() {return m_dungeonID;}
    uint32  GetChannelID() {return m_channelID;}

    uint32  GetPlayerNum();
    std::vector<Player*> GetPlayerMap();
    int32   GetPlayerCount() {return m_PlayerCount;}

    void    OnCreatureEnter(BattleUnit* creature);
    void    OnCreatureLeave(BattleUnit* creatrue);

    bool    IsClose();          // 判断一个副本是否已关闭

    bool    NeedDestory();      // 检测副本是否可以销毁掉了

    void    SetCanDestory(bool can) {m_CanDestory = can;}
    bool    GetCanDestory() {return m_CanDestory;}

    void    SetVirgin(bool isVirgin) {m_IsVirgin = isVirgin;}
    bool    GetVirgin() {return m_IsVirgin;}

    void    SetInvalide(bool val) {m_invalide = val;}
    bool    GetInvalide() {return m_invalide;}

    void    SendEvent(uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime);

    typedef std::map<uint32, Scene*> SceneMap;      // <场景id, 场景对象>

private:
    CoWorld* MakeCoWorld(const std::string& path);
    b2World* MakeB2World(const std::string& path);

    void    OnPlayerEnter(Player* player);
    void    OnPlayerLeave(Player* player);

    void    OnChangeDifficulty(uint32 playerNum);           // 根据人数改变副本难度

private:
    SceneMap    m_scenes;       // 该副本的所有场景 <场景id, 场景对象>

    DungeonRule*    m_DungeonRule;  // 副本的规则逻辑

    DungeonSet* m_pDungeonSet;
    uint32  m_dungeonID;        // 副本id
    uint32  m_channelID;        // 频道id

    Scene*  m_pDefaultScene;

    bool    m_IsVirgin;         // 是否有玩家进入过
    bool    m_CanDestory;       // 是否可以被销毁了
    
    uint32  m_DungeonTime;      // 副本时间ms
    int32   m_NoPlayerTimer;    // 无玩家计时
    uint32  m_SecondTimer;      // 秒计时

    int32   m_PlayerCount;      // 玩家数
    int32   m_PetCount;         // 宠物数
    int32   m_MonsterCount;     // 怪物数
    int32   m_BossCount;        // BOSS数

    bool    m_invalide;         // 是否为非法副本流程所建的频道

    int32  m_PeakPlayerNum;     // 玩家人数峰值
};


#endif // dungeon_h__
