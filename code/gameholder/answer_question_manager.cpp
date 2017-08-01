#include "gameholder_pch.h"
#include "answer_question_manager.h"
#include "answer_question_config.h"
#include "player.h"
#include "message_encoder.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "sdtime.h"
#include "center_handler.h"
#include "item_manager.h"
#include "vip_config.h"
#include "scene.h"
#include "event_define.h"
#include "trade_manager.h"
#include "activity_manager.h"
#include "game_util.h"
#include "guild_science_recorder.h"
#include "answer_manager.h"
#include "server_config.h"



IMPLEMENT_SINGLETON(AnswerQuestionManager)

// 构造函数
AnswerQuestionManager::AnswerQuestionManager()
{
    Clear();
}

// 析构函数 
AnswerQuestionManager::~AnswerQuestionManager()
{

}

// 清空
void AnswerQuestionManager::Clear()
{
    m_state = ANSWER_STATE_END;             // 活动状态

    m_activityID            = 0;            // 活动ID号
    m_questionID            = 0;            // 当前问题
    m_questionSN            = 0;            // 当前问题序号

    m_rightResult           = 0;
    m_nextQuestionTime      = 0;
    m_openTime              = 0;
    m_readyTime             = 0;

    m_toprank.Clear();                      // 排行信息
}

// 接收网络消息
void AnswerQuestionManager::OnRecv(Player* player, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_ANSWER_READY_REQ:      // 客户端请求开始答题
        SendReadyToPlayer(player);
        break;
    case MSG_ANSWER_READY_ACK:      // cs
        OnAnswerReadAck(data);
        break;
    case MSG_ANSWER_QUESTION_CMF:       // 客户端发送过来答题确认消息 player
        OnAnswerQuestionCmf(player, data);
        break;
    case MSG_ANSWER_QUESTION_NTF:       // 答题题目通知 cs
        OnAnswerQuestionNtf(data);
        break;
    case MSG_ANSWER_RESULT_NTF:         // 答题结果通知 cs
        OnAnswerResultNtf(data);
        break;
    case MSG_BUY_DOUBLE_REQ:            // 请求双倍积分
        OnBuyDouble(player, data);
        break;
    case MSG_ANSWER_TOPRANK_REQ:
        OnAnswerTopRankReq(player, data);
        break;
    case MSG_ANSWER_TOPRANK_ACK:
        OnAnswerTopRankAck(data);
        break;
    case MSG_ANSWER_PLAYER_RANK_REQ:
        OnAnswerPlayerRankReq(player, data);
        break;
    case MSG_ANSWER_REWARD_REQ:
        OnAnswerPlayerRewardReq(player, data);
        break;
    case MSG_ANSWER_REWARD_ACK:
        OnAnswerPlayerRewardAck(player, data);
        break;
    case MSG_ANSWER_END_NTF:
        OnAnswerEndNtf(data);
        break;
    case MSG_ANSWER_EXIT_NTF:
        OnAnswerExit(player);
        break;
    default:
        break;
    }
}

// ==================================================
// 通知答题题目
// ==================================================
void AnswerQuestionManager::OnAnswerQuestionNtf(void* data)
{
    ANSWER_QUESTION_NTF* recvMsg = (ANSWER_QUESTION_NTF*)data;

    if(!recvMsg) return;

    // 检查状态
    SetState(ANSWER_STATE_PROCESS);

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    m_questionID = recvMsg->question_id;                                            // 当前问题的ID
    m_rightResult = AnswerQuestionConfig::Instance()->GetRightResult(m_questionID); // 当前问题的答案
    m_nextQuestionTime = now + recvMsg->left_time;                                  // 回答下一题的时间

    // 检查每个玩家
    //for (std::map<uint64, Player*>::iterator playerIt = PlayerManager::Instance()->GetPlayers().begin(); 
    //    playerIt != PlayerManager::Instance()->GetPlayers().end(); playerIt++)
    //{
    //    ANSWER_INFO& answerInfo = playerIt->second->GetAnswerInfo();

    //    if ((playerIt->second->GetNetState() == Player::kStateInGame) && (answerInfo.answer_version == m_readyTime))
    //    {
    //        if(answerInfo.question_id != m_questionID)
    //        {
    //            answerInfo.continuation = 0;
    //            SendAnswerScoreSave(playerIt->second, answerInfo.add_score);
    //        }
    //    }
    //}

    // 发送题目给客户端
    SendToJoinPlayer(MSG_ANSWER_QUESTION_NTF, recvMsg);
}

