#include "gameholder_pch.h"
#include "ashura_recorder.h"
#include "player.h"
#include "offline_player.h"
#include "ashura_config.h"
#include "event_define.h"
#include "pet.h"
#include "ashura_manager.h"

AshuraRecorder::AshuraRecorder(Player* player) :
m_player(player)
{
    Clear();
    m_Dirty = false;
    m_ashura.Clear();
}

AshuraRecorder::~AshuraRecorder()
{

}

void AshuraRecorder::LoadAshuraInfo(const ASHURA_INFO& info)
{
    m_ashura = info;
}

void AshuraRecorder::FillAshuraInfo(ASHURA_INFO& info)
{
    info = m_ashura;
}

void AshuraRecorder::Update()
{
    if(m_Dirty)
    {
        //Notify();
        m_Dirty = false;
    }
}

void AshuraRecorder::Clear()
{
    //m_ashura.Clear();

    m_Dirty = true;
}

void AshuraRecorder::AddDeadCount(BattleUnit* Attacker)
{
    m_ashura.dead_count++;

    m_Dirty = true;

    BattleInfo(Attacker, NULL);
}
void AshuraRecorder::AddKillCount(BattleUnit* Deader)
{
    m_ashura.kill_count++;
    m_ashura.all_kill_count++;

    m_Dirty = true;

    BattleInfo(NULL, Deader);
}

//
//int32 AshuraRecorder::GetKillCount()
//{
//    return m_ashura.kill_count;
//}
//int32  AshuraRecorder::GetDeadCount()
//{
//    return m_ashura.dead_count;
//}
//
//void AshuraRecorder::Notify()
//{
//    ASHURA_INFO sendMsg;
//    sendMsg.Clear();
//
//    //CalDailyRefresh();
//
//    sendMsg = m_ashura;
//    sendMsg.refresh_time = m_ashura.refresh_time - (uint32)Crown::SDNow().GetTimeValue();
//
//    m_player->SendToGate(MSG_ASHURA_INFO_NTF, &sendMsg);
//}

void AshuraRecorder::BattleInfo(BattleUnit* Attacker, BattleUnit* Deader)
{
    ASHURA_BATTLE_ACK sendMsg;
    sendMsg.Clear();

    if(Attacker == NULL && Deader == NULL)
    {
        CnAssert(false);
        return;
    }

    // 你被杀  攻击者存在
    if(Attacker != NULL)
    {
        AshuraManager::Instance()->AddDeadNum(m_player->GetName(), m_player->GetLevel());

        if(Attacker->GetObjectType() == OBJ_PLAYER)
        {
            Player* attackPlayer = (Player*)Attacker;
            sendMsg.attacker = attackPlayer->GetName();
            m_player->SendTaskEvent(TASK_EVENT_ASHURA_KILL, attackPlayer->GetSex(), 0, 0);
        }
        else if(Attacker->GetObjectType() == OBJ_OFFLINE_PLAYER)
        {
            OfflinePlayer* attackOfflinePlayer = (OfflinePlayer*)Attacker;
            sendMsg.attacker = attackOfflinePlayer->GetPlayerInfo().name;
            m_player->SendTaskEvent(TASK_EVENT_ASHURA_KILL, attackOfflinePlayer->GetPlayerInfo().sex, 0, 0);
        }
        else if(Attacker->GetObjectType() == OBJ_PET)
        {
            Pet* attackPet = (Pet*)Attacker;
            BattleUnit* petMaster = attackPet->GetMaster();
            if(petMaster == NULL)
            {
                sendMsg.attacker = attackPet->GetLogicName();
            }
            else
            {
                if(petMaster->GetObjectType() == OBJ_PLAYER)
                {
                    sendMsg.attacker = ((Player*)petMaster)->GetName();
                }
                else if(petMaster->GetObjectType() == OBJ_OFFLINE_PLAYER)
                {
                    sendMsg.attacker = ((OfflinePlayer*)petMaster)->GetPlayerInfo().name;
                }
                else
                {
                    // 修罗场的宠物应该只有玩家和离线玩家可以召唤
                    CnAssert(false);
                }
            }
        }
        else
        {
            // 不可能存在玩家,离线玩家, 战宠以外的人杀人
            CnAssert(false);
        }
    }

    // 你杀人  被杀者存在
    if(Deader != NULL)
    {
        if(Deader->GetObjectType() == OBJ_PLAYER)
        {
            Player* DeaderPlayer = (Player*)Deader;
            sendMsg.deader = DeaderPlayer->GetName();
            AshuraManager::Instance()->AddKillNum(m_player->GetName(), m_player->GetLevel());
        }
        else if(Deader->GetObjectType() == OBJ_OFFLINE_PLAYER)
        {
            OfflinePlayer* DeaderOfflinePlayer = (OfflinePlayer*)Deader;
            sendMsg.deader = DeaderOfflinePlayer->GetPlayerInfo().name;
            AshuraManager::Instance()->AddKillNum(m_player->GetName(), m_player->GetLevel());
        }
        else if(Deader->GetObjectType() == OBJ_PET)
        {
        }
        else
        {
            // 不可能存在玩家和离线玩家以外的人被杀
            CnAssert(false);
        }
    }

    m_player->SendToGate(MSG_ASHURA_BATTLE_ACK, &sendMsg);
}

