#include "gameholder_pch.h"
#include "guild_boss_communicator.h"
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
#include "center_handler.h"
#include "guild_config.h"
#include "guild_manager.h"
#include "game_util.h"
#include "mail_manager.h"

GuildBossCommunicator::GuildBossCommunicator(DungeonSet* dungeonSet):
BattleUnitCommunicator(BattleUnitCommunicator::GUILD_BOSS_COMMUNICATOR, dungeonSet)
{
    Clear();
}

GuildBossCommunicator::~GuildBossCommunicator()
{
    //DisconnectAllBoss();
}

void GuildBossCommunicator::OnConnect(BattleUnit* pBattleUnit)
{
    CnDbg("GuildBossCommunicator::OnConnect, start,%d \n", m_start?1:0);

    if(!m_start)
    {
        Clear();
    }

    if(pBattleUnit->GetObjectType() == OBJ_BOSS)
    {
        m_bosses.push_back((Boss*)pBattleUnit);
    }

    m_bossCreatureID = ((Boss*)pBattleUnit)->GetCreatureId();
}

void GuildBossCommunicator::OnDisconnect(BattleUnit* pBattleUnit)
{
    //ClearDamageInfo();
}

void GuildBossCommunicator::OnDead(BattleUnit* attacker)
{
    Player* player = 0;
    bool success = false;

    CnDbg("GuildBossCommunicator::OnDead, start,%d \n", m_start?1:0);
    if(!m_start) return;

    // 世界Boss死亡事件,通知所有频道
    if(attacker && (attacker->GetObjectType() == OBJ_PLAYER))
    {
        player = (Player*)attacker;
    }
    else if(attacker && (attacker->GetObjectType() == OBJ_PET))
    {
        player = CnDynamicCast(Player, ((Pet*)attacker)->GetMaster());
    }

    if(player)
        m_playerID = player->GetPlayerID();

    if(attacker)
        success = true;

    OnEnd(success);
}

void GuildBossCommunicator::OnDamage( BattleUnit* pAttacker, int32 damage )
{
    DamageMap::iterator damageIt;
    uint64  playerID = 0;
    Player* player = 0;

    if(!pAttacker)
        return;

    if(pAttacker->GetObjectType() == OBJ_PET)
    {
        // 如果攻击者为宠物，则获得宠物的主人指针
        player = CnDynamicCast(Player, ((Pet*)pAttacker)->GetMaster());
    }
    else if(pAttacker->GetObjectType() == OBJ_PLAYER)
    {
        // 获得攻击者的指针
        player = (Player*)pAttacker;
    }

    if(!player)
        return;

    playerID = player->GetPlayerID();

    damageIt = m_damageMap.find(playerID);
    if(damageIt == m_damageMap.end())
    {
        GuildBossDamageInfo damageInfo;
        damageInfo.Clear();
        damageInfo.center_id    = player->GetCenterSvrID();
        damageInfo.player_name  = player->GetName();
        damageInfo.guild_id     = player->GetGuildID();
        damageInfo.damage       = 0;
        damageInfo.battle_power = player->GetBattlePower();
        damageInfo.level        = player->GetLevel();

        m_damageMap[playerID]   = damageInfo;
        m_damageList.push_back(&m_damageMap[playerID]);
    }
    else
    {
        damageIt->second.damage += damage;
        player->GetBattleRecorder()->SetWorldBossDamage(damageIt->second.damage);
    }

    // 总伤害量
    m_allDamage += damage;
}