// ==================================================
// 答题结果通知
// ==================================================
void AnswerQuestionManager::OnAnswerResultNtf(void* data)
{
    ANSWER_RESULT_NTF sendMsg;

    SetState(ANSWER_STATE_RANK);

    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
    for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); 
        playerIt != playerMap.end(); playerIt++)
    {
        ANSWER_INFO& answerInfo = playerIt->second->GetAnswerInfo();
        if ((playerIt->second->GetNetState() == Player::kStateInGame) && (answerInfo.answer_version == m_readyTime))
        {
            ANSWER_INFO& answerInfo = playerIt->second->GetAnswerInfo();

            if((answerInfo.add_score > 0) && (answerInfo.use_double_flag > 0))
            {
                answerInfo.score += answerInfo.add_score;
            }

            answerInfo.use_double_flag = 0;
            answerInfo.add_score = 0;

            sendMsg.question_id = m_questionID;
            sendMsg.result_id = m_rightResult;
            sendMsg.score = answerInfo.score;

            //CnDbg("Answer score:%d\n", sendMsg.score);

            playerIt->second->SendToGate(MSG_ANSWER_RESULT_NTF, &sendMsg);

            SendAnswerScoreSave(playerIt->second, answerInfo.score);
        }
    }
}

// ==================================================
// 答题TOP排行请求
// ==================================================
void AnswerQuestionManager::OnAnswerTopRankReq(Player* player, void* data)
{
    if(!player) return;

    player->SendToGate(MSG_ANSWER_TOPRANK_ACK, &m_toprank);
}

// ==================================================
// 答题TOP排行应答（CenterServer发送过来的同步消息）
// ==================================================
void AnswerQuestionManager::OnAnswerTopRankAck(void* data)
{
    ANSWER_TOPRANK_INFO* recvMsg = (ANSWER_TOPRANK_INFO*)data;

    if(!recvMsg) return;

    m_toprank = *recvMsg;

    SendToJoinPlayer(MSG_ANSWER_TOPRANK_ACK, recvMsg);
}

// ==================================================
// 答题个人排名请求
// ==================================================
void AnswerQuestionManager::OnAnswerPlayerRankReq(Player* player, void* data)
{
    NONE* recvMsg = (NONE*)data;

    if(!recvMsg || !player) return;

    player->SendToCenter(MSG_ANSWER_PLAYER_RANK_REQ, recvMsg);
}