void AshuraRecorder::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_ASHURA_GET_AWARD_REQ:
        {
            //OnAshuraGetAwardReq(data);
        }
        break;
    }
}
//
//// ==================================================
//// 获取修罗场奖励
//// ==================================================
//void AshuraRecorder::OnAshuraGetAwardReq(void* data)
//{
//    ASHURA_GET_AWARD_REQ* recvMsg = (ASHURA_GET_AWARD_REQ*)data;
//
//    // 检查领取次数
//    if(m_ashura.delay_count <= 0)
//    {
//        m_player->NotifyErrorCode(ERR_ASHURA_NO_ENOUGH_COUNT);
//        return;
//    }
//
//    // 杀人数且被杀数为0 无奖励
//    if(m_ashura.dead_count == 0 && m_ashura.kill_count == 0)
//    {
//        m_player->NotifyErrorCode(ERR_ASHURA_NO_REWARD);
//        return;
//    }
//
//    REWARD_TABLE rewardList = CalAshuraRewardList();
//
//    // 得到奖励
//    m_player->TakeInRewardTable(&rewardList);
//
//    m_player->SendPlayerEvent(EVENT_ASHUAR_TITLE,m_ashura.dead_count, m_ashura.kill_count,0);
//
//    if(!m_player->FinishAchive(ACHIVE_ASHURA))
//    {
//        m_player->SendPlayerEvent(EVENT_ACHIVE_ASHURA,m_ashura.dead_count, m_ashura.kill_count,0);
//    }
//
//    // 发送杀人数到数据库
//    m_ashura.all_kill_count += m_ashura.kill_count;
//
//    // 清除杀人数
//    m_ashura.kill_count = 0;
//    m_ashura.dead_count = 0;
//    // 扣除领取次数
//    m_ashura.delay_count--;
//
//    // 通知客户端
//    Notify();
//}
//
//void AshuraRecorder::CalDailyRefresh()
//{
//    Crown::CSDDateTime nowTime = Crown::SDNow();
//    // 每天刷新一次
//    if((uint32)nowTime.GetTimeValue() >= m_ashura.refresh_time)
//    {
//        Crown::CSDDateTime tomorrow = nowTime;
//        tomorrow.IncDay(1);
//        tomorrow.SetTime(0,0,0);
//        m_ashura.refresh_time = (uint32)tomorrow.GetTimeValue();
//
//        m_ashura.dead_count = 0;
//        m_ashura.kill_count = 0;
//        m_ashura.delay_count = AshuraConfig::Instance()->GetRefreshCount();
//    }
//}
//
//REWARD_TABLE AshuraRecorder::CalAshuraRewardList()
//{
//    REWARD_TABLE rewardList;        // 要奖励的项
//    rewardList.Clear();
//    rewardList.reason = REWARD_REASON_ASHURAN;
//
//    // 人头数奖励
//    CalPlayerNumReward(rewardList);
//
//    // 称号奖励
//    CalTitleRewardList(rewardList);
//
//    return rewardList;
//}
//
//void AshuraRecorder::CalPlayerNumReward(REWARD_TABLE& rewardTable)
//{
//    // 得到奖励模板
//    const AshLevelReward* rewardTmp = AshuraConfig::Instance()->GetReward(m_player->GetLevel());
//    if(!rewardTmp)
//    {
//        CnError("ashura reward temple leve:%d is not exist\n", m_player->GetLevel());
//        return;
//    }
//
//    // 统计奖励
//    uint32 gamepointCount = 0;
//    uint32 soulpointCount = 0;
//    uint32 expCount = 0;
//    // 杀人奖励
//    CalNumberReward(rewardTmp->m_killRewardList, m_ashura.kill_count, gamepointCount, soulpointCount, expCount);
//    // 被杀奖励
//    CalNumberReward(rewardTmp->m_deadRewardList, m_ashura.dead_count, gamepointCount, soulpointCount, expCount);
//
//    // 最大值检查
//    if(gamepointCount > (uint32)rewardTmp->m_maxGamePoint)
//    {
//        gamepointCount = rewardTmp->m_maxGamePoint;
//    }
//    if(soulpointCount > (uint32)rewardTmp->m_maxSoulPoint)
//    {
//        soulpointCount = rewardTmp->m_maxSoulPoint;
//    }
//    if(expCount > (uint32)rewardTmp->m_maxExp)
//    {
//        expCount = rewardTmp->m_maxExp;
//    }
//
//    REWARD_ITEM rewardItem;
//    rewardItem.Clear();
//    rewardItem.type = PROP_GAMEPOINT;
//    rewardItem.num = gamepointCount;
//    rewardTable.items.push_back(rewardItem);
//    rewardTable.count++;
//
//    rewardItem.Clear();
//    rewardItem.type = PROP_SOULPOINT;
//    rewardItem.num = soulpointCount;
//    rewardTable.items.push_back(rewardItem);
//    rewardTable.count++;
//
//    rewardItem.Clear();
//    rewardItem.type = PROP_EXP;
//    rewardItem.num = expCount;
//    rewardTable.items.push_back(rewardItem);
//    rewardTable.count++;
//}
//
//void AshuraRecorder::CalNumberReward(const AshRewardList& rewardListTmp, uint32& num, uint32& gamepoint, uint32& soulpoint, uint32& exp)
//{
//    // 计算杀人数奖励
//    gamepoint += num * rewardListTmp.m_gamepoint;
//    soulpoint += num * rewardListTmp.m_soulpoint;
//    exp += num * rewardListTmp.m_exp;
//}
//
//void AshuraRecorder::CalTitleRewardList(REWARD_TABLE& rewardTable)
//{
//    // 得到奖励模板
//    const AshLevelReward* rewardTmp = AshuraConfig::Instance()->GetReward(m_player->GetLevel());
//    if(!rewardTmp)
//    {
//        CnError("ashura reward temple leve:%d is not exist\n", m_player->GetLevel());
//        return;
//    }
//
//    CalTitleReward(rewardTmp->m_killRewardList.m_reward, rewardTable);
//    CalTitleReward(rewardTmp->m_deadRewardList.m_reward, rewardTable);
//}
//
//void AshuraRecorder::CalTitleReward(const std::map<uint32, REWARD_TABLE>& rewardTmp, REWARD_TABLE& rewardTable)
//{
//    std::map<uint32, REWARD_TABLE>::const_iterator rewardIt = rewardTmp.begin();
//    for(; rewardIt != rewardTmp.end(); ++rewardIt)
//    {
//        //!!!!
//        if(m_ashura.kill_count > rewardIt->first)
//        {
//            std::vector<REWARD_ITEM>::const_iterator itemIt = rewardIt->second.items.begin();
//            for(; itemIt != rewardIt->second.items.end(); ++itemIt)
//            {
//                REWARD_ITEM items = *itemIt;
//                rewardTable.items.push_back(items);
//                rewardTable.count++;
//            }
//        }
//    }
//}