void GuildBossCommunicator::OnEnd(bool success)
{
    std::set<uint64> scenesPlayers;
    DungeonSet* dungeonSet = 0;
    Player* player = 0;
    Scene* pScene = 0;
    uint64 guildID = 0;

    CnDbg("GuildBossCommunicator::OnEnd, start,%d \n", m_start?1:0);
    if(!m_start)
        return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(m_playerID);

    if(m_bosses.size() > 0)
    {
        guildID = Crown::SDAtou64(m_bosses[0]->GetInstanceName().c_str());
    }

    CnDbg("GuildBossCommunicator::OnEnd, guildID:%llu, player:%u\n", guildID, player);

    // 公会Boss死亡，最后一个排序
    UpdateDamage();

    // 公会Boss击杀处理
    BossKillProcess(player);

    // 提升guldBoss能力
   // if(player)
       // GuildManager::Instance()->LevelUpBoss(m_bossCreatureID);

    // 更新工会BOSS的死亡时间（因为有时是由于时间到了将怪物杀死，所以不能依赖attacker）
    GuildManager::Instance()->UpdateGuildBossDeadTime(guildID);

    for(std::vector<Boss*>::iterator it = m_bosses.begin();
            it != m_bosses.end(); ++it)
    {
        pScene = (*it)->GetScene();
        if(!pScene)
            continue;
        pScene->SendEvent(0, BATTLE_EVENT_GUILD_BOSS_DEAD, U64ID_HIGH(guildID), U64ID_LOW(guildID), 0);
    }

    if(player)
    {
        RewardByRank();
        SendCrusadeBossRankSave();
    }

    // ================================================================================

    // 获得BOSS场景中的角色
    GetPlayersByBoss(scenesPlayers);

    // 奖励
    Reward(success, scenesPlayers);

    // 清除伤害信息
    ClearDamageInfo();

    m_start = false;
}

// 获得BOSS场景中的角色
void GuildBossCommunicator::GetPlayersByBoss(std::set<uint64>& scenesPlayers)
{
    Scene* scene = 0;

    scenesPlayers.clear();

    for(std::vector<Boss*>::iterator bossIt = m_bosses.begin();
            bossIt != m_bosses.end(); ++bossIt)
    {
        scene = (*bossIt)->GetScene();

        if(!scene) continue;

        std::map<uint32, Player*>& players = scene->GetPlayers();
        for(std::map<uint32, Player*>::iterator playerIt = players.begin();
                playerIt != players.end(); ++playerIt)
        {
            scenesPlayers.insert(playerIt->second->GetPlayerID());
        }
    }
}

void GuildBossCommunicator::Clear()
{
    m_start  = true;
    m_allDamage = 0;
    m_bossCreatureID = 0;
    m_playerID = 0;
    m_secondTimer = 0;

    for(std::vector<Boss*>::iterator it = m_bosses.begin(); it != m_bosses.end(); ++it)
    {
        (*it)->SetCommunicator(NULL);
    }

    ClearDamageInfo();
}

void GuildBossCommunicator::ClearDamageInfo()
{
    m_damageMap.clear();
    m_damageList.clear();
}

void GuildBossCommunicator::BossKillProcess(BattleUnit* KillAttacker)
{
    SERVER_MARQUEE sendMsg;
    Crown::CSDDateTime now = Crown::SDNow();
    GUILD_INFO* guild_info = 0;
    std::string name = "";
    int32 viplevel = 0;

    if(!KillAttacker) return;

    // 公告最后一击
    sendMsg.id = MARQUEE_GUILD_LAST_KILL;

    if(KillAttacker->GetObjectType() == OBJ_PLAYER)
    {
        name = ((Player*)KillAttacker)->GetName();
        viplevel = ((Player*)KillAttacker)->GetVipLevel();
        guild_info = GuildManager::Instance()->GetGuildInfo(((Player*)KillAttacker)->GetGuildID());
    }

    if(guild_info)
    {
        guild_info->guild_boss_dead_time = (uint32)now.GetTimeValue();
    }

    sendMsg.parame_list.push_back(MarQueePlayerName(viplevel, name));

    Player::OnMarQuee(sendMsg);
}

