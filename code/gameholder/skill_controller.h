/*
 *	技能控制器
 */
#ifndef skill_controller_h__
#define skill_controller_h__

#include "skill_data.h"

class BattleUnit;
class SkillObject;

class SkillController
{
public:
    SkillController(BattleUnit* owner);
    ~SkillController();

    void FillSkillState(STATE_INFO& info);
    void LoadSkillState(const STATE_INFO& info);

    void Update(uint32 delta_time);

    SkillObject* UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);               // 使用技能(skill_obj_id用来设置技能对象ID用)
    SkillObject* UseSkillByPassive(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_slot);      // 被动使用技能(弹射、触发类技能)
    //SkillObject* UseSkillByFollowPet(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args);    // 非自身使用技能(跟宠技能，自动释放的技能)
    //SkillObject* UseSkillByScene(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args);        // 场景技能使用

    void OnStateChange(uint32 state);

    void StopAllSkill();

    int32 MakeSkillCD(uint32 skill_id, int32 cdtime);      // 使技能进入CD
    void ClearCD();                     // 清除所有技能的CD
    bool inCD(uint32 skill_id);         // 技能是否CD中

    void SetSilence(uint32 time) {m_silenceTime = time;};
    int32 GetSilence() {return m_silenceTime;}

    std::vector<SkillObject*>&  GetSkills() {return m_Skills;}
    const std::map<uint32, int32>& GetSkillCDMap() {return m_SkillCDMap;}

private:
    bool    CheckCanUseSkill(uint32 skill_id);

    SkillObject* _UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);

    void NotifyUseSkill(uint32 skill_id, uint32 skill_cd, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot);
    void NotifyEndSkill(uint32 skill_id, uint32 skill_obj_id);

    int32 SubSkillCD(uint32 skill_id);

private:
    BattleUnit* m_Owner;

    std::vector<SkillObject*> m_Skills;             // 角色当前正在使用的技能
    std::map<uint32, int32> m_SkillCDMap;           // 技能CD记录<skillid, cd time>

    bool    m_inUpdate;     // 技能update中标记
    int32   m_silenceTime;  // 沉默时间

    uint16   m_skillObjectIDCounter;   // 技能唯一ＩＤ计数
};



#endif // skill_controller_h__
