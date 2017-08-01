/*
*	小怪物
*/
#ifndef monster_h__
#define monster_h__

#include "creature.h"
#include "regular_data.h"

class AIController;
class Spawn;
class HatredManager;
struct MonsterTemplate;

class Monster : public Creature
{
    CnDeclareRTTI
public:
    Monster();
    virtual ~Monster();

    virtual void Update(uint32 frame_count);

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void OnDead(BattleUnit* killer);
    virtual void OnDestory();

    virtual void Damage(const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill /* = NULL */);

    void InitMonster(const MonsterData &data);

    void ConnectSpawn(Spawn* pSpawn);        // 连接巢穴

    virtual bool IsAIRun() {return !m_IsSlowTime;}   // 进入慢时钟的怪，不执行AI

    virtual bool IsAppearNotifierAll() {return m_monsterData.isAppearNotifyAll;}

    const std::vector<uint32>&  GetModifierIDs() const {return m_ModifierIDs;}

    uint32 GetQuality() const {return m_monsterData.quality;}

    const MonsterData& GetMonsterData() const {return m_monsterData;}

    HatredManager* GetHatredManager() { return m_HatredManager; }

private:


private:
    MonsterData m_monsterData;              // 此怪物的数值
    std::vector<uint32>  m_ModifierIDs;     // 品质属性列表

    int32 m_SceondTimer;        // 秒计时器
    
    bool m_IsSlowTime;          // 慢时钟怪(当没有玩家看见怪的时候,怪物进入慢时钟状态,慢时钟下10帧Update一次)
    int32 m_AccumulateTick;     // 慢时钟下的累积帧数

    Spawn* m_BirthSpawn;       // 巢穴
    HatredManager* m_HatredManager; // 仇恨系统

};


#endif // monster_h__
