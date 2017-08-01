/*
*	BOSS
*   与Monster的区别: 有仇恨系统,能够转生,BossUI
*/
#ifndef boss_h__
#define boss_h__

#include "creature.h"
#include "reincarnated_data.h"
#include "regular_data.h"

class AIController;
struct BossTemplate;
class RelifeHelper;
class HatredManager;


class Boss : public Creature
{
    CnDeclareRTTI
public:
    Boss();
    virtual ~Boss();

    virtual void Update(uint32 frame_count);

    virtual bool BeforeDead();
    virtual void OnDead(BattleUnit* killer);
    virtual void OnDestory();
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);

    virtual void Damage(const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill = NULL);

    void SetBossUI(bool show) {m_BossUI = show;}
    bool GetBossUI() {return m_BossUI;}

    void SetInstanceName(const std::string& val) { m_instanceName = val; }
    const std::string& GetInstanceName() { return m_instanceName; }

    void InitBoss(const BossData &data);

    void SetReincarnated(const ReincarnatedList& list);

    HatredManager* GetHatredMgr(){return m_HatredManager;}

    void SetBcorpse(bool bcorpse) { m_Bcorpse = bcorpse;}
    bool GetBcorpse() {return m_Bcorpse;}

    void SetLifeTime(uint32 time) {m_lifetime = time;}
    uint32 GetLifeTime() {return m_lifetime;}

    void SetCreateTime() {m_createTime = (uint32)Crown::SDNow().GetTimeValue();}

    uint32 GetCreateTime() {return m_createTime;}

    uint32 GetCorpseTime(); // 计算boss鞭尸还剩多长时间

    //virtual uint32 GetDefaultSkill();
private:
    BossData m_bossData;        // 此boss的数值
    std::string m_instanceName;         // 怪物实例名称（这个名称在整个世界地图中是唯一的，与显示名称无关）
    ReincarnatedList m_ReincarnatedList;     // Boss的转生链
    int32 m_SceondTimer;          // 秒计时器
    bool m_BossUI;               // 该Boss是否显示UI
    bool m_Bcorpse;              // 是否创建的是鞭尸状态
    uint32 m_lifetime;             // 生存时间
    uint32 m_createTime;           // 创建boss时间
    HatredManager* m_HatredManager;       // Boss的仇恨系统
};


#endif // boss_h__