// ==================================================
// 答题个人奖励请求
// ==================================================
void AnswerQuestionManager::OnAnswerPlayerRewardReq(Player* player, void* data)
{
    NONE* recvMsg = (NONE*)data;

    if(!recvMsg || !player) return;

    // 检查是否已奖励过
    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    if(answerInfo.reward_version >= m_readyTime)
    {
        player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    player->SendToCenter(MSG_ANSWER_REWARD_REQ, recvMsg);
}

// ==================================================
// 答题个人奖励应答
// ==================================================
void AnswerQuestionManager::OnAnswerPlayerRewardAck(Player* player, void* data)
{
    ANSWER_PLAYER_RANK* recvMsg = (ANSWER_PLAYER_RANK*)data;
    const REWARD_TABLE* rewards = 0;
    REWARD_TABLE rewardTable;

    if(!player || !recvMsg)
    {
        return;
    }

    // 检查是否已奖励过
    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    if(answerInfo.reward_version >= m_readyTime)
    {
        player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    rewards = AnswerQuestionConfig::Instance()->GetRankReward(recvMsg->rank);
    if(!rewards)
    {
        return;
    }

    // 工会科技加成：单人问答精魂比例加成
    std::vector<REWARD_ITEM> outList;
    REWARD_ITEM rewardItem;
    for(std::vector<REWARD_ITEM>::const_iterator it = rewards->items.begin(); it != rewards->items.end(); ++it)
    {
        rewardItem = *it;
        if (rewardItem.id == PROP_SOULPOINT)
        {
            player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_SINGLE_ANSWER_SOUL, rewardItem.num);
        }

        outList.push_back(rewardItem);
    }

    answerInfo.reward_version = m_readyTime;

    // 根据防沉迷状态获取物品数量
    rewardTable.reason = REWARD_REASON_ANSWER;
    rewardTable.reason_arg = rewards->reason_arg;
    player->GetFcmRewardList(outList/*rewards->items*/, rewardTable.items);

    player->TakeInRewardTable(&rewardTable);

    player->GetScene()->SendEvent(player->GetId(), EVENT_ANSWER_RANK, recvMsg->rank, 0, 0);

   /* if(!player->FinishAchive(ACHIVE_ANSWERING))
    {
       player->GetScene()->SendEvent(player->GetId(), EVENT_ACHIVE_ANSWERING, recvMsg->rank, answerInfo.continuation, 0);
    }*/
}

// ==================================================
// 答题结束
// ==================================================
void AnswerQuestionManager::OnAnswerEndNtf(void* data)
{
    ANSWER_END_NTF* recvMsg = (ANSWER_END_NTF*)data;

    if(!recvMsg) return;

    if(GetState() != ANSWER_STATE_RANK)
    {
        CnAssert(false);
        return;
    }

    SetState(ANSWER_STATE_END);

    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
    for(PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
    {
        ANSWER_INFO& answerInfo = playerIt->second->GetAnswerInfo();
        if ((playerIt->second->GetNetState() == Player::kStateInGame) && (answerInfo.answer_version == m_readyTime))
        {
            playerIt->second->SendToGate(MSG_ANSWER_END_NTF, recvMsg);

            if(playerIt->second->GetAnswerInfo().continuation == AnswerQuestionConfig::Instance()->GetGenQuestionNum())
            {
                SERVER_MARQUEE sendMsg;
                sendMsg.Clear();
                sendMsg.id = MARQUEE_ANSWER_ALL;
                sendMsg.parame_list.push_back(MarQueePlayerName(playerIt->second->GetVipLevel(), playerIt->second->GetName()));
                Player::OnMarQuee(sendMsg);
            }
        }
    }

    // 所有题都回答完毕,关闭活动
    ActivityManager::Instance()->CloseActivityByType(ACTIVITY_TYPE_ANSWER_QUESTION);
}

// ==================================================
// 退出答题
// ==================================================
void AnswerQuestionManager::OnAnswerExit(Player* player)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if(!player) return;

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();
    answerInfo.answer_version = now;

    player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_ANSWER_QUESTION, ACTIVITY_LEAVE));
}

// ==================================================
// 发送积分
// ==================================================
void AnswerQuestionManager::SendAnswerScoreSave(Player* player, int32 score)
{
    ANSWER_SCORE_SAVE sendMsg;
    sendMsg.Clear();

    if(!player) return;

    sendMsg.score = score;

    player->SendToCenter(MSG_ANSWER_SCORE_SAVE_NTF, &sendMsg);
}

// ==================================================
// 发送答题倍积分应答
// ==================================================
void AnswerQuestionManager::SendAnswerDoubleScoreAck(Player* player, uint16 errcode)
{
    DOUBLE_SCORE_ACK sendMsg;
    sendMsg.Clear();

    if(!player) return;

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    sendMsg.errcode = errcode;
    sendMsg.use_double_flag = answerInfo.use_double_flag;
    sendMsg.buy_double_num = answerInfo.buy_double_num;
    sendMsg.sys_double_num = answerInfo.sys_double_num;

    player->SendToGate(MSG_DOUBLE_SCORE_ACK, &sendMsg);
}

