#include "gameholder_pch.h"
#include "shop_manager.h"
#include "player.h"
#include "player_manager.h"
#include "center_handler.h"
#include "item_base.h"
#include "item_config_manager.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "event_define.h"
#include "mall_config.h"
#include "limitbuy_manager.h"
#include "game_define.h"
#include "task_festive_config.h"
#include "game_util.h"
#include "mall_config_manager.h"


ShopManager::ShopManager(Player* player) :
m_player(player)
{
    Clear();
}

ShopManager::~ShopManager()
{

}

void ShopManager::Clear()
{

}

void ShopManager::LoadMallInfo(const RESTRICTION_LIST& restrictionList)
{
    m_RestrictionList = restrictionList;
}

void ShopManager::FillMallInfo(RESTRICTION_LIST& restrictionList)
{
    restrictionList = m_RestrictionList;
}

void ShopManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MALL_INFO_REQ:     // 商城信息
        {
            OnMallInfoReq(data);
        }
        break;
    case MSG_MALL_PRODUCT_REQ:  // 询价
        {
            OnMallProductReq(data);
        }
        break;
    case MSG_MALL_BUY_REQ:      // 购买
        {
            OnMallBuyReq(data);
        }
        break;
    default:
        break;
    }
}

void ShopManager::GMReloadMallInfo()
{
    NONE sendMsg;
    m_player->SendToCenter(MSG_RELOAD_MALL_INFO, &sendMsg);
}

void ShopManager::GMBuyReq(uint32 msgID, void* data)
{
    OnMallBuyReq(data);
}

void ShopManager::GMMallInfoReq()
{
    NONE sendMsg;
    sendMsg.Clear();
    OnMallInfoReq(&sendMsg);
}

// ==================================================
// 接收商品通知信息
// ==================================================
void ShopManager::OnMallInfoNotify(void* data)
{
    MALL_LIST* recvMsg = (MALL_LIST*)data;

    if(!recvMsg) return;

    if(recvMsg->opt_flag == OPT_INIT_FLAG)
    {
        CnDbg("Sync CenterServer MallList Size:%u\n", recvMsg->mall_list.size());

        MallConfigManager::Instance()->ClearDBProductInfo();
        //MallConfig::Instance()->ClearDBProductInfo();
        for(std::vector<PRODUCT_INFO>::iterator productIt = recvMsg->mall_list.begin();
            productIt != recvMsg->mall_list.end(); ++productIt)
        {
            MallConfigManager::Instance()->AddDBProductInfo(*productIt);
        }
    }
    else if(recvMsg->opt_flag == OPT_ADD_FLAG)
    {
        
    }
    else if(recvMsg->opt_flag == OPT_DEL_FLAG)
    {
        
    }
    else if(recvMsg->opt_flag == OPT_UPDATE_FLAG)
    {
        
    }
}

// ==================================================
// 商城信息请求
// ==================================================
void ShopManager::OnMallInfoReq(void* data)
{
    NONE* recvMsg = (NONE*)data;
    if(!recvMsg) return;

    m_player->SendToCenter(MSG_MALL_INFO_REQ, recvMsg);

    // 发送个人限购列表
    SendRestrictionList();
}

// ==================================================
// 请求单个商品信息
// ==================================================
void ShopManager::OnMallProductReq(void* data)
{
    MALL_PRODUCT_REQ* recvMsg = (MALL_PRODUCT_REQ*)data;
    if(!recvMsg) return;

    m_player->SendToCenter(MSG_MALL_PRODUCT_REQ, recvMsg);
}

