#include "centerserver_pch.h"
#include "pay_job.h"
#include "dbdriver.h"
#include "player.h"
#include "player_manager.h"
#include "special_activity_manager_gc.h"

IMPLEMENT_SINGLETON(PayJob)

// ==============================
// 插入充值订单信息
// ==============================
void PayJob::InsertPayOrder(PAY_ORDER_INFO& payOrder)
{
    uint64 payID = 0;
    uint64 playerID = 0;

    payID = U64ID(payOrder.payid.id_h, payOrder.payid.id_l);
    playerID = U64ID(payOrder.player_id.id_h, payOrder.player_id.id_l);

    m_dbExecutor->Query(this, &PayJob::CBInsertPayOrder, payOrder, "INSERT INTO `pay_order` SET "
        "`payid`=%llu, `player_id`=%llu, `item_id`=%u, `money_point`=%d, `reason`=%d, `paystate`=%d", 
        payID, playerID, payOrder.item_id, payOrder.money_point, payOrder.reason, payOrder.paystate);
}

// ==============================
// 插入充值订单信息回调
// ==============================
void PayJob::CBInsertPayOrder(DBRecorder& res, PAY_ORDER_INFO payOrder)
{
    uint64 payID = 0;
    uint64 playerID = 0;

    payID = U64ID(payOrder.payid.id_h, payOrder.payid.id_l);
    playerID = U64ID(payOrder.player_id.id_h, payOrder.player_id.id_l);

    if(!res.HasExecut())
    {
        CnError("Insert pay_order failed: %llu, %llu, %u, %d, %d, %d\n",
            payID, playerID, payOrder.item_id, payOrder.money_point, payOrder.reason, payOrder.paystate);
        return;
    }

    UpdatePlayerMoneyPoint(payOrder);
}

// ==============================
// 更新角色魔石数量
// ==============================
void PayJob::UpdatePlayerMoneyPoint(PAY_ORDER_INFO& payOrder)
{
    uint64 payID = 0;
    uint64 playerID = 0;

    payID = U64ID(payOrder.payid.id_h, payOrder.payid.id_l);
    playerID = U64ID(payOrder.player_id.id_h, payOrder.player_id.id_l);

    m_dbExecutor->Query(this, &PayJob::CBUpdatePlayerMoneyPoint, payOrder, 
        "UPDATE `pay_order` AS T1, `player` AS T2 SET T1.`paystate`=%d, "
        "T1.`before_pay_moneypoint`=T2.`money_point`, "
        "T1.`after_pay_moneypoint`=T2.`money_point`+T1.`money_point`, "
        "T2.`money_point`=T2.`money_point`+T1.`money_point` "
        "WHERE T1.payid=%llu AND T2.player_id=%llu", 
        PAY_STATE_SUCCEED, payID, playerID);
}

// ==============================
// 更新好友魔石数量
// ==============================
void PayJob::CBUpdatePlayerMoneyPoint(DBRecorder& res, PAY_ORDER_INFO payOrder)
{
    uint64 payID = 0;
    uint64 playerID = 0;
    Player* player = 0;

    payID = U64ID(payOrder.payid.id_h, payOrder.payid.id_l);
    playerID = U64ID(payOrder.player_id.id_h, payOrder.player_id.id_l);

    if(!res.HasExecut())
    {
        CnError("Update pay_order failed: %llu, %llu, %u, %d, %d, %d\n",
            payID, playerID, payOrder.item_id, payOrder.money_point, payOrder.reason, payOrder.paystate);
        return;
    }

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player && (player->GetState() >= Player::kStateInGame))
    {
        player->AddMoneyPoint(payOrder.money_point, PAY_TYPE_DONGYI);
        player->NotifyGamePlayerProp(PROP_MONEYPOINT, player->GetMoneyPoint());
    }
}

void PayJob::GetTotalRechargeByPlayerId( uint64 playerId )
{
    m_dbExecutor->Query(this, &PayJob::CBGetTotalRechargeByPlayerId, playerId,
        "SELECT `money_point` FROM `recharge_info` WHERE `id`='%d' AND `player_id`='%llu'", RECHARGE_ID_TOTAL_RECHARGE, playerId);
}

void PayJob::CBGetTotalRechargeByPlayerId( DBRecorder& res, uint64 playerId )
{
    int32 money = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(money);
    }

    SpecialActivityManager::Instance()->CBGetTotalRechargeByPlayerId(playerId, money);
}

void PayJob::GetDailyRechargeByPlayerId( uint64 playerId )
{
    m_dbExecutor->Query(this, &PayJob::CBGetDailyRechargeByPlayerId, playerId,
        "SELECT `money_point` FROM `recharge_info` WHERE `id`='%d' AND `player_id`='%llu'", RECHARGE_ID_DAILY_RECHARGE, playerId);
}

// 重新加载充值信息
void PayJob::ReloadRechargeInfo(uint64 playerID)
{
    m_dbExecutor->Query(this, &PayJob::CBReloadRechargeInfo, playerID, 
        "SELECT %d,`money_point`, `money_point`, UNIX_TIMESTAMP(`update_time`) FROM `recharge_total` WHERE `player_id`=%llu UNION "
        "SELECT %d,`money_point`, `max_single`, UNIX_TIMESTAMP(`update_time`) FROM `recharge_daily` WHERE `player_id`=%llu",
        RECHARGE_ID_TOTAL_RECHARGE, playerID,
        RECHARGE_ID_DAILY_RECHARGE, playerID);
}

void PayJob::CBGetDailyRechargeByPlayerId( DBRecorder& res, uint64 playerId )
{
    int32 money = 0;
    if(res.GetRowNum() > 0)
    {
        res.GetRecord(money);
    }

    // 回调
    //SpecialActivityManager::Instance()->CBGetDailyRechargeByPlayerId(playerId, money);
}

// 获得充值信息
void PayJob::CBReloadRechargeInfo(DBRecorder& res, uint64 playerID)
{
    int32 rechargeType = 0;
    PLAYER_RECHARGE_RECORDER rechargeRecord;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    PLAYER_RECHARGE_INFO& rechargeInfo = player->GetRechargeInfo();
    rechargeInfo.Clear();

    for(uint32 i =0; i < res.GetRowNum(); ++i)
    {
        res.GetRecord(rechargeType, false);
        res.GetRecord(rechargeRecord.money_point);
        res.GetRecord(rechargeRecord.max_single);
        res.GetRecord(rechargeRecord.update_time, false);

        if(rechargeType == RECHARGE_ID_TOTAL_RECHARGE)
        {
            rechargeInfo.recharge_total = rechargeRecord;
        }
        else if(rechargeType == RECHARGE_ID_DAILY_RECHARGE)
        {
            rechargeInfo.recharge_daily_list.push_back(rechargeRecord);
        }
    }

    player->SendToGame(MSG_PLAYER_RECHARGE_INFO_NTF, &rechargeInfo);
}
