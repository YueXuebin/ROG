#ifndef regular_base_h__
#define regular_base_h__

#include "regular_data.h"
#include "rtti.h"

class Monster;
class Npc;
class Boss;
class Trigger;
class Checkpoint;
class Spawn;
class RegularDoc;
class BattleUnit;
class Scene;
class Dungeon;
class OfflinePlayer;
class RegularEvent;
class Touch;
class DropObject;
struct DropData;
class ItemBase;

struct OfflinePlayerData
{
    uint64 player_id;
    std::string player_name;

    int32 x;
    int32 y;
    int32 angle;

    OfflinePlayerData()
    {
        player_id = 0;
        x = y = angle = 0;
    }
};

/*
 *	规则基础,基础功能
 */
class RegularBase
{
    CnDeclareRootRTTI(RegularBase)

    friend class DungeonTemplate;       // 只有DungeonTemplate访问GetRaidsMonsterList()
public:
    RegularBase(RegularDoc* pDoc);
    virtual ~RegularBase();

    uint32 GetType();  // 副本类型
    void SetFileName(const std::string& fileName) {m_FileName = fileName;}
    const std::string& GetFileName() {return m_FileName;}

    virtual void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    virtual void SetScene(Scene* pScene) {m_pScene = pScene;}
    Scene* GetScene() {return m_pScene;}
    
    Monster* AddMonster(MonsterData& data);
    Monster* AddMonster(int32 creatureId, int32 x, int32 y ,int32 angle, int32 level, int32 quality);
    void KillMonster(int32 number, BattleUnit* pKiller = NULL);

    Boss* AddBoss(BossData& data, const std::string& name = "", bool connect= true);
    Boss* AddBoss(int32 bossId, int32 x, int32 y, int32 angle, bool bossUI, bool bcorpse = false, uint32 lifetime = 0);
    void KillBoss(int32 number, BattleUnit* pKiller = NULL);

    virtual Trigger* CreateTrigger(const TriggerData& data);
    virtual Checkpoint* CreateCheckPoint(const CheckPointData& data);
    virtual Spawn* CreateSpawn(const SpawnData& data);
    virtual Touch* CreateTouch(const TouchData& data);

    DropObject* AddDropObject(const DropData& dropdata, ItemBase* pItemBase);

    void KillAll();                     // 杀死所有的战斗单位

    void CreateOfflinePlayer(uint64 offlinePlayerID, int32 x, int32 y, int32 angle);
    void CreateOfflinePlayerByName(const std::string& offlinePlayerName, int32 x, int32 y, int32 angle);
    void OnCreateOfflinePlayer(OfflinePlayer* pOfflinePlayer);
    OfflinePlayer* AddOfflinePlayer(const OFFLINE_PLAYER_INFO& OfflinePlayerInfo, int32 x, int32 y, int32 angle);

protected:
    virtual void ParseRegular(TiXmlElement* RegularElement) {};

    void ConnectBoss(Boss* pBoss);          // 建立boss联系

    void CreateAllBattleUnits();        // 创建所有生物
    void CreateSceneObjects();
    void CreateObjects();
    
    void ChangeStrong(BattleUnit* pBattleUnit);    // 改变怪物强度
    uint32 GetStrongVal(uint32 type, uint32 DifficultLevel);     // 不同难度下的属性倍率值

    virtual int32 GetDifficultLevel() {return 1;}

private:
    void _CreateOfflinePlayer(const OFFLINE_PLAYER_INFO* pRecord, int32 x, int32 y, int32 angle);

    void GetRaidsMonsterList(std::map<uint32, int32>& list);

protected:
    std::string m_FileName;         // 规则文件名
    RegularDoc* m_pRegularDoc;
    Scene* m_pScene;
    int32 m_monsterSum;       // 怪物总数
    int32 m_bossSum;          // boss总数
    int32 m_npcSum;           // NPC总数
    int32 m_dropObjectSum;    // 掉落物品总数
    std::vector<OfflinePlayerData> m_CacheCreateOfflinePlayerData;             // 要创建的离线玩家数据缓存
};


#endif // regular_base_h__
