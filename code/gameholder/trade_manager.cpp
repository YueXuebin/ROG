#include "gameholder_pch.h"
#include "trade_manager.h"
#include "player_manager.h"
#include "player.h"
#include "center_handler.h"
#include "item_manager.h"
#include "shop_manager.h"
#include "json_helper.h"
#include "guild_manager.h"
#include "event_define.h"
#include "game_define.h"
#include "item_config_manager.h"
#include "item_factory.h"



IMPLEMENT_SINGLETON(TradeManager)

TradeManager::TradeManager()
{

}

TradeManager::~TradeManager()
{

}

// ==================================================
// 接收信息
// ==================================================
void TradeManager::OnCSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TRADE_ORDER_ACK:
        RecvTradeOrderAck(data);
        break;
    case MSG_TRADE_ORDER_GROWTH_POINT:
        RecvTradeOrderGrowthPoint(playerID, data);
        break;
    default:
        break;
    }
}

void TradeManager::SendTradeOrderReq(Player* player, int32 pay_reason, 
                                     int32 item_type, uint32 item_id, int32 item_num, int32 total_price, const std::string& ext)
{
    // 超过购买限制的直接设置为购买最大值,此处不能更改值，因为此处接口是已经购买成功的处理，如果有问题也是逻辑的问题
    //item_num = (item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (item_num);

    if( (item_num  < 1 || item_num > g_MaxBuyNum) && (item_type == PROP_ITEM) )
        return;

    if(player->GetBuying())
    {   // 玩家正在购买中,不允许再次购买
        player->NotifyErrorCode(ERR_TRADE_IN_TRADING);
        return;
    }
    player->SetBuying(true);          // 开始交易

    TRADE_ORDER_INFO sendMsg;

    /*sendMsg.buyer_id.id_h = U64ID_HIGH(player->GetPlayerID());
    sendMsg.buyer_id.id_l = U64ID_LOW(player->GetPlayerID());
    sendMsg.buyer_account = player->GetAccount();
    sendMsg.buyer_name = player->GetName();
    sendMsg.receiver_name = player->GetName();
    sendMsg.pay_reason = pay_reason;
    sendMsg.product_id = 0;
    sendMsg.product_num = 0;
    sendMsg.item_type = item_type;
    sendMsg.item_id = item_id;
    sendMsg.item_num = item_num;
    sendMsg.unit_price = total_price;
    sendMsg.total_price = total_price;
    sendMsg.param_1 = 0;
    sendMsg.ext = ext;

    CenterHandler::Instance()->SendByPlayer(player, MSG_TRADE_ORDER_REQ, &sendMsg);*/
}

// ==================================================
// 发送交易订单请求消息(商城购买调用)
// ==================================================
void TradeManager::SendTradeOrderToReq(uint64 buyerID, const std::string& buyerAccount, const std::string& buyerName, 
                                       const std::string& receiverName, int32 pay_reason, 
                                       int32 item_type, uint32 item_id, int32 item_num, int32 unit_price, int32 total_price,
                                       uint32 product_id, int32 product_num, int32 param_1, const std::string& ext)
{
    // 超过购买限制的直接设置为购买最大值,此处不能更改值，因为此处接口是已经购买成功的处理，如果有问题也是逻辑的问题
    //item_num = (item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (item_num);

    if( (item_num  < 1 || item_num > g_MaxBuyNum) && (item_type == PROP_ITEM) )
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
    if(!player)
        return;

    if(player->GetBuying())
    {   // 玩家正在购买中,不允许再次购买
        player->NotifyErrorCode(ERR_TRADE_IN_TRADING);
        return;
    }
    player->SetBuying(true);          // 开始交易

    TRADE_ORDER_INFO sendMsg;

    sendMsg.center_id = player->GetCenterSvrID();
    sendMsg.buyer_id.id_h = U64ID_HIGH(buyerID);
    sendMsg.buyer_id.id_l = U64ID_LOW(buyerID);
    sendMsg.buyer_account = buyerAccount;
    sendMsg.buyer_name = buyerName;
    sendMsg.receiver_name = receiverName;
    sendMsg.pay_reason = pay_reason;
    sendMsg.product_id = product_id;
    sendMsg.product_num = product_num;
    sendMsg.item_type = item_type;
    sendMsg.item_id = item_id;
    sendMsg.item_num = item_num;
    sendMsg.unit_price = unit_price;
    sendMsg.total_price = total_price;
    sendMsg.param_1 = param_1;
    sendMsg.ext = ext;

    CenterHandler::Instance()->SendByPlayer(player, MSG_TRADE_ORDER_REQ, &sendMsg);
}