void GuildBossCommunicator::Reward(bool success, std::set<uint64>& scenesPlayers)
{
    GUILD_CRUSADE_BOSS_END_NTF sendMsg;
    const GuildBossTmpl* guildBossTmpl = 0;
    GUILD_INFO* guildInfo = 0;
    Player* player = 0;
    int32 bossLevel = 0;
    uint64 guildID = 0;

    // 遍历所有人，查找到一个公会ID
    for(std::set<uint64>::iterator playerIt = scenesPlayers.begin(); 
        playerIt != scenesPlayers.end(); ++playerIt)
    {
        player = PlayerManager::Instance()->GetPlayerByPlayerID(*playerIt);
        if(!player) continue;

        guildID = player->GetGuildID();
        guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);

        if(!guildInfo) continue;

        bossLevel = guildInfo->guild_boss_level;
        break;
    }

    if(success)
    {
        sendMsg.err_code = ERR_SUCCEED;
        guildBossTmpl = GuildConfig::Instance()->GetGuildBossTmpl(bossLevel);
        if(guildBossTmpl)
        {
            sendMsg.reward_table = guildBossTmpl->rewardTable;
        }
    }
    else
    {
        sendMsg.err_code = ERR_FAILED;
    }

    sendMsg.reward_table.reason = REWARD_REASON_GUILD_CRUSADE_MONSTER;

    // 发送奖励通知
    for(std::set<uint64>::iterator playerIt = scenesPlayers.begin(); 
            playerIt != scenesPlayers.end(); ++playerIt)
    {
        player = PlayerManager::Instance()->GetPlayerByPlayerID(*playerIt);
        if(!player)
        {
            DamageMap::iterator damageIt= m_damageMap.find(*playerIt);
            if(damageIt == m_damageMap.end())
            {
                continue;
            }
            MailManager::SendMessageMail(damageIt->second.center_id, 0, damageIt->second.player_name, sendMsg.reward_table.items, MAIL_MESSAGE_TYPE_TWILIGHT_WAR, 0);
            continue;
        }

        player->AddRewardTableToPool(sendMsg.reward_table);
        player->SendToGate(MSG_GUILD_CRUSADE_BOSS_END_NTF, &sendMsg);
    }
}

void GuildBossCommunicator::RewardByRank()
{
    Player* player = 0;
    int32 rank = 0;
    // 第一名发称号
    for(int32 i=0; (i < (int32)m_damageList.size()); i++)
    {
       player = PlayerManager::Instance()->GetPlayerByName(m_damageList[i]->player_name);
       if(player)
       {
            // 所有参与人员
            player->SendMyselfEvent(EVENT_GUIDBOSS_RANK,0,0,0);
       }
    }
}

void GuildBossCommunicator::SendCrusadeBossRankSave()
{
    GUILD_BOSS_DAMAGE_INFO guildDamageInfo;
    GUILD_DAMAGE_RANK_LIST sendMsg;
    GuildBossDamageInfo* pInfo = 0;
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;
    uint32 damageReward = 0;
    Player* player = 0;
    uint64 guildID = 0;

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    for(uint32 i=0; i<m_damageList.size(); i++)
    {
        pInfo = m_damageList[i];
        guildDamageInfo.rank           = i+1;
        guildDamageInfo.name           = pInfo->player_name;
        guildDamageInfo.damage         = pInfo->damage;
        guildDamageInfo.level          = pInfo->level;
        sendMsg.damage_list.push_back(guildDamageInfo);

        // 伤害奖励数值
        damageReward = guildDamageInfo.damage / 1000;
        // 查找玩家
        player = PlayerManager::Instance()->GetPlayerByName(pInfo->player_name);
        if(player && (damageReward > 0))
        {
            rewardTable.Clear();

            rewardItem.Clear();
            rewardItem.id = PROP_GAMEPOINT;
            rewardItem.num = damageReward;

            rewardTable.reason = REWARD_REASON_GUILD_CRUSADE_MONSTER;
            rewardTable.items.push_back(rewardItem);

            player->AddRewardTableToPool(rewardTable);
        }

        if(!guildID && pInfo->guild_id)
        {
            guildID = pInfo->guild_id;
        }
    }

    if(!guildID) return;

    sendMsg.guild_id.id_h = U64ID_HIGH(guildID);
    sendMsg.guild_id.id_l = U64ID_LOW(guildID);

    if(sendMsg.damage_list.size() < 1)
        return;

    CenterHandler::Instance()->SendToMasterCenter(MSG_GUILD_CRUSADE_BOSS_RANK_SAVE, &sendMsg);
}

