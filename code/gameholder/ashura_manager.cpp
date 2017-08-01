#include "gameholder_pch.h"
#include "ashura_manager.h"
#include "scene.h"
#include "player.h"
#include "player_manager.h"
#include "regular.h"
#include "mail_title_config.h"
#include "mail_manager.h"
#include "activity_manager.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "game_util.h"
#include "gate_handler.h"
#include "event_define.h"
#include "server_config.h"

IMPLEMENT_SINGLETON(AshuraManager)

AshuraManager::AshuraManager()
{
    Clear();
}

AshuraManager::~AshuraManager()
{

}

void AshuraManager::Clear()
{
    m_RankList.clear();
    m_dirty = false;
    m_MinuteTimer = 0;
    m_RankListToOne.Clear();
    m_currentTime = 0;
    m_isOpen = false;
}

void AshuraManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ASHURA_START_NTF:
        OnStart(data);
        break;
    case MSG_ASHURA_END_NTF:
        OnEnd();
        break;
    default:
        break;
    }
}

void AshuraManager::Update()
{
    m_MinuteTimer += g_ServerUpdateTime;

    // 活动开启 活动时间不断上加
    if(m_isOpen)
    {
        // 活动时间不断更新
        m_currentTime += g_ServerUpdateTime;
    }

    if(m_MinuteTimer >= 2000)
    {   // 每2秒更新一次
        m_MinuteTimer = 0;

        if(m_dirty == true)
        {
            m_dirty = false;
            RefreshRankList();
            Notify();
            return;
        }
    }

}

void AshuraManager::AddKillNum(const std::string& killerName, int32 killLevel)
{
    RankInfo rankInfo;
    rankInfo.Clear();

    RankList::iterator killRankIt = m_RankList.begin();
    for(; killRankIt != m_RankList.end(); ++killRankIt)
    {
        if(killRankIt->name == killerName)
        {
            break;
        }
    }
    if(killRankIt == m_RankList.end())
    {
        rankInfo.name = killerName;
        rankInfo.killNum = 1;
        rankInfo.level = killLevel;
        m_RankList.push_back(rankInfo);
    }
    else
    {
        ++killRankIt->killNum;
        killRankIt->level = killLevel;

        Player* pPlayer = PlayerManager::Instance()->GetPlayerByName(killRankIt->name);
        if(pPlayer)
        {
            pPlayer->SendMyselfEvent(EVENT_ASHUAR_TITLE, 0, killRankIt->killNum, 0);
        }
 
        if(killRankIt->killNum == 1000) 
        {
            SERVER_MARQUEE sendMsg;
            sendMsg.id = MARQUEE_ASHURA_KILL;

            // 玩家等级
            std::string player_name = killRankIt->name;

            // 玩家vip等级
            int32 viplevel = 0;
            Player* pPlayer = (Player*)PlayerManager::Instance()->GetPlayerByName(killRankIt->name);
            if(pPlayer)
            {
                viplevel = pPlayer->GetVipLevel();
            }

            sendMsg.parame_list.push_back(MarQueePlayerName(viplevel, player_name));
            Player::OnMarQuee(sendMsg);
        }
    }

    m_dirty = true;
}

void AshuraManager::AddDeadNum(const std::string& deaderName, int32 deadLevel)
{
    RankInfo rankInfo;
    rankInfo.Clear();

    RankList::iterator deadRankIt = m_RankList.begin();
    for(; deadRankIt != m_RankList.end(); ++deadRankIt)
    {
        if(deadRankIt->name == deaderName)
        {
            break;
        }
    }
    if(deadRankIt == m_RankList.end())
    {
        rankInfo.name = deaderName;
        rankInfo.deadNum = 1;
        rankInfo.level = deadLevel;
        m_RankList.push_back(rankInfo);
    }
    else
    {
        ++deadRankIt->deadNum;
        deadRankIt->level = deadLevel;

        Player* pPlayer = PlayerManager::Instance()->GetPlayerByName(deadRankIt->name);
        if(pPlayer)
        {
            pPlayer->SendMyselfEvent(EVENT_ASHUAR_TITLE,deadRankIt->deadNum,0,0);
        }

    }

    m_dirty = true;
}

