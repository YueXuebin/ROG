#include "gameholder_pch.h"
#include "sub_regular_boss.h"
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
#include "world_map_config.h"
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
#include "void_field_config.h"
#include "regular_field_boss.h"
#include "center_handler.h"
#include "server_config.h"
#include "hatred_manager.h"
#include "dungeon.h"
#include "scene.h"

const uint32 TEST_BOSS_DURATION = 30;

SubRegularBoss::SubRegularBoss(Regular* pRegular) :
SubRegular(pRegular)
{
    m_masterRegular = CnDynamicCast(RegularFiledBoss, pRegular);
    CnAssert(m_masterRegular);

    m_bossData.Clear();

    m_next_refresh_time = 0;
    m_orderid = 0;
    m_how_time = 0;
}

SubRegularBoss::~SubRegularBoss()
{
}

void SubRegularBoss::OneSecondUpdate( int32 deltatime )
{
    // 定时刷 Boss
    BossRefreshCheck();

    // 重置 Boss 伤害值
    ResetDamageByPlayerState();

    // 通知 Boss 伤害值
    DamageRankNtf();

    // 检查 Boss 销毁
    BossDestroyCheck();

    // 检查 Boss 鞭尸奖励
    ScourgeCorpseCheck();

    //设置boss的攻击目标
    SetAttackTarget();
}

void SubRegularBoss::OnCreatureEnter( BattleUnit* creature )
{
}

void SubRegularBoss::OnCreatureLeave( BattleUnit* creature )
{
    SubRegular::OnCreatureLeave(creature);

    if (creature->GetObjectType() == OBJ_PLAYER)
    {
        Player *player = static_cast<Player*>(creature);
        if(player)
        {
            SendLeveSene(player);

            LeveSceneDropItem(player); //掉落物品奖励
        }
    }
}


void SubRegularBoss ::SendLeveSene(Player *player)
{
    if(!player) return;

    NONE sendMsg;
    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    HatredInfo* hatred_info = hatred_mgr->GetHatredInfo(player->GetId());
    if (!hatred_info) return;

    if(m_bossData.state == kBOSS_SATE_ALIVE)
        player->SendToGate(MSG_FIELD_BOSS_PLAYER_LEAVE, &sendMsg);//离开所有的大地图场景在发消息
}
void SubRegularBoss::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    SubRegular::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_BOSS_DEAD:
        {
            if((uint32)arg1 == m_bossData.bossID)
            {
                OnBossDeadProcess((uint32)arg2);
            }
        }
        break;
    case BATTLE_EVENT_DAMAGE:
        {
            uint32 defencerID = arg3;
            if(defencerID == m_bossData.bossCreatureID)
            {
                if (m_bossData.state == kBOSS_SATE_ALIVE)
                {
                    OnDamage(arg1, arg2, defencerID);
                }
                else if (m_bossData.state == kBOSS_SATE_DEAD)
                {
                    OnBossCorpseProcess(arg1, arg2);
                }
            }
        }
        break;
    case BATTLE_EVENT_PLAYER_DEAD:
        {
            OnBattleEventKill(arg2, arg1);
        }
        break;
    case SCENE_EVENT_BOSS_DEAD_DROP_ITEM:
        {
            uint32 bosscreateid = arg3;
            BossDeadDropItemEvent(arg1, arg2, bosscreateid);
        }
        break;
    case SCENE_EVENT_SCOURGE_CORPSE_DROP_ITEM:
        {
            uint32 bosscreateid = arg3;
            CorpseBossDropEvent(arg1, arg2, bosscreateid);
        }
        break;
        break;
    default:
        break;
    }
}

void SubRegularBoss::Update(uint32 curr_tick)
{
    SubRegular::Update(curr_tick);
    CheckDropItemTime();
}


void SubRegularBoss::CheckDropItemTime()
{
    //FIELD_BOSS_UPDATE_DROP_STATE_ACK sendMsg;
    //sendMsg.Clear();

    //uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
    //Scene::DropObjectMap& dropmap = GetScene()->GetDropItemsMap();
    //Scene::DropObjectMap::iterator dropiter;
    //for(dropiter = dropmap.begin(); dropiter != dropmap.end(); dropiter++)
    //{
    //    DropObject* dropObj = dropiter->second;
    //    if(!dropObj) continue; 
    //    if(dropObj->GetFromtype() == OBJ_BOSS)
    //    {
    //        if(!dropObj->isDestory() &&  curtime >= dropObj->GetCanPickTime()&& dropObj->GetPlayerObjectID())
    //        {
    //            dropObj->SetArriveTime(1);
    //            sendMsg.object_id = dropObj->GetId();
    //            sendMsg.arrive_time = dropObj->GetArriveTime();
    //            Scene* curscene = GetScene();
    //            if(curscene)
    //                curscene->NotifyMessage(dropObj,MSG_FIELD_BOSS_UPDATE_DROP_STATE_ACK, &sendMsg);//某对象的范围通知
    //        }
    //    }
    //}
}

