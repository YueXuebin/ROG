#include "centerserver_pch.h"
#include "trade_job.h"
#include "dbdriver.h"
#include "trade_manager.h"
#include "player.h"
#include "player_manager.h"
#include "config_loader.h"
#include "guild_manager.h"

IMPLEMENT_SINGLETON(TradeJob)

// ==================================================
// 获取接收者的ID和账户名
// ==================================================
void TradeJob::GetReceiverIDAndAccount(TRADE_ORDER_INFO& tradeOrder)
{
    char receiver_name[PLAYER_NAME_LEN*2+1] = {0};

    // 收货账户信息
    m_dbExecutor->EscapeString(receiver_name, tradeOrder.receiver_name.c_str(), tradeOrder.receiver_name.length());

    // 插入订单信息
    m_dbExecutor->Query(this, &TradeJob::CBGetReceiverIDAndAccount, tradeOrder,
        "SELECT `player_id`, `account_name` FROM `player` WHERE `player_name`='%s'", receiver_name);
}

// ==================================================
// 插入加入交易订单信息
// ==================================================
void TradeJob::InsertTradeInfo(TRADE_ORDER_INFO& tradeOrder)
{
    uint64 orderID = 0;
    uint64 buyerID = 0;
    uint64 receiverID = 0;

    char buyer_account[PLAYER_ACCOUNT_LEN*2+1] = {0};
    char buyer_name[PLAYER_NAME_LEN*2+1] = {0};
    char receiver_account[PLAYER_ACCOUNT_LEN*2+1] = {0};
    char receiver_name[PLAYER_NAME_LEN*2+1] = {0};
    char ext[1024] = {0};

    // 写订单号
    orderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    // 购买者信息
    m_dbExecutor->EscapeString(buyer_account, tradeOrder.buyer_account.c_str(), tradeOrder.buyer_account.length());
    m_dbExecutor->EscapeString(buyer_name, tradeOrder.buyer_name.c_str(), tradeOrder.buyer_name.length());
    buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);

    // 收货账户信息
    m_dbExecutor->EscapeString(receiver_account, tradeOrder.receiver_account.c_str(), tradeOrder.receiver_account.length());
    m_dbExecutor->EscapeString(receiver_name, tradeOrder.receiver_name.c_str(), tradeOrder.receiver_name.length());
    receiverID = U64ID(tradeOrder.receiver_id.id_h, tradeOrder.receiver_id.id_l);

    // 扩展参数
    m_dbExecutor->EscapeString(ext, tradeOrder.ext.c_str(), tradeOrder.ext.length());

    // 检查删除货币的总价格
    if((tradeOrder.unit_price < 1) || (tradeOrder.total_price < 1))
    {
        uint64 buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
        Player* pBuyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
        if(pBuyer)
        {
            tradeOrder.err_code = ERR_FAILED;
            TradeManager::Instance()->SendTradeOrderAck(pBuyer, tradeOrder);
        }
        return;
    }

    // 插入订单信息
    m_dbExecutor->Query(this, &TradeJob::CBInsertTradeInfo, tradeOrder,
        "INSERT INTO `trade_order` SET `order_id`=%llu, `order_state`=%d, "
        "`buyer_account`='%s', `buyer_name`='%s', `buyer_id`=%llu, "
        "`receiver_account`='%s', `receiver_name`='%s', `receiver_id`=%llu, "
        "`pay_reason`=%d, `product_id`=%u, `product_num`=%d, "
        "`item_type`=%d, `item_id`=%u, `item_num`=%d, "
        "`unit_price`=%d, `total_price`=%d, `param_1`=%d, `ext`='%s', `buy_time`=CURRENT_TIMESTAMP",
        orderID, tradeOrder.order_state,
        buyer_account, buyer_name, buyerID, 
        receiver_account, receiver_name, receiverID, 
        tradeOrder.pay_reason, tradeOrder.product_id, tradeOrder.product_num,
        tradeOrder.item_type, tradeOrder.item_id, tradeOrder.item_num,
        tradeOrder.unit_price, tradeOrder.total_price, tradeOrder.param_1, ext);
}

// ==================================================
// 设置交易订单状态
// ==================================================
void TradeJob::SetTradeInfoPayState(TRADE_ORDER_INFO& tradeOrder)
{
    uint64 orderID = 0;

    orderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    // 插入订单信息
    m_dbExecutor->Query(this, &TradeJob::CBSetTradeInfoPayState, tradeOrder,
        "UPDATE `trade_order` SET `order_state`=%d WHERE `order_id`=%llu", tradeOrder.order_state, orderID);
}

