#include "gameholder_pch.h"
#include "item_controller.h"
#include "player.h"
#include "item_manager.h"
#include "item_consume.h"
#include "random_package_manager.h"
#include "skill_config_manager.h"
#include "skill_info.h"
#include "skill_info_manager.h"
#include "action_controller.h"
#include "buff_controller.h"

ItemManager* ItemController::GetItemMgr()
{
    return m_player->GetItemManager();
}

ItemController::ItemController(Player* player) :
m_player(player),
m_LastFrameCounter(0)
{
}

ItemController::~ItemController(void)
{
}

void ItemController::Update(uint32 frame_count)
{
    uint32 delta_Frame = 1;
    if(m_LastFrameCounter != 0)
    {
        delta_Frame = frame_count - m_LastFrameCounter;
    }
    m_LastFrameCounter = frame_count;
    uint32 delta_time = delta_Frame * g_ServerUpdateTime;

    for (std::list<TRIPLE_VALUE>::iterator mIter = m_ConsumeCD.begin(); mIter != m_ConsumeCD.end(); )
    {
        mIter->value -= delta_time;

        if (mIter->value <= g_ServerUpdateTime)
        {
            AddNtf(mIter->key);
            mIter = m_ConsumeCD.erase(mIter);
        }
        else
        {
            ++mIter;
        }
    }

    ForceUpdate();
}

void ItemController::ForceUpdate()
{
    if (m_ConsumeCDInfoNTF.list.empty())
    {
        return;
    }

    m_player->SendToGate(MSG_CONSUME_CD_NTF, &m_ConsumeCDInfoNTF);
    m_ConsumeCDInfoNTF.Clear();
}

uint16 ItemController::UseConsume(uint32 reason, ItemConsume* pConsume)
{
    if (!pConsume)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (PLACE_BAG != pConsume->GetPlace())
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (pConsume->GetEffectData().empty())
    {
        return ERR_FAILED;
    }

    if (IsConsumeInCD(pConsume->GetCDType()))
    {
        return ERR_ITEM_CD;
    }

    if (pConsume->GetLevel() > m_player->GetLevel())
    {
        return ERR_NO_ENOUGH_PLAYER_LEVEL;
    }

    for (std::vector<Data3>::const_iterator cIter = pConsume->GetEffectData().begin();
        cIter != pConsume->GetEffectData().end(); ++cIter)
    {
        DoItemEffect(pConsume, *cIter);
    }

    AddConsumeCD(pConsume->GetCDType(), pConsume->GetCDTime());

    GetItemMgr()->RemoveItemNum(reason, 1, pConsume->GetPlace(), pConsume->GetPos());

    return ERR_SUCCEED;
}

void ItemController::AddConsumeCD(uint32 cdType, uint32 cdTime)
{
    TRIPLE_VALUE tv;
    tv.type = cdType;
    tv.key = cdTime;
    tv.value = cdTime;
    m_ConsumeCD.push_back(tv);

    AddNtf(cdType, cdTime);
}

bool ItemController::IsConsumeInCD(uint32 cdType) const
{
    if(cdType == 0)
        return false;             // 无冷�?

    for (std::list<TRIPLE_VALUE>::const_iterator cIter = m_ConsumeCD.begin();
        cIter != m_ConsumeCD.end(); ++cIter)
    {
        if (cIter->type == cdType)
        {
            return cIter->value > 0;
        }
    }

    return false;
}

void ItemController::AddNtf(uint32 cdType, uint32 cdTime /*= 0*/)
{
    TRIPLE_VALUE tv;
    tv.type = cdType;
    tv.key = cdTime;
    tv.value = cdTime;
    m_ConsumeCDInfoNTF.list.push_back(tv);
}

void ItemController::DoItemEffect(const ItemConsume* pConsume, const Data3& data)
{
    switch(data.value1)
    {
    case EFFECT_TYPE_PROP:
        {
            DoEffectProp(pConsume, data.value2, data.value3);
        }
        break;
    case EFFECT_TYPE_PACKAGE:
        {
            DoEffectPackage(pConsume, data.value2, data.value3);
        }
        break;
    case EFFECT_TYPE_SKILL:
        {
            DoEffectSkill(pConsume, data.value2, data.value3);
        }
        break;
    case EFFECT_TYPE_BUFF:
        {
            DoEffectBuff(pConsume, data.value2, data.value3);
        }
        break;
    default:
        break;
    }
}

void ItemController::DoEffectProp(const ItemConsume* pConsume, int32 key, int32 value)
{
    switch(key)
    {
    case PROP_HP:
        {
            m_player->AddHP(value);
        }
        break;
    case PROP_MAXHP_MUL:
        {
            m_player->AddHP(m_player->GetMaxHp() * value / BASE_RATE_NUM);
        }
        break;
    case PROP_MP:
        {
            m_player->AddMP(value);
        }
        break;
    case PROP_MAXMP_MUL:
        {
            m_player->AddMP(m_player->GetMaxMp() * value / BASE_RATE_NUM);
        }
        break;
    case PROP_ENERGY_SHIELD:
        {
            m_player->AddEnergyShield(value);
        }
        break;
    case PROP_MAX_ENERGY_SHIELD_MUL:
        {
            m_player->AddEnergyShield(m_player->GetMaxEnergyShield() * value / BASE_RATE_NUM);
        }
        break;

    default:
        GetItemMgr()->AddRewardItemByType(key, value);
        break;
    }
}

void ItemController::DoEffectPackage(const ItemConsume* pConsume, int32 key, int32 value)
{
    REWARD_TABLE table;
    for (int32 i=0; i<value; ++i)
    {
        if (!m_player->GetRandomPackageMgr()->GetPackage(0, key, table))
            break;
    }

    if (table.items.empty())
    {
        return;
    }

    GetItemMgr()->AddRewardTable(&table);
}

void ItemController::DoEffectSkill(const ItemConsume* pConsume, int32 key, int32 value)
{
    uint32  skillID(key);
    int32   skillLevel(value);
    int32   skillQuality(QUALITY_WHITE);

    const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(skillID);
    if (!skillConfig)
    {
        CnAssert(false);
        return;
    }

    if ((!skillConfig->to_me) &&
        (skillConfig->feature & SKILL_FEATURE_SPELL) &&
        (skillConfig->feature & SKILL_FEATURE_DURATION))
    {
        CnError("Item do effect skill error! to_me=%d feature=%d", skillConfig->to_me, skillConfig->feature);
        return;
    }

    SHOOT_TARGET target;
    target.caster_id = m_player->GetId();
    target.target_id = m_player->GetId();
    target.target_x = m_player->GetX();
    target.target_y = m_player->GetY();

    SkillInfo skillInfo;
    skillInfo.Clear();
    skillInfo.SetSkillId(skillID);
    skillInfo.SetLevel(skillLevel);
    skillInfo.SetQuality(skillQuality);
    skillInfo.SetSkillValid(true);
    skillInfo.SetBookValid(false);
    
    m_player->GetSkillInfoMgr()->CalcSkillInfoEx(&skillInfo, NULL, NULL);

    SkillArgumentData args;
    args.fromNetArgument(*skillInfo.GetSkillArgs());
    args.m_ConsumeSkill = true;
    args.m_SkillMode = SKILL_MODE_SELF;
    args.m_Mp = skillInfo.GetMp();
    // 技能是否暴�?
    if(int32(Crown::SDRandom(BASE_RATE_NUM)) < m_player->GetCrit())
    {
        args.m_IsCritical = true;
    }
    // 技能等�?
    args.m_SkillLevel = skillInfo.GetLevel();
    // 技能品�?
    args.m_SkillQuality = skillInfo.GetQuality();
    skillInfo.FillPropertySet(args.m_SkillProps);

    m_player->GetActionController()->UseSkill(skillID, target, args, 0, 0);
}

void ItemController::DoEffectBuff(const ItemConsume* pConsume, int32 key, int32 value)
{
    m_player->GetBuffController()->AddBuff(key, pConsume->GetContinuedTime(), m_player, value, 0);
}