void GuildBossCommunicator::Update()
{
    const int32 SecondTimer = 1000;
    m_secondTimer += g_ServerUpdateTime;
    if(m_secondTimer > SecondTimer)
    {
        SecondUpdate();
        m_secondTimer -= SecondTimer;
    }
}

void GuildBossCommunicator::SecondUpdate()
{
    UpdateDamage();
}

void GuildBossCommunicator::UpdateDamage()
{
    WORLD_BOSS_DAMAGE_LIST topDamageList;
    WORLD_BOSS_DAMAGE_INFO guildDamageInfo;
    WORLD_BOSS_DAMAGE_LIST sendMsg;
    GuildBossDamageInfo* pInfo = 0;
    Player* player = 0;
    int32 maxHP = GetMaxHp();
    int32 num = 0;

    // 前 10 排行
    num = Crown::SDMin((uint32)10, (uint32)m_damageList.size());

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    for(int32 i=0; i<num; i++)
    {
        guildDamageInfo.Clear();

        pInfo = m_damageList[i];
        guildDamageInfo.rank           = i+1;
        guildDamageInfo.damage         = pInfo->damage;
        guildDamageInfo.name           = pInfo->player_name;
        if(maxHP > 0)
        {
            guildDamageInfo.damage_percent = int32((int64)pInfo->damage * BASE_RATE_NUM / maxHP);
        }
        topDamageList.damage_list.push_back(guildDamageInfo);
    }

    if(topDamageList.damage_list.size() < 1)
        return;

    // 发送给所有玩家伤害排名
    for(uint32 i=0; i<m_bosses.size(); i++)
    {
        Scene* pScene = m_bosses[i]->GetScene();
        if(!pScene) continue;

        Scene::PlayerMap& players = pScene->GetPlayers();
        for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
        {
            pInfo = GetNotifyGuildDamageInfo(topDamageList, it->second);
            if(pInfo)
            {
                guildDamageInfo.rank = 0;
                guildDamageInfo.damage         = pInfo->damage;
                guildDamageInfo.name           = pInfo->player_name;
                if(maxHP > 0)
                {
                    guildDamageInfo.damage_percent = (int32)((int64)pInfo->damage * BASE_RATE_NUM / maxHP);
                }

                sendMsg = topDamageList;
                sendMsg.damage_list.push_back(guildDamageInfo);
                it->second->SendToGate(MSG_GUILD_CRUSADE_BOSS_DAMAGE_NTF, &sendMsg);
            }
            else
            {
                it->second->SendToGate(MSG_GUILD_CRUSADE_BOSS_DAMAGE_NTF, &topDamageList);
            }
        }
    }
}

bool GuildBossCommunicator::SortDamage(GuildBossDamageInfo* a, GuildBossDamageInfo* b)
{
    return a->damage > b->damage;
}

GuildBossCommunicator::GuildBossDamageInfo* GuildBossCommunicator::GetNotifyGuildDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player)
{
    DamageMap::iterator damageIt;

    if(!player) return 0;

    for(std::vector<WORLD_BOSS_DAMAGE_INFO>::iterator it = list.damage_list.begin(); it != list.damage_list.end(); ++it)
    {
        if(it->name == player->GetName())
        {
            return 0;
        }
    }

    damageIt = m_damageMap.find(player->GetPlayerID());
    if(damageIt == m_damageMap.end())
    {
        return 0;
    }

    return &(damageIt->second);
}
