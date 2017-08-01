#ifndef soldier_h__
#define soldier_h__

#include "pet.h"
#include "item_equip.h"
#include "item_stone.h"
#include "skill_info.h"

class SoldierPropNotifier;
class EquipManager;
class SkillInfoManager;
class SoldierTalentManager;
class Player;

const int32 MaxResurgenceTime = 10000;

/*
 *	佣兵
 */
class Soldier : public Pet
{
    CnDeclareRTTI
public:
    Soldier(Player* pMaster);
    virtual ~Soldier();

    void    LoadInfo(const SOLDIER_INFO& info);
    void    FillInfo(SOLDIER_INFO& info) const;

    void    OnRecv(uint32 msgID, void* data);                              // 接收网络数据

public:
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3); // 接收事件

    virtual void    onEnterScene(Scene* pScene);
    virtual void    onLeaveScene(Scene* pScene);
    virtual void    Update(uint32 frame_count);

    virtual void    OnKill(BattleUnit* sufferer);
    virtual void    OnDead(BattleUnit* killer);
    virtual void    OnResurgence();                                    // 复活处理

    virtual void    OnStateChange(uint32 oldState, uint32 newState);       // 状态改变回调

    virtual bool    UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);

    virtual void    CalcProperty();                                        // 计算角色属性
    virtual void    ClearProperty();                                       // 清除角色属性
    virtual void    SetProp(uint32 type, int32 val);

    virtual bool    SendToGate(uint32 msgID, void* data);
    virtual void    NotifyEquipInfo(UPDATE_EQUIP_INFO_NTF& info);
    virtual int32   GetEquipAnimation() const;

    /////////////////////////////////////////////////////////////////////////
    // 属性
    void    SetPropDirty();
    // 力量
    int32   GetStrength() const {return m_strength;}
    void    SetStrength(int32 val) {m_strength = val; SetPropDirty();}
    void    ClearStrength() {m_strength = 0;}

    // 智力
    int32   GetAgility() const {return m_agility;}
    void    SetAgility(int32 val) {m_agility = val; SetPropDirty();}
    void    ClearAgility() {m_agility = 0;}

    // 精神
    int32   GetSpirit() const {return m_spirit;}
    void    SetSpirit(int32 val) {m_spirit = val; SetPropDirty();}
    void    ClearSpirit() {m_spirit = 0;}

    // 体质
    int32   GetConstitution() const {return m_constitution;}
    void    SetConstitution(int32 val) {m_constitution = val; SetPropDirty();}
    void    ClearConstitution() {m_constitution = 0;}

    // 由全局通知器通知的属性,佣兵还需要再通知给客户端
    virtual void    SetHP(int32 val);
    virtual void    SetMP(int32 val);
    virtual void    SetEnergyShield(int32 val);
    virtual void    SetMaxHp(int32 val);
    virtual void    SetMaxMp(int32 val);
    virtual void    SetMaxEnergyShield(int32 val);
    virtual void    SetMoveSpeed(int32 val);

    void    SetLevel(int32 val);
    void    AddLevel(int32 val = 1) {SetLevel(GetLevel() + val);}

    // 获得经验
    int32   GetExp() const {return m_exp;}
    void    SetExp(int32 val);
    void    AddExp(int32 val);

    int32   GetMaxExp() const;                                          // 获得最大经验值

    const std::string& GetName() const {return m_name;}
    void    SetName(const std::string& name) {m_name = name;}

    uint8   GetSex() const {return m_sex;}
    void    SetSex(uint8 sex) {m_sex = sex;}

    void    ReloadAI();

    SkillInfo* GetDefaultSkillInfo();

    void    CalcDefaultSkillInfo();

    uint32  DefaultSkillId();
    void    UseDefaultSkill(const SHOOT_TARGET& target);

    EquipManager* GetEquipMgr() {return m_EquipMgr;}
    SkillInfoManager* GetSkillInfoMgr() {return m_SkillInfoMgr;}
    SoldierTalentManager* GetSoldierTalentMgr() {return m_SoldierTalentMgr;}

public:
    void OnResurgenceNtf();

private:
    void    CalcLevelProp();                                            // 计算等级属性

private:
    SoldierPropNotifier*  m_SoldierPropNotifier;                             // 佣兵属性通知器

    EquipManager* m_EquipMgr;                                           // 佣兵装备
    SkillInfoManager* m_SkillInfoMgr;                                   // 佣兵技能
    SoldierTalentManager* m_SoldierTalentMgr;                           // 佣兵天赋

    // 属性
    int32           m_strength;                                         // 力量
    int32           m_spirit;                                           // 智力
    int32           m_agility;                                          // 敏捷
    int32           m_constitution;                                     // 体力

    int32           m_exp;
    std::string     m_name;
    uint8           m_sex;

    uint32          m_LastSkillId;
    SkillInfo       m_DefaultSkillInfo;
};

#endif // soldier_h__