// ==================================================
// 准备答题通知
// ==================================================
void AnswerQuestionManager::OnAnswerReadAck(void* data)
{
    ANSWER_READY_ACK* recvMsg = (ANSWER_READY_ACK*)data;
    uint32 now = 0;
    uint32 doubletimes = 0;

    if(!recvMsg) return;

    now = (uint32)Crown::SDNow().GetTimeValue();

    SetState(ANSWER_STATE_READY);

    SetActivityID(recvMsg->activity_id);
    m_readyTime = recvMsg->ready_time;
    m_openTime = now + recvMsg->left_time;

    m_toprank.Clear();
}

// 回答问题
void AnswerQuestionManager::OnAnswerQuestionCmf(Player* player, void* data)
{
    ANSWER_QUESTION_CMF* recvMsg = (ANSWER_QUESTION_CMF*) data;

    if(!recvMsg || !player) return;

    if(GetState() != ANSWER_STATE_PROCESS) return;

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    // 检查是否参加了答题
    if(answerInfo.answer_version != m_readyTime)
    {
        return;
    }

    // 检查是否为当前正在问的问题
    if(recvMsg->question_id != m_questionID)
    {
        return;
    }

    // 答过这个题目
    if(answerInfo.question_id == m_questionID)
    {
        return;
    }

    // 记录玩家回答过这道题
    answerInfo.question_id = m_questionID;

    // 计算积分
    CountScore(player,recvMsg->result);

    // 活跃度
    player->SendMyselfEvent(LIVENESS_EVENT_TARGET_ANSWER, 1, 0, 0);
}

// 购买双倍积分
void AnswerQuestionManager::OnBuyDouble(Player* player, void* data)
{
    int32 useMoneyPoint = 0;
    uint32 maxBuyTimes = 0;
    uint16 errCode= ERR_FAILED;

    if(!player) return;

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    // 检查是否使用了双倍经验
    if(answerInfo.use_double_flag)
    {
        return;
    }

    // 如果有系统次数，直接使用系统次数
    if(answerInfo.sys_double_num < AnswerQuestionConfig::Instance()->GetSysDoubleTimes())
    {
        answerInfo.use_double_flag = 1;
        ++answerInfo.sys_double_num;
        SendAnswerDoubleScoreAck(player, errCode);
        return;
    }

    // 获得并检查需要花费的魔石数量
    useMoneyPoint = AnswerQuestionConfig::Instance()->GetBuyTimeMoneyPoint();
    if(useMoneyPoint < 1)
    {
        player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    maxBuyTimes = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_51,player->GetVipLevel());

    // 检查购买次数 
    if ((uint32)answerInfo.buy_double_num >= maxBuyTimes)
    {
        return;
    }

    // 检查魔石数
    if (player->GetMoneyPoint() < useMoneyPoint)
    {
        player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    // 消耗购买次数
    answerInfo.buy_double_num++;

    // 修改使用双倍积分
    // !!! 这里由于用户体验，我允许用户打了次擦边球，并没有严格地检验他是否能扣费成功
    answerInfo.use_double_flag = 1;

    // 发送魔石交易订单
    TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_ANSWER_DOUBLE_NUM,
        PROP_ANSWER_DOUBLE_NUM, 0, 1, useMoneyPoint);

    SendAnswerDoubleScoreAck(player, errCode);
}