// ==================================================
// 商城购买请求
// ==================================================
void ShopManager::OnMallBuyReq(void* data)
{
    // 测试代码
   /* uint32 cost2 = GetCostMoney(14118,3);
    uint32 cost3 = GetCostMoney(14001,3);
    bool IS = IsEnoughMoney(14118,3);
    bool succ = DoMallBuy(14118,3,1);

    BuyMap itemMap;

    itemMap[14118] = 3;
    itemMap[14001] = 3;

    ListIsEnoughMoney(&itemMap);
    GetListCostMoney(&itemMap); 
    DoListMallBuy(&itemMap , 11);*/

    uint16        errCode = ERR_FAILED;
    MALL_BUY_REQ* recvMsg = (MALL_BUY_REQ*)data;
    PRODUCT_INFO  product;
    uint32        nowTime = 0;
    uint32        moneyType = 0;
    MALL_BUY_ACK  sendMsg;

    static ItemConfigManager* pItemManager = ItemConfigManager::Instance();
    uint32 itemId = 0;
    uint32 itemType = 0;
    const void* pItem = NULL;
    uint32 itemNum = 0;
    int32 cost = 0;
    std::map<uint32, uint32> targetItems;

    if(!recvMsg)
        return;

    if(recvMsg->num < 1)
    {
        CnDbg("player %s buy 0 num in mall\n", m_player->GetName().c_str());
        return;
    }

    // 验证用户相关
    if(!m_player || (m_player->GetNetState() < Player::kStateInGame))
    {
        return;
    }

    // 购买的商品和物品是否存在
    const PRODUCT_INFO* pProductTpl = MallConfigManager::Instance()->GetDBInfoByProductID(recvMsg->product_id);
    if(!pProductTpl)
    {
        errCode = ERR_MALL_PRODUCT_UNFOUND;
        goto _ERROR;
    }

    // 检查生成的物品需要的背包空间
    targetItems[pProductTpl->item_id] = pProductTpl->item_num;
    if (false == m_player->GetItemManager()->CanAddToBag(&targetItems, NULL))
    {
        errCode = ERR_NO_ENOUGH_BAGPOS;
        goto _ERROR;
    }

    itemId = pProductTpl->item_id;
    itemType = pItemManager->GetItemTypeById(itemId);

    switch (itemType)
    {
        case ITEM_TYPE_EQUIP:
            pItem = pItemManager->FindEquipTpltById(itemId);
            break;

        case ITEM_TYPE_CONSUME:
            pItem = pItemManager->FindConsumeTpltById(itemId);
            break;

        case ITEM_TYPE_MATERIAL:
            pItem = pItemManager->FindMaterialTpltById(itemId);
            break;

        case ITEM_TYPE_STONE:
            pItem = pItemManager->FindStoneTpltById(itemId);
            break;

        default:
            break;
    }

    if(!pItem)
    {
        CnWarn("item not found by id: %d", itemId);
        errCode = ERR_UNFOUND_ITEM;
        goto _ERROR;
    }

    // 如果接收者的名字为空，这说明这个玩家想购买东西给自己
    if(recvMsg->receiver_name == "")
    {
        recvMsg->receiver_name = m_player->GetName();
    }

    // 获取当前时间
    nowTime = (uint32)Crown::SDNow().GetTimeValue();

    // 验证商品数量、价格等
    // 商品数量不足
    if((recvMsg->num < 1) || (pProductTpl->item_num < 1))
    {
        errCode = ERR_FAILED;
        goto _ERROR;
    }
    if(recvMsg->num > pProductTpl->product_num)
    {
        errCode = ERR_MALL_PRODUCT_NOENOUGH;
        goto _ERROR;
    }
    // 计算花费总价
    cost = pProductTpl->price * recvMsg->num;
    // 判断商城道具价格
    if((pProductTpl->price < 0) || (cost < 0))
    {
        goto _ERROR;
    }

    // 判断用户身上的货币够不够
    moneyType = GetMoneyType(pProductTpl);
    switch(moneyType)
    {
    case MONEY_TYPE_MONEY_POINT:
        if(m_player->GetMoneyPoint() < cost)
        {
            errCode = ERR_NO_ENOUGH_MONEYPOINT;
            goto _ERROR;
        }
        break;
    case MONEY_TYPE_GOLD:
        if(m_player->GetGamePoint() < cost)
        {
            errCode = ERR_NO_ENOUGH_GAMEPOINT;
            goto _ERROR;
        }
        break;
    default:
        CnError("unkown money type :%d", moneyType);
        goto _ERROR;
        break;
    }

    itemNum = recvMsg->num * pProductTpl->item_num;
    TradeManager::Instance()->TradeOrder(m_player->GetPlayerID(),
        m_player->GetAccount(), m_player->GetName(), 
        recvMsg->receiver_name, REWARD_REASON_TRADE_ORDER, 
        PROP_ITEM, itemId, (int32)itemNum, pProductTpl->price, (int32)cost,
        pProductTpl->product_id, pProductTpl->product_num, recvMsg->buy_reason);

    // 活跃度
    //m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_BUY_FROM_MALL, (int32)itemNum, 0, 0);

    // NOTICE
   /* if (VIP_LIMIT_SELL_TYPE == recvMsg->product_info.sell_type)
    {
        marquee.id = MARQUEE_VIP_LIMITED_PURCHASING;
        marquee.parame_list.push_back(MarQueePlayerName(player->GetVipLevel(), player->GetName()));
        marquee.parame_list.push_back(MarQueeLoginReceive(marquee.id, itemID));
        player->OnMarQuee(marquee);
    }*/

    errCode = ERR_SUCCEED;

    SendMallBuyNtf(errCode);
    return;

    //MALL_BUY_ACK sendMsg;
    //std::map<uint32, int32>::iterator rushPurchaseIt;
    //const PRODUCT_INFO* product = NULL;
    //int32   price   = 0;
    //int64   cost    = 0;
    //int32   itemNum = 0;
    //uint32  nowTime = 0;
    //uint16  errCode = ERR_FAILED;

    ////sendMsg.Clear();

    //// 如果接收者的名字为空，这说明这个玩家想购买东西给自己
    //if(recvMsg->receiver_name == "")
    //{
    //    recvMsg->receiver_name = m_player->GetName();
    //}

    //// 获取当前时间
    //nowTime = (uint32)Crown::SDNow().GetTimeValue();


    //// 如果是赠送他人，检查是否可赠送
    ///*if((player->GetName() != recvMsg->receiver_name) && !product->present_flag)
    //{
    //    errCode = ERR_MALL_CANT_PRESENT;
    //    goto _ERROR;
    //}*/

    //sendMsg.product_info = *product;

    //// 计算商品信息
    //errCode = CalProductInfo(product, sendMsg.product_info, nowTime);
    //if(ERR_SUCCEED != errCode)
    //{
    //    goto _ERROR;
    //}

    //// 检查商品的购买数量是否超过剩余数量
    ////if(recvMsg->num > sendMsg.product_info.product_num)
    ////{
    ////    errCode = ERR_MALL_PRODUCT_NOENOUGH;
    ////    goto _ERROR;
    ////}

    //// 赋值购买数量
    //sendMsg.num = recvMsg->num;

    //// 物品数量
    //// itemNum = recvMsg->num * sendMsg.product_info.item_num;
    //// 检查参数有效性
    ////if((recvMsg->num < 1) || (sendMsg.product_info.item_num < 1) || (itemNum < 1))

    //if((recvMsg->num < 1) || (sendMsg.product_info.item_num < 1))
    //{
    //    errCode = ERR_FAILED;
    //    goto _ERROR;
    //}

    //if (VIP_LIMIT_SELL_TYPE == sendMsg.product_info.sell_type)
    //{
    //    price = sendMsg.product_info.vip_price;
    //}
    //else
    //{
    //    if(IsHaveDiscount(product, nowTime))
    //        price = sendMsg.product_info.again_price;
    //    else 
    //        price = sendMsg.product_info.price;

    //    sendMsg.product_info.price = price; // 如果在限购时间内，则按限购的钱算
    //}

    //// 计算总价
    //cost = price * recvMsg->num;

    //// 判断商城道具价格
    //if((price < 0) || (cost < 0))
    //{
    //    goto _ERROR;
    //}

    //if(m_player->GetMoneyPoint() < cost)
    //{
    //    errCode = ERR_NO_ENOUGH_MONEYPOINT;
    //    goto _ERROR;
    //}

    //sendMsg.receiver_name   = recvMsg->receiver_name;       // 赋值接收者名称
    //sendMsg.buy_reason      = recvMsg->buy_reason;          // 购买原因

    //player->SendToGame(MSG_MALL_BUY_ACK, &sendMsg);
    //return;

_ERROR:
    //m_player->NotifyErrorCode(errCode);
    SendMallBuyNtf(errCode);

    // 检查VIP限购的时间
    //if (recvMsg->sell_type == VIP_LIMIT_SELL_TYPE)
    //{
    //    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_VIP_LIMITED_PURCHASING_ID);
    //    if (festivalTmpl)
    //    {
    //        if(festivalTmpl->begin_time && festivalTmpl->end_time) // 非零，已经GMT配置了时间
    //        {
    //            uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    //            if((festivalTmpl->begin_time <= now) && (now < festivalTmpl->end_time)) // 在有效时间内
    //            {
    //                // dummy
    //            }
    //            else    // 时间失效
    //            {
    //                return;
    //            }
    //        }
    //        else // 未配置
    //        {
    //            return;
    //        }
    //    }
    //    else // 无该活动
    //    {
    //        return;
    //    }
    //}

    // 超过购买限制的直接设置为购买最大值
    /*recvMsg->num = (recvMsg->num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->num);
    if( recvMsg->num > g_MaxBuyNum)
        return;
    */
    //m_player->SendToCenter(MSG_MALL_BUY_REQ, recvMsg);
}