// ==================================================
// 设置交易订单状态
// ==================================================
void TradeJob::SetTradeInfoDeliverState(uint64 tradeOrderID, int32 orderState)
{
    // 插入订单信息
    m_dbExecutor->Query(this, &TradeJob::CBSetTradeInfoDeliverState, tradeOrderID, orderState, 
        "UPDATE `trade_order` SET `order_state`=%d WHERE `order_id`=%llu", orderState, tradeOrderID);
}

// ==================================================
// 保存交易数据
// ==================================================
void TradeJob::DeductMoneyPoint(TRADE_ORDER_INFO& tradeOrder)
{
    uint64 playerID = 0;
    uint64 tradeID = 0;

    playerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    tradeID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    // 插入订单信息
    m_dbExecutor->Query(this, &TradeJob::CBDeductMoneyPoint, tradeOrder,
        "UPDATE trade_order as T1, player as T2 SET "
        "T1.`before_pay_moneypoint` = T2.`money_point`, "
        "T1.`after_pay_moneypoint` = T2.`money_point`-%d, "
        "T2.`money_point`=T2.`money_point`-%d "
        "WHERE T1.`order_id`=%llu AND T2.`player_id`=%llu AND T2.`money_point`>0 AND T2.`money_point`>=%d", 
        tradeOrder.total_price, tradeOrder.total_price, tradeID, playerID, tradeOrder.total_price);
}

// ==================================================
// 查询接收者的ID和账号名
// ==================================================
void TradeJob::CBGetReceiverIDAndAccount(DBRecorder& res, TRADE_ORDER_INFO tradeOrder)
{
    uint64 buyerID = 0;
    uint64 receiverID = 0;
    Player* buyer = 0;

    buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    buyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);

    // 插入订单信息失败
    if(!res.HasExecut())
    {
        if(buyer)
        {
            buyer->NotifyErrorCode(ERR_FAILED);

            tradeOrder.err_code = ERR_FAILED;
            TradeManager::Instance()->SendTradeOrderAck(buyer, tradeOrder);
        }
        // !!! TODO 购买者不存在
        //GateHandler::Instance()->SendCreatePlayerAck(ERR_FAILED, gateSvrID, loginID);
        return;
    }

    if(res.GetRowNum() < 1)
    {
        if(buyer)
        {
            buyer->SendMallBuyNtf(ERR_PLAYER_NOT_EXIT);
        }
        return;
    }

    // 接收者ID
    res.GetRecord(receiverID);

    tradeOrder.receiver_id.id_h = U64ID_HIGH(receiverID);
    tradeOrder.receiver_id.id_l = U64ID_LOW(receiverID);

    // 接收者名称
    res.GetRecord(tradeOrder.receiver_account);

    TradeJob::Instance()->InsertTradeInfo(tradeOrder);
}

// ==================================================
// 插入交易订单信息
// ==================================================
void TradeJob::CBInsertTradeInfo(DBRecorder& res, TRADE_ORDER_INFO tradeOrder)
{
    Player* buyer = 0;
    uint64 buyerID = 0;
    uint64 tradeOrderID = 0;

    // 插入订单信息失败
    if(!res.HasExecut())
    {
        // 获取购买者的指针，给购买者发送错误消息
        buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
        tradeOrderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);
        buyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
        if(buyer)
        {
            buyer->NotifyErrorCode(ERR_TRADE_ORDER_FAILED);
            tradeOrder.err_code = ERR_FAILED;
            TradeManager::Instance()->SendTradeOrderAck(buyer, tradeOrder);
        }
        CnInfo("TradeOrderID:%llu set pay state:%d failed\n", tradeOrderID, tradeOrder.order_state);
        return;
    }

    // 扣除魔石
    DeductMoneyPoint(tradeOrder);
}

// ==================================================
// 设置订单状态
// ==================================================
void TradeJob::CBSetTradeInfoPayState(DBRecorder& res, TRADE_ORDER_INFO tradeOrder)
{
    Player* player = 0;
    uint64 playerID = 0;
    uint64 tradeOrderID = 0;

    tradeOrderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    playerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    if(res.GetAffNum() < 1)
    {
        CnInfo("TradeOrderID:%llu set pay state:%d failed\n", tradeOrderID, tradeOrder.order_state);
        goto _END;
    }

    CnInfo("TradeOrderID:%llu set pay state:%d succeed\n", tradeOrderID, tradeOrder.order_state);

    if(TRADE_STATE_PAY_SUCCEED != tradeOrder.order_state)
    {
        goto _END;
    }

    // 检查买家是否在线，如果在线则需要同步魔石
    if(player)
    {
        player->SubMoneyPoint(tradeOrder.total_price);
        player->NotifyGamePlayerProp(PROP_MONEYPOINT, player->GetMoneyPoint());

        // 增加使玩家的回存时间
        player->QuicklyAutoSave();
    }

    if(tradeOrder.pay_reason == REWARD_REASON_GUILD_BOSS_EXP)
    {
        GuildManager::Instance()->DeliverTradeOrder(tradeOrder);
    }
    else
    {
        TradeManager::Instance()->DeliverTradeOrder(tradeOrder);
    }

    return;
_END:
    if(player)
    {
        tradeOrder.err_code = ERR_FAILED;
        TradeManager::Instance()->SendTradeOrderAck(player, tradeOrder);
    }
}

// ==================================================
// 设置订单状态
// ==================================================
void TradeJob::CBSetTradeInfoDeliverState(DBRecorder& res, uint64 tradeOrderID, int32 orderState)
{
    if(res.GetAffNum() < 1)
    {
        CnInfo("TradeOrderID:%llu set state:%d failed\n", tradeOrderID, orderState);
        return;
    }

    CnInfo("TradeOrderID:%llu set state:%d succeed\n", tradeOrderID, orderState);
}

// ==================================================
// 减少魔石
// ==================================================
void TradeJob::CBDeductMoneyPoint(DBRecorder& res, TRADE_ORDER_INFO tradeOrder)
{
    uint64 tradeOrderID = 0 ;

    tradeOrderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    if(res.GetAffNum() < 1)
    {
        tradeOrder.order_state = TRADE_STATE_PAY_FAILED;
        CnInfo("TradeOrderID:%llu deduct money:%d failed\n", tradeOrderID, tradeOrder.total_price);
    }
    else
    {
        tradeOrder.order_state = TRADE_STATE_PAY_SUCCEED;
        CnInfo("TradeOrderID:%llu deduct money:%d succeed\n", tradeOrderID, tradeOrder.total_price);
    }

    SetTradeInfoPayState(tradeOrder);
}

bool TradeJob::SaveInvestmentOperation( void* data )
{
    INVESTMENT_OPERATION_NTF* info = (INVESTMENT_OPERATION_NTF*) data;
    if (!info)
        return false;

    DBRecorder recorder;
    Json::FastWriter writer;
    Json::Value info_json;
    std::string info_txt;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};

    info_json["name"] = info->name;
    info_json["type"] = info->type;
    info_json["operation"] = info->operation;
    info_json["money"] = info->money;
    info_txt = writer.write(info_json);

    m_dbExecutor->EscapeString(sqlTxt, info_txt.c_str(), info_txt.length());
    recorder = m_dbExecutor->Query("INSERT INTO `investment_operation` SET `operation_info`='%s'", sqlTxt);

    if(!recorder.HasExecut())
        return false;

    return true;


}

// 加载投资操作
void TradeJob::LoadInvestmentOperation( uint32 count, uint64 playerID)
{
    m_dbExecutor->Query(this, &TradeJob::CBLoadInvestmentOperation, playerID,
        "SELECT `operation_info` FROM `investment_operation` ORDER BY `operation_index` DESC LIMIT %d", count);
}

// 加载投资操作回调,给玩家发MSG_INVESTMENT_OPERATION_ACK
void TradeJob::CBLoadInvestmentOperation( DBRecorder& res, uint64 playerID )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if (!player)
        return;

    INVESTMENT_OPERATION_ACK ack;
    
    uint32 ack_count = (int32)res.GetRowNum();  // 获取的数量是自定义的,一般不会超过int32

    Json::Reader reader;
    Json::Value json_info;
    std::string string_info;
    INVESTMENT_OPERATION_NTF info;
    for (uint32 i = 0; i < ack_count; i++)
    {
        res.GetRecord(string_info);
        if (!reader.parse(string_info, json_info, false))
        {
            CnError("Parse investment_operation:%s failed\n", string_info.c_str());
            return;
        }
        
        info.Clear();
        info.name = json_info["name"].asString();
        info.type = json_info["type"].asUInt();
        info.operation = json_info["operation"].asUInt();
        info.money = json_info["money"].asUInt();
        
        ack.operation_list.push_back(info);
    }
    
    player->SendToGate(MSG_INVESTMENT_OPERATION_ACK, &ack);
}
