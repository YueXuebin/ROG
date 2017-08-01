#include "gameholder_pch.h"
#include "player.h"
#include "level_investment_manager.h"
#include "level_investment_config.h"
#include "trade_manager.h"
#include "investment_manager.h"
#include "gameplaytimes_notifier.h"
#include "game_util.h"

LevelInvestmentManager::LevelInvestmentManager( Player* player ):m_player(player)
{
    Clear();
}

LevelInvestmentManager::~LevelInvestmentManager()
{

}

void LevelInvestmentManager::Clear()
{
    m_waiting_invest_trade_call_back = false;

    m_invested_money = 0;
    m_level_investment_map.clear();

    const LevelInvestmentInfoMap* invetment_map = LevelInvestmentConfig::Instance()->GetLevelInvestmentMap();
    LevelInvestmentInfoMap::const_iterator iter;
    for (iter = invetment_map->begin(); iter != invetment_map->end(); iter++)
    {
        m_level_investment_map.insert(std::make_pair(iter->first, NO_REWARD));
    }
}

void LevelInvestmentManager::LoadLevelInvestmentInfo( LEVEL_INVESTMENT_INFO& level_investment_info )
{
    m_invested_money = level_investment_info.invested_money;
    ONE_LEVEL_INVESTMENT_INFO* info = nullptr;
    for (uint32 i = 0; i < (uint32)level_investment_info.info_list.size(); i++)
    {
        info = &(level_investment_info.info_list[i]);
        m_level_investment_map[info->level] = info->status;
    }
    
}

void LevelInvestmentManager::FillLevelInvestmentInfo( LEVEL_INVESTMENT_INFO& level_investment_info )
{
    level_investment_info.invested_money = m_invested_money;

    ONE_LEVEL_INVESTMENT_INFO info;
    LevelInvesementStatusMap::iterator iter;
    for (iter = m_level_investment_map.begin(); iter != m_level_investment_map.end(); iter++)
    {
        info.Clear();
        info.level = iter->first;
        info.status = (uint8)iter->second;
        level_investment_info.info_list.push_back(info);
    }
}

void LevelInvestmentManager::OnRecv( uint32 msgID, void * data )
{
    switch (msgID)
    {
    case MSG_LEVEL_INVESTMENT_REQ:
        {
          OnLevelInvestmentReq(data);
          break;
        }
    case MSG_LEVEL_INVESTMENT_INFO_REQ:
        {
        OnLevelInvestmentInfoReq(data);
        break;
        }
    case MSG_LEVEL_INVESTMENT_ACCEPT_REWARD_REQ:
        {
          OnLevelInvestmentAcceptRewardReq(data);
          break;
        }
    default:
        break;
    }
}

void LevelInvestmentManager::OnLevelInvestmentReq( void *data )
{
    if (m_waiting_invest_trade_call_back)
        return;

    if (IsInvested())
        return;

    LEVEL_INVESTMENT_REQ* req = (LEVEL_INVESTMENT_REQ*) data;
    if (!LevelInvestmentConfig::Instance()->IsMoneyInInvestmentRange(req->invest_money))
    {
        LEVEL_INVESTMENT_ACK ack;
        ack.Clear();
        ack.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACK, &ack);
        return;
    }

    if (m_player->GetMoneyPoint() < req->invest_money)
    {
        LEVEL_INVESTMENT_ACK ack;
        ack.Clear();
        ack.errcode = ERR_NO_ENOUGH_MONEYPOINT;
        m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACK, &ack);
        return;
    }

    // 发送魔石交易订单,回调内再给客户端回执
    //TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_LEVEL_INVESTMENT,
    //    PROP_LEVEL_INVESTMENT, 0, req->invest_money, req->invest_money);

    m_waiting_invest_trade_call_back = true;
}

void LevelInvestmentManager::CBLevelInvestmentTrade( uint32 money )
{
    m_waiting_invest_trade_call_back = false;
    m_invested_money = money;

    // 先更新到最新状态
    Update();

    LEVEL_INVESTMENT_ACK ack;
    ack.Clear();
    ack.errcode = ERR_SUCCEED;
    FillLevelInvestmentInfo(ack.investment_info);
    m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACK, &ack);

    // 发通告
    INVESTMENT_OPERATION_NTF info;
    info.Clear();
    info.name = m_player->GetName();
    info.type = InvestmentManager::TYPE_LEVEL_INVESTMENT;
    info.operation = InvestmentManager::OPERATION_STORE;
    info.money = money;
    InvestmentManager::Instance()->InvestmentOperationNTF(m_player, &info);
    InvestmentMarquee(MARQUEE_INVWSTMENT, 0);
}

void LevelInvestmentManager::OnLevelInvestmentInfoReq( void *data )
{
    // 先更新到最新状态
    Update();

    LEVEL_INVESTMENT_INFO_ACK ack;
    ack.Clear();
    FillLevelInvestmentInfo(ack.investment_info);

    m_player->SendToGate(MSG_LEVEL_INVESTMENT_INFO_ACK, &ack);
}