// ==================================================
// 商城购买应答
// ==================================================
void ShopManager::OnMallBuyAck(uint64 playerID, void* data)
{
    MALL_BUY_ACK*   recvMsg = (MALL_BUY_ACK*)data;
    REWARD_TABLE    rewardTable;
    REWARD_ITEM     rewardItem;
    Player*         player  = 0;
    int32           unit_price = 0;
    int64           total_price = 0;
    uint32          itemID  = 0;
    int64           itemNum = 0;
    uint64          product_id = 0;
    uint16          errCode = ERR_FAILED;
    SERVER_MARQUEE  marquee;

    if(!recvMsg) 
        return;

    // 查找角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || (player->GetNetState() < Player::kStateInGame))
    {
        goto _ERR;
    }

    // 检查物品是否存在, !!! 做好日志
    itemID = recvMsg->product_info.item_id;
    if (itemID > 100000)
        itemID /= 1000;
    //if(!ItemConfig::Instance()->GetItemTmpl(itemID))
    //{
    //    errCode = ERR_UNFOUND_ITEM;
    //    goto _ERR;
    //}

    // 物品数量
    itemNum = recvMsg->num * recvMsg->product_info.item_num;

    // 检查参数有效性
    if((recvMsg->num < 1) || (recvMsg->product_info.item_num < 1) || (itemNum < 1))
    {
        goto _ERR;
    }

    // 根据策划需求,都用原价 [3/1/2014 ZhangHM]
    // 根据角色 VIP 等级获取购买价格
    //if(player->GetVipLevel() > 0)
    //{
    //    unit_price = recvMsg->product_info.vip_price;
    //}

    int32 curType;
    if (VIP_LIMIT_SELL_TYPE == recvMsg->product_info.sell_type)
    {
        unit_price = recvMsg->product_info.vip_price;
        curType = VIP_SELL_TYPE;
    }
    else
    {
        unit_price = recvMsg->product_info.price;
        curType = MALL_SELL_TYPE;
    }

    // 计算总价
    total_price = unit_price * recvMsg->num;

    // 判断商城道具价格
    if((unit_price < 0) || (recvMsg->num < 1) || (total_price < 0))
    {
        goto _ERR;
    }

    if(player->GetMoneyPoint() < total_price)
    {
        errCode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERR;
    }

    
    if (VIP_LIMIT_SELL_TYPE == recvMsg->product_info.sell_type)
    {
        if(player->GetLimitBuyManager()->JudgeLimitNumToNum(recvMsg->product_info.product_id, recvMsg->num, curType, recvMsg->product_info.personal_limit_num))
        {
            errCode = ERR_LIMT_BUY_NUM;
            goto _ERR;
        }
    }
    else if(player->GetLimitBuyManager()->JudgeLimitNum(recvMsg->product_info.product_id, recvMsg->num, curType))
    {
        // 限购最大购买次数
        errCode = ERR_LIMT_BUY_NUM;
        goto _ERR;
    }

    TradeManager::Instance()->SendTradeOrderToReq(player->GetPlayerID(), player->GetAccount(), player->GetName(), 
        recvMsg->receiver_name, REWARD_REASON_TRADE_ORDER, 
        PROP_ITEM, itemID, (int32)itemNum, unit_price, (int32)total_price,
        recvMsg->product_info.product_id, recvMsg->product_info.product_num, recvMsg->buy_reason);

    // 活跃度
    player->SendMyselfEvent(LIVENESS_EVENT_TARGET_BUY_FROM_MALL, (int32)itemNum, 0, 0);

    // NOTICE
    if (VIP_LIMIT_SELL_TYPE == recvMsg->product_info.sell_type)
    {
        marquee.id = MARQUEE_VIP_LIMITED_PURCHASING;
        marquee.parame_list.push_back(MarQueePlayerName(player->GetVipLevel(), player->GetName()));
        marquee.parame_list.push_back(MarQueeLoginReceive(marquee.id, itemID));
        player->OnMarQuee(marquee);
    }

    return;

_ERR:
    player->NotifyErrorCode(errCode);    
}

