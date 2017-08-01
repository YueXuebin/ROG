#include "gameholder_pch.h"
#include "world_prop_communicator.h"
#include "prop_notifier.h"
#include "boss.h"
#include "player.h"
#include "game_define.h"
#include "scene.h"
#include "player_manager.h"
#include "battle_recorder.h"
#include "dungeon_set.h"
#include "event_define.h"
#include "world_boss_config.h"
#include "pet.h"
#include "world_boss_manager.h"
#include "gate_handler.h"
#include "activity_manager.h"
#include "mail_manager.h"
#include "game_util.h"
#include "formula_manager.h"

WorldPropCommunicator::WorldPropCommunicator(DungeonSet* dungeonSet):
BattleUnitCommunicator(BattleUnitCommunicator::WORLD_BOSS_COMMUNICATOR, dungeonSet)
{
    Clear();
    m_startcreatebooss = (uint32)Crown::SDNow().GetTimeValue();
}

WorldPropCommunicator::~WorldPropCommunicator()
{
    ClearDamageInfo();
}

void WorldPropCommunicator::OnConnect(BattleUnit* pBattleUnit)
{
    CnDbg("WorldPropCommunicator::OnConnect, start,%d \n", m_start);

    if(!m_start)
    {
        Clear();
    }

    m_bossCreatureID = ((Boss*)pBattleUnit)->GetCreatureId();
}

void WorldPropCommunicator::OnDisconnect(BattleUnit* pBattleUnit)
{
    //ClearDamageInfo();
}

void WorldPropCommunicator::OnDead(BattleUnit* attacker)
{
    Player* player = 0;
    bool success = false;

    CnDbg("WorldPropCommunicator::OnDead, m_start,%d \n", m_start?1:0);
    if(!m_start)
        return;

    CnDbg("WorldPropCommunicator::OnDead, attacker:%u, objectType:%d\n", attacker, attacker?attacker->GetObjectType():0);

    // 世界Boss死亡事件,通知所有频道
    if(attacker == NULL)
    {
        // 代表时间到了，无人击杀BOSS
    }
    else if(attacker && attacker->GetObjectType() == OBJ_PLAYER)
    {
        player = (Player*)attacker;
    }
    else if(attacker && attacker->GetObjectType() == OBJ_PET)
    {   // 宠物杀死了世界BOSS
        player = CnDynamicCast(Player, ((Pet*)attacker)->GetMaster());
    }
    else
    {
        // 非玩家击杀了这个工会BOSS
        CnAssert(false);
    }

    CnDbg("WorldPropCommunicator::OnDead, player:%u\n", player);

    if(player)
    {
        m_playerID = player->GetPlayerID();
        success = false;
    }

    OnEnd(success);
}

void WorldPropCommunicator::OnDamage( BattleUnit* pAttacker, int32 damage )
{
    DamageMap::iterator damageIt;
    uint64 playerID = 0;
    Player* player = NULL;

    if(!pAttacker) return;

    if(pAttacker->GetObjectType() == OBJ_PLAYER)
    {
        player = (Player*)pAttacker;
    }
    else if(pAttacker->GetObjectType() == OBJ_PET)
    {
        player = CnDynamicCast(Player, ((Pet*)pAttacker)->GetMaster());
    }

    if(!player) return;

    playerID = player->GetPlayerID();

    damageIt = m_DamageMap.find(playerID);
    if(damageIt == m_DamageMap.end())
    {
        WorldBossDamageInfo damageInfo;
        damageInfo.Clear();
        damageInfo.player_name  = player->GetName();
        damageInfo.player_id    = playerID;
        damageInfo.center_id    = player->GetCenterSvrID();
        damageInfo.damage       = 0;
        m_DamageMap[playerID] = damageInfo;
        m_damageList.push_back(&m_DamageMap[playerID]);
    }
    else
    {
        damageIt->second.damage += damage;
        player->GetBattleRecorder()->SetWorldBossDamage(damageIt->second.damage);
    }

    m_allDamage += damage;
}