bool SortKillRank(RankInfo rankA, RankInfo rankB)
{
    return rankA.killNum > rankB.killNum;
}

bool SortDeadRank(RankInfo rankA, RankInfo rankB)
{
    return rankA.deadNum > rankB.deadNum;
}

void AshuraManager::RefreshRankList()
{
    // 排序并通知客户端
    RankSort();
}

void AshuraManager::RankSort()
{
    ASHURA_RANK_ACK rankList;

    ASHURA_RANK_INFO killRankInfo;
    ASHURA_RANK_INFO deadRankInfo;

    int32 rank = 0;

    // 杀人排行
    std::sort(m_RankList.begin(), m_RankList.end(), SortKillRank);
    for(RankList::iterator rankIt = m_RankList.begin(); rankIt != m_RankList.end(); ++rankIt)
    {
        ++rank;
        if(rank <= 10)
        {
            killRankInfo.Clear();
            killRankInfo.rank = rank;
            killRankInfo.name = rankIt->name;
            killRankInfo.num  = rankIt->killNum;
            rankList.kill_rank_list.push_back(killRankInfo);
        }

        rankIt->killRank = rank;
    }

    // 被杀排行
    rank = 0;
    std::sort(m_RankList.begin(), m_RankList.end(), SortDeadRank);
    for(RankList::iterator rankIt = m_RankList.begin(); rankIt != m_RankList.end(); ++rankIt)
    {
        ++rank;
        if(rank <= 10)
        {
            deadRankInfo.Clear();
            deadRankInfo.rank = rank;
            deadRankInfo.name = rankIt->name;
            deadRankInfo.num  = rankIt->deadNum;
            rankList.dead_rank_list.push_back(deadRankInfo);
        }

        rankIt->deadRank = rank;
    }

    // 临时保存一个前10的排行
    m_RankListToOne = rankList;

    // 通知客户端
    rank = 0;
    for(RankList::iterator rankIt = m_RankList.begin(); rankIt != m_RankList.end(); ++rankIt)
    {
        Player* pPlayer = PlayerManager::Instance()->GetPlayerByName(rankIt->name);
        if(pPlayer)
        {
            if(pPlayer->GetScene() && pPlayer->GetScene()->GetRegular()->GetType() == REGULAR_TYPE_ASHURA)
            {
                rankList.my_kill_rank = rankIt->killRank;
                rankList.my_dead_rank = rankIt->deadRank;
                pPlayer->SendToGate(MSG_ASHURA_RANK_ACK, &rankList);
            }
        }
    }
}

void AshuraManager::SendRankToOne(Player* pPlayer)
{
    if(!pPlayer)
    {
        return;
    }

    ASHURA_INFO ashuraInfo;

    ASHURA_RANK_ACK rankAck;
    rankAck = m_RankListToOne;

    for(RankList::iterator rankIt = m_RankList.begin(); rankIt != m_RankList.end(); ++rankIt)
    {
        if(rankIt->name == pPlayer->GetName())
        {
            ashuraInfo.kill_count = rankIt->killNum;
            ashuraInfo.dead_count = rankIt->deadNum;

            rankAck.my_kill_rank = rankIt->killRank;
            rankAck.my_dead_rank = rankIt->deadRank;
            break;
        }
    }

    pPlayer->SendToGate(MSG_ASHURA_INFO_NTF, &ashuraInfo);
    pPlayer->SendToGate(MSG_ASHURA_RANK_ACK, &rankAck);
}

int16 AshuraManager::CanChangeToAshura(CHANGE_DUNGEON_REQ* dungeonReq)
{
    // 判断修罗场是否开启
    if(!IsOpen())
    {
        return ERR_ACTIVITY_NO_OPEN;
    }

    return ERR_SUCCEED;
}

void AshuraManager::OnStart(void* data)
{
    ASHURA_START_NTF* recvMsg = (ASHURA_START_NTF*)data;

    m_RankList.clear();
    m_RankListToOne.Clear();
    m_dirty = true;
    m_currentTime = recvMsg->current_time;
    m_isOpen    = true;
}

