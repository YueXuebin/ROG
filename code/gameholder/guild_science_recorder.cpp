#include "gameholder_pch.h"
#include "guild_science_recorder.h"
#include "guild_science_config.h"
#include "expend_manager.h"
#include "trade_manager.h"
#include "guild_manager.h"

GuildScienceRecorder::GuildScienceRecorder(Player* pPlayer) :
m_player(pPlayer)
{
    m_additionList.clear();
    m_lock = false;
}

GuildScienceRecorder::~GuildScienceRecorder()
{

}

void GuildScienceRecorder::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GUILD_SCIENCE_PLAYER_INFO_REQ:
        onGuildSciencePlayerInfo();
        break;
    case MSG_GUILD_ADDITION_PROMOTE_REQ:
        onAdditionPromoteReq(data);
        break;
    case MSG_GUILD_ADDITION_PROMOTE_TO_GAME:
        onGuildAdditionPromoteToGame(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void GuildScienceRecorder::LoadGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info)
{
    m_additionList = info.guild_player_addition_list;

    // 初始化公会科技信息
    const GuildAdditionVert& guildAdditions = GuildScienceConfig::Instance()->GetGuildAdditions();
    for(GuildAdditionVert::const_iterator additionIt = guildAdditions.begin(); additionIt != guildAdditions.end(); ++additionIt)
    {
        GUILD_PLAYER_ADDITION* playerAdditionItem = GetGuildPlayerAddition(additionIt->id);
        if(playerAdditionItem)      // 列表中有当前科技信息
            continue;

        // 列表中没有加入列表
        GUILD_PLAYER_ADDITION playerAddition;
        playerAddition.id = additionIt->id;
        playerAddition.promote_time_gamepoint = 0;
        playerAddition.promote_time_moneypoint = 0;
        m_additionList.push_back(playerAddition);
    }
}

void GuildScienceRecorder::FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info)
{
    info.guild_player_addition_list = m_additionList;
}

void GuildScienceRecorder::CalDailyRefresh()
{
    for(std::vector<GUILD_PLAYER_ADDITION>::iterator additionIt = m_additionList.begin(); additionIt != m_additionList.end(); ++additionIt)
    {
        additionIt->promote_time_gamepoint = 0;
        additionIt->promote_time_moneypoint = 0;
    }
}

void GuildScienceRecorder::AddAdditionPromoteNum(int32 additionID, int32& num)
{
    GUILD_ADDITION* addition = GetGuildAddition(additionID);
    if(!addition)
        return;

    const GuildPromoteLevelTmpl* levelTmp = GuildScienceConfig::Instance()->GetPromoteLevelTmpl(additionID, addition->level);
    if(!levelTmp)
    {
        CnAssert(false);
        return;
    }
    num += num * levelTmp->addvalue / BASE_RATE_NUM;
}

void GuildScienceRecorder::onGuildSciencePlayerInfo()
{
    GUILD_PLAYER_SCIENCE_INFO sendMsg;
    sendMsg.guild_player_addition_list = m_additionList;
    m_player->SendToGate(MSG_GUILD_SCIENCE_PLAYER_INFO_ACK, &sendMsg);
}

void GuildScienceRecorder::onAdditionPromoteReq(void* data)
{
    GUILD_ADDITION_PROMOTE_REQ* recvMsg = (GUILD_ADDITION_PROMOTE_REQ*)data;

    // 公会不存在
    if(!GuildManager::Instance()->GetGuildInfo(m_player->GetGuildID()))
    {
        m_player->NotifyErrorCode(ERR_HAVENT_GUILD);
        return;
    }

    // 公会加成模板
    const GuildAdditionTmpl* additionTmp = GuildScienceConfig::Instance()->GetScienceAdditionTmpl(recvMsg->addition_id);
    if(!additionTmp)
    {
        CnError("guild science addition tmp is not exist! playerID:%d\n", m_player->GetPlayerID());
        return;
    }

    // 个人加成信息
    GUILD_PLAYER_ADDITION* playerAdditionItem = GetGuildPlayerAddition(additionTmp->id);
    if(!playerAdditionItem)
    {
        CnError("guild science player addition is not exist! additionID:%d\n", additionTmp->id);
        return;
    }

    // 捐献类型
    if(recvMsg->cost_type == PROP_GAMEPOINT)
    {
        // 金币捐献
        PromoteGamePoint(playerAdditionItem);
    }
    else if(recvMsg->cost_type == PROP_MONEYPOINT)
    {
        // 魔石捐献
        PromoteMoneyPoint(playerAdditionItem);
    }
}