void SubRegularBoss::LeveSceneDropItem(Player* player)
{
    if(!player)
        return;
    //玩家离开场景，其他人也能捡我自己物品

    FIELD_BOSS_UPDATE_DROP_STATE_ACK sendMsg;
    sendMsg.Clear();

    //uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
    //Scene::DropObjectMap& dropmap = GetScene()->GetDropItemsMap();
    //Scene::DropObjectMap::iterator dropiter;
    //for(dropiter = dropmap.begin(); dropiter != dropmap.end(); dropiter++)
    //{
    //    DropObject* dropObj = dropiter->second;
    //    if(!dropObj) continue; 
    //    if(dropObj->GetFromtype() == OBJ_BOSS)
    //    {
    //        if(dropObj->GetPlayerObjectID() == player->GetId())
    //        {
    //            dropObj->SetArriveTime(1);
    //            sendMsg.object_id = dropObj->GetId();
    //            sendMsg.arrive_time = dropObj->GetArriveTime();
    //            Scene* curscene = GetScene();
    //            if(curscene)
    //                curscene->NotifyMessage(dropObj,MSG_FIELD_BOSS_UPDATE_DROP_STATE_ACK, &sendMsg);//某对象的范围通知
    //        }
    //    }
    //}

}

void SubRegularBoss::OnFieldBossScourgeCorpseReq(Player* player, uint32 msgID, void* data)
{
    m_scourgeCorpseMap[player->GetPlayerID()] = true;
}

bool SubRegularBoss::IsInBossLeakTime()
{
    if (m_bossData.state ==kBOSS_SATE_NONE || m_bossData.state == kBOSS_SATE_ALIVE)
    {
        return false;
    }

    uint64 now = Crown::SDNow().GetTimeValue();
    uint32 leak_time = WorldMapConfig::Instance()->GetBossLeakDuration();// Boss霸气侧漏持续时间
    return now < (m_bossData.createTime+leak_time);
}

std::string SubRegularBoss::GetBossKillerName()
{
    return m_bossData.killer_name;
}

bool SubRegularBoss::IsInBossScourgeCorpseTime()
{
    if (m_bossData.state != kBOSS_SATE_DEAD)
    {
        return false;
    }

    //GetCorpseTime();

    Scene::BossMap::iterator it;
    it= GetScene()->GetBosses().find(m_bossData.bossCreatureID);
    if(it != GetScene()->GetBosses().end())
    {
        Boss* curboss = it->second;
        if(curboss)
        {
            CnWarn("test time%u", curboss->GetCorpseTime());
            return curboss->GetCorpseTime();
        }
    }


    return false;

   // uint32 now = (uint32)Crown::SDNow().GetTimeValue();
   //// uint32 scourge_corpse_time = WorldMapConfig::Instance()->GetBossScourgeCorpseDuration();// Boss鞭尸持续时间

   // //return now < (m_bossData.deadTime+scourge_corpse_time);

   // uint32 scourge_corpse_time = m_bossData.bossData.lifetime; 
   // uint32 ng = m_bossData.createTime+scourge_corpse_time;
   // uint32 howtime  = 0;
   // if (now < ng)
   // {
   //     howtime = ng - now;
   //     return true;
   // }
   // else 
   // {
   //     return false;
   // }
}

bool SubRegularBoss::NeedRefreshBoss()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    return now >= CalNextRefreshBossTime();
}