// 发送准备答题给刚上线玩家
void AnswerQuestionManager::SendReadyToPlayer(Player* player)
{
    ANSWER_READY_ACK sendMsg;
    uint16 errCode = 0;

    if(!player) return;

    // 检查活动状态
    errCode = player->CheckEnterActivity(ACTIVITY_TYPE_ANSWER_QUESTION);
    if(errCode != ERR_SUCCEED)
    {
        sendMsg.err_code = errCode;
        player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
        return;
    }

    if(GetState() == ANSWER_STATE_END)
    {
        sendMsg.err_code = ERR_FAILED;
        player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
        return;
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    if(answerInfo.answer_version > m_readyTime)
    {
        sendMsg.err_code = ERR_ANSWER_HAVE_JOIN;
        player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
        return;
    }

	// 在跨服Server上,不允许参加答题
	if(ServerConfig::Instance()->GetMyType() == GAMESVR_TYPE_CROSS)
	{
		sendMsg.err_code = ERR_ANSWER_IN_CROSS_GAMESERVER;
		player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
		return;
	}

    // 发送参加答题活动的消息
    player->SendTaskEvent(TASK_EVENT_JOIN_ACTIVITY, ACTIVITY_TYPE_ANSWER_QUESTION, 0);

    // 发送资源挽回信息
    player->SendMyselfEvent(GBACK_ACTIVITY_ENTER, ACTIVITY_TYPE_ANSWER_QUESTION, 0, 0);

    player->GetAnswerManager()->CalAnswerInfo(m_readyTime);

    sendMsg.ready_time = m_readyTime;
    sendMsg.left_time = (m_openTime > nowTime)? (m_openTime - nowTime): 0;
    sendMsg.use_double_flag = answerInfo.use_double_flag;
    sendMsg.buy_double_num = answerInfo.buy_double_num;
    sendMsg.sys_double_num = answerInfo.sys_double_num;

    if(GetState() == ANSWER_STATE_READY)
    {
        player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
    }
    else if(GetState() == ANSWER_STATE_PROCESS)
    {
        player->SendToGate(MSG_ANSWER_READY_ACK, &sendMsg);
        SendAnswerQuestionNtf(player);
    }

    player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_ANSWER_QUESTION, ACTIVITY_INTO));
}

// ==================================================
// 发送题目通知
// ==================================================
void AnswerQuestionManager::SendAnswerQuestionNtf(Player* player)
{
    ANSWER_QUESTION_NTF sendMsg;
    uint32 now = 0;

    if(!player) return;

    now = (uint32)Crown::SDNow().GetTimeValue();

    sendMsg.activity_id = GetActivityID();
    sendMsg.question_id = m_questionID;
    sendMsg.question_sn = m_questionSN;
    sendMsg.left_time = (now>m_nextQuestionTime)? (now-m_nextQuestionTime): 0;

    player->SendToGate(MSG_ANSWER_QUESTION_NTF, &sendMsg);
}

// ==================================================
// 计算积分
// ==================================================
void AnswerQuestionManager::CountScore(Player* player, uint8 result)
{
    uint64 playerID = 0;
    uint32 now = 0;
    uint32 diff_time = 0;

    if(!player) return;

    playerID = player->GetPlayerID();

    now = (uint32)Crown::SDNow().GetTimeValue();

    diff_time = (m_nextQuestionTime > now)? (m_nextQuestionTime - now): 0;

    ANSWER_INFO& answerInfo = player->GetAnswerInfo();

    // 答错了
    if(result != m_rightResult)
    {
        answerInfo.continuation = 0;
        return;
    }

    // 加积分
    uint8 timeScore = AnswerQuestionConfig::Instance()->GetTimeScore(diff_time);

    answerInfo.continuation++;

    answerInfo.add_score = timeScore + AnswerQuestionConfig::Instance()->GetContinationScore(answerInfo.continuation);
    answerInfo.score += answerInfo.add_score;

    //SendAnswerScoreSave(player, answerInfo.add_score);
}

// ==================================================
// 发送给参加答题活动的玩家
// ==================================================
void AnswerQuestionManager::SendToJoinPlayer(uint32 msgID, void* data)
{
    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
    for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
    {
        Player* player = playerIt->second;

        if (IsPlayerInAnswer(player))
        {
            playerIt->second->SendToGate(msgID, data);
        }
    }
}

bool AnswerQuestionManager::IsPlayerInAnswer(Player* player)
{
    if(player->GetNetState() != Player::kStateInGame)
        return false;
    
    ANSWER_INFO& answerInfo = player->GetAnswerInfo();
    return answerInfo.answer_version == m_readyTime;
}

