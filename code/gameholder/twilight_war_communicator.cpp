#include "gameholder_pch.h"
#include "twilight_war_communicator.h"
#include "prop_notifier.h"
#include "boss.h"
#include "player.h"
#include "player_manager.h"
#include "game_define.h"
#include "regular.h"
#include "scene.h"
#include "player_manager.h"
#include "battle_recorder.h"
#include "dungeon_set.h"
#include "event_define.h"
#include "pet.h"
#include "world_boss_config.h"
#include "center_handler.h"
#include "guild_manager.h"
#include "guild_config.h"
#include "world_boss_manager.h"
#include "mail_manager.h"
#include "guild_manager.h"
#include "scene.h"
#include "dungeon.h"
#include "item_manager.h"
#include "game_util.h"
#include "server_config.h"

TwilightWarCommunicator::TwilightWarCommunicator(DungeonSet* dungeonSet):
BattleUnitCommunicator(BattleUnitCommunicator::TWILIGHT_WAR_COMMUNICATOR, dungeonSet)
{
    Clear();
    m_startcreatebooss = (uint32)Crown::SDNow().GetTimeValue();
}

TwilightWarCommunicator::~TwilightWarCommunicator()
{
    //DisconnectAllBoss();
}

void TwilightWarCommunicator::OnConnect(BattleUnit* pBattleUnit)
{
#ifdef DEBUG
    if(m_bossCreatureID != 0)
        CnAssert(m_bossCreatureID == ((Boss*)pBattleUnit)->GetCreatureId());
#endif

    CnDbg("TwilightWarCommunicator::OnConnect, start,%d \n", m_start?1:0);

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

void TwilightWarCommunicator::OnDisconnect(BattleUnit* pBattleUnit)
{
    for(std::vector<Boss*>::iterator bossIt = m_bosses.begin(); 
            bossIt != m_bosses.end(); ++bossIt)
    {
        if(pBattleUnit != *bossIt) continue;
        m_bosses.erase(bossIt);
        break;
    }
}

void TwilightWarCommunicator::OnDead(BattleUnit* attacker)
{
    DungeonSet* dungeonSet = 0;
    Player* player = 0;

    CnDbg("TwilightWarCommunicator::OnDead, m_start,%d \n", m_start);
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

    // 有输出就算赢
    OnEnd(true);
}

void TwilightWarCommunicator::OnDamage(BattleUnit* pAttacker, int32 damage)
{
    Player* player = 0;
    uint64  guildID = 0;
    GUILD_INFO* guildInfo = 0;
    TwilightWarDamageInfo damageInfo;

    if(!pAttacker) return;

    if(pAttacker->GetObjectType() == OBJ_PLAYER)
    {
        // 获得攻击者的指针
        player = (Player*)pAttacker;
    }
    else if(pAttacker->GetObjectType() == OBJ_PET)
    {
        // 如果攻击者为宠物，则获得宠物的主人指针
        player = CnDynamicCast(Player, ((Pet*)pAttacker)->GetMaster());
    }

    if(!player) return;

    guildID = player->GetGuildID();

    if(m_damageMap.find(guildID) == m_damageMap.end())
    {
        damageInfo.Clear();
        damageInfo.guild_name   = player->GetGuildName();
        damageInfo.guild_id     = guildID;
        damageInfo.damage       = 0;

        guildInfo = GuildManager::Instance()->GetGuildInfo(damageInfo.guild_id);
        if(guildInfo)
        {
            damageInfo.battle_power = guildInfo->battle_power;
            damageInfo.level = guildInfo->level;
        }

        m_damageMap[guildID] = damageInfo;
        m_damageList.push_back(&m_damageMap[guildID]);
    }

    m_damageMap[guildID].damage += damage;
    player->GetBattleRecorder()->SetWorldBossDamage(m_damageMap[guildID].damage);

    // 总伤害量
    m_allDamage += damage;
}

void TwilightWarCommunicator::OnEnd(bool success)
{
    DungeonSet* dungeonSet = GetDungeonSet();
    std::set<uint64> scenesPlayers;
    Player* player = 0;

    CnDbg("TwilightWarCommunicator::OnEnd, m_start,%d \n", m_start?1:0);
    if(!m_start) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(m_playerID);

    for(uint32 i=0; i<m_bosses.size(); i++)
    {
        if(!m_bosses[i]->IsDead())
            m_bosses[i]->Dead(NULL, NULL);
    }

    if(player)
    {
        m_attackGuild = player->GetGuildName();
    }

    // 世界Boss死亡，最后一个排序
    UpdateDamage();

    // 世界Boss击杀处理
    BossKillProcess(player);

    dungeonSet = GetDungeonSet();
    if(dungeonSet)
    {
        dungeonSet->SendEvent(BATTLE_EVENT_GUILD_BOSS_DEAD, m_bossCreatureID, 0, 0);
    }

    SendTwilightWarRankSave();

    if(success)
    {
        BossLevelUp();
    }
    // ====================================================================================================

    // 计算场景中所有的角色
    GetScenesPlayers(scenesPlayers);

    // 奖励排行
    RewardByRank(scenesPlayers);

    // 清除伤害信息
    ClearDamageInfo();

    m_start = false;
    m_startcreatebooss = 0;
}

void TwilightWarCommunicator::Clear()
{
    m_start = true;
    m_bossName = "";
    m_attackGuild = "";
    m_playerID = 0;
    m_SecondTimer = 0;

    for(std::vector<Boss*>::iterator it = m_bosses.begin(); it != m_bosses.end(); ++it)
    {
        (*it)->SetCommunicator(NULL);
    }

    ClearDamageInfo();
}

void TwilightWarCommunicator::ClearDamageInfo()
{
    m_bossCreatureID = 0;
    m_allDamage = 0;
    m_damageList.clear();
    m_damageMap.clear();
}

void TwilightWarCommunicator::BossKillProcess(BattleUnit* KillAttacker)
{
    Crown::CSDDateTime now = Crown::SDNow();

    // 公会(三头犬=30200)
    WorldBossManager::Instance()->SetBossDeadTime(m_bossCreatureID, (uint32)now.GetTimeValue());

    // 更新世界BOSS信息
    WorldBossManager::Instance()->SendWorldBossInfoToCenter();
}

void TwilightWarCommunicator::Update()
{
    m_SecondTimer += g_ServerUpdateTime;
    const int32 SecondTimer = 1000;
    if(m_SecondTimer > SecondTimer)
    {
        SecondUpdate();
        m_SecondTimer -= SecondTimer;
    }
}

bool TwilightWarCommunicator::SortDamage(TwilightWarDamageInfo* a, TwilightWarDamageInfo* b)
{
    return a->damage > b->damage;
}

TwilightWarCommunicator::TwilightWarDamageInfo* TwilightWarCommunicator::GetNotifyGuildDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player)
{
    DamageMap::iterator damageIt;

    if(!player) return 0;

    for(std::vector<WORLD_BOSS_DAMAGE_INFO>::iterator it = list.damage_list.begin(); it != list.damage_list.end(); ++it)
    {
        if(it->name == player->GetGuildName())
        {
            return 0;
        }
    }

    damageIt = m_damageMap.find(player->GetGuildID());
    if(damageIt == m_damageMap.end())
    {
        return 0;
    }

    return &(damageIt->second);
}

void TwilightWarCommunicator::SecondUpdate()
{
    UpdateDamage();
}

void TwilightWarCommunicator::UpdateDamage()
{
    WORLD_BOSS_DAMAGE_LIST topDamageList;
    WORLD_BOSS_DAMAGE_INFO guildDamageInfo;
    WORLD_BOSS_DAMAGE_LIST sendMsg;
    TwilightWarDamageInfo* pInfo = 0;
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
        guildDamageInfo.name           = pInfo->guild_name;
        if(maxHP > 0)
        {
            guildDamageInfo.damage_percent = (int32)((int64)pInfo->damage * BASE_RATE_NUM / maxHP);
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
                guildDamageInfo.name           = pInfo->guild_name;
                if(maxHP > 0)
                {
                    guildDamageInfo.damage_percent = int32((int64)(pInfo->damage) * BASE_RATE_NUM / maxHP);
                }

                sendMsg = topDamageList;
                sendMsg.damage_list.push_back(guildDamageInfo);
                it->second->SendToGate(MSG_GUILD_TWILIGHT_WAR_DAMAGE_NTF, &sendMsg);
            }
            else
            {
                it->second->SendToGate(MSG_GUILD_TWILIGHT_WAR_DAMAGE_NTF, &topDamageList);
            }
        }
    }
}

void TwilightWarCommunicator::SendTwilightWarRankSave()
{
    GUILD_BOSS_DAMAGE_INFO guildDamageInfo;
    GUILD_DAMAGE_RANK_LIST sendMsg;
    TwilightWarDamageInfo* pInfo = 0;
    Player* player = 0;

    int32 num = 0;

    // 前 10 排行
    num = Crown::SDMin((uint32)10, (uint32)m_damageList.size());

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    for(int32 i=0; i<num; i++)
    {
        pInfo = m_damageList[i];
        guildDamageInfo.rank            = i+1;
        guildDamageInfo.name            = pInfo->guild_name;
        guildDamageInfo.damage          = pInfo->damage;
        guildDamageInfo.level           = pInfo->level;
        sendMsg.damage_list.push_back(guildDamageInfo);
    }

    if(sendMsg.damage_list.size() < 1)
        return;

    CenterHandler::Instance()->SendToMasterCenter(MSG_GUILD_TWILIGHT_WAR_RANK_SAVE, &sendMsg);
}