void WorldPropCommunicator::OnEnd(bool success)
{
    DungeonSet* dungeonSet = 0;
    Player* player = 0;
    uint32 killerID = 0;

    CnDbg("WorldPropCommunicator::OnEnd, m_start,%d\n", m_start?1:0);
    if(!m_start)
        return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(m_playerID);
    if(player)
    {
        killerID = player->GetId();
    }

    CnDbg("WorldPropCommunicator::OnEnd, playerID:%llu, player:%u\n", m_playerID, player);

    // 世界Boss死亡，最后一个排序
    UpdateDamage();

    // 世界Boss击杀处理
    BossKillProcess(player);

    // 提升世界Boss能力
    if(player)
        WorldBossManager::Instance()->LevelUpBoss(m_bossCreatureID ,m_startcreatebooss);

    dungeonSet = GetDungeonSet();
    if(dungeonSet)
    {
        dungeonSet->SendEvent(BATTLE_EVENT_WORLD_BOSS_DEAD, m_bossCreatureID, killerID, 0);
    }

    CnInfo("WorldPropCommunicator::RewardDamage\n");
    RewardDamage();

    // 通知TS，世界BOSS活动结束
    ActivityManager::Instance()->CloseActivityByType(ACTIVITY_TYPE_WORLD_BOSS);

    ClearDamageInfo();

    m_start = false;

    m_startcreatebooss = 0;
}

void WorldPropCommunicator::Clear()
{
    m_start = true;
    m_bossCreatureID = 0;
    m_playerID = 0;
    m_SecondTimer = 0;

    ClearDamageInfo();
}

void WorldPropCommunicator::ClearDamageInfo()
{
    m_allDamage = 0;
    m_damageList.clear();
    m_DamageMap.clear();
}

void WorldPropCommunicator::BossKillProcess(BattleUnit* attacker)
{
    WORLD_BOSS_KILL_INFO_NTF bossKillInfoNtf;
    const REWARD_TABLE* rewardTable = NULL;
    WorldBossDamageInfo* damageInfo = NULL;
    Player* killer = NULL;
    Player* player = NULL;
    std::set<uint64> playerSet;
    std::set<uint64>::iterator playerSetIt;

    CnDbg("WorldPropCommunicator::BossKillProcess, attacker:%u, objectType:%u\n", attacker, attacker?attacker->GetObjectType():0);
    if(attacker && (attacker->GetObjectType() == OBJ_PLAYER))
    {
        killer = (Player*)attacker;
    }

    if(killer)
        CnInfo("%s kill world boss\n", killer->GetName().c_str());

    // 填充世界BOSS的击杀信息
    FillWorldBossKillInfo(bossKillInfoNtf, killer);

    // 先在伤害排行中遍历所有玩家，发送世界BOSS击杀信息
    for(int32 rankI=0; rankI<(int32)m_damageList.size(); rankI++)
    {
        damageInfo = m_damageList[rankI];
        player = PlayerManager::Instance()->GetPlayerByPlayerID(damageInfo->player_id);
        if(!player) continue;

        playerSetIt = playerSet.find(damageInfo->player_id);
        if(playerSetIt != playerSet.end())
        {
            continue;
        }

        playerSet.insert(damageInfo->player_id);

        NotifyWorldBossClear(player, killer);
    }

    // 再在副本中遍历所有玩家，发送世界BOSS击杀信息
    std::vector<Player*> dungeonPlayers = m_dungeonSet->GetPlayerMap();
    for(std::vector<Player*>::iterator playerIt = dungeonPlayers.begin(); playerIt != dungeonPlayers.end(); ++playerIt)
    {
        player = *playerIt;
        if(!player) continue;

        playerSetIt = playerSet.find(player->GetPlayerID());
        if(playerSetIt != playerSet.end())
        {
            continue;
        }

        playerSet.insert(player->GetPlayerID());

        NotifyWorldBossClear(player, killer);
    }

    // 发送世界 BOSS 击杀公告
    GateHandler::Instance()->SendToAll(MSG_WORLD_BOSS_KILL_INFO_NTF, &bossKillInfoNtf);

    // 公告最后一击
    if(killer)
    {
        SERVER_MARQUEE sendMsg;
        sendMsg.Clear();
        sendMsg.id = MARQUEE_LAST_KILL;
        sendMsg.parame_list.push_back(MarQueePlayerName(killer->GetVipLevel(), killer->GetName()));
        Player::OnMarQuee(sendMsg);
    }
}