// 订单交易
void TradeManager::TradeOrder(uint64 buyerID, const std::string& buyerAccount, const std::string& buyerName, 
                              const std::string& receiverName, int32 pay_reason, int32 item_type, 
                              uint32 item_id, int32 item_num, int32 unit_price, int32 total_price, 
                              uint32 product_id /*= 0*/, int32 product_num /*= 0*/, int32 param_1 /*= 0*/, 
                              const std::string& ext /*= ""*/)
{
    // 钻石是不是已经被扣了
    bool isSubMoney = false;
    uint32 errCode = ERR_SUCCEED;
    TRADE_ORDER_INFO sendMsg;

    uint32 itemType = 0;
    ItemBase* pItemBase = NULL;
    const PRODUCT_INFO* pProduct = NULL;
    uint32 moneyType = 0;

    
    // 超过购买限制的直接设置为购买最大值,此处不能更改值，因为此处接口是已经购买成功的处理，如果有问题也是逻辑的问题
    //item_num = (item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (item_num);
    if( (item_num  < 1 || item_num > g_MaxBuyNum) && (item_type == PROP_ITEM) )
        return;

    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
    if(!pPlayer)
        return;

    //if(pPlayer->GetBuying())
    //{   // 玩家正在购买中,不允许再次购买
    //    pPlayer->NotifyErrorCode(ERR_TRADE_IN_TRADING);
    //    return;
    //}
    //pPlayer->SetBuying(true);          // 开始交易

    pProduct = pPlayer->GetShopManager()->GetProduct(item_id);
    moneyType = pPlayer->GetShopManager()->GetMoneyType(pProduct);

    // TODO 后面要加交易的原因
    switch(moneyType)
    {
    case MONEY_TYPE_MONEY_POINT:
        if(!pPlayer->SubMoneyPoint(total_price,pay_reason))
        {
            goto _ERROR;
        }
        break;
    case MONEY_TYPE_GOLD:
        if(!pPlayer->SubGamePoint(total_price,pay_reason))
        {
            goto _ERROR;
        }
        break;
    default:
        CnError("unkown money type :%d", moneyType);
        goto _ERROR;
        break;
    }

    // 到这里玩家身上的钻石，就已经被减掉了
    isSubMoney = true;

    // 创建物品
    itemType = ItemConfigManager::Instance()->GetItemTypeById(item_id);

    switch (itemType)
    {
    case ITEM_TYPE_EQUIP:
        pItemBase = ItemFactory::CreateEquip(item_id);
        break;

    case ITEM_TYPE_CONSUME:
        pItemBase = ItemFactory::CreateConsume(item_id);
        break;

    case ITEM_TYPE_MATERIAL:
        pItemBase = ItemFactory::CreateMaterial(item_id);
        break;

    case ITEM_TYPE_STONE:
        pItemBase = ItemFactory::CreateStone(item_id);
        break;

    default:
        break;
    }

    if(!pItemBase)
    {
        CnWarn("item not found by id: %d", item_id);
        errCode = ERR_UNFOUND_ITEM;
        goto _ERROR;
    }


    // 到时候，这里要记录真实的购买商品的原因 AddToBag(reason,pItem)
    // 加商品的原因
    if (pPlayer->GetItemManager()->AddToBag(pay_reason,pItemBase) != ERR_SUCCEED)
    {
        goto _ERROR;
    }

    return;

    //// 把成功订单消息发给center去做记录
    //// =============================================================
    //sendMsg.center_id = pPlayer->GetCenterSvrID();
    //sendMsg.buyer_id.id_h = U64ID_HIGH(buyerID);
    //sendMsg.buyer_id.id_l = U64ID_LOW(buyerID);
    //sendMsg.buyer_account = buyerAccount;
    //sendMsg.buyer_name = buyerName;
    //sendMsg.receiver_name = receiverName;
    //sendMsg.pay_reason = pay_reason;
    //sendMsg.product_id = product_id;
    //sendMsg.product_num = product_num;
    //sendMsg.item_type = item_type;
    //sendMsg.item_id = item_id;
    //sendMsg.item_num = item_num;
    //sendMsg.unit_price = unit_price;
    //sendMsg.total_price = total_price;
    //sendMsg.param_1 = param_1;
    //sendMsg.ext = ext;
    //// ============================================================

    //TRADE_ORDER_INFO* tradeOrder;
    // // 生成交易订单ID
    ////uint64 orderID = GenTradeUUID();
    //uint64 orderID = 11111;
    //tradeOrder->order_id.id_l = U64ID_LOW(orderID);
    //tradeOrder->order_id.id_h = U64ID_HIGH(orderID);
    //tradeOrder->order_state = TRADE_STATE_INIT;
    //tradeOrder->receiver_name = receiverName;

_ERROR:
    // 失败了，就把魔石再给加回去
    if (isSubMoney)
    {
        // TODO 要写添加魔石的原因
        switch(moneyType)
        {
        case MONEY_TYPE_MONEY_POINT:
            pPlayer->AddMoneyPoint(total_price,pay_reason);
            break;
        case MONEY_TYPE_GOLD:
            pPlayer->AddMoneyPoint(total_price,pay_reason);
            break;
        }
    }
}

bool TradeManager::TradeOrderService(uint64 buyerID, const std::string& buyerAccount, 
                                     const std::string& buyerName, const std::string& receiverName, 
                                     int32 pay_reason, int32 item_type, uint32 item_id, 
                                     int32 item_num, int32 unit_price, int32 total_price, 
                                     uint32 product_id /*= 0*/, int32 product_num /*= 0*/, 
                                     int32 param_1 /*= 0*/, const std::string& ext /*= ""*/)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
    if(!pPlayer)
        return false;

    //if(pPlayer->GetBuying())
    //{   // 玩家正在购买中,不允许再次购买
    //    pPlayer->NotifyErrorCode(ERR_TRADE_IN_TRADING);
    //    return;
    //}
    //pPlayer->SetBuying(true);          // 开始交易

    const PRODUCT_INFO* pProduct = pPlayer->GetShopManager()->GetProduct(item_id);
    uint32 moneyType = pPlayer->GetShopManager()->GetMoneyType(pProduct);

    // TODO 后面要加交易的原因
    switch(moneyType)
    {
    case MONEY_TYPE_MONEY_POINT:
        if(!pPlayer->SubMoneyPoint(total_price,pay_reason))
        {
            return false;
        }
        break;
    case MONEY_TYPE_GOLD:
        if(!pPlayer->SubGamePoint(total_price,pay_reason))
        {
            return false;
        }
        break;
    default:
        CnError("unkown money type :%d", moneyType);
        return false;
        break;
    }

    return true;

    // TODO发给center的消息还没做

//_ERROR:
    // 失败了，就把魔石再给加回去
    //if (isSubMoney)
    //{
    //    // TODO 要写添加魔石的原因
    //    pPlayer->AddMoneyPoint(total_price,1);
    //}
    //return false;
}

bool TradeManager::TradeListOrder(uint64 buyerID, const BuyMap* buyItems, int32 pay_reason)
{
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);
    if(!pPlayer)
        return false;

    // 钱够么
    if(!pPlayer->GetShopManager()->ListIsEnoughMoney(buyItems))
    {
        return false;
    }

    // 获取总共要花多少
    CostMap cost = pPlayer->GetShopManager()->GetListCostMoney(buyItems);

    int32 costMoneyPoint = cost[MONEY_TYPE_MONEY_POINT];
    int32 costGamePoint  = cost[MONEY_TYPE_GOLD];

    if (costMoneyPoint > 0)
    {
        pPlayer->SubMoneyPoint(costMoneyPoint,pay_reason);
    }

    if (costGamePoint > 0)
    {
        pPlayer->SubMoneyPoint(costGamePoint,pay_reason);
    }

    return true;

    // TODO发给center的消息还没做
}

// ==================================================
// 查询接收者的ID和账号名
// ==================================================
//void TradeManager::CBGetReceiverIDAndAccount(DBRecorder& res, TRADE_ORDER_INFO tradeOrder)
//{
    //uint64 buyerID = 0;
    //uint64 receiverID = 0;
    //Player* buyer = 0;

    //buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    //buyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);

    //// 插入订单信息失败
    //if(!res.HasExecut())
    //{
    //    if(buyer)
    //    {
    //        buyer->NotifyErrorCode(ERR_FAILED);

    //        tradeOrder.err_code = ERR_FAILED;
    //        TradeManager::Instance()->SendTradeOrderAck(buyer, tradeOrder);
    //    }
    //    // !!! TODO 购买者不存在
    //    //GateHandler::Instance()->SendCreatePlayerAck(ERR_FAILED, gateSvrID, loginID);
    //    return;
    //}

    //if(res.GetRowNum() < 1)
    //{
    //    if(buyer)
    //    {
    //        buyer->SendMallBuyNtf(ERR_PLAYER_NOT_EXIT);
    //    }
    //    return;
    //}

    //// 接收者ID
    //res.GetRecord(receiverID);

    //tradeOrder.receiver_id.id_h = U64ID_HIGH(receiverID);
    //tradeOrder.receiver_id.id_l = U64ID_LOW(receiverID);

    //// 接收者名称
    //res.GetRecord(tradeOrder.receiver_account);

    //TradeJob::Instance()->InsertTradeInfo(tradeOrder);