void AshuraManager::OnEnd()
{
    SendReward();

    m_RankList.clear();
    m_RankListToOne.Clear();

    // 关闭所有修罗场
    //DungeonManager::Instance()->StopAllRegular(ASHURA_MAP);

    m_currentTime = 0;
    m_isOpen    = false;
}

void AshuraManager::Notify()
{
    ASHURA_INFO sendMsg;

    for(RankList::iterator playerIt = m_RankList.begin(); playerIt != m_RankList.end(); ++playerIt)
    {
        sendMsg.Clear();

        Player* player = PlayerManager::Instance()->GetPlayerByName(playerIt->name);
        if(!player || player->GetNetState() != Player::kStateInGame)
            continue;

        Scene* scene = player->GetScene();
        CnAssert(scene);
        //if(scene->GetDungeonID() != ASHURA_MAP)
        //    continue;

        sendMsg.kill_count = playerIt->killNum;
        sendMsg.dead_count = playerIt->deadNum;
        player->SendToGate(MSG_ASHURA_INFO_NTF, &sendMsg);
    }
}

void AshuraManager::SendReward()
{
    REWARD_TABLE rewardList;

    MailTitleData* mailData = MailTitleConfig::Instance()->GetMailData(MAIL_TITLE_ID_ASHURA_REWARD);
    if(!mailData)
    {
        CnError("mail message id:%d is not exist!\n", MAIL_TITLE_ID_ASHURA_REWARD);
        return;
    }

    // 发奖励
    for(RankList::iterator rankIt = m_RankList.begin(); rankIt != m_RankList.end(); ++rankIt)
    {
        rewardList.Clear();
        rewardList = CalAshuraRewardList(*rankIt);
        FuseSameReward(rewardList);

        
        Player* player = PlayerManager::Instance()->GetPlayerByName(rankIt->name);
        if(player && player->GetScene() && player->GetScene()->GetRegular()->GetType() == REGULAR_TYPE_ASHURA)
        {
            ASHURA_END_ACK sendMsg;
            sendMsg.reward_table = rewardList;
            player->AddRewardTableToPool(rewardList);
            player->SendToGate(MSG_ASHURA_END_ACK, &sendMsg);
        }
        else
        {
            MailManager::SendMessageMail(ServerConfig::Instance()->GetCenterID(), 0, rankIt->name, rewardList.items, MAIL_MESSAGE_TYPE_ASHURA, 
                2, Crown::ToString(rankIt->killRank).c_str(), Crown::ToString(rankIt->deadRank).c_str());
        }

        ASHURA_MARQUEE_NTF marqueeNtf;
        if(rankIt->killRank == 1)
        {
            marqueeNtf.Clear();
            marqueeNtf.type = ASHURA_RANK_TYPE_KILL;
            marqueeNtf.player_name = rankIt->name;
            marqueeNtf.reward_table = rewardList;
            GateHandler::Instance()->SendToAll(MSG_ASHURA_MARQUEE_NTF, &marqueeNtf, BROADCAST_PLAYER_ID);
        }

        if(rankIt->deadRank == 1)
        {
            marqueeNtf.Clear();
            marqueeNtf.type = ASHURA_RANK_TYPE_DEAD;
            marqueeNtf.player_name = rankIt->name;
            marqueeNtf.reward_table = rewardList;
            GateHandler::Instance()->SendToAll(MSG_ASHURA_MARQUEE_NTF, &marqueeNtf, BROADCAST_PLAYER_ID);
        }
    }
}

REWARD_TABLE AshuraManager::CalAshuraRewardList(RankInfo rankInfo)
{
    REWARD_TABLE rewardList;        // 要奖励的项
    rewardList.Clear();
    rewardList.reason = REWARD_REASON_ASHURAN;

    // 人头数奖励
    CalPlayerNumReward(rewardList, rankInfo);

    // 称号奖励
    CalTitleRewardList(rewardList, rankInfo);

    // 排名奖励
    CalRankReward(rewardList, rankInfo);

    return rewardList;
}

