#include "gameholder_pch.h"
#include "soldier.h"
#include "game_define.h"
#include "pet_controller.h"
#include "relife_helper.h"
#include "ai_controller.h"
#include "scene.h"
#include "regular.h"
#include "property_set_calculater.h"
#include "player_config_manager.h"
#include "soldier_prop_notifier.h"
#include "item_factory.h"
#include "player.h"
#include "gate_handler.h"
#include "system_config_manager.h"
#include "resurgence_helper.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "prop_notifier.h"
#include "bag_manager.h"
#include "player.h"
#include "equip_manager.h"
#include "skill_info_manager.h"
#include "soldier_talent_manager.h"
#include "skill_config_manager.h"
#include "action_controller.h"

const char* ATT_AI = "ai/soldier_attack.aix";
const char* AUX_AI = "ai/soldier_auxiliary.aix";
const char* AUR_AI = "ai/soldier_aura.aix";
const char* DEF_AI = "ai/soldier_defense.aix";

CnImplementRTTI(Soldier, Pet);

Soldier::Soldier(Player* pMaster):
Pet(pMaster),
m_exp(0),
m_LastSkillId(0),
m_name(""),
m_sex(0)
{
    m_SoldierPropNotifier       = CnNew SoldierPropNotifier(this);
    m_EquipMgr                  = CnNew EquipManager(this, PLACE_SOLDIER_EQUIP);
    m_SkillInfoMgr              = CnNew SkillInfoManager(this, PLACE_SOLDIER_SKILL, SKILL_SLOT_Q);
    m_SoldierTalentMgr          = CnNew SoldierTalentManager(this);

    m_DefaultSkillInfo.Clear();

    ClearProperty();
}

Soldier::~Soldier()
{
    CnDelete m_SoldierTalentMgr;
    m_SoldierTalentMgr = NULL;

    CnDelete m_SkillInfoMgr;
    m_SkillInfoMgr = NULL;

    CnDelete m_EquipMgr;
    m_EquipMgr = NULL;

    CnDelete m_SoldierPropNotifier;
    m_SoldierPropNotifier = NULL;
}

void Soldier::LoadInfo(const SOLDIER_INFO& info)
{
    SetEnable(0 == info.is_enable ? false : true);

    m_level = info.level > 1 ? info.level : 1;
    m_exp   = info.exp;

    m_EquipMgr->LoadEquipInfo(info.equip_info);
    m_SkillInfoMgr->LoadSkillComponentInfo(info.skill_component_info);
    m_SoldierTalentMgr->LoadStoneInfo(info.stone_info);
}

void Soldier::FillInfo(SOLDIER_INFO& info) const
{
    info.Clear();

    info.is_enable  = IsEnable() == true ? 1 : 0;
    info.sex        = GetSex();
    info.level      = GetLevel();
    info.exp        = GetExp();

    m_EquipMgr->FillEquipInfo(info.equip_info);
    m_SkillInfoMgr->FillSkillComponentInfo(info.skill_component_info);
    m_SoldierTalentMgr->FillStoneInfo(info.stone_info);
}

void Soldier::OnRecv(uint32 msgID, void* data)
{
}

void Soldier::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    Pet::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    m_SkillInfoMgr->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

void Soldier::onEnterScene(Scene* pScene)
{
    Pet::onEnterScene(pScene);

    CalcProperty();

    OnResurgenceNtf();
}

void Soldier::onLeaveScene(Scene* pScene)
{
    Pet::onLeaveScene(pScene);

//     if (!IsDead())
//     {
//         ChangeState(STATE_DIE);
//     }
// 
//     GetResurgenceHelper()->SetResurgenceFullHp();
}

void Soldier::Update(uint32 frame_count)
{
    Pet::Update(frame_count);

    m_SoldierPropNotifier->Update();
}

void Soldier::OnKill(BattleUnit* sufferer)
{
    Pet::OnKill(sufferer);
}