void LevelInvestmentManager::OnLevelInvestmentAcceptRewardReq( void *data )
{
    // 先更新到最新状态
    Update();

    LEVEL_INVESTMENT_ACCEPT_REWARD_REQ* req = (LEVEL_INVESTMENT_ACCEPT_REWARD_REQ*) data;

    LevelInvesementStatusMap::iterator iter = m_level_investment_map.find(req->level);
    // 没有该等级奖励,或者状态不符
    if ((iter == m_level_investment_map.end()) || (iter->second != CAN_ACCEPT_REWARD))
    {
        LEVEL_INVESTMENT_ACCEPT_REWARD_ACK ack;
        ack.Clear();
        ack.level = req->level;
        ack.reward_money = 0;
        ack.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    uint32 reward_percent = LevelInvestmentConfig::Instance()->GetLevelInvestmentRewardPercent(req->level);
    if (reward_percent == 0)
    {
        LEVEL_INVESTMENT_ACCEPT_REWARD_ACK ack;
        ack.Clear();
        ack.level = req->level;
        ack.reward_money = 0;
        ack.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACCEPT_REWARD_ACK, &ack);
        return;
    }

    iter->second = ACCEPTED_REWARD;
    uint32 reward_money = m_invested_money * reward_percent / 100;
    
    // 先请求gc存档投资信息,再存档完成回调里给等级投资奖励
    SaveLevelInvestment(req->level, reward_money);
}

void LevelInvestmentManager::SaveLevelInvestment( uint32 level, uint32 reward_money )
{
    SAVE_LEVEL_INVESTMENT_INFO_REQ req;
    req.Clear();
    req.level = level;                  // level用于扩展,若level为0,仅用于请求存档,若非0,需要给相应的等级投资奖励
    req.reward_money = reward_money;    // 将奖励发过去,用于玩家不在线时,通过邮件发奖励
    FillLevelInvestmentInfo(req.investment_info);

    m_player->SendToCenter(MSG_SAVE_LEVEL_INVESTMENT_INFO_REQ, &req);
}

void LevelInvestmentManager::CBSaveLevelInvestment(void* data)
{
    SAVE_LEVEL_INVESTMENT_INFO_ACK* recv_ack = (SAVE_LEVEL_INVESTMENT_INFO_ACK*) data;
    if (recv_ack->level == 0)
        return;

    LEVEL_INVESTMENT_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_SUCCEED;
    ack.level = recv_ack->level;
    ack.reward_money = recv_ack->reward_money;

    if (ack.reward_money > 0)
    {
        // 有奖励
        m_player->AddMoneyPoint(ack.reward_money, REWARD_REASON_LEVEL_INVESTMENT);

        // 发通告
        INVESTMENT_OPERATION_NTF info;
        info.Clear();
        info.name = m_player->GetName();
        info.type = InvestmentManager::TYPE_LEVEL_INVESTMENT;
        info.operation = InvestmentManager::OPERATION_FETCH;
        info.money = ack.reward_money;
        InvestmentManager::Instance()->InvestmentOperationNTF(m_player, &info);
    }

    m_player->SendToGate(MSG_LEVEL_INVESTMENT_ACCEPT_REWARD_ACK, &ack);
    InvestmentMarquee(MARQUEE_LEVEL_INVWSTMENT, ack.reward_money);
}

void LevelInvestmentManager::Update()
{
    if (!IsInvested())
        return;

    bool hasReward = false;
    uint32 player_level = m_player->GetLevel();
    LevelInvesementStatusMap::iterator iter;
    for (iter = m_level_investment_map.begin(); iter != m_level_investment_map.end(); iter++)
    {
        if ((player_level >= iter->first) && (iter->second != ACCEPTED_REWARD))
        {
            iter->second = CAN_ACCEPT_REWARD;

            hasReward = true;
        }
    }

    // 通知客户端是否有奖励可领
    //m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_INVESTMENT, hasReward);
}

bool LevelInvestmentManager::IsInvested()
{
    return m_invested_money > 0;
}

void LevelInvestmentManager::GetAllReward()
{
    Update();

    for(LevelInvesementStatusMap::iterator levelIt = m_level_investment_map.begin(); levelIt != m_level_investment_map.end(); ++levelIt)
    {
        if(levelIt->second != CAN_ACCEPT_REWARD)
            continue;

        uint32 reward_percent = LevelInvestmentConfig::Instance()->GetLevelInvestmentRewardPercent(levelIt->first);
        if (reward_percent == 0)
            continue;

        levelIt->second = ACCEPTED_REWARD;
        uint32 reward_money = m_invested_money * reward_percent / 100;
        // 先请求gc存档投资信息,再存档完成回调里给等级投资奖励
        SaveLevelInvestment(levelIt->first, reward_money);
    }
}

void LevelInvestmentManager::InvestmentMarquee(uint32 type, uint32 money)
{
    SERVER_MARQUEE sendMsg;
    sendMsg.id = type;
    sendMsg.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    if(type == MARQUEE_LEVEL_INVWSTMENT)
    {
        sendMsg.parame_list.push_back(MarQueeLoginReceive(type, money));
    }

    m_player->OnMarQuee(sendMsg);
}
