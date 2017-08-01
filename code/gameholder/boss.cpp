#include "gameholder_pch.h"
#include "boss.h"
#include "ai_controller.h"
#include "game_define.h"
#include "scene.h"
#include "event_define.h"
#include "player.h"
#include "pet.h"
#include "relife_helper.h"
#include "regular.h"
#include "world_prop_communicator.h"
#include "hatred_manager.h"
#include "player_team_manager.h"
#include "buff_controller.h"
#include "property_set_calculater.h"


CnImplementRTTI(Boss, Creature);

Boss::Boss() : Creature(OBJ_BOSS)
{
    m_SceondTimer = Crown::SDRandom(500);
    m_BossUI = false;
    m_HatredManager = CnNew HatredManager(this);
    m_lifetime = 0;
    m_createTime = 0;
}

Boss::~Boss()
{
}

void Boss::InitBoss(const BossData &data)
{
    m_bossData = data;

    SetLevel(m_bossData.level);
    SetRadius(m_bossData.radius);

    CnAssert(false);            // 暂无boss对象

    //PropertySet& monsterLevelPropSet = GetPropertySetCalculater()->GetLevelProp();
    //monsterLevelPropSet.Set(PROP_MAXHP, m_bossData.hp);
    //monsterLevelPropSet.Set(PROP_MAXMP, m_bossData.mp);
    //monsterLevelPropSet.Set(PROP_MAX_ENERGY_SHIELD, m_bossData.energyShield);
    //monsterLevelPropSet.Set(PROP_HP_REC_RATE, m_bossData.hpRecovery);
    //monsterLevelPropSet.Set(PROP_MP_REC_RATE, m_bossData.mpRecovery);
    //monsterLevelPropSet.Set(PROP_MOVE_SPEED, m_bossData.moveSpeed);
    //monsterLevelPropSet.Set(PROP_ATTACK_PER_SEC, m_bossData.attackSpeed);
    //monsterLevelPropSet.Set(PROP_CAST_PER_SEC, m_bossData.castSpeed);
    //monsterLevelPropSet.Set(PROP_HIT_RATE, m_bossData.hit);
    //monsterLevelPropSet.Set(PROP_DODGE, m_bossData.dodge);
    //monsterLevelPropSet.Set(PROP_CRITICAL_ATTACK, m_bossData.crit);
    //monsterLevelPropSet.Set(PROP_CRITICAL_DAMAGE_ADDITION, m_bossData.critDamage);
    //monsterLevelPropSet.Set(PROP_ARMOR, m_bossData.armor);
    //monsterLevelPropSet.Set(PROP_FIRE_DEFENCE, m_bossData.fireResistance);
    //monsterLevelPropSet.Set(PROP_ICE_DEFENCE, m_bossData.frostResistance);
    //monsterLevelPropSet.Set(PROP_THUNDER_DEFENCE, m_bossData.lightningResistance);
    //monsterLevelPropSet.Set(PROP_BLOCK, m_bossData.attackBlock);
    //monsterLevelPropSet.Set(PROP_MAGIC_BLOCK, m_bossData.magicBlock);

    //GetPropertySetCalculater()->CalcFinalProp();

    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_MAXHP, m_bossData.hp);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_HP_REC_RATE, m_bossData.hpRate);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_MOVE_SPEED, m_bossData.speed);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_HIT_RATE, m_bossData.hitRate);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_DODGE, m_bossData.dodge);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_CRITICAL_ATTACK, m_bossData.criticalAttack);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_CRITICAL_DAMAGE_ADDITION, m_bossData.criticalDamageAddition);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_ARMOR, m_bossData.defence);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_FIRE_DEFENCE, m_bossData.fireDefence);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_ICE_DEFENCE, m_bossData.iceDefence);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_THUNDER_DEFENCE, m_bossData.thunderDefence);
    //GetPropertySetCalculater()->SetPropUnit(PROP_UNIT_BASE, PROP_ATTACK_PER_SEC, m_bossData.attackRate);
    //GetPropertySetCalculater()->SetFinalProp();
    
    SetHP(m_bossData.hp);
}

void Boss::Update(uint32 frame_count)
{
    Creature::Update(frame_count);

    if (m_HatredManager)
    {
        m_HatredManager->Update(frame_count);
    }
}

void Boss::SetReincarnated(const ReincarnatedList& list)
{
    m_ReincarnatedList = list;
}