void Soldier::OnDead(BattleUnit* killer)
{
    Pet::OnDead(killer);

    if (IsDead())
    {
        GetResurgenceHelper()->SetResurgenceFullHp();
        GetResurgenceHelper()->EventResurgence(0, MaxResurgenceTime);
        OnResurgenceNtf();
    }
}

void Soldier::OnResurgence()
{
    Pet::OnResurgence();

    if (!GetScene())
    {
        GetMaster()->GetPetController()->AddPet(this);
    }

    OnResurgenceNtf();
}

void Soldier::OnStateChange(uint32 oldState, uint32 newState)
{
    Pet::OnStateChange(oldState, newState);
}

bool Soldier::UseSkill(uint32 skill_id, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_obj_id, uint32 skill_slot)
{
    bool ret = Pet::UseSkill(skill_id, target, args, skill_obj_id, skill_slot);

    return ret;
}

void Soldier::CalcProperty()
{    // 计算角色等级提供的属性
    CalcLevelProp();

    // 计算装备提供的属性
    m_EquipMgr->CalcAllEquipProp();

    // 计算天赋提供的属性
    m_SoldierTalentMgr->CalcProp();

    // 计算地图属性
    if(GetScene())
    {
        GetScene()->GetRegular()->CalcMapProp(this);
    }

    Pet::CalcProperty();

    CalcDefaultSkillInfo();

    SetPropDirty();
}

void Soldier::ClearProperty()
{
    Pet::ClearProperty();

    // 一级属性
    ClearStrength();
    ClearAgility();
    ClearSpirit();
    ClearConstitution();
}

void Soldier::SetProp(uint32 type, int32 val)
{
    switch(type)
    {
        // 力量
    case PROP_STRENGTH:
        SetStrength(val);
        break;
        // 智力
    case PROP_SPIRIT:
        SetSpirit(val);
        break;
        // 敏捷
    case PROP_AGILITY:
        SetAgility(val);
        break;
        // 体质
    case PROP_CONSTITUTION:
        SetConstitution(val);
        break;
    default:
        BattleUnit::SetProp(type, val);
        break;
    }

    SetPropDirty();
}

void Soldier::SetHP(int32 val)
{
    Pet::SetHP(val);
    SetPropDirty();
}

void Soldier::SetMP(int32 val)
{
    Pet::SetMP(val);
    SetPropDirty();
}

void Soldier::SetEnergyShield(int32 val)
{
    Pet::SetEnergyShield(val);
    SetPropDirty();
}

void Soldier::SetMaxHp(int32 val)
{
    Pet::SetMaxHp(val);
    SetPropDirty();
}

void Soldier::SetMaxMp(int32 val)
{
    Pet::SetMaxMp(val);
    SetPropDirty();
}

void Soldier::SetMaxEnergyShield(int32 val)
{
    Pet::SetMaxEnergyShield(val);
    SetPropDirty();
}

void Soldier::SetMoveSpeed(int32 val)
{
    Pet::SetMoveSpeed(val);
    SetPropDirty();
}

bool Soldier::SendToGate(uint32 msgID, void* data)
{
    Player* pPlayer = CnDynamicCast(Player, GetMaster());
    if (!pPlayer) return false;

    return GateHandler::Instance()->SendByGateID(pPlayer->GetGateSvrID(), msgID, data, pPlayer->GetPlayerID());
}

void Soldier::NotifyEquipInfo(UPDATE_EQUIP_INFO_NTF& info)
{
    info.object_id = GetId();

    if (GetScene())
    {
        GetScene()->NotifyMessage(this, MSG_UPDATE_EQUIP_INFO_NTF, &info);
    }

    m_PropNotifier->NotifyProperty(PROP_EQUIP_ANIMATION, GetEquipMgr()->GetEquipAnimation());
}

int32 Soldier::GetEquipAnimation() const
{
    return m_EquipMgr->GetEquipAnimation();
}