//}







// ==================================================
// 请求商城数据
// ==================================================
void TradeManager::RecvTradeOrderAck(void* data)
{
    TRADE_ORDER_INFO* recvMsg = (TRADE_ORDER_INFO*)data;
    // 物品
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    if(!recvMsg)
        return;

    // 角色ID
    uint64 playerID = U64ID(recvMsg->receiver_id.id_h, recvMsg->receiver_id.id_l);

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    // 解锁
    if(player)
        player->SetBuying(false);       // 交易完成

    // 交易失败
    if(recvMsg->err_code != ERR_SUCCEED)
        return;

    // 订单ID
    uint64 tradeOrderID = U64ID(recvMsg->order_id.id_h, recvMsg->order_id.id_l);
    uint32 buy_reason = recvMsg->param_1;

    rewardTable.reason = recvMsg->pay_reason;
    //rewardItem.type = recvMsg->item_type;
    rewardItem.id = recvMsg->item_id;
    rewardItem.num = recvMsg->item_num;
    rewardTable.items.push_back(rewardItem);

    // 发放交易物品
    //switch(buy_reason)
    //{
    //case BUY_REASON_AUTO:
    //    ItemManager::AddRewardTableToHideBag(recvMsg->center_id, playerID, rewardTable, tradeOrderID);
    //    break;
    //case BUY_REASON_NORMAL:
    //default:
    //    ItemManager::AddRewardTableWithEmail(recvMsg->center_id, playerID, rewardTable, tradeOrderID);
    //    break;
    //}

    // 客户端根据这个消息可以知道下一步具体做什么
    //ShopManager::SendMallBuyNtf(playerID, ERR_SUCCEED);

    // 处理交易的扩展信息

    if(player && (player->GetNetState() >= Player::kStateInGame))
    {
        // 消费
        player->SendTaskEvent(TASK_EVENT_CONSUME, recvMsg->total_price, 0);
        player->SendTaskEvent(TASK_EVENT_DAILY_CONSUME, recvMsg->total_price, 0);
        player->SendTaskEvent(TASK_EVENT_TOTAL_CONSUMPTION, recvMsg->total_price, 0);
        if(recvMsg->ext != "")
        {
            player->DealTradeOrder(recvMsg->pay_reason, recvMsg->ext);
        }
        // 玩家数据回存
        player->AskAutoSave();
    }
}

bool TradeManager::GuildTrunTable(uint64 playerID, REWARD_TABLE rewardTable, std::string str)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return false;
    }

    // 判断如果是龙盘购买，需要解析龙盘参数给客户端
    if(rewardTable.reason == REWARD_REASON_GUILD_MEMBER_TURNTABLE)
    {
        uint32 id = 0;
        read_PERSONAL_TURNTABLE_ID(id, str);
        //rewardTable.reason_arg = (int32)id;
        GuildManager::Instance()->OnGuilddMemberTurntableAck(player, id);
        return true;
    }

    return false;
}
// ==================================================
// 接收交易成长点消息
// ==================================================
void TradeManager::RecvTradeOrderGrowthPoint(uint64 playerID, void* data)
{
    TRADE_ORDER_GROWTH_POINT* recvMsg = (TRADE_ORDER_GROWTH_POINT*)data;

    if(!recvMsg) return;

    // 处理交易的扩展信息
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

}

// ==================================================
// 发送交易状态数据
// ==================================================
void TradeManager::SendTradeOrderStateNtf(Player* player, uint64 tradeOrderID, int32 orderState)
{
    TRADE_ORDER_STATE_NTF sendMsg;

    if(!player)
        return;

    sendMsg.order_id.id_h = U64ID_HIGH(tradeOrderID);
    sendMsg.order_id.id_l = U64ID_LOW(tradeOrderID);
    sendMsg.order_state = orderState;

    CenterHandler::Instance()->SendByPlayer(player, MSG_TRADE_ORDER_STATE_NTF, &sendMsg);
}

// ==================================================
// 生成交易订单ID
// ==================================================
uint64 TradeManager::GenTradeUUID()
{
    //static uint16 serialNum = 0;
    //// 时间戳 32bit, 序列号16bit, 
    //uint32 high = (uint32)time(0);
    //uint32 low = (++serialNum << 16);
    //low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // 区ID

    //uint64 sn = high;
    //sn = sn << 32;
    //sn += low;

    //return sn;

    return 1111;
}