void SubRegularBoss::CreateBoss()
{
    Trigger* pTrigger = GetScene()->GetFirstTrigger();
    if(!pTrigger)
        return;

    // 检查是否有这个 BOSS
    m_bossData.bossID = WorldMapConfig::Instance()->GetBossIDByUnionID(GetScene()->GetUnionID());
    CnAssert(m_bossData.bossID);

    m_bossData.bossData.creatureId = m_bossData.bossID;
    m_bossData.bossData.x = pTrigger->GetX() + 200;
    m_bossData.bossData.y = pTrigger->GetY() + 200;
    m_bossData.bossData.angle = pTrigger->GetAngle();
    m_bossData.bossData.cantMove = false;
    m_bossData.bossData.cantBuff = true;
    m_bossData.bossData.bossui = true;
    m_bossData.bossData.worldboss = true;
    m_bossData.bossData.b_corpse = false;
    m_bossData.bossData.name = "world_map_" + Crown::ToString(m_bossData.bossData.creatureId);
    Boss* newBoss = m_Regular->AddBoss(m_bossData.bossData, "", false);

    if(newBoss == NULL)
    {
        CnError("world boss id:%d is not exist\n", m_bossData.bossData.creatureId);
        return;
    }

    newBoss->GetAIController()->LoadAI("ai/active_atk_skill_boss.aix");

    m_bossData.bossCreatureID = newBoss->GetId();

    // Boss 创建时间
   // m_bossData.createTime = (uint32)Crown::SDNow().GetTimeValue();

   CnWarn("create  boss %u\n",m_bossData.bossID);
   uint32 dugeionid = m_Regular->GetScene()->GetDungeon()->GetDungeonID();
   CnWarn("create  dugenonid %u\n",dugeionid);

    m_bossData.state = kBOSS_SATE_ALIVE;
}

void SubRegularBoss::BossRefreshCheck()
{
        // if (NeedRefreshBoss())
    if (JudgeIsCreateBoss())
    {
        DestroyBoss();
        if (m_bossData.state == kBOSS_SATE_NONE || m_bossData.state == kBOSS_SATE_DESTROY)
        {
            CreateBoss();
           // CnWarn("create boss%u\n",m_bossData.bossCreatureID);
        }
    }

    //if (m_bossData.state == kBOSS_SATE_NONE || m_bossData.state == kBOSS_SATE_DESTROY)
    //{
    //    //定时刷新后清除玩家仇恨值
    //    //HatredManager* hatred_mgr = GetBossHatredMgr();
    //    //if (!hatred_mgr) return;
    //    //hatred_mgr->ClearAllHatreList();

    //    CreateBoss();
    //    
    //    m_test = true;

    //   CnWarn("create boss%u\n",m_bossData.bossCreatureID);
    //}
}

void SubRegularBoss::DestroyBoss()
{
    // 删除 BOSS
    if(m_bossData.bossCreatureID)
    {
        GetScene()->DelBoss(m_bossData.bossCreatureID);
        m_bossData.bossCreatureID = 0;
    }

    // Boss销毁
    m_bossData.state = kBOSS_SATE_DESTROY;
}

void SubRegularBoss::BossReincarnation()
{

    // 检查是否有这个 BOSS
    m_bossData.bossID = WorldMapConfig::Instance()->GetBossIDByUnionID(GetScene()->GetUnionID());
    CnAssert(m_bossData.bossID);

    m_bossData.bossData.creatureId = m_bossData.bossID;
    m_bossData.bossData.cantMove = true;
    m_bossData.bossData.cantAngle = false;
    m_bossData.bossData.cantBuff = false;
    m_bossData.bossData.cantPush = false;
    m_bossData.bossData.pathloop = false;
    m_bossData.bossData.attackable = true;
    m_bossData.bossData.bossui = false;
    m_bossData.bossData.worldboss = true;
    m_bossData.bossData.b_corpse = true;
    m_bossData.bossData.lifetime  = WorldMapConfig::Instance()->GetBossScourgeCorpseDuration();
    m_bossData.bossData.name = "world_map_" + Crown::ToString(m_bossData.bossData.creatureId);

    Boss* newBoss = m_Regular->AddBoss(m_bossData.bossData, "", false);

   // m_Regular->GetScene()->GetDungeon()->GetDungeonID();
    if(newBoss == NULL)
    {
        CnError("world boss id:%d is not exist\n", m_bossData.bossData.creatureId);
        return;
    }

    m_bossData.bossCreatureID = newBoss->GetId();
    m_bossData.createTime = newBoss->GetCreateTime();

      //  uint32 ss = newBoss->GetLifeTime();

    // CnWarn("create Reincarnation boss %u\n",m_bossData.bossCreatureID);
}

