#include "gameholder_pch.h"
#include "regular_field_boss.h"
#include "void_field_manager.h"
#include "void_field_config.h"
#include "pet.h"
#include "event_define.h"
#include "scene.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "game_util.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "item_manager.h"
#include "trigger.h"
#include "monster.h"
#include "spawn.h"
#include "setting_manager.h"
#include "drop_object.h"
#include "boss.h"
#include "regular_event.h"
#include "event_define.h"
#include "battle_unit.h"
#include "scene.h"
#include "checkpoint.h"
#include "player.h"
#include "boss.h"
#include "ai_controller.h"
#include "enemy_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "guild_manager.h"
#include "world_boss_manager.h"
#include "game_util.h"
#include "sub_regular_hook.h"
#include "sub_regular_boss.h"
#include "sub_regular_monster.h"


CnImplementRTTI(RegularFiledBoss, RegularWorld);

RegularFiledBoss::RegularFiledBoss(RegularDoc* pDoc) :
RegularWorld(pDoc)
{
    m_regular_hook = CnNew SubRegularHook(this);
    m_regular_boss = CnNew SubRegularBoss(this);
    m_regular_monster = CnNew SubRegularMonster(this);     // 打小怪掉宝箱

    AddSubRegular(m_regular_hook);
    AddSubRegular(m_regular_boss);
    AddSubRegular(m_regular_monster);
}

RegularFiledBoss::~RegularFiledBoss()
{
}

void RegularFiledBoss::OneSecondUpdate( int32 deltatime )
{
    RegularWorld::OneSecondUpdate(deltatime);
}

void RegularFiledBoss::OnCreatureEnter( BattleUnit* creature )
{
    RegularWorld::OnCreatureEnter(creature);
    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        AddRecord(((Player*)creature));
    }
}

void RegularFiledBoss::OnCreatureLeave( BattleUnit* creature )
{
    RegularWorld::OnCreatureLeave(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        DelRecord(((Player*)creature));
    }
}

void RegularFiledBoss::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularWorld::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

void RegularFiledBoss::Update(uint32 curr_tick)
{
    RegularWorld::Update(curr_tick);
}

bool RegularFiledBoss::IsEnemy(BattleUnit* attacker, BattleUnit* defencer)
{
   Player* attackPlayer    = NULL;
   Player* defencePlayer   = NULL;
   Pet* pet          = NULL;
   ENEMY_RECORD* record    = NULL;
   EnemyManager* attackEnemy     = NULL;

   if(!attacker || !defencer || (attacker == defencer))
   {
       return false;
   }

   switch(attacker->GetObjectType())
   {
   case OBJ_PLAYER:            // 玩家
   case OBJ_OFFLINE_PLAYER:    // 离线玩家
   case OBJ_PET:        // 战宠
       {
           if((defencer->GetObjectType() == OBJ_MONSTER) || 
               (defencer->GetObjectType() == OBJ_BOSS))
           {
               return true;
           }
       }
       break;
   case OBJ_MONSTER:           // 怪物
   case OBJ_BOSS:              // BOSS
       {
           if((defencer->GetObjectType() == OBJ_MONSTER) || 
              (defencer->GetObjectType() == OBJ_BOSS))
           {
               return false;
           }
           return true;
       }
       break;
   default:                    // 不应该
       {
           CnAssert(false);
           return false;
       }
       break;
   }

   switch(defencer->GetObjectType())
   {
   case OBJ_PLAYER:            // 玩家
   case OBJ_OFFLINE_PLAYER:    // 离线玩家
   case OBJ_PET:        // 战宠
       break;
   default:                    // 不应该
       {
           CnAssert(false);
           return false;
       }
       break;
   }

   // 获得攻击者指针
   if(attacker->GetObjectType() == OBJ_PET)
   {
       pet = CnDynamicCast(Pet, attacker);
       attackPlayer = CnDynamicCast(Player, pet->GetMaster());
   }
   else
   {
       attackPlayer = CnDynamicCast(Player, attacker);
   }

   // 获得防御者指针
   if(defencer->GetObjectType() == OBJ_PET)
   {
       pet = CnDynamicCast(Pet, defencer);
       defencePlayer = CnDynamicCast(Player, pet->GetMaster());
   }
   else
   {
       defencePlayer = CnDynamicCast(Player, defencer);
   }

   if(!attackPlayer || !defencePlayer)
       return false;

   if (attackPlayer == defencePlayer)
       return false;

   attackEnemy = attackPlayer->GetEnemyMgr();
   if(!attackEnemy)
       return false;

   record = attackEnemy->GetEnemyRecord(defencePlayer->GetPlayerID());
   if(record)
   {
       if(record->flag == ENEMY_FLAG_ATTACK || record->flag == ENEMY_FLAG_FIGHTBACK)
       {
           return true;
       }
   }

   switch(attackEnemy->GetFightMode())
   {
   case FIGHT_MODE_PEACE:         // 和平模式
       {
           return false;
       }
       break;
   case FIGHT_MODE_KILLEVIL:      // 除恶模式
       {   // 检查是否为红名玩家
           if(!defencePlayer->IsRedName())
           {
               return false;
           }
       }
       break;
   case FIGHT_MODE_GUILD:         // 帮会模式
       {   // 无工会，则与任何人都敌对
           if(!attackPlayer->GetGuildID() || !defencePlayer->GetGuildID())
           {
               return true;
           }
           // 检查公会是否相同
           if(attackPlayer->GetGuildID() == defencePlayer->GetGuildID())
           {
               return false;
           }
       }
       break;
   case FIGHT_MODE_MASSACRE:      // 屠杀模式
       break;
   default:
       return false;
       break;
   }

   return true;
}