void AshuraManager::CalPlayerNumReward(REWARD_TABLE& rewardTable, RankInfo rankInfo)
{
    // 统计奖励
    uint32 gamepointCount = 0;
    uint32 soulpointCount = 0;
    uint32 expCount = 0;

    // 得到奖励模板
    const AshLevelReward* rewardTmp = AshuraConfig::Instance()->GetReward(rankInfo.level);
    if(!rewardTmp)
    {
        CnError("ashura reward temple level:%d is not exist\n", rankInfo.level);
        return;
    }

    // 杀人奖励
    CalNumberReward(rewardTmp->m_killRewardList, rankInfo.killNum, gamepointCount, soulpointCount, expCount);
    // 被杀奖励
    CalNumberReward(rewardTmp->m_deadRewardList, rankInfo.deadNum, gamepointCount, soulpointCount, expCount);

    // 不超过最大值
    if((int32)gamepointCount > rewardTmp->m_maxGamePoint)
        gamepointCount = rewardTmp->m_maxGamePoint;

    if((int32)soulpointCount > rewardTmp->m_maxSoulPoint)
        soulpointCount = rewardTmp->m_maxSoulPoint;

    if((int32)expCount > rewardTmp->m_maxExp)
        expCount = rewardTmp->m_maxExp;

    REWARD_ITEM rewardItem;
    rewardItem.Clear();
    rewardItem.id = PROP_GAMEPOINT;
    rewardItem.num = gamepointCount;
    rewardItem.flag = REWARD_FLAG_NORMAL;
    rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id = PROP_SOULPOINT;
    rewardItem.num = soulpointCount;
    rewardItem.flag = REWARD_FLAG_NORMAL;
    rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id = PROP_EXP;
    rewardItem.num = expCount;
    rewardItem.flag = REWARD_FLAG_NORMAL;
    rewardTable.items.push_back(rewardItem);
}

void AshuraManager::CalNumberReward(const AshRewardList& rewardListTmp, int32 num, uint32& gamepoint, uint32& soulpoint, uint32& exp)
{
    // 计算杀人数奖励
    gamepoint += num * rewardListTmp.m_gamepoint;
    soulpoint += num * rewardListTmp.m_soulpoint;
    exp += num * rewardListTmp.m_exp;
}

void AshuraManager::CalTitleRewardList(REWARD_TABLE& rewardTable, RankInfo rankInfo)
{
    // 得到奖励模板
    const AshLevelReward* rewardTmp = AshuraConfig::Instance()->GetReward(rankInfo.level);
    if(!rewardTmp)
    {
        CnError("ashura reward temple leve:%d is not exist\n", rankInfo.level);
        return;
    }

    CalTitleReward(rankInfo.killNum, rewardTmp->m_killRewardList.m_reward, rewardTable);
    CalTitleReward(rankInfo.deadNum, rewardTmp->m_deadRewardList.m_reward, rewardTable);
}

void AshuraManager::CalTitleReward(int32 num, const std::map<uint32, REWARD_TABLE>& rewardTmp, REWARD_TABLE& rewardTable)
{
    std::map<uint32, REWARD_TABLE>::const_iterator rewardIt = rewardTmp.begin();
    for(; rewardIt != rewardTmp.end(); ++rewardIt)
    {
        if(num > (int32)rewardIt->first)
        {
            rewardTable.items.insert(rewardTable.items.end(), rewardIt->second.items.begin(), rewardIt->second.items.end());
        }
    }
}

void AshuraManager::CalRankReward(REWARD_TABLE& rewardTable, const RankInfo& rankInfo)
{
    int32 exp = 0;
    const REWARD_TABLE* rewards = NULL;
    rewards = AshuraConfig::Instance()->GetKillRankReward(rankInfo.killRank);
    if(rewards)
    {
        rewardTable.items.insert(rewardTable.items.begin(), rewards->items.begin(), rewards->items.end());
    }

    rewards = AshuraConfig::Instance()->GetDeadRankReward(rankInfo.deadRank);
    if(rewards)
    {
        rewardTable.items.insert(rewardTable.items.begin(), rewards->items.begin(), rewards->items.end());
    }

}
 