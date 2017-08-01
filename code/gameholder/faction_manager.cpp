#include "gameholder_pch.h"
#include "faction_manager.h"
#include "player.h"
#include "faction_pk_config.h"
#include "player_manager.h"
#include "activity_manager.h"
#include "game_util.h"
#include "dungeon_manager.h"
#include "regular_faction_pk.h"
#include "dungeon_set.h"

IMPLEMENT_SINGLETON(FactionManager)

// ==================================================
// 构造函数
// ==================================================
FactionManager::FactionManager()
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
FactionManager::~FactionManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void FactionManager::Clear()
{
    m_dirty = false;
    m_factionPlayerList.clear();
    m_blueFactionScore = 0;
    m_redFactionScore = 0;
    m_MinuteTimer = 0;
    m_nextAddScoreTime = FactionPkConfig::Instance()->GetScoreTimeSecond();
    m_currTime = 0;
    m_state = kEnd;
}

void FactionManager::Update()
{
    m_currTime += g_ServerUpdateTime;

    m_MinuteTimer += g_ServerUpdateTime;
    if(m_MinuteTimer >= 1000)
    {   // 每1秒更新一次
        m_MinuteTimer = 0;
        OneSecondUpdate(1000);
        return;
    }
}

void FactionManager::OneSecondUpdate(uint32 delayTime)
{
    if(m_state == kStart)
    {
        m_nextAddScoreTime -= delayTime;
        if(m_nextAddScoreTime <= 0)
        {
            AddTimeScore();
            m_nextAddScoreTime += FactionPkConfig::Instance()->GetScoreTimeSecond();
            SetDirty();
        }

        if(m_dirty)
        {
            m_dirty = false;
            RankRefresh();
        }
        return;
    }
    else if(m_state == kReady)
    {
        // 准备时间结束
        if(m_currTime >= FACTION_PK_READY_TIME)
        {
            m_state = kStart;

            // 分配并跳入规则2
            Start();
        }
    }
}

// ==================================================
// 接收网络消息
// ==================================================
void FactionManager::OnRecv(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FACTION_PK_REQ:                    // 队伍信息请求
        OnFactionPKReq(player, data);
        break;
    case MSG_FACTION_PK_START_NTF:
        OnActivityStart(data);
        break;
    case MSG_FACTION_PK_END_NTF:
        OnActivityEnd();
        break;
    default:
        break;
    }
}

void FactionManager::SendReward()
{
    FACTION_PK_END_ACK sendMsg;

    RankRefresh();

    uint8 winFaction = 0;
    if(m_blueFactionScore > m_redFactionScore)
    {
        winFaction = FACTION_PK_BLUE;
    }
    else if(m_blueFactionScore < m_redFactionScore)
    {
        winFaction = FACTION_PK_RED;
    }

    for(FactionPlayerList::iterator playerIt = m_factionPlayerList.begin(); playerIt != m_factionPlayerList.end(); ++playerIt)
    {
        // 玩家离开不给奖励
        if(playerIt->second.is_leave)
        {
            continue;
        }

        sendMsg.Clear();
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerIt->first);
        if(!player)
        {
            continue;
        }

        const REWARD_TABLE* rewardTmp = NULL;
        // 排名奖励
        rewardTmp = FactionPkConfig::Instance()->GetRankRewards(playerIt->second.rank);
        if(!rewardTmp)
        {
            CnError("faction pk score:%d rewards is not exist!\n", playerIt->second.rank);
            continue;
        }
        sendMsg.reward_table.items.insert(sendMsg.reward_table.items.end(), rewardTmp->items.begin(), rewardTmp->items.end());

        // 排名奖励
        if(playerIt->second.group_type == winFaction)
        {
            rewardTmp = FactionPkConfig::Instance()->GetFactionReward(TRUE);
            sendMsg.success = TRUE;
        }
        else
        {
            rewardTmp = FactionPkConfig::Instance()->GetFactionReward(FALSE);
            sendMsg.success = FALSE;
        }
        sendMsg.reward_table.items.insert(sendMsg.reward_table.items.end(), rewardTmp->items.begin(), rewardTmp->items.end());

        FuseSameReward(sendMsg.reward_table);
        player->AddRewardTableToPool(sendMsg.reward_table);
        player->SendToGate(MSG_FACTION_PK_END_ACK, &sendMsg);

        // 排名第一公告
        if(playerIt->second.rank == 1  && (m_blueFactionScore != m_redFactionScore))
        {
            SERVER_MARQUEE serverMarquee;
            serverMarquee.id = MARQUEE_FACTION_PK;
            serverMarquee.parame_list.push_back(MarQueeFactionPk(playerIt->second.group_type));
            serverMarquee.parame_list.push_back(MarQueePlayerName(player->GetVipLevel(), player->GetName()));
            player->OnMarQuee(serverMarquee);
        }
    }
}