// ==================================================
// 商城购买通知
// ==================================================
void ShopManager::OnMallBuyNtf(uint64 playerID, void* data)
{
    ERROR_CODE* recvMsg = (ERROR_CODE*)data;
    Player* player = 0;

    if(!recvMsg)
        return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player)
    {
        player->SetBuying(false);
    }

    //SendMallBuyNtf(playerID, recvMsg->errcode);
}

// ==================================================
// 购买服务
// ==================================================
bool ShopManager::DoMallBuy(uint32 id, uint32 buyNum,uint32 reason)
{
   const PRODUCT_INFO* pProduct = GetProduct(id);
   if (!pProduct)
   {
       CnError("product not found by id :%d", id);
       return false;
   }

   // 验证用户相关
   if(!m_player || (m_player->GetNetState() < Player::kStateInGame))
   {
       return false;
   }

   // 验证用户的钱是否够
   if( !IsEnoughMoney(id,buyNum) )
   {
       return false;
   }
   int32 cost = buyNum * pProduct->price;
   uint32 itemNum = buyNum * pProduct->item_num;

   bool isTradeSuccess = TradeManager::Instance()->TradeOrderService(m_player->GetPlayerID(),
        m_player->GetAccount(), m_player->GetName(), 
        m_player->GetName(), REWARD_REASON_TRADE_ORDER, 
        PROP_ITEM, id, (int32)itemNum, pProduct->price, (int32)cost,
        pProduct->product_id, pProduct->product_num, reason);

   if (!isTradeSuccess)
   {
       return false;
   }

    return true;
}