GUILD_PLAYER_ADDITION* GuildScienceRecorder::GetGuildPlayerAddition(int32 additionID)
{
    for(std::vector<GUILD_PLAYER_ADDITION>::iterator additionIt = m_additionList.begin(); additionIt != m_additionList.end(); ++additionIt)
    {
        if(additionIt->id == additionID)
        {
            return &(*additionIt);
        }
    }
    return NULL;
}

void GuildScienceRecorder::PromoteGamePoint(GUILD_PLAYER_ADDITION* playerAdditionItem)
{
    int32 times = playerAdditionItem->promote_time_gamepoint + 1;
    const GuildPromoteDonatePointTmpl* promoteDonateGamePtTmp = GuildScienceConfig::Instance()->GetPromoteDonateGamePointTmpl(times);
    if(!promoteDonateGamePtTmp)
    {
        // 次数用尽
        m_player->NotifyErrorCode(ERR_GUILD_ADD_GAMEPOINT_TIME_LIMIT);
        return;
    }

    // 检查金币
    if(m_player->GetExpendManager()->CheckGamePoint(promoteDonateGamePtTmp->donate_num) != ERR_SUCCEED)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_GAMEPOINT);
        return;
    }

    if(m_lock == true)
        return;
    m_lock = true;

    // 发给Center
    GUILD_ADDITION_PROMOTE_TO_CENTER sendMsg;
    sendMsg.addition_id = playerAdditionItem->id;
    sendMsg.cost_type = PROP_GAMEPOINT;
    sendMsg.times = times;
    sendMsg.point_num = promoteDonateGamePtTmp->donate_num;
    m_player->SendToCenter(MSG_GUILD_ADDITION_PROMOTE_TO_CENTER, &sendMsg);
}

void GuildScienceRecorder::PromoteMoneyPoint(GUILD_PLAYER_ADDITION* playerAdditionItem)
{
    int32 times = playerAdditionItem->promote_time_moneypoint + 1;
    const GuildPromoteDonatePointTmpl* promoteDonateMoneyPtTmp = GuildScienceConfig::Instance()->GetPromoteDonateMoneyPointTmpl(times);
    if(!promoteDonateMoneyPtTmp)
    {
        // 次数用尽
        m_player->NotifyErrorCode(ERR_GUILD_ADD_MONEYPOINT_TIME_LIMIT);
        return;
    }

    // 魔石够不够
    if(m_player->GetMoneyPoint() < promoteDonateMoneyPtTmp->donate_num)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    if(m_player->GetBuying())
    {
        m_player->NotifyErrorCode(ERR_TRADE_IN_TRADING);
        return;
    }

    if(m_lock == true)
        return;
    m_lock = true;

    // 发给Center
    GUILD_ADDITION_PROMOTE_TO_CENTER sendMsg;
    sendMsg.addition_id = playerAdditionItem->id;
    sendMsg.cost_type = PROP_MONEYPOINT;
    sendMsg.times = times;
    sendMsg.point_num = promoteDonateMoneyPtTmp->donate_num;
    m_player->SendToCenter(MSG_GUILD_ADDITION_PROMOTE_TO_CENTER, &sendMsg);
}