bool SubRegularBoss::JudgeIsCreateBoss()
{
    Crown::CSDDateTime today_time = Crown::SDNow();//今日凌晨时间点
    today_time.SetTime(0, 0, 0);

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    //CnWarn("cur time:%d \n", now);

    const WorldMapConfig::RefreshTimeMap& timeMap = WorldMapConfig::Instance()->GetRefreshTimeConfig();
    if (m_next_refresh_time == 0)
    {
        CnAssert(timeMap.begin() != timeMap.end());
        bool b_exe = false;
        WorldMapConfig::RefreshTimeMap::const_iterator timeIter;//必须从小大大排序
        for (timeIter = timeMap.begin(); timeIter != timeMap.end(); ++timeIter)
        {
            uint32 curtime = (uint32)(Crown::TimeToUint(timeIter->second) + today_time.GetTimeValue());
            if(curtime > now)
            {
                m_next_refresh_time = (uint32)(Crown::TimeToUint(timeIter->second) + today_time.GetTimeValue());
                m_orderid = timeIter->first;
                b_exe = true;
                break;
            }
        }

        if(!b_exe)
        {
            today_time.IncDay();
            m_orderid = timeMap.begin()->first;
            m_next_refresh_time = (uint32)(Crown::TimeToUint(timeMap.begin()->second) + today_time.GetTimeValue());
        }
    }

    if(now >= m_next_refresh_time )
    {
        if(m_orderid == timeMap.size())
        {
            CnAssert(timeMap.begin() != timeMap.end());
            today_time.IncDay();
            m_orderid = timeMap.begin()->first;
            m_next_refresh_time = (uint32)(Crown::TimeToUint(timeMap.begin()->second) + today_time.GetTimeValue());
            m_how_time = (uint32)(m_next_refresh_time - today_time.GetTimeValue());
            //CnWarn("cur fresh boss1 time:%d \n", m_next_refresh_time);
            return true;
        }

        //找下一个时间
        WorldMapConfig::RefreshTimeMap::const_iterator timeIter;
        for (timeIter = timeMap.begin(); timeIter != timeMap.end(); ++timeIter)
        {
            if(timeIter->first == m_orderid+1)
            {
                m_next_refresh_time = (uint32)(Crown::TimeToUint(timeIter->second) + today_time.GetTimeValue());
                m_orderid = m_orderid+1;
                break;
            }
        }

        // CnWarn("cur fresh boss2 time:%d \n", m_next_refresh_time);
        //CnWarn("howtime1:%d \n", 0);
        m_how_time = (uint32)(m_next_refresh_time - today_time.GetTimeValue());
        return true;
    }
    else
    {
        // CnWarn("cur fresh boss3 time:%d \n", m_next_refresh_time);
        uint32 cutime = m_next_refresh_time - now;
        m_how_time = cutime;
        // CnWarn("howtime:%d \n", cutime);
        return false;
    }

}
uint32 SubRegularBoss::CalNextRefreshBossTime()
{
    Crown::CSDDateTime today_time = Crown::SDNow();//今日凌晨时间点
    today_time.SetTime(0, 0, 0);
    uint32 now =  (uint32)Crown::SDNow().GetTimeValue();

    uint32 next_refresh_time = 0;

    const WorldMapConfig::RefreshTimeMap& timeMap = WorldMapConfig::Instance()->GetRefreshTimeConfig();
    WorldMapConfig::RefreshTimeMap::const_iterator timeIter;
    for (timeIter = timeMap.begin(); timeIter != timeMap.end(); ++timeIter)
    {
        uint32 time_point =  (uint32)(Crown::TimeToUint(timeIter->second) + today_time.GetTimeValue());
        if (now < time_point)
        {
            next_refresh_time = time_point;
            break;
        }
    }

    // 今天刷新次数用完
    if (next_refresh_time == 0)
    {
        CnAssert(timeMap.begin() != timeMap.end());
        today_time.IncDay();
        next_refresh_time = (uint32)(Crown::TimeToUint(timeMap.begin()->second) + today_time.GetTimeValue());
    }

    return next_refresh_time;
}
void SubRegularBoss::BossDestroyCheck()
{
    if (m_bossData.state != kBOSS_SATE_DEAD)
    {
        return;
    }

    //uint32 scourge_corpse_time = m_bossData.bossData.lifetime; 
    //CnWarn("life time %u\n", scourge_corpse_time);
    //CnWarn("create time %u\n", m_bossData.createTime);
    //uint32 maxtime =scourge_corpse_time + m_bossData.createTime;
    //CnWarn("max time %u\n", maxtime);
    //uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    //if (now < maxtime)
    //{
    //    uint32 howtime = maxtime - now;
    //    CnWarn("have time %u\n", howtime);
    //}
    //else
    //{
    //    CnWarn("time  arrrive\n");
    //}


    // 在Boss鞭尸时间内不销毁
    if (IsInBossScourgeCorpseTime())
    {
        return;
    }


   // CnWarn("destoryboss Reincarnation boss %u\n",m_bossData.bossCreatureID);

    DestroyBoss();
}