void RegularFiledBoss::FillStartPostion(Player* player, uint32 triggerID)
{
    Trigger* trigger = 0;

    if(!player) return;

    triggerID = CalPlayerTriggerID(player);
    trigger = m_pScene->GetTriggerByID(triggerID);
    if(!trigger)
    {   // 如果策划少配了出生点, 就从出生点 1 出生
        trigger = m_pScene->GetTriggerByID(1);
    }

    if(!trigger)
    {
        CnAssert(false);
        return;
    }

    CoVec2 randPos = GetScene()->FindValidPositionInCircle(trigger->GetPosition(), trigger->GetRadius(), player->GetRadius()); // trigger范围内查找一个有用的随机点
    player->SetPosition((int32)randPos.x, (int32)randPos.y);
    player->SetAngle(trigger->GetAngle());
}

void RegularFiledBoss::OnPlayerRecv(Player* player, uint32 msgID, void* data)
{
    //switch(msgID)
    //{
    //case MSG_FIELD_BOSS_FRESH_TIME_REQ: //处理逻辑其他消息
    //    {
    //        FIELD_BOSS_FRESH_TIME_ACK sendMsg;
    //        sendMsg.time = GetFreshBossTime();
    //        player->SendToGate(MSG_FIELD_BOSS_FRESH_TIME_ACK, &sendMsg);
    //    }
    //    break;
    //default:
    //    break;
    //}
}

WorldMapPlayerRecord* RegularFiledBoss::GetRecord(Player* player)
{
    std::map<uint64, WorldMapPlayerRecord>::iterator playerIt;

    if(!player)
        return NULL;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt != m_recordMap.end())
    {
        return &playerIt->second;
    }

    return NULL;
}

Player* RegularFiledBoss::GetPlayerByUnitID(uint32 uintID)
{
    BattleUnit* battleUnit = NULL;
    Pet*  battlePet  = NULL;
    Player*     battlePlayer = NULL;

    battleUnit = GetScene()->GetBattleUnit(uintID);
    if (!battleUnit)
    {
        return battlePlayer;
    }

    if (battleUnit->GetObjectType() == OBJ_PLAYER || battleUnit->GetObjectType() == OBJ_OFFLINE_PLAYER)
    {
        battlePlayer = (Player*)battleUnit;
    }
    else if (battleUnit->GetObjectType() == OBJ_PET)
    {
        battlePet = (Pet*)battleUnit;
        if (battlePet)
        {
            battlePlayer = (Player*)battlePet->GetMaster();
        }
    }

    return battlePlayer;
}

bool RegularFiledBoss::IsInBossLeakTime()
{
    SubRegularBoss* regular = static_cast<SubRegularBoss*>(m_regular_boss);
    if (regular)
    {
        return regular->IsInBossLeakTime();
    }

    return false;
}

std::string RegularFiledBoss::GetBossKillerName()
{
    SubRegularBoss* regular = static_cast<SubRegularBoss*>(m_regular_boss);
    if (regular)
    {
       return regular->GetBossKillerName();
    }

    return "";
}

uint32 RegularFiledBoss::GetFreshBossTime()
{

    SubRegularBoss* regular = static_cast<SubRegularBoss*>(m_regular_boss);
    if (regular)
    {
        if(m_regular_boss)
        return regular->GetFreshBossTime();
    }

    return 0;
}
void RegularFiledBoss::ClearBossDamage()
 {

 }
void RegularFiledBoss::AddRecord(Player* player)
{
    int32 interval_time = VoidFieldConfig::Instance()->GetRewardExpInterval(1);
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    std::map<uint64, WorldMapPlayerRecord>::const_iterator playerIt;
    WorldMapPlayerRecord record;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt == m_recordMap.end())
    {
        record.Clear();
        record.player_id = player->GetPlayerID();
        record.add_exp_time_point = now + interval_time;
        m_recordMap[player->GetPlayerID()] = record;
    }
    else
    {
        CnAssert(false);
        CnError("Duplicate player record\n");
    }
}

void RegularFiledBoss::DelRecord(Player* player)
{
    std::map<uint64, WorldMapPlayerRecord>::iterator playerIt;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt != m_recordMap.end())
    {
        m_recordMap.erase(playerIt);
    }
    else
    {
        CnAssert(false);
        CnError("Duplicate player record\n");
    }
}

uint32 RegularFiledBoss::CalPlayerTriggerID(Player* player)
{
    uint32 triggerID = 0;

    // 根据等级随机出生点
    if(player->GetLevel() < 50)
    {
        triggerID = Crown::SDRandom(4) + 1;
    }
    else if(player->GetLevel() < 60)
    {
        triggerID = Crown::SDRandom(3) + 5;
    }
    else
    {
        triggerID = Crown::SDRandom(2) + 8;
    }

    return triggerID;
}