void WorldPropCommunicator::Update()
{
    m_SecondTimer += g_ServerUpdateTime;
    const int32 SecondTimer = 1000;
    if(m_SecondTimer > SecondTimer)
    {
        SecondUpdate();
        m_SecondTimer -= SecondTimer;
    }
}

bool SortDamage(WorldBossDamageInfo* a, WorldBossDamageInfo* b)
{
    return a->damage > b->damage;
}

void WorldPropCommunicator::SecondUpdate()
{
    UpdateDamage();
}

void WorldPropCommunicator::UpdateDamage()
{
    WorldBossDamageInfo* pInfo = 0;
    WORLD_BOSS_DAMAGE_INFO playerDamageInfo;

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    int32 num = Crown::SDMin((uint32)10, (uint32)m_damageList.size());
    int32 maxHp = GetMaxHp();
    WORLD_BOSS_DAMAGE_LIST damageList;

    for(int32 rankI=0; rankI<(int32)m_damageList.size(); rankI++)
    {
        pInfo = m_damageList[rankI];

        playerDamageInfo.Clear();
        playerDamageInfo.rank           = rankI+1;
        playerDamageInfo.damage         = pInfo->damage;
        playerDamageInfo.name           = pInfo->player_name;

        if(maxHp)
        {
            playerDamageInfo.damage_percent = int32((int64)pInfo->damage * BASE_RATE_NUM / maxHp);
        }

        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(pInfo->player_id);
        if(player)
        {
            player->GetBattleRecorder()->SetWorldBossDamage(pInfo->damage);
            player->GetBattleRecorder()->SetWorldBossRank(rankI+1);

            player->SendToGate(MSG_WORLD_BOSS_DAMAGE_NTF, &playerDamageInfo);
        }

        // 记录前十名伤害
        if(rankI<num)
        {
            damageList.damage_list.push_back(playerDamageInfo);
        }
    }

    // 发送给所有玩家伤害排名
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        Scene* pScene = m_BattleUnits[i]->GetScene();
        pScene->NotifyMessageToAll(MSG_WORLD_BOSS_DAMAGE_LIST_NTF, &damageList);
    }
}

void WorldPropCommunicator::RewardDamage()
{
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;
    uint32 damageReward = 0;
    Player* player = NULL;

    for(DamageList::iterator damageIt = m_damageList.begin(); damageIt != m_damageList.end(); ++damageIt)
    {
        if(!(*damageIt)) 
            continue;

        // 伤害奖励数值
        // 注意!!! 这里 damageReward 有可能是 0 元
        damageReward = FormulaManager::Instance()->CalcWorldBossRewardByDamage((*damageIt)->damage);

        if(damageReward <= 0) continue;

        // 奖励数据
        rewardTable.Clear();

        rewardItem.Clear();
        rewardItem.id = PROP_GAMEPOINT;
        rewardItem.num = damageReward;

        rewardTable.reason = REWARD_REASON_WORLD_BOSS;
        rewardTable.items.push_back(rewardItem);

        // 查找玩家
        player = PlayerManager::Instance()->GetPlayerByName((*damageIt)->player_name);
        if(player && (DUNGEON_ID(player->GetUnionMapID()) == WORLD_BOSS_MAP))
        {
            player->AddRewardTableToPool(rewardTable);
        }
        else
        {
            MailManager::SendMessageMail((*damageIt)->center_id, 0, (*damageIt)->player_name, rewardTable.items, MAIL_MESSAGE_TYPE_WORLD_BOSS, 0);
        }
    }
}

