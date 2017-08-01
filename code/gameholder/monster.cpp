#include "gameholder_pch.h"
#include "monster.h"
#include "ai_controller.h"
#include "game_define.h"
#include "scene.h"
#include "event_define.h"
#include "player.h"
#include "pet.h"
#include "relife_helper.h"
#include "action_controller.h"
#include "spawn.h"
#include "item_manager.h"
#include "player_team_manager.h"
#include "buff_controller.h"
#include "property_set_calculater.h"
#include "monster_config_manager.h"
#include "hatred_manager.h"


CnImplementRTTI(Monster, Creature);

Monster::Monster() : Creature(OBJ_MONSTER)
{
    m_BirthSpawn = NULL;

    m_SceondTimer = Crown::SDRandom(500);
    m_IsSlowTime = false;
    m_AccumulateTick = Crown::SDRandom(10);
    m_HatredManager = CnNew HatredManager(this);
}

Monster::~Monster()
{
}

void Monster::InitMonster(const MonsterData &data)
{
    m_monsterData = data;

    SetLevel(m_monsterData.level);

    // 怪物基础值
    PropertySet& monsterLevelPropSet = GetPropertySetCalculater()->GetLevelProp();
    monsterLevelPropSet.Set(PROP_PHYSICS_DAMAGE, m_monsterData.attack);
    monsterLevelPropSet.Set(PROP_MAXHP, m_monsterData.hp);
    monsterLevelPropSet.Set(PROP_MAXMP, m_monsterData.mp);
    monsterLevelPropSet.Set(PROP_MAX_ENERGY_SHIELD, m_monsterData.energyShield);
    monsterLevelPropSet.Set(PROP_HP_REC_RATE, m_monsterData.hpRecovery);
    monsterLevelPropSet.Set(PROP_MP_REC_RATE, m_monsterData.mpRecovery);
    monsterLevelPropSet.Set(PROP_MOVE_SPEED, m_monsterData.moveSpeed);
    monsterLevelPropSet.Set(PROP_ATTACK_PER_SEC, m_monsterData.attackSpeed);
    monsterLevelPropSet.Set(PROP_CAST_PER_SEC, m_monsterData.castSpeed);
    monsterLevelPropSet.Set(PROP_HIT_RATE, m_monsterData.hit);
    monsterLevelPropSet.Set(PROP_DODGE, m_monsterData.dodge);
    monsterLevelPropSet.Set(PROP_CRITICAL_ATTACK, m_monsterData.crit);
    monsterLevelPropSet.Set(PROP_CRITICAL_DAMAGE_ADDITION, m_monsterData.critDamage);
    monsterLevelPropSet.Set(PROP_ARMOR, m_monsterData.armor);
    monsterLevelPropSet.Set(PROP_FIRE_DEFENCE, m_monsterData.fireResistance);
    monsterLevelPropSet.Set(PROP_ICE_DEFENCE, m_monsterData.frostResistance);
    monsterLevelPropSet.Set(PROP_THUNDER_DEFENCE, m_monsterData.lightningResistance);
    monsterLevelPropSet.Set(PROP_BLOCK, m_monsterData.attackBlock);
    monsterLevelPropSet.Set(PROP_MAGIC_BLOCK, m_monsterData.magicBlock);

    // 怪物高级属性
    monsterLevelPropSet.Set(PROP_MAX_FIRE_DEFENCE, 95);
    monsterLevelPropSet.Set(PROP_MAX_ICE_DEFENCE, 95);
    monsterLevelPropSet.Set(PROP_MAX_LIGHTNING_DEFENCE, 95);
    monsterLevelPropSet.Set(PROP_MAX_ATTACK_BLOCK, 75);
    monsterLevelPropSet.Set(PROP_MAX_MAGIC_BLOCK, 50);

    // 怪物品质(提升生命值和生命回复)
    PropertySet& monsterQualityPropSet = GetPropertySetCalculater()->GetTalentProp();
    switch(m_monsterData.quality)
    {
    case QUALITY_WHITE:
        {
            monsterQualityPropSet.Set(PROP_MAXHP_MUL, 0);
            monsterQualityPropSet.Set(PROP_HP_REC_RATE_MUL, 0);
        }
        break;
    case QUALITY_GREEN:
        {
            monsterQualityPropSet.Set(PROP_MAXHP_MUL, 10000);
            monsterQualityPropSet.Set(PROP_HP_REC_RATE_MUL, 10000);
        }
        break;
    case QUALITY_BLUE:
        {
            monsterQualityPropSet.Set(PROP_MAXHP_MUL, 30000);
            monsterQualityPropSet.Set(PROP_HP_REC_RATE_MUL, 20000);
        }
        break;
    case QUALITY_PURPLE:
        {
            monsterQualityPropSet.Set(PROP_MAXHP_MUL, 70000);
            monsterQualityPropSet.Set(PROP_HP_REC_RATE_MUL, 30000);
        }
        break;
    case QUALITY_GOLD:
        {
            monsterQualityPropSet.Set(PROP_MAXHP_MUL, 150000);
            monsterQualityPropSet.Set(PROP_HP_REC_RATE_MUL, 40000);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    // 随机怪物品质属性
    const std::vector<MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT>& modifierList = MONSTER_QUALITY_MODIFIER_ENTRY::Instance()->GetStruct().modifier_list;
    std::vector<uint32> allModifierIDs;
    for(uint32 i=0; i<modifierList.size(); i++)
    {
        allModifierIDs.push_back(modifierList[i].id);
    }

    int32 modifierNum = m_monsterData.quality;
    for(int32 i=0; i<modifierNum; i++)
    {
        int32 index = Crown::SDRandom(allModifierIDs.size());
        uint32 modifierId = allModifierIDs[index];
        m_ModifierIDs.push_back(modifierId);
        allModifierIDs.erase(allModifierIDs.begin() + index);
    }

    // 设置怪物品质属性
    for(uint32 i=0; i<m_ModifierIDs.size(); i++)
    {
        uint32 modifierId = m_ModifierIDs[i];
        if(modifierId == 0)
            continue;
        const MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT* modifierConfig = MonsterConfigManager::Instance()->GetMonsterQualityModifierById(modifierId);
        if(!modifierConfig)
        {
            CnAssert(false);
            continue;
        }

        if(modifierConfig->prop1_id > 0 && modifierConfig->prop1_value > 0)
        {
            monsterQualityPropSet.Add(modifierConfig->prop1_id, modifierConfig->prop1_value);
        }
        if(modifierConfig->prop2_id > 0 && modifierConfig->prop2_value > 0)
        {
            monsterQualityPropSet.Add(modifierConfig->prop2_id, modifierConfig->prop2_value);
        }
    }

    // 怪物属性汇总
    GetPropertySetCalculater()->CalcFinalProp();


    // 初始满血满蓝
    SetHP(GetMaxHp());
    SetMP(GetMaxMp());
    SetEnergyShield(GetMaxEnergyShield());

    SetRadius(m_monsterData.radius);
    SetShowFlag(m_monsterData.showflag);
}

void Monster::Update(uint32 frame_count)
{
    m_IsSlowTime = !GetScene()->IsPlayerSee(this);

    // 暂时取消慢时钟功能
    //if (m_IsSlowTime)
    //{
    //    m_AccumulateTick++;
    //    if (m_AccumulateTick < AI_SLOW_UPDATE_TICK)       // 慢时钟下
    //    {
    //        return;
    //    }
    //    m_AccumulateTick = 0;
    //}

    Creature::Update(frame_count);

    // 仇恨处理
    if (m_HatredManager)
    {
        m_HatredManager->Update(frame_count);
    }
}

void Monster::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    Creature::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case WORD_MAP_PLYAER_OFFLINE:
        {
            uint64 playerid = U64ID(arg1, arg2);
            if (m_HatredManager)
            {
                m_HatredManager->ClearHatredByPlayreID(playerid);
            }
        }
        break;
    default:
        break;
    }
}

void Monster::OnDead(BattleUnit* killer)
{
    Creature::OnDead(killer);
    if (killer)
    {
        Player* gainer = NULL;  // 经验和精魂获得者
        if (killer->IsKindOf(&Pet::ms_RTTI))
        {
            BattleUnit* master = ((Pet*)killer)->GetMaster();
            if (master && master->IsKindOf(&Player::ms_RTTI))
            {
                gainer = (Player*)master;
            }
        }
        else if (killer->IsKindOf(&Player::ms_RTTI))
        {
            gainer = ((Player*)killer);
        }

        if (gainer)
        {
            // 获得经验
            //if (m_monsterData.exp > 0)
            //{
            //    gainer->AddExp(gainer->GetBuffController()->CanDoubleExp() * m_monsterData.exp);
            //    gainer->GetTeamManager()->ShareExpToTeamInSameScene(m_monsterData.exp);
            //}

            // 获得精魂
            //int32 soul_count = m_monsterData.soul;
            //if (soul_count > 0)
            //{
            //    gainer->AddSoulPoint(soul_count, REWARD_REASON_KILL_MONSTER_DROP);
            //    gainer->GetTeamManager()->ShareSoulPointToTeamInSameScene(soul_count, REWARD_REASON_KILL_MONSTER_DROP);
            //}

            // 获得金币
            //int32 game_point = m_monsterData.gamePoint;
            //if (game_point > 0)
            //{
            //    gainer->AddGamePoint(game_point, REWARD_REASON_KILL_MONSTER_DROP);
            //    gainer->GetTeamManager()->ShareGamePointToTeamInSameScene(game_point, REWARD_REASON_KILL_MONSTER_DROP);
            //}

            // 掉落物品
            //bool isDrop = false;
            //for (std::vector<MonsterDropData>::const_iterator dropIt = m_monsterData.dropList.begin(); dropIt != m_monsterData.dropList.end(); dropIt++)
            //{
            //    const MonsterDropData& dropData = *dropIt;
            //    if (dropData.probability >= Crown::SDRandom(BASE_RATE_NUM))
            //    {
            //        gainer->GetBag()->AddItemByNum(dropData.itemId, dropData.num, REWARD_REASON_KILL_MONSTER_DROP);
            //        isDrop = true;
            //    }
            //}

            // kill通知
            gainer->SendTaskEvent(TASK_EVENT_KILL_MONSTER, m_monsterData.creatureId, 1, gainer->GetUnionMapID());     // 通知任务
            gainer->GetTeamManager()->SendTaskEventToTeamInSameScene(TASK_EVENT_KILL_MONSTER, m_monsterData.creatureId, 1, gainer->GetUnionMapID());      // 队友共享杀怪任务

        }
    }


    if(m_BirthSpawn)
    {
        m_BirthSpawn->OnMonsterDead(this);
    }

    GetScene()->SendEvent(0, BATTLE_EVENT_MONSTER_DEAD, GetId(), (killer ? killer->GetId() : 0), 0);
}

// 怪物析构时
void Monster::OnDestory()
{
    if (m_HatredManager != NULL)
    {
        CnDelete m_HatredManager;
        m_HatredManager = NULL;
    }

    Creature::OnDestory();

    if (m_BirthSpawn)
    {
        m_BirthSpawn->OnMonsterDestory(this);
    }
}

void Monster::ConnectSpawn(Spawn* pSpawn)
{
    CnAssert(!m_BirthSpawn);
    m_BirthSpawn = pSpawn;
}

void Monster::Damage(const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill /* = NULL */)
{
    Creature::Damage(damage, pAttacker, pSkill);

    if (m_HatredManager)
    {
        m_HatredManager->OnDamage(damage, pAttacker);
    }
}