void TwilightWarCommunicator::BossLevelUp()
{
    Crown::CSDDateTime now = Crown::SDNow();
    const GuildBossTmpl* bossTmpl = 0;
    uint32 bossID = 0;

    /// !!! 需要整改

    // 检查是否有这个 BOSS
    bossID = WorldBossManager::Instance()->GetBossID_TwilightWar();
    if(!bossID) 
        return;

  /*  bossTmpl = GuildConfig::Instance()->GetTwilightWarTmplByID(bossID);
    if(!bossTmpl) 
        return;

    bossTmpl = GuildConfig::Instance()->GetTwilightWarTmplByLevel(bossTmpl->level + 1);
    if(!bossTmpl)
        return;*/

    WorldBossManager::Instance()->SetBoss(GUILDBOSS_SAVE_TYPE_ID, bossID , (uint32)now.GetTimeValue());
    WorldBossManager::Instance()->LevelUpBoss(bossID , m_startcreatebooss);
    WorldBossManager::Instance()->SendWorldBossInfoToCenter();
}

void TwilightWarCommunicator::RewardByRank(std::set<uint64>& scenesPlayers)
{
    GUILD_TWILIGHT_WAR_END_NTF sendMsg;
    GUILD_INFO* guildInfo = 0;
    const TwilightWarRankReward* rewardTmpl = 0;
    uint64 memberID = 0;
    Player* player = 0;
    DungeonSet* dungeonSet = GetDungeonSet();

    std::set<uint64>    rewardPlayerSet;

    for(uint32 i=0; i < m_damageList.size()/* && (i < GuildConfig::Instance()->GetTwilightWarRanRewardNum())  By ZCD*/; ++i)
    {
        sendMsg.Clear();

        // 获取公会模板
        guildInfo = GuildManager::Instance()->GetGuildInfo(m_damageList[i]->guild_id);
        if(!guildInfo) continue;

        // 获取公会排名奖励
        rewardTmpl = GuildConfig::Instance()->GetTwilightWarRankReward(i+1);
        if(!rewardTmpl) continue;

        // 公告第一名
        if(i == 0)
        {
            SERVER_MARQUEE mqSendMsg;
            mqSendMsg.Clear();
            mqSendMsg.id = MARQUEE_GUILD_FIRST;
            mqSendMsg.parame_list.push_back(guildInfo->guild_name);
            Player::OnMarQuee(mqSendMsg);
        }

        // 发送邮件奖励物品
        //RewardByRank(guildInfo, rewardTmpl);

        // 发送奖励通知
        sendMsg.err_code = ERR_SUCCEED;
        sendMsg.boss_id = m_bossCreatureID;
        sendMsg.guild_name = m_attackGuild;
        sendMsg.damage = m_damageList[i]->damage;
        sendMsg.rank = i + 1;
        sendMsg.reward_table = rewardTmpl->reward_table;

        for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin(); 
                memberIt != guildInfo->member_list.end(); ++memberIt)
        {
            // 拼公会成员ID
            memberID = U64ID(memberIt->player_id_h, memberIt->player_id_l);

            // 检查场景中是否存在此角色
            if(scenesPlayers.find(memberID) == scenesPlayers.end()) continue;

            // 查找玩家
            player = PlayerManager::Instance()->GetPlayerByPlayerID(memberID);
            if(!player)
            {
                // 通过邮件发奖励
                MailManager::SendMessageMail(ServerConfig::Instance()->GetCenterID(), 0, memberIt->name, rewardTmpl->reward_table.items, MAIL_MESSAGE_TYPE_CRUSADE, 1, Crown::ToString(sendMsg.rank).c_str());
                continue;
            }

            // 发放奖励
            player->AddRewardTableToPool(rewardTmpl->reward_table);

            rewardPlayerSet.insert(memberID);
            int32 currank = 0;
            currank = i + 1;
            player->SendMyselfEvent(EVENT_TWIGHT_WAR,currank,0,0);
            player->SendToGate(MSG_GUILD_TWILIGHT_WAR_END_NTF, &sendMsg);
        }
    }

/*  // By ZCD
    // 发送奖励通知
    sendMsg.Clear();
    sendMsg.err_code = ERR_FAILED;
    sendMsg.boss_id = m_bossCreatureID;
    sendMsg.guild_name = m_attackGuild;

    // 给剩余的玩家弹出提示
    for(std::set<uint64>::iterator playerIt = scenesPlayers.begin(); playerIt != scenesPlayers.end(); ++playerIt)
    {
        if(rewardPlayerSet.find(*playerIt) != rewardPlayerSet.end()) continue;

        player = PlayerManager::Instance()->GetPlayerByPlayerID(*playerIt);
        if(!player) continue;

        rewardPlayerSet.insert(*playerIt);

        player->SendToGate(MSG_GUILD_TWILIGHT_WAR_END_NTF, &sendMsg);
    }
*/

    // 广播暮光之战开启时间
    //GuildManager::Instance()->BroadcastTwilightOpenID();
}