void WorldPropCommunicator::FillWorldBossKillInfo(WORLD_BOSS_KILL_INFO_NTF& info, Player* killer)
{
    const REWARD_TABLE*     rewardTable = NULL;
    WorldBossDamageInfo*    damageInfo = NULL;

    info.Clear();

    info.boss_kill = killer? true: false;

    // Boss击杀公告
    if(!killer) return;

    // 世界BOSS击杀信息
    for(uint32 rank=0; (rank<(uint32)m_damageList.size()) && (rank < 3); rank++)
    {
        damageInfo = m_damageList[rank];

        if(rank == 0)
        {   // 第一名
            info.first_name = damageInfo->player_name;
            rewardTable = WorldBossConfig::Instance()->GetWorldBossReward(GetDungeonID(), 1, info.boss_kill);
            if(rewardTable)
                info.first_reward = *rewardTable;
        }

        if(rank == 1)
        {   // 第二名
            info.second_name = damageInfo->player_name;
            rewardTable = WorldBossConfig::Instance()->GetWorldBossReward(GetDungeonID(), 2, info.boss_kill);
            if(rewardTable)
                info.second_reward = *rewardTable;
        }

        if(rank == 2)
        {   // 第三名
            info.third_name = damageInfo->player_name;
            rewardTable = WorldBossConfig::Instance()->GetWorldBossReward(GetDungeonID(), 3, info.boss_kill);
            if(rewardTable)
                info.third_reward = *rewardTable;
        }
    }

    if(killer)
    {   // 最后一击者
        info.last_hit_name = killer->GetName();
        rewardTable = WorldBossConfig::Instance()->GetWorldBossReward(GetDungeonID(), killer->GetBattleRecorder()->GetWorldBossRank(), info.boss_kill);
        if(rewardTable)
            info.last_hit_reward = *rewardTable;
    }
}

void WorldPropCommunicator::NotifyWorldBossClear(Player* player, Player* killer)
{
    WORLD_BOSS_CLEAR_NTF    sendMsg;
    const REWARD_TABLE*     rewardTable = NULL;

    if(!player) return;

    sendMsg.Clear();
    sendMsg.boss_id    = m_bossCreatureID;
    sendMsg.succeed    = (killer? true: false);
    sendMsg.damage     = player->GetBattleRecorder()->GetWorldBossDamage();

    // 最后击杀者
    if(killer)
        sendMsg.last_straw = killer->GetName();

    sendMsg.rank = player->GetBattleRecorder()->GetWorldBossRank();
    if (sendMsg.rank == 0)
        sendMsg.rank = m_damageList.size();

    // 奖励
    rewardTable = WorldBossConfig::Instance()->GetWorldBossReward(GetDungeonID(), player->GetBattleRecorder()->GetWorldBossRank(), killer ? true : false);
    if(rewardTable)
    {
        sendMsg.reward_table.reason = rewardTable->reason;
        sendMsg.reward_table.reason_arg = rewardTable->reason_arg;
        player->GetFcmRewardList(rewardTable->items, sendMsg.reward_table.items);

        // 奖励物品
        for (std::vector<REWARD_ITEM>::const_iterator gainIt = sendMsg.reward_table.items.begin(); 
            gainIt != sendMsg.reward_table.items.end(); ++gainIt)
        {
            player->GainItem(*gainIt);
        }

        player->GainPoolToRewardInfo();
    }

    if(player->GetScene())  // 玩家进入下线流程
    {
        //排名是按照伤害输出的排名
        if(killer)
        {
            player->GetScene()->SendEvent(0, EVENT_WORLD_BOSS_DAMAGE_RANK, sendMsg.rank, 0, 0);
            //称号排名
          /*  if(!player->FinishAchive(ACHIVE_WORLD_BOSS))
            {
                player->GetScene()->SendEvent(0, EVENT_ACHIVE_WORLD_BOSS, sendMsg.rank, 0, 0);
            }*/
        }

        if(player->GetScene()->GetDungeonID() == m_dungeonSet->GetDungeonID())  // 玩家在世界Boss场景中，则发送通知
            player->SendToGate(MSG_WORLD_BOSS_CLEAR_NTF, &sendMsg);
    }
}