void SubRegularBoss::OnBattleEventKill(uint32 attackerID, uint32 defencerID)
{
    Player* defencePlayer = NULL;
    HatredInfo* damageInfo = NULL;
    int64 val = 0;

    // 不是 Boss 杀死不减伤害值
    if (attackerID != m_bossData.bossCreatureID)
    {
        return;
    }

    // 防御者
    defencePlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!defencePlayer)
        return;

    // 伤害信息
    //damageInfo = GetBossDamageInfo(defencePlayer->GetPlayerID());
    damageInfo = GetBossDamageInfo(defencerID);
    if(!damageInfo)
        return;

    val = damageInfo->damage * WorldMapConfig::Instance()->GetBossPlayerDeadRate();
    damageInfo->damage = (uint32)(val / BASE_RATE_NUM);
}

bool SubRegularBoss::IsPlayerEscape(uint64 playerID)
{
    std::map<uint64, WorldMapPlayerRecord>::const_iterator playerIter = m_masterRegular->GetRecordMap().find(playerID);
    return playerIter == m_masterRegular->GetRecordMap().end();
}

bool SubRegularBoss::IsPlayerOffLine(uint64 playerID)
{
   return (PlayerManager::Instance()->GetPlayerByPlayerID(playerID) == NULL);
}

void SubRegularBoss::OnBossDeadProcess(uint32 killerID)
{
    DamageRankReward();
    BossDeadReward();

    // Boss 死亡时间
    m_bossData.killed = true;
    m_bossData.killerID = killerID;
   // m_bossData.deadTime = (uint32)Crown::SDNow().GetTimeValue();
    m_bossData.state = kBOSS_SATE_DEAD;

    // 记录 Boss 击杀者
    Player* player = GetScene()->GetPlayerByObjectId(killerID);
    m_bossData.killer_name = (player) ? (player->GetName()) : ("");

    // Boss 转生
    BossReincarnation();
}

void SubRegularBoss::OnBossCorpseProcess(uint32 attackerID, int32 damage)
{
    // 非Boss鞭尸时间
    if (!IsInBossScourgeCorpseTime())
    {
        return;
    }

    Player* player = GetScene()->GetPlayerByObjectId(attackerID);
    if(!player)
        return;
    uint64 playerID = player->GetPlayerID();

    // 记录玩家鞭尸
    m_scourgeCorpseMap[playerID] = true;

}

void SubRegularBoss::OnDamage(uint32 attackerID, int32 damage, uint32 defenderID)
{
    Boss*  boss = (Boss*)GetScene()->GetBattleUnit(defenderID);
    if(!boss) return;

    HatredManager* hatred_mgr = boss->GetHatredMgr();
    if (!hatred_mgr) return;

    Player* player  = GetScene()->GetPlayerByObjectId(attackerID);
    if(!player) return;

    hatred_mgr->AddHatredByPlayer(player, damage);
}

void SubRegularBoss::DamageRankNtf()
{
    WORLD_BOSS_DAMAGE_LIST topDamageList;
    WORLD_BOSS_DAMAGE_INFO guildDamageInfo;
    WORLD_BOSS_DAMAGE_LIST sendMsg;
    HatredInfo* pInfo = 0;
    Player* player = 0;
    int32 num = 0;

    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    std::vector<HatredInfo*> hatred_list = hatred_mgr->GetSortHatred();

    if(hatred_list.size()< 1) return;

    // 前 10 排行
    num = Crown::SDMin((uint32)10, (uint32)hatred_list.size());

    // 获得前10排行数据
    for(int32 i=0; i<num; i++)
    {
        guildDamageInfo.Clear();

        pInfo = hatred_list[i];
        guildDamageInfo.rank           = i+1;
        guildDamageInfo.damage         = pInfo->damage;
        guildDamageInfo.name           = pInfo->player_name;
        topDamageList.damage_list.push_back(guildDamageInfo);
    }

    if(topDamageList.damage_list.size() < 1)
        return;

    // 发送给所有玩家伤害排名
    Scene::PlayerMap& players = GetScene()->GetPlayers();
    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        pInfo = GetNotifyDamageInfo(topDamageList, it->second);
        if(pInfo)
        {
            guildDamageInfo.rank = 0;
            guildDamageInfo.damage         = pInfo->damage;
            guildDamageInfo.name           = pInfo->player_name;
            sendMsg = topDamageList;
            sendMsg.damage_list.push_back(guildDamageInfo);
            it->second->SendToGate(MSG_FIELD_BOSS_DAMAGE_NTF, &sendMsg);
        }
        else
        {
            it->second->SendToGate(MSG_FIELD_BOSS_DAMAGE_NTF, &topDamageList);
        }
    }
}