void GuildScienceRecorder::onGuildAdditionPromoteToGame(void* data)
{
    GUILD_ADDITION_PROMOTE_TO_GAME* recvMsg = (GUILD_ADDITION_PROMOTE_TO_GAME*)data;

    // 解锁
    m_lock = false;

    GUILD_PLAYER_ADDITION* playerAdditionItem = GetGuildPlayerAddition(recvMsg->addition_id);
    CnAssert(playerAdditionItem);

    if(recvMsg->err_code != ERR_SUCCEED)
    {
        m_player->NotifyErrorCode(recvMsg->err_code);
        return;
    }

    if(recvMsg->cost_type == PROP_GAMEPOINT)
    {
        const GuildPromoteDonatePointTmpl* promoteDonateTmp = GuildScienceConfig::Instance()->GetPromoteDonateGamePointTmpl(playerAdditionItem->promote_time_gamepoint + 1);
        if(!promoteDonateTmp)
        {
            // 次数用尽
            m_player->NotifyErrorCode(ERR_GUILD_ADD_GAMEPOINT_TIME_LIMIT);
            CnAssert(false);
            return;
        }

        // 扣除金币
        uint16 errCode = m_player->SubItemOne(REWARD_REASON_GUILD_ADD_PROMOTE, PROP_GAMEPOINT, promoteDonateTmp->donate_num);
        if(errCode != ERR_SUCCEED)
        {
            m_player->NotifyErrorCode(ERR_NO_ENOUGH_GAMEPOINT);
            return;
        }

        playerAdditionItem->promote_time_gamepoint++;
        m_player->AddGuildPoint(promoteDonateTmp->guild_point, REWARD_REASON_GUILD_ADD_PROMOTE);
    }
    else if(recvMsg->cost_type == PROP_MONEYPOINT)
    {
        const GuildPromoteDonatePointTmpl* promoteDonateTmp = GuildScienceConfig::Instance()->GetPromoteDonateMoneyPointTmpl(recvMsg->times);
        if(!promoteDonateTmp)
        {
            // 次数用尽
            m_player->NotifyErrorCode(ERR_GUILD_ADD_GAMEPOINT_TIME_LIMIT);
            CnAssert(false);
            return;
        }

        // 正在交易中...
        if(m_player->GetBuying())
        {
            m_player->NotifyErrorCode(ERR_TRADE_IN_TRADING);
            return;
        }

        // 扣魔石trade
        TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_GUILD_ADD_PROMOTE, PROP_GUILD_ADDITION_PROMOTE, 0, 0, promoteDonateTmp->donate_num); // trade

        playerAdditionItem->promote_time_moneypoint++;
        m_player->AddGuildPoint(promoteDonateTmp->guild_point, REWARD_REASON_GUILD_ADD_PROMOTE);
    }

    GUILD_PLAYER_SCIENCE_INFO sendMsg;
    sendMsg.guild_player_addition_list = m_additionList;
    m_player->SendToGate(MSG_GUILD_SCIENCE_PLAYER_INFO_ACK, &sendMsg);
}
//
//void GuildScienceRecorder::CBGuildAdditionPromoteTrade(const std::string& str)
//{
//    Json::Value JValue;
//    Json::Reader Jreader;
//    int32 id = 0;
//
//    if(!Jreader.parse(str, JValue))
//        return;
//
//    int32 addition_id = -1;
//    addition_id = JValue["addition_id"].asInt();
//    if(addition_id < 0)
//    {
//        CnAssert(false);
//        return;
//    }
//
//    int32 moneypoint = -1;
//    moneypoint = JValue["money_point"].asInt();
//    if(moneypoint < 0)
//    {
//        CnAssert(false);
//        return;
//    }
//
//    // 个人加成信息
//    GUILD_PLAYER_ADDITION* playerAdditionItem = GetGuildPlayerAddition(addition_id);
//    if(!playerAdditionItem)
//    {
//        CnError("guild science player addition is not exist! additionID:%d\n", addition_id);
//        return;
//    }
//}

GUILD_ADDITION* GuildScienceRecorder::GetGuildAddition(int32 additionID)
{
    return GuildManager::Instance()->GetGuildAdditionItem(m_player->GetGuildID(), additionID);
}