bool Boss::BeforeDead()
{
    // 如果有转生链，则创建转生Boss，并销毁此对象
    ReincarnatedData data = m_ReincarnatedList.Pop();
    if (data.IsClear())
    {
        return true;           // 不转生了，死亡
    }
    else
    {
        // 创建转生Boss
        bool nextBossUI = GetBossUI();
        if (data.m_Num > 1)
        {
            nextBossUI = false;
        }

        for (int32 bi=0; bi<data.m_Num; bi++)
        {
            // 位置
            CoVec2 newPos;
            if (GetCantMove())
            {
                newPos = GetPosition();
            }
            else
            {
                newPos = GetScene()->FindValidPositionInCircle(CoVec2(GetX(), GetY()), 500, GetRadius());
            }

            Boss* pNewBoss = GetScene()->GetRegular()->AddBoss(data.m_BossId, (int32)newPos.x, (int32)newPos.y, GetAngle(), nextBossUI);
            if (!pNewBoss)
            {
                CnWarn("dungeon %d reincarnated bossid %d not failed\n", GetScene()->GetDungeonID(), data.m_BossId);
                return true;
            }

            // 属性传递
            pNewBoss->SetCantAngle(GetCantAngle());
            pNewBoss->SetAttackable(GetAttackable());
            pNewBoss->SetCantMove(GetCantMove());
            pNewBoss->SetCantBuff(GetCantBuff());
            pNewBoss->SetCantHeal(GetCantHeal());
            pNewBoss->SetCantTaunt(GetCantTaunt());
            pNewBoss->SetCantPush(GetCantPush());
            if (data.m_AIName.length() > 0)
            {
                pNewBoss->GetAIController()->LoadAI( data.m_AIName );
            }
            pNewBoss->SetReincarnated(m_ReincarnatedList);  // 传递转生链
        }

        SetDestory();
        return false;       // 不死亡，销毁掉
    }
}

void Boss::OnDead(BattleUnit* killer)
{
    Creature::OnDead(killer);
    
    if (killer)
    {
        Player* gainer = NULL;  // 精魂获得者
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
            if (m_bossData.exp > 0)
            {
                int32 nMultiple(BASE_RATE_NUM);
                gainer->GetBuffController()->IsExpMultiple(&nMultiple);
                gainer->AddExp(m_bossData.exp * nMultiple / BASE_RATE_NUM);
                gainer->GetTeamManager()->ShareExpToTeamInSameScene(m_bossData.exp);
            }

            // 获得精魂
            int32 soul_count = m_bossData.soul;
            if (soul_count > 0)
            {
                gainer->AddSoulPoint(soul_count, REWARD_REASON_KILL_MONSTER_DROP);
                gainer->GetTeamManager()->ShareSoulPointToTeamInSameScene(soul_count, REWARD_REASON_KILL_MONSTER_DROP);
            }
            
            // 获得金币
            int32 game_point = m_bossData.gamePoint;
            if (game_point > 0)
            {
                gainer->AddGamePoint(m_bossData.gamePoint, REWARD_REASON_KILL_MONSTER_DROP);
                gainer->GetTeamManager()->ShareGamePointToTeamInSameScene(game_point, REWARD_REASON_KILL_MONSTER_DROP);
            }

            // kill通知
            gainer->SendTaskEvent(TASK_EVENT_KILL_MONSTER, m_bossData.creatureId, 1, gainer->GetUnionMapID());    // 通知任务

        }
    }

    GetScene()->SendEvent(0, BATTLE_EVENT_BOSS_DEAD, m_bossData.creatureId, killer? killer->GetId(): 0, 0);
}

void Boss::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
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

uint32 Boss::GetCorpseTime()
{
    //if (m_bossData.state != kBOSS_SATE_DEAD)
    //{
    //    return false;
    //}

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    // uint32 scourge_corpse_time = WorldMapConfig::Instance()->GetBossScourgeCorpseDuration();// Boss鞭尸持续时间

    //return now < (m_bossData.deadTime+scourge_corpse_time);

   // uint32 scourge_corpse_time = m_bossData.bossData.lifetime; 
   // uint32 ng = m_bossData.createTime+scourge_corpse_time;
    uint32 ng = m_lifetime + m_createTime;
    uint32 howtime  = 0;
    if (now < ng)
    {
        howtime = ng - now;
        return howtime;
    }
    else 
    {
        return 0;
    }
}

void Boss::OnDestory()
{
    if (m_communicator)
    {
        m_communicator->OnDestroy();    // 一个频道的Boss销毁，其他频道的Boss都销毁
    }

    if (m_HatredManager != NULL)
    {
        CnDelete m_HatredManager;
        m_HatredManager = NULL;
    }

    m_lifetime = 0;
    m_createTime = 0;

    Creature::OnDestory();
}

//uint32 Boss::GetDefaultSkill()
//{
//    return MONSTER_DEFAULT_SKILLID_START;
//}

void Boss::Damage(const DamageResult& damage, BattleUnit* pAttacker, SkillObject* pSkill)
{
    Creature::Damage(damage, pAttacker, pSkill);

    if (m_HatredManager)
    {
        m_HatredManager->OnDamage(damage, pAttacker);       // 仇恨系统
    }
}