void SubRegularBoss::IsPlayerOfflineReq(uint64 playerID)
{
    FIELD_BOSS_PLAYER_IS_OFFLINE_REQ sendMsg;
    sendMsg.player_id.id_h = U64ID_HIGH(playerID);
    sendMsg.player_id.id_l = U64ID_LOW(playerID);
    CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), MSG_FIELD_BOSS_PLAYER_IS_OFFLINE_REQ, &sendMsg);
}

//void SubRegularBoss::SubDamangeByLeveDugeon()
//{
//    if (m_bossData.state != kBOSS_SATE_ALIVE)
//    {
//        return;
//    }
//
//    HatredManager* hatred_mgr = GetBossHatredMgr();
//    if (!hatred_mgr) return;
//
//    std::map<uint32, HatredInfo>& hatred_map = hatred_mgr->GetHatredMap();
//
//    for(std::map<uint32, HatredInfo>::iterator it = hatred_map.begin(); it != hatred_map.end(); ++it)
//    {
//        //// 玩家离开副本，扣伤害值
//       if (IsPlayerEscape(it->second.player_id))
//        {
//            SubDamage(it->first); //离开BOSS所属的地图，仇恨按照时间减少
//        }
//    }
//}

void SubRegularBoss::ResetDamageByPlayerState()
{
    // Boss活着的时候伤害值才有意义
    if (m_bossData.state != kBOSS_SATE_ALIVE)
    {
        return;
    }

    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    std::map<uint32, HatredInfo>& hatred_map = hatred_mgr->GetHatredMap();
    for(std::map<uint32, HatredInfo>::iterator it = hatred_map.begin(); it != hatred_map.end(); ++it)
    {
        HatredInfo* curHatreInfo = &(it->second);
        if(!curHatreInfo) return;
        // 玩家下线，清除伤害值
        if (IsPlayerOffLine(curHatreInfo->player_id))
        {
            IsPlayerOfflineReq(curHatreInfo->player_id);
        }
        // 玩家离开副本，扣伤害值
        else if (IsPlayerEscape(curHatreInfo->player_id))
        {
            SubDamage(it->first); //离开BOSS所属的地图，仇恨按照时间减少
        }
        //// 玩家离开副本，扣伤害值
        //else if (IsPlayerEscape(it->first))
        //{
        //    SubDamage(it->first); //离开BOSS所属的地图，仇恨按照时间减少
        //}
        //else 
        //{
        //    m_subDamageMap[it->first] = 0;
        //}
    }
}

void SubRegularBoss::SubDamage(uint32 objectID)
{
    Player* player = GetScene()->GetPlayerByObjectId(objectID);
    if (!player) return;

    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    HatredInfo* hatred_info = hatred_mgr->GetHatredInfo(objectID);
    if (!hatred_info) return;

    int64 num = 0;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 subTime = GetPlayerSubDamageTime(player->GetPlayerID());
    // TODO:暂时定位玩家刚出圈时，每5秒，扣玩家积分时，通知玩家超出范围
    if (subTime == 0)
    {
        m_subDamageMap[player->GetPlayerID()] = now + WorldMapConfig::Instance()->GetLeveSceneBossOverTime();
       // player->SendToGate(MSG_VOID_FIELD_OVER_RANGE_NTF, &sendMsg);//离开所有的大地图场景在发消息

    }

    if (0 < subTime && subTime <= now) 
    {
        num = hatred_info->damage * WorldMapConfig::Instance()->GetLeveSceneBossOverRate();
        num /= BASE_RATE_NUM;
        hatred_info->damage = (int32)num;

        m_subDamageMap[player->GetPlayerID()] = now + WorldMapConfig::Instance()->GetLeveSceneBossOverTime();
    }
}

void SubRegularBoss::ClearDamage(uint64 playerID)
{
    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    hatred_mgr->ClearHatredByPlayreID(playerID);
}

uint32 SubRegularBoss::GetPlayerSubDamageTime(uint64 playerID)
{
    SubDamageTimeMap::const_iterator iter = m_subDamageMap.find(playerID);
    if (iter != m_subDamageMap.end())
    {
        return iter->second;
    }

    return 0;
}

HatredInfo* SubRegularBoss::GetNotifyDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player)
{
    if(!player) return 0;

    for(std::vector<WORLD_BOSS_DAMAGE_INFO>::iterator it = list.damage_list.begin(); it != list.damage_list.end(); ++it)
    {
        if(it->name == player->GetName())
        {
            return NULL;
        }
    }

    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return NULL;

    return hatred_mgr->GetHatredInfo(player->GetId());
}