// ==================================================
// 接收网络消息
// ==================================================
void FactionManager::OnFactionPKReq(Player* player, void* data)
{
    CHANGE_DUNGEON_REQ changeDungeonReq;

    if(!player) return;

    //changeDungeonReq.dungeon_id = FACTION_PK_MAP;

    player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &changeDungeonReq);
}

void FactionManager::SetPlayerFaction(Player* player, uint8 factionType)
{
    FACTION_PLAYER_INFO playerInfo;
    playerInfo.Clear();
    uint64 playerID = player->GetPlayerID();

    // 已有玩家
    if(m_factionPlayerList.find(playerID) != m_factionPlayerList.end())
    {
        m_factionPlayerList[playerID].is_leave = FALSE;
        CnError("player id :%d is already in the playerList\n", player->GetPlayerID());
        return;
    }

    playerInfo.player_name = player->GetName();

    // 派别处理
    if(factionType == FACTION_PK_BLUE)
    {
        playerInfo.group_type = FACTION_PK_BLUE;
    }
    else if(factionType == FACTION_PK_RED)
    {
        playerInfo.group_type = FACTION_PK_RED;
    }
    else
    {
        CnAssert(false);
    }

    m_factionPlayerList[playerID] = playerInfo;

    SetDirty();
}

void FactionManager::AddTimeScore()
{
    int32 score = FactionPkConfig::Instance()->GetScoreTimePlayerScore();

    for(FactionPlayerList::iterator playerIt = m_factionPlayerList.begin(); playerIt != m_factionPlayerList.end(); ++playerIt)
    {
        if(playerIt->second.is_leave)
        {
            continue;
        }

        playerIt->second.score += score;
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerIt->first);
        if(player)
        {
            FACTION_PK_ADD_SCORE sendMsg;
            sendMsg.score = score;
            player->SendToGate(MSG_FACTION_PK_ADD_SCORE, &sendMsg);
        }
    }
}

bool FactionPkRankSort(FACTION_PLAYER_INFO* a, FACTION_PLAYER_INFO* b)
{
    return a->score > b->score;
}

void FactionManager::RankRefresh()
{
    std::vector<FACTION_PLAYER_INFO*> rankList;
    rankList.clear();
    // 所有玩家放入排名列表进行排名
    for(std::map<uint64, FACTION_PLAYER_INFO>::iterator playerIt = m_factionPlayerList.begin(); 
        playerIt != m_factionPlayerList.end(); ++playerIt)
    {
        if(playerIt->second.is_leave)
        {
            continue;
        }
        rankList.push_back(&playerIt->second);
    }

    std::sort(rankList.begin(), rankList.end(), FactionPkRankSort);

    int32 rank = 0;
    for(std::vector<FACTION_PLAYER_INFO*>::iterator playerIt = rankList.begin(); playerIt != rankList.end(); ++playerIt)
    {
        (*playerIt)->rank = ++rank;
    }

    SendRankList(rankList);
}

void FactionManager::SendRankList(std::vector<FACTION_PLAYER_INFO*> rankList)
{
    FACTION_PK_RANK_NTF sendMsg;
    FACTION_PLAYER_INFO playerInfo;

    // 保存前10名
    for(std::vector<FACTION_PLAYER_INFO*>::iterator playerIt = rankList.begin(); 
        (playerIt != rankList.end() && ((playerIt - rankList.begin()) <= 10)); ++playerIt)
    {
        playerInfo.Clear();
        playerInfo = **playerIt;

        Player* player = PlayerManager::Instance()->GetPlayerByName((*playerIt)->player_name);
        if(player)
        {
            playerInfo.channel = CHANNEL_ID(player->GetUnionMapID());
        }

        sendMsg.rank_list.push_back(playerInfo);
    }

    // 发送给所有玩家
    for(std::vector<FACTION_PLAYER_INFO*>::iterator playerIt = rankList.begin(); 
        playerIt != rankList.end(); ++playerIt)
    {
        if((*playerIt)->is_leave)
        {
            continue;
        }

        Player* player = PlayerManager::Instance()->GetPlayerByName((*playerIt)->player_name);
        if(!player)
        {
            return;
        }
        sendMsg.my_info = **playerIt;
        player->SendToGate(MSG_FACTION_PK_RANK_NTF, &sendMsg);

        // 发送积分信息
        SendScore(player);
    }
}