bool ShopManager::DoListMallBuy(const BuyMap* buyItems, uint32 reason)
{
    // 验证用户相关
    if(!m_player || (m_player->GetNetState() < Player::kStateInGame))
    {
        return false;
    }

    if (buyItems->size() <= 0)
    {
        CnError("buyItems size is :%d",buyItems->size());
        return false;
    }

    // 验证用户的钱是否够
    if( !ListIsEnoughMoney(buyItems) )
    {
        return false;
    }

    bool isTradeSuccess = TradeManager::Instance()->TradeListOrder(m_player->GetPlayerID(), buyItems, reason);

    if (!isTradeSuccess)
    {
        return false;
    }

    return true;

}

// ==================================================
// 计算总共花费(可能是金币、也可能是钻石)
// ==================================================
int32 ShopManager::GetCostMoney(uint32 id, uint32 buyNum)
{
    const PRODUCT_INFO* pProduct = GetProduct(id);
    if (!pProduct)
    {
        CnError("product not found by id :%d", id);
        return -1;
    }

    int32 totalMoney = pProduct->price * buyNum;

    return totalMoney;
}

CostMap ShopManager::GetListCostMoney(const BuyMap* buyItems)
{
    CostMap cost;

    if (buyItems->size() <= 0)
    {
        CnError("buyItems size is :%d",buyItems->size());
        cost[MONEY_TYPE_GOLD]        = -1;
        cost[MONEY_TYPE_MONEY_POINT] = -1;
        return cost;
    }

    uint32 totalMoneyPoint = 0;
    uint32 totatGamePoint = 0;
    for (BuyMap::const_iterator cIter = buyItems->begin(); cIter != buyItems->end(); cIter++)
    {
        // 验证商品是否都ok
        const PRODUCT_INFO* pProduct = GetProduct(cIter->first);
        if (!pProduct)
        {
            CnError("product not found by id :%d", cIter->first);
            // TODO 
            cost[MONEY_TYPE_GOLD]        = -2;
            cost[MONEY_TYPE_MONEY_POINT] = -2;

            return cost;
        }

        uint32 moneyType = GetMoneyType(pProduct);
        switch(moneyType)
        {
        case MONEY_TYPE_MONEY_POINT:
            totalMoneyPoint += GetCostMoney(cIter->first,cIter->second);
            break;
        case MONEY_TYPE_GOLD:
            totatGamePoint += GetCostMoney(cIter->first,cIter->second);
            break;
        default:
            CnError("unkown money type :%d", moneyType);
            break;
        }
    }
    cost[MONEY_TYPE_GOLD]        = totatGamePoint;
    cost[MONEY_TYPE_MONEY_POINT] = totalMoneyPoint;

    return cost;
}