HatredInfo* SubRegularBoss::GetBossDamageInfo(uint32 objectID)
{
    HatredManager* hatred_mgr = GetBossHatredMgr();
    return hatred_mgr->GetHatredInfo(objectID);
}


void SubRegularBoss ::BossDeadReward()
{
    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    Player* player = NULL;
    if(hatred_mgr->GetSortHatred().size() > 0)
    {
        player = GetScene()->GetPlayerByPlayerId(hatred_mgr->GetSortHatred()[0]->player_id);//伤害力最高的玩家
    }

    if(!player) return;

    BattleUnit*  battleunit = GetScene()->GetBattleUnit(m_bossData.bossCreatureID);
    if(!battleunit) return;
    AddDropOjectList(battleunit->GetId(), player);
}

void SubRegularBoss::AddDropOjectList(uint32 bosscreateid,  Player* player)
{
    if(!player) return;

    std::string str = Crown::ToString(bosscreateid);

   // for (int32 i = 1; i<7; i++)
    int32 i = 1;
    {
        int32 angle= 60*i; //角度
        int32 randtime = i*100 + 100;

        GetScene()->SendEvent(0, SCENE_EVENT_BOSS_DEAD_DROP_ITEM, player->GetId(), angle, randtime, bosscreateid);
    }
        //int32 angle= 60*1; //角度
        //int32 randtime = 1*100 + 100;

        //GetScene()->SendEvent(0, SCENE_EVENT_BOSS_DEAD_DROP_ITEM, player->GetId(), angle, randtime ,str);


}

//boss死亡
void SubRegularBoss::BossDeadDropItemEvent(uint32 playerid, int32 angle, uint32 bosscreateid)
{
    //Player* player  = GetScene()->GetPlayerByObjectId(playerid);
    //if(!player) return;

    //BattleUnit* pkillBoss = GetScene()->GetBattleUnit(bosscreateid);
    //if(!pkillBoss) return;

    //CoVec2 vec = pkillBoss->GetPosition();

    //int32 randNum = Crown::SDRandom(BASE_RATE_NUM);
    //const REWARD_ITEM* rewardItem = WorldMapConfig::Instance()->GetBossDeadRandItem(m_bossData.bossID, randNum);

    //if(!rewardItem) return;

    //int32 rLengh = 500+ Crown::SDRandom(100); //半径
    //CoVec2 endVec;  //终点
    //endVec.x = rLengh* cos((real)ANGLE_TO_RADIAN(angle)) + vec.x; //圆上的点
    //endVec.y = rLengh*sin((real)ANGLE_TO_RADIAN(angle)) + vec.y;

    ////CoVec2 endVec;  //终点
    ////endVec.x =vec.x + rand() % 200;
    ////endVec.y= vec.y  + rand() % 200;
    ////
    //DropData dropdata;
    //dropdata.dropid = 1;
    //dropdata.x = (int32)vec.x;
    //dropdata.y = (int32) vec.y;
    //dropdata.fromid = pkillBoss->GetId();
    //dropdata.pos = endVec;
    //dropdata.playerobjid = player->GetId();
    //dropdata.picktime = WorldMapConfig::Instance()->GetBossDeadPickTime();
    //dropdata.sysappeartime = WorldMapConfig::Instance()->GetBossDeadLifeTime();
    //dropdata.fromtype = OBJ_BOSS;
    //dropdata.arrvietime = 0;
    //dropdata.reward_item.id = rewardItem->id;
    //dropdata.reward_item.type = rewardItem->type;
    //dropdata.reward_item.num = rewardItem->num;
    //m_Regular->AddDropObject(dropdata);
}