void FactionManager::SendScore(Player* player)
{
    if(!player)
    {
        return;
    }

    FACTION_PK_SCORE_NTF sendMsg;
    sendMsg.red_score = m_redFactionScore;
    sendMsg.blue_score = m_blueFactionScore;
    player->SendToGate(MSG_FACTION_PK_SCORE_NTF, &sendMsg);
}

void FactionManager::CalScore(Player* pAttacker, Player* pDeader, RegularFactionPK* regular)
{
    if(!pAttacker || !pDeader || !regular)
    {
        return;
    }

    FACTION_PLAYER_INFO* pAttackerInfo = GetPlayerInfo(pAttacker->GetPlayerID());
    if(!pAttackerInfo)
    {
        return;
    }
    FACTION_PLAYER_INFO* pDeaderInfo = GetPlayerInfo(pDeader->GetPlayerID());
    if(!pDeaderInfo)
    {
        return;
    }

    // 击杀者增加杀人数和连杀数
    pAttackerInfo->kill_count++;
    pAttackerInfo->combo_kill++;

    // 添加积分
    int32 playerScore = 0;  // 个人积分
    int32 groupScore  = 0;  // 团队积分

    // 连杀奖励
    const FactionPKKillScore* comboScore = FactionPkConfig::Instance()->GetComboKillScore(pAttackerInfo->combo_kill);
    if(!comboScore)
    {
        CnError("combo kill:%d tmp is not exist\n", pAttackerInfo->combo_kill);
        return;
    }
    playerScore += comboScore->player_score;
    groupScore += comboScore->group_score;
    regular->ComboKill(pAttackerInfo->player_name, pAttackerInfo->combo_kill);

    // 阻止连杀
    if(pDeaderInfo->combo_kill >= FactionPkConfig::Instance()->GetComboStopNum())
    {
        playerScore += FactionPkConfig::Instance()->GetComboStopPlayerScore();
        groupScore += FactionPkConfig::Instance()->GetComboStopGroupScore();
        regular->StopCombo(pAttackerInfo->player_name, pDeaderInfo->player_name, pDeaderInfo->combo_kill);
    }

    // 杀人数到达
    const FactionPKKillScore* totalScore = FactionPkConfig::Instance()->GetKillTotalScore(pAttackerInfo->kill_count);
    if(totalScore)
    {
        playerScore += totalScore->player_score;
        groupScore += totalScore->group_score;
    }

    // 被杀者去除连杀
    pDeaderInfo->combo_kill = 0;

    pAttackerInfo->score += playerScore;
    if(pAttackerInfo->group_type == FACTION_PK_BLUE)
    {
        m_blueFactionScore += groupScore;
    }
    else if(pAttackerInfo->group_type == FACTION_PK_RED)
    {
        m_redFactionScore += groupScore;
    }

    SetDirty();
}

FACTION_PLAYER_INFO* FactionManager::GetPlayerInfo(uint64 playerID)
{
    FactionPlayerList::iterator playerIt = m_factionPlayerList.find(playerID);
    if(playerIt == m_factionPlayerList.end())
    {
        return NULL;
    }
    return &playerIt->second;
}

void FactionManager::DelPlayer(Player* player)
{
    // 玩家离开标记
    FactionPlayerList::iterator playerIt;
    playerIt = m_factionPlayerList.find(player->GetPlayerID());
    if(playerIt != m_factionPlayerList.end())
    {
        m_factionPlayerList.erase(playerIt);
    }

    m_dirty = true;
}

void FactionManager::OnActivityStart(void* data)
{
    FACTION_PK_START_NTF* recvMsg = (FACTION_PK_START_NTF*)data;
    Clear();
    m_currTime = recvMsg->curr_time;
    m_state = kReady;
}

void FactionManager::OnActivityEnd()
{
    SendReward();

    //DungeonManager::Instance()->StopAllRegular(FACTION_PK_MAP);

    Clear();
    m_currTime = 0;
    m_state = kEnd;
}

