#include "gameholder_pch.h"
#include "daily_fashion_manager.h"
#include "center_handler.h"
#include "player.h"
#include "player_manager.h"
#include "daily_fashion_config.h"
#include "package_manager.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "shorcut_manager.h"

IMPLEMENT_SINGLETON(DailyFashionManager)

DailyFashionManager::DailyFashionManager(void)
{
}

DailyFashionManager::~DailyFashionManager(void)
{
}

void DailyFashionManager::OnRecv( uint64 playerID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_DAILY_FASHION_DB_INFO_ACK:
        OnDailyFashionInfoAck(data, playerID);
        break;
    case MSG_DAILY_FASHION_INFO_REQ:
        GetDailyFashionInfo(data, playerID);
        break;
    case MSG_DAILY_FASHION_DRAW_REQ:
        OnDailyFashionDrawReq(data, playerID);
        break;
    default:
        break;
    }
}

void DailyFashionManager::OnDailyFashionInfoAck( void* data, uint64 playerID )
{
    DAILY_DRAW_FASHION_INFO* info = (DAILY_DRAW_FASHION_INFO*)data;
    if(!info) 
        return;

    m_dailyFashionInfo.Clear();
    m_dailyFashionInfo.cur_week = info->cur_week;
    m_dailyFashionInfo.cur_day = info->cur_day;
    m_dailyFashionInfo.next_week = info->next_week;
    m_dailyFashionInfo.last_time = info->last_time;
}

void DailyFashionManager::OnServerReady()
{
    NONE sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_DAILY_FASHION_DB_INFO_REQ, &sendMsg);
}

void DailyFashionManager::GetDailyFashionInfo( void* data, uint64 playerID )
{
    // 返回信息
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    player->SendToGate(MSG_DAILY_FASHION_INFO_ACK, &m_dailyFashionInfo);
}

void DailyFashionManager::OnDailyFashionDrawReq( void* data, uint64 playerID )
{
    FASHION_GET_TREASURE_REQ* info = (FASHION_GET_TREASURE_REQ*)data;
    if(!info)
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    uint32 package_id = DailyFashionConfig::Instance()->GetDailyDrawFashionPackageID(m_dailyFashionInfo.cur_week, m_dailyFashionInfo.cur_day, info->type);

    uint32 money = DailyFashionConfig::Instance()->GetConsumeMoney((uint32)info->type);
    if(money < 1)
    {
         player->NotifyErrorCode(ERR_FAILED);
         return;
    }
      
    if(info->type == 1)
    {
        if(player->GetShorcutManager()->JudgeDailyFashionFreeTime())
            money = 0;
    }


    if (player->GetMoneyPoint() < (int32)money) // 检查金币数量
    {
        player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    FASHION_GET_TREASURE_ACK sendMsg;
    REWARD_TABLE rewardTable;
    if(info->type == 1)
    {
        player->GetPackageManager()->RandomPackageItem(package_id, &rewardTable.items, PACKAGE_TYPE_ITEM_REWARD);
    }
    else
    {
        REWARD_ITEM willItem = DailyFashionConfig::Instance()->GetDailyDrawFashionWillItem(m_dailyFashionInfo.cur_week);
        rewardTable.items.push_back(willItem);
        for (int32 i = 0; i < 9; i++)
        {
            player->GetPackageManager()->RandomPackageItem(package_id, &rewardTable.items, PACKAGE_TYPE_ITEM_REWARD);
        }
    }

    if(rewardTable.items.size() <= 0)
    {
        player->NotifyErrorCode(ERR_FAILED);
    }

    // 检查包裹
    //if(!player->GetItemManager()->CanAddRewardTable(rewardTable))
    //{
    //    player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
    //    return;
    //}

    //for (std::vector<REWARD_ITEM>::iterator re = rewardTable.items.begin(); re != rewardTable.items.end(); re++)
    //{
    //    player->GetItemManager()->AddItemByNum(re->id, re->num, REWARD_REASON_DAILY_DRAW_FASHION);
    //}

    if(money == 0)
    {
        player->GetShorcutManager()->SetDailyFashionFreeTime();
    }
    else if (player->GetMoneyPoint() > (int32)money) // 魔石足够
    {
        TradeManager::Instance()->SendTradeOrderReq(player, REWARD_REASON_DAILY_DRAW_FASHION_CONSUME, PROP_DAILY_FASHION, 0, 0, money);
    }

    sendMsg.errcode = ERR_SUCCEED;
    sendMsg.type = info->type;
    sendMsg.item_list = rewardTable.items;
    sendMsg.last_finish_time = 0;
    player->SendToGate(MSG_DAILY_FASHION_DRAW_ACK, &sendMsg);

}