// ==================================================
// 是否有足够的钱来购买服务
// ==================================================
bool ShopManager::IsEnoughMoney(uint32 id, uint32 buyNum)
{
    const PRODUCT_INFO* pProduct = GetProduct(id);
    if (!pProduct)
    {
        CnError("product not found by id :%d", id);
        return false;
    }

    int32 totalMoney = GetCostMoney(id, buyNum);

    if(totalMoney < 0){
        return false;
    }
    uint32 moneyType = GetMoneyType(pProduct);

    switch(moneyType)
    {
    case MONEY_TYPE_MONEY_POINT:
        if (m_player->GetMoneyPoint() > totalMoney)
            return true;
        break;
    case MONEY_TYPE_GOLD:
        if (m_player->GetGamePoint() > totalMoney)
            return true;
        break;
    default:
        CnError("unkown money type :%d", moneyType);
        break;
    }

    return false;
}

bool ShopManager::ListIsEnoughMoney(const BuyMap* buyItems)
{
    if (buyItems->size() <= 0)
    {
        CnError("buyItems size is :%d",buyItems->size());
        return false;
    }

    CostMap cost = GetListCostMoney(buyItems);

    int32 costMoneyPoint = cost[MONEY_TYPE_MONEY_POINT];
    int32 costGamePoint = cost[MONEY_TYPE_GOLD];
    if(costMoneyPoint < 0 || costGamePoint < 0){
        return false;
    }

    if (m_player->GetMoneyPoint() < costMoneyPoint || m_player->GetGamePoint() < costMoneyPoint)
    {
        return false;
    }

    return true;
}

// ==================================================
// 商城购买错误通知
// ==================================================
void ShopManager::SendMallBuyNtf(uint16 errCode)
{
    ERROR_CODE sendMsg;
    if(m_player && (m_player->GetNetState() >= Player::kStateInGame))
    {
        sendMsg.errcode = errCode;
        m_player->SendToGate(MSG_MALL_BUY_NTF, &sendMsg);
    }
}

void ShopManager::SendRestrictionList()
{
    // 发送限购已经购买商品列表
    RESTRICTION_LIST sendMsg = m_RestrictionList;
    m_player->SendToGate(MSG_RESTRICTION_LIST_ACK, &sendMsg);
}

uint32 ShopManager::GetMoneyType(const PRODUCT_INFO* pProduct)
{
    uint32 type = (uint32)pProduct->money_type;
   
    return type;
}