uint16 FactionManager::CanEnter(Player* player)
{
    if(!player)
    {
        return ERR_FAILED;
    }

    if(m_state == kEnd)
    {
        return ERR_ACTIVITY_NO_OPEN;
    }

    FACTION_PLAYER_INFO* playerInfo = GetPlayerInfo(player->GetPlayerID());
    if(playerInfo)
    {
        // 玩家离开过就不能再进
        if(playerInfo->is_leave)
        {
            return ERR_TD_NO_ENOUGH_ENTER;
        }
    }

    return ERR_SUCCEED;
}

bool FactionBattlePowerRank(PlayerBattlePower player1, PlayerBattlePower player2)
{
    return player1.battlePower > player2.battlePower;
}

void FactionManager::Start()
{
    std::vector<PlayerBattlePower> battlePowerList;
    battlePowerList.clear();

    // 把等待大厅里的所有玩家战斗力放入战力表
    WaitBattlePowerList(battlePowerList);

    // 战力排行
    std::sort(battlePowerList.begin(), battlePowerList.end(), FactionBattlePowerRank);

    // 战斗力分组
    std::vector<PlayerBattlePower*> highBattlePowerList;
    highBattlePowerList.clear();
    std::vector<PlayerBattlePower*> middleBattlePowerList;
    middleBattlePowerList.clear();
    std::vector<PlayerBattlePower*> lowBattlePowerList;
    lowBattlePowerList.clear();
    std::vector<PlayerBattlePower>::iterator battlePowerIt = battlePowerList.begin();

    int32 size = (int32)battlePowerList.size();
    for(int32 i = 1; i <= size; ++i)
    {
        if(i <= size * 10 / 100)
        {
            highBattlePowerList.push_back(&(*battlePowerIt));
        }
        else if(i <= size * 40 / 100)
        {
            middleBattlePowerList.push_back(&(*battlePowerIt));
        }
        else
        {
            lowBattlePowerList.push_back(&(*battlePowerIt));
        }

        battlePowerIt++;
    }

    // 把玩家丢入对应频道 关卡1
    uint32 channelId = 1;
    int32 currChannelPlayerNum = 0;
    // 前10%
    ChangeScene(highBattlePowerList, channelId, currChannelPlayerNum);
    // 前40%
    ChangeScene(middleBattlePowerList, channelId, currChannelPlayerNum);
    // 其他
    ChangeScene(lowBattlePowerList, channelId, currChannelPlayerNum);
}

void FactionManager::WaitBattlePowerList(std::vector<PlayerBattlePower>& list)
{
    PlayerBattlePower battlePower;
    battlePower.Clear();

    //DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(FACTION_PK_MAP);
    //if(!dungeonSet)
    //{
    //    return;
    //}

    //std::vector<Player*> playerMap = dungeonSet->GetPlayerMap();
    //for(std::vector<Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    //{
    //    battlePower.battlePower = (*playerIt)->GetBattlePower();
    //    battlePower.playerID = (*playerIt)->GetPlayerID();
    //    list.push_back(battlePower);
    //}
}

void FactionManager::ChangeScene(std::vector<PlayerBattlePower*>& battlePowerList, uint32& channelID, int32& currChannelPlayerNum)
{
    std::vector<PlayerBattlePower*>::iterator battlePowerIt;
    while(battlePowerList.size() > 0)
    {
        // 随机取一个丢入关卡1
        uint32 random = Crown::SDRandom((uint32)battlePowerList.size());

        battlePowerIt = battlePowerList.begin() + random;

        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID((*battlePowerIt)->playerID);
        if(!player)
        {
            continue;
        }

        // 不在两派PK地图
        //if(DUNGEON_ID(player->GetUnionMapID()) != FACTION_PK_MAP)
        //{
        //    CnAssert(false);
        //    continue;
        //}

        // 10人一组进入频道
        ++currChannelPlayerNum;
        if(currChannelPlayerNum >= 10)
        {
            ++channelID;
            currChannelPlayerNum = 0;
        }

        // 再次跳入指定频道两派PK地图
        //CHANGE_DUNGEON_REQ changeDungeonReq;
        //changeDungeonReq.dungeon_id = FACTION_PK_MAP;
        //changeDungeonReq.channel_id = channelID;
        //player->SendToCenter(MSG_CHANGE_DUNGEON_REQ, &changeDungeonReq);

        // 去除
        battlePowerList.erase(battlePowerIt);
    }
}