//鞭尸boss掉落物品
void SubRegularBoss::CorpseBossDropEvent(uint32 playerid, int32 angle, uint32 bosscreateid)
{
    //Player* player  = GetScene()->GetPlayerByObjectId(playerid);
    //if(!player) return;

    //BattleUnit* pkillBoss = GetScene()->GetBattleUnit(bosscreateid);
    //if(!pkillBoss) return;

    //CoVec2 vec = pkillBoss->GetPosition();

    //int32 randNum = Crown::SDRandom(BASE_RATE_NUM);
    //const REWARD_ITEM* rewardItem = WorldMapConfig::Instance()->GetBossCorpseRandItem(m_bossData.bossID, randNum);
    //if(!rewardItem) return;

    //int32 rLengh = 500+ Crown::SDRandom(100); //半径
    //CoVec2 endVec;  //终点
    //endVec.x = rLengh* cos((real)ANGLE_TO_RADIAN(angle)) + vec.x; //圆上的点
    //endVec.y = rLengh*sin((real)ANGLE_TO_RADIAN(angle)) + vec.y;

    //DropData dropdata;
    //dropdata.dropid = 1;
    //dropdata.x = (int32)vec.x;
    //dropdata.y = (int32) vec.y;
    //dropdata.fromid = pkillBoss->GetId();
    //dropdata.pos = endVec;
    //dropdata.playerobjid = player->GetId();
    //dropdata.picktime = WorldMapConfig::Instance()->GetBossCorpsePickTime();
    //dropdata.sysappeartime = WorldMapConfig::Instance()->GetBossCorpseLifeTime();
    //dropdata.fromtype = OBJ_BOSS;
    //dropdata.arrvietime = 0;
    //dropdata.reward_item.id = rewardItem->id;
    //dropdata.reward_item.type = rewardItem->type;
    //dropdata.reward_item.num = rewardItem->num;
    //m_Regular->AddDropObject(dropdata);
}
void SubRegularBoss::DamageRankReward()
{
    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

    std::vector<HatredInfo*> hatred_list = hatred_mgr->GetSortHatred();
    if(hatred_list.size() < 1) return;

    HatredInfo* damageInfo = NULL;

    // 前十名奖励
    for (uint32 i = 0; i < hatred_list.size() && i < 10; ++i)
    {
        damageInfo = hatred_list[i];
        if(!damageInfo)
            return;

        const REWARD_TABLE* rewardTable = WorldMapConfig::Instance()->GetDamageRankRewardItems(m_bossData.bossID, i+1);
        if (rewardTable)
        {
            /*
            MailManager::SendMail(damageInfo->center_id, damageInfo->player_id, "", 
                MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
                MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
                MAIL_TYPE_PLAYER_SYS, rewardTable->items, damageInfo->player_id);
            */
            MailManager::SendMail(damageInfo->center_id, damageInfo->player_id, "", "", 
                Crown::ToString(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
                MAIL_TYPE_PLAYER_SYS, rewardTable->items, damageInfo->player_id);
        }
    }
}

void SubRegularBoss::ScourgeCorpseCheck()
{
    ScourgeCorpseMap::iterator iter;
    for (iter = m_scourgeCorpseMap.begin(); iter != m_scourgeCorpseMap.end(); ++iter)
    {
        if (iter->second)
        {
            ScourgeCorpseReward(iter->first);
        }
        iter->second =false;
    }
}

void SubRegularBoss::ScourgeCorpseReward(uint64 playerID)
{
    Player* player  = GetScene()->GetPlayerByPlayerId(playerID);
    if(!player) return;

    BattleUnit*  battleunit = GetScene()->GetBattleUnit(m_bossData.bossCreatureID);
    if(!battleunit) return;

     int32 angle = 1 + Crown::SDRandom(360);
     std::string str = Crown::ToString(battleunit->GetId());

    //int32 randtime = 100 + Crown::SDRandom(200);//延迟的时间
    int32 randtime = 1;//延迟的时间
    GetScene()->SendEvent(0, SCENE_EVENT_SCOURGE_CORPSE_DROP_ITEM, player->GetId(), angle, randtime, battleunit->GetId());
}

void SubRegularBoss::SetAttackTarget()
{
    if(m_bossData.state != kBOSS_SATE_ALIVE)
        return;

    BattleUnit*  battleunit = GetScene()->GetBattleUnit(m_bossData.bossCreatureID);
    if(!battleunit) return;

    HatredManager* hatred_mgr = GetBossHatredMgr();
    if (!hatred_mgr) return;

     Player* player = NULL;
    if(hatred_mgr->GetSortHatred().size()> 0 )
    {
        player = GetScene()->GetPlayerByPlayerId(hatred_mgr->GetSortHatred()[0]->player_id); //得到伤害排名最高的玩家
    }
    else
    {
        if(GetScene()->GetPlayers().size() > 0)
        {
            Scene::PlayerMap::iterator it = GetScene()->GetPlayers().begin();
            player =  it->second;
        }
    }

    if(!player) return;

    if(m_maxdamagePlayerid && player->GetId() != m_maxdamagePlayerid) 
    {
        battleunit->GetAIController()->SetTargetUnitId(player->GetId());
        m_maxdamagePlayerid = player->GetId();
    }

}

HatredManager* SubRegularBoss::GetBossHatredMgr()
{
    Boss*  boss = (Boss*)GetScene()->GetBattleUnit(m_bossData.bossCreatureID);
    if(!boss) return NULL;

    HatredManager* hatred_mgr = boss->GetHatredMgr();
    return hatred_mgr;
}