const PRODUCT_INFO* ShopManager::GetProduct(uint32 id/*serviceId/itemId*/)
{
    // 验证itemID或者serviceId是否配置
    const void* pItem = NULL;
    if(id >= MALL_SERVICE_ID_MIN_VALUE && id<= MALL_SERVICE_ID_MAX_VALUE)
    {
        pItem = MallConfigManager::Instance()->FindServiceTplById(id);
    }else{

        static ItemConfigManager* pItemManager = ItemConfigManager::Instance();
        uint32 itemType = pItemManager->GetItemTypeById(id);

        switch (itemType)
        {
        case ITEM_TYPE_EQUIP:
            pItem = pItemManager->FindEquipTpltById(id);
            break;

        case ITEM_TYPE_CONSUME:
            pItem = pItemManager->FindConsumeTpltById(id);
            break;

        case ITEM_TYPE_MATERIAL:
            pItem = pItemManager->FindMaterialTpltById(id);
            break;

        case ITEM_TYPE_STONE:
            pItem = pItemManager->FindStoneTpltById(id);
            break;

        default:
            break;
        }
    }

    if (!pItem)
    {
        CnError("service or item not found by ID :%d", id);
        return NULL;
    }

    const PRODUCT_INFO* pProduct = MallConfigManager::Instance()->GetDBInfoByItemID(id);
    if (!pProduct)
    {
        CnError("product not found by id :%d", id);
        return NULL;
    }

    return pProduct;
}

// ==================================================
// 获得产品信息
// ==================================================
uint16 ShopManager::CalProductInfo(const PRODUCT_INFO* tmpl, PRODUCT_INFO& productInfo, uint32 nowTime)
{
    std::map<uint32, int32>::iterator limitIt;
    uint16 errcode = ERR_SUCCEED;

    if(!tmpl)
    {
        errcode = ERR_MALL_PRODUCT_UNFOUND;
        goto _END;
    }

    // 售卖商品
    if(MALL_NORMAL_SELL_TYPE == tmpl->sell_type)
    {
        // 正常通过，不做任何处理
        productInfo.product_num = tmpl->product_num;
    }
    // 非卖商品
    else if(MALL_NOT_SELL_TYPE == productInfo.sell_type)
    {
        errcode = ERR_MALL_PRODUCT_NOTSELL;
        goto _END;
    }
    // 限购商品
    else if(MALL_LIMIT_SELL_TYPE == productInfo.sell_type || VIP_LIMIT_SELL_TYPE == productInfo.sell_type)
    {
        //// 商品被抢信息
        //limitIt = m_globalLimit.find(tmpl->product_id);
        //if(limitIt != m_globalLimit.end())
        //{
        //    // 抢购列表中有信息
        //    if(productInfo.global_limit_num <= limitIt->second)
        //    {
        //        errcode = ERR_MALL_PRODUCT_SELLOUT;
        //        goto _END;
        //    }
        //    else
        //    {
        //        productInfo.product_num = productInfo.global_limit_num - limitIt->second;
        //    }
        //}
        //else
        //{
        //    productInfo.product_num = tmpl->product_num;
        //}
    }
    // VIP限购商品
    //    else if(VIP_LIMIT_SELL_TYPE == productInfo.sell_type)
    //    {
    //        // TODO:
    //    }
    // 非正常类型
    else
    {
        CnAssert(false);
        errcode = ERR_MALL_PRODUCT_UNFOUND;
        goto _END;
    }

    // 检查是否为限时物品
    if((productInfo.start_time != 0) && (productInfo.end_time != 0))
    {
        // 商品不在时间内不显示
        if((nowTime < productInfo.start_time) || (nowTime > productInfo.end_time))
        {
            errcode = ERR_MALL_PRODUCT_TIMEOUT;
            goto _END;
        }

        // 把时间转换为倒计时
        productInfo.end_time -= nowTime;
    }

_END:
    return errcode;
}

// 商城是否有折扣
bool ShopManager::IsHaveDiscount(const PRODUCT_INFO* product, uint32  nowTime)
{
    if(!product)
    {
        CnDbg("product is null");
        return 1;
    }

    if(product->again_price > 0)    // 说明有限时打折的话就限时打折的价格计算，如果没有显示打折，那么显示正常限时打折
    {
        if((nowTime >= product->again_start_time) && (nowTime <= product->again_end_time))
            return true;
    }


    return false;
}