void Soldier::SetPropDirty()
{
    m_SoldierPropNotifier->SetDirty();
}

void Soldier::SetLevel(int32 val)
{
    val = Crown::SDRange(1, val, SystemConfigManager::Instance()->GetMaxLevel());

    m_level = val;

    m_PropNotifier->NotifyProperty(PROP_LEVEL, GetLevel()); // 广播等级

    CalcProperty();    // 重新计算角色属性
}

void Soldier::SetExp(int32 val)
{
    if (GetLevel() >= SystemConfigManager::Instance()->GetMaxLevel())
    {
        return;
    }

    val = Crown::SDRange(0, val, GetMaxExp());
    m_exp = val;

    SetPropDirty();
}

void Soldier::AddExp(int32 val)
{
    if(val <= 0)
    {
        return;
    }

    // 给技能也提供经验
    GetSkillInfoMgr()->AddExp(val);

    const int32 nMaxLevel = SystemConfigManager::Instance()->GetMaxLevel();
    if (GetLevel() >= nMaxLevel)
    {
        return;
    }

    int32 nMaxExp = GetMaxExp();
    val += GetExp();
    while(val >= nMaxExp)
    {
        val -= nMaxExp;
        AddLevel();

        nMaxExp = GetMaxExp();

        SetExp(0);

        if (GetLevel() >= nMaxLevel)
        {
            return;
        }
    }

    SetExp(val);
}

int32 Soldier::GetMaxExp() const
{
    return PlayerConfigManager::Instance()->GetMaxExp(GetLevel());
}

void Soldier::ReloadAI()
{
    SkillInfo* pSkillInfo = GetSkillInfoMgr()->GetSkillInfoBySlot(1);
    if (!pSkillInfo)
    {
        CnAssert(false);
        return;
    }

    if (pSkillInfo->GetSkillId() == m_LastSkillId)
    {
        return;
    }

    m_LastSkillId = pSkillInfo->GetSkillId();

    const FINAL_SKILL_INFO& info = pSkillInfo->GetSkillInfo();
    const SKILL_ROOT_SKILL_STRUCT* pSkillData = SkillConfigManager::Instance()->GetSkillConfigData(info.skill_id);
    if (!pSkillData)
    {
        CnAssert(false);
        return;
    }

    // 光环
    if (pSkillData->feature & SKILL_FEATURE_AURA)
    {
        GetAIController()->LoadAI(AUR_AI);
    }
    // 被动
    else if (pSkillInfo->IsTriggered())
    {
        GetAIController()->LoadAI(DEF_AI);
    }
    // 对自己生效
    else if (1 == pSkillData->to_me)
    {
        GetAIController()->LoadAI(AUX_AI);
    }
    // 其他
    else
    {
        GetAIController()->LoadAI(ATT_AI);
    }
}

SkillInfo* Soldier::GetDefaultSkillInfo()
{
    return &m_DefaultSkillInfo;
}

void Soldier::CalcDefaultSkillInfo()
{
    const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(DefaultSkillId());
    if (!skillConfig)
    {
        return;
    }

    PropertySet skillPropSet;

    m_DefaultSkillInfo.Clear();

    m_DefaultSkillInfo.SetSkillId(DefaultSkillId());
    m_DefaultSkillInfo.SetQuality(QUALITY_WHITE);
    m_DefaultSkillInfo.SetLevel(1);
    m_DefaultSkillInfo.SetBookValid(false);
    m_DefaultSkillInfo.SetSkillValid(true);
    m_DefaultSkillInfo.SetMp(skillConfig->mp);

    // 技能属性
    int32 skill_grow = SkillConfigManager::Instance()->GetSkillGrowByLevelQuality(m_DefaultSkillInfo.GetLevel(), m_DefaultSkillInfo.GetQuality());

    int32 skill_prop1_id = skillConfig->prop1_id;
    int32 skill_prop1_val = int32(skillConfig->prop1_base + skillConfig->prop1_grow * ToPercent(skill_grow));
    if(skill_prop1_id)
    {
        skillPropSet.Add(skill_prop1_id, skill_prop1_val);
    }

    int32 skill_prop2_id = skillConfig->prop2_id;
    int32 skill_prop2_val = int32(skillConfig->prop2_base + skillConfig->prop2_grow * ToPercent(skill_grow));
    if(skill_prop2_id)
    {
        skillPropSet.Add(skill_prop2_id, skill_prop2_val);
    }

    // 技能原始参数
    int32 cooldown = skillConfig->cooldown;                   // 冷却时间
    int32 attack_speed = skillConfig->attack_speed;           // 施法速度
    int32 cast_multi = skillConfig->cast_multi;               // 重复施法次数
    int32 bullet_multi = skillConfig->bullet_multi;           // 多重弹道
    int32 bullet_multi_angle = skillConfig->bullet_multi_angle;   // 多重弹道夹角
    int32 bullet_repeat = skillConfig->bullet_repeat;         // 弹道重复
    int32 bullet_repeat_interval = skillConfig->bullet_repeat_interval;   // 弹道重复间隔
    int32 bullet_speed = 10000;                             // 弹道速度
    int32 bullet_chain = skillConfig->bullet_chain;           // 弹射
    int32 bullet_chain_id = skillConfig->bullet_chain_id;     // 弹射技能id

    m_DefaultSkillInfo.SetCoolDown(cooldown);

    ////////////////////////////////////////////////////////////////////////////////////
    // 技能和符文汇总
    ////////////////////////////////////////////////////////////////////////////////////
    for(PropertyList::iterator iter = skillPropSet.m_list.begin(); iter != skillPropSet.m_list.end(); iter++)
    {
        Property& src_prop = *iter;
        Property des_prop = PropertySetCalculater::CollectProperty(this, src_prop, skillPropSet, true);
        src_prop = des_prop;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // 技能属性受到角色属性的影响
    ////////////////////////////////////////////////////////////////////////////////////
    SkillInfoManager::makeSkillPropSetByBattleUnit(&skillPropSet, this, skillConfig);


    ////////////////////////////////////////////////////////////////////////////////////
    // 设置技能属性条目
    ////////////////////////////////////////////////////////////////////////////////////
    for(PropertyList::iterator propIter = skillPropSet.m_list.begin(); propIter != skillPropSet.m_list.end(); propIter++)
    {
        KEY_VALUE prop;
        prop.key = propIter->type;
        prop.value = propIter->value;
        if(prop.key > 0 && prop.value > 0)
            m_DefaultSkillInfo.AddProp(prop);
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // 根据技能属性计算最终技能参数
    ////////////////////////////////////////////////////////////////////////////////////
    PropertySet& finalPlayerPropSet = GetPropertySetCalculater()->GetFinalProp();

    // 攻击/施法速度
    if(skillConfig->attack_type == SKILL_TYPE_ATTACK)
    {   // 技能攻击速度=技能配置攻速*(符文增强攻速+角色攻速)
        attack_speed = int32( attack_speed * (ToPercent(skillPropSet.GetValue(PROP_ATTACK_PER_SEC)) + ToPercent(finalPlayerPropSet.GetValue(PROP_ATTACK_PER_SEC))) );
    }
    else if(skillConfig->attack_type == SKILL_TYPE_MAGIC)
    {   // 技能施法速度=技能配置施法速度*(符文增加施法速度+角色施法速度)
        attack_speed = int32( attack_speed * (ToPercent(skillPropSet.GetValue(PROP_CAST_PER_SEC)) + ToPercent(finalPlayerPropSet.GetValue(PROP_CAST_PER_SEC))) );
    }

    // 多重打击/多重施法
    int32 cast_multi_add = 0;
    if(skillConfig->attack_type == SKILL_TYPE_ATTACK)
        cast_multi_add += skillPropSet.GetValue(PROP_SKILL_ATTACK_REPEAT);
    else if(skillConfig->attack_type == SKILL_TYPE_MAGIC)
        cast_multi_add += skillPropSet.GetValue(PROP_SKILL_CAST_REPEAT);

    // 多重投射
    int bullet_multi_add = 0;
    bullet_multi_add += skillPropSet.GetValue(PROP_SKILL_BULLET_ADD);


    ////////////////////////////////////////////////////////////////////////////////////
    // 设置技能参数
    SKILL_ARGUMETNS* skillArgs = m_DefaultSkillInfo.GetSkillArgs();
    skillArgs->skill_mode = SKILL_MODE_SELF;
    skillArgs->cast_speed = attack_speed;
    skillArgs->cast_multi = cast_multi + cast_multi_add;
    skillArgs->bullet_multi = bullet_multi + bullet_multi_add;
    skillArgs->bullet_multi_angle = skillArgs->bullet_multi>0 ? 15 : 0;         // 15度弹道夹角
    skillArgs->bullet_repeat = bullet_repeat;
    skillArgs->bullet_repeat_interval = bullet_repeat_interval;
    skillArgs->bullet_speed = bullet_speed;
    skillArgs->bullet_chain = bullet_chain;
    skillArgs->bullet_chain_id = bullet_chain_id;
}

uint32 Soldier::DefaultSkillId()
{
    if(GetEquipAnimation() == EQUIP_ANIMATION_GJ)
    {
        return NORMAL_ATTACK_FAR;
    }
    else
    {
        return NORMAL_ATTACK_NEAR;
    }
}

void Soldier::UseDefaultSkill(const SHOOT_TARGET& target)
{
    SkillArgumentData args;
    args.fromNetArgument(*m_DefaultSkillInfo.GetSkillArgs());
    // 技能的mp消耗
    args.m_Mp = GetDefaultSkillInfo()->GetMp();

    // 技能是否暴击
    if(int32(Crown::SDRandom(BASE_RATE_NUM)) < GetCrit())
    {
        args.m_IsCritical = true;
    }

    // 技能等级
    args.m_SkillLevel = GetDefaultSkillInfo()->GetLevel();
    // 技能品质
    args.m_SkillQuality = GetDefaultSkillInfo()->GetQuality();

    // 设置技能的属性条目
    m_DefaultSkillInfo.FillPropertySet(args.m_SkillProps);

    GetActionController()->UseSkill(DefaultSkillId(), target, args, 0, 0);
}

void Soldier::OnResurgenceNtf()
{
    SOLDIER_RESURGENCE_INFO sendMsg;
    sendMsg.timer = GetResurgenceHelper()->GetTesurgenceTime();
    sendMsg.cd = MaxResurgenceTime;
    SendToGate(MSG_SOLDIER_RESURGENCE_NTF, &sendMsg);
}

void Soldier::CalcLevelProp()
{
    // 根据玩家等级获取基础属性
    const std::vector<uint32>& basePropertyIdList = GetBasePropertyIdList();

    PropertySet& propSet = GetPropertySetCalculater()->GetLevelProp();
    propSet.Clear();

    for(uint32 i=0; i< basePropertyIdList.size(); i++)
    {
        Property prop;
        prop.type = basePropertyIdList[i];
        prop.value = PlayerConfigManager::Instance()->GetPropByLevel(GetLevel(), prop.type);
        propSet.Add(prop);
    }

    // 高级属性
    propSet.Set(PROP_MAX_FIRE_DEFENCE, 75);
    propSet.Set(PROP_MAX_ICE_DEFENCE, 75);
    propSet.Set(PROP_MAX_LIGHTNING_DEFENCE, 75);
    propSet.Set(PROP_MAX_ATTACK_BLOCK, 75);
    propSet.Set(PROP_MAX_MAGIC_BLOCK, 50);
}