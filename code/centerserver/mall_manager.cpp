#include "centerserver_pch.h"
#include "mall_manager.h"
#include "trade_job.h"
#include "game_handler.h"
#include "player_manager.h"
#include "player.h"
#include "mall_config.h"
#include "mall_config_manager.h"
#include "gate_handler.h"
#include "dbjob.h"
#include "player_manager.h"

IMPLEMENT_SINGLETON(MallManager)

MallManager::MallManager()
{

}

MallManager::~MallManager()
{

}

// ==================================================
// 接收信息
// ==================================================
void MallManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_RELOAD_MALL_INFO:
        RecvReloadMallInfo();
        break;
    case MSG_MALL_INFO_REQ:
        RecvMallInfoReq(playerID);
        break;
    case MSG_MALL_PRODUCT_REQ:
        RecvMallProductReq(playerID, data);
        break;
    case MSG_MALL_BUY_REQ:
        RecvMallBuyReq(playerID, data);
        break;
    default:
        break;
    }
}

// ==================================================
// 重新加载商城限购数据
// ==================================================
void MallManager::RecvReloadMallInfo()
{
    m_globalLimit.clear();
}

// ==================================================
// 重新加载商DB数据
// ==================================================
void MallManager::RecvReloadMallDBInfo()
{
    MallConfigManager::Instance()->ClearDBProductInfo();
    MallJob::Instance()->LoadDBMallInfo();

    //重新设置设置商城数据
    NONE recvMsg;
    GameHandler::Instance()->SendToAll(MSG_MALL_INFO_REQ, &recvMsg);
}
// ==================================================
// 请求商城数据
// ==================================================
void MallManager::RecvMallInfoReq(uint64 playerID)
{
    PRODUCT_INFO productInfo;

    MALL_LIST mallList;
    mallList.Clear();

    uint16 errcode = ERR_FAILED;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player == NULL)
    {
        return;
    }

    // 当前时间
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    const MallInfoMap& mallInfoMap = MallConfigManager::Instance()->GetDBMallInfoMap();

    for(MallInfoMap::const_iterator mallIt = mallInfoMap.begin(); mallIt != mallInfoMap.end(); ++mallIt)
    {
        productInfo.Clear();
        productInfo = mallIt->second;

        // 商品不显示
        /*if(!productInfo.show_flag)
        {
            continue;
        }*/

        // 计算商品信息
        errcode = CalProductInfo(&(mallIt->second), productInfo, nowTime);
        if(ERR_SUCCEED != errcode)
        {
            continue;
        }

        const PRODUCT_INFO* tmpl = (&(mallIt->second));
        if(tmpl) 
        {
            CalMallPrice(tmpl, productInfo,nowTime);
        }

        mallList.mall_list.push_back(productInfo);
    }

    player->SendToGate(MSG_MALL_INFO_ACK, &mallList);
}

// 商城限购时间段的值限购价钱
void MallManager::CalMallPrice(const PRODUCT_INFO* product,PRODUCT_INFO& productInfo,uint32 nowTime)
{
    if(!product) 
        return;

    // 在限购时间内才给值
    if(IsHaveDiscount(product, nowTime))
    {
        productInfo.again_price = product->again_price;
        productInfo.show_again_price =product->show_again_price;
        productInfo.again_discount = product->again_discount;
        productInfo.again_start_time = product->again_start_time;
        productInfo.again_end_time =  product->again_end_time;
    }
    else
    {
        productInfo.again_discount = 0;
        productInfo.show_again_price = 0;
        productInfo.again_price = 0;
        productInfo.again_start_time = 0;
        productInfo.again_end_time = 0;
    }
}

// ==================================================
// 单个商品信息请求
// ==================================================
void MallManager::RecvMallProductReq(uint64 playerID, void* data)
{
    MALL_PRODUCT_REQ* recvMsg = (MALL_PRODUCT_REQ*)data;
    MALL_PRODUCT_ACK sendMsg;
    std::map<uint32, int32>::iterator rushPurchaseIt;
    Player* player = NULL;
    const PRODUCT_INFO* product = 0;
    uint32 nowTime = 0;
    uint16 errCode = ERR_FAILED;

    if(!recvMsg) 
        return;

    sendMsg.Clear();

    // 查找角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player == NULL)
    {
        return;
    }

    // 获取当前时间
    nowTime = (uint32)Crown::SDNow().GetTimeValue();

    // 查找商城中的物品
    product = MallConfigManager::Instance()->GetDBInfoByProductID(recvMsg->product_id);
    if(!product)
    {
        errCode = ERR_MALL_PRODUCT_UNFOUND;
        goto _ERROR;
    }

    sendMsg.product_info = *product;

    // 计算商品信息
    errCode = CalProductInfo(product, sendMsg.product_info, nowTime);

    // 客户端可能会用这条消息查询当前商品的剩余个数、超时等信息
    if(ERR_SUCCEED != errCode)
    {
        goto _ERROR;
    }

    CalMallPrice(product, sendMsg.product_info, nowTime);

    player->SendToGate(MSG_MALL_PRODUCT_ACK, &sendMsg);
    return;

_ERROR:
    player->NotifyErrorCode(errCode);
}

void MallManager::RecvMallBuyReq(uint64 playerID, void* data)
{
    MALL_BUY_REQ* recvMsg = (MALL_BUY_REQ*)data;
    MALL_BUY_ACK sendMsg;
    std::map<uint32, int32>::iterator rushPurchaseIt;
    Player* player = NULL;
    const PRODUCT_INFO* product = NULL;
    int32   price   = 0;
    int64   cost    = 0;
    int32   itemNum = 0;
    uint32  nowTime = 0;
    uint16  errCode = ERR_FAILED;

    if(!recvMsg) 
        return;

    if(recvMsg->num < 1)
        return;

    sendMsg.Clear();

    // 查找角色
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(player == NULL)
    {
        return;
    }

    // 如果接收者的名字为空，这说明这个玩家想购买东西给自己
    if(recvMsg->receiver_name == "")
    {
        recvMsg->receiver_name = player->GetName();
    }

    // 获取当前时间
    nowTime = (uint32)Crown::SDNow().GetTimeValue();

    // 查找商城中的物品
    product = MallConfigManager::Instance()->GetDBInfoByProductID(recvMsg->product_id);
    if(!product)
    {
        errCode = ERR_MALL_PRODUCT_UNFOUND;
        goto _ERROR;
    }

    // 如果是赠送他人，检查是否可赠送
    if((player->GetName() != recvMsg->receiver_name) && !product->present_flag)
    {
        errCode = ERR_MALL_CANT_PRESENT;
        goto _ERROR;
    }

    sendMsg.product_info = *product;

    // 计算商品信息
    errCode = CalProductInfo(product, sendMsg.product_info, nowTime);
    if(ERR_SUCCEED != errCode)
    {
        goto _ERROR;
    }

    // 检查商品的购买数量是否超过剩余数量
    //if(recvMsg->num > sendMsg.product_info.product_num)
    //{
    //    errCode = ERR_MALL_PRODUCT_NOENOUGH;
    //    goto _ERROR;
    //}

    // 赋值购买数量
    sendMsg.num = recvMsg->num;

    // 物品数量
    itemNum = recvMsg->num * sendMsg.product_info.item_num;

    // 检查参数有效性
    if((recvMsg->num < 1) || (sendMsg.product_info.item_num < 1) || (itemNum < 1))
    {
        errCode = ERR_FAILED;
        goto _ERROR;
    }

    if (VIP_LIMIT_SELL_TYPE == sendMsg.product_info.sell_type)
    {
        price = sendMsg.product_info.vip_price;
    }
    else
    {
        if(IsHaveDiscount(product, nowTime))
            price = sendMsg.product_info.again_price;
        else 
            price = sendMsg.product_info.price;

        sendMsg.product_info.price = price; // 如果在限购时间内，则按限购的钱算
    }

    // 计算总价
    cost = price * recvMsg->num;

    // 判断商城道具价格
    if((price < 0) || (cost < 0))
    {
        goto _ERROR;
    }

    if(player->GetMoneyPoint() < cost)
    {
        errCode = ERR_NO_ENOUGH_MONEYPOINT;
        goto _ERROR;
    }

    sendMsg.receiver_name   = recvMsg->receiver_name;       // 赋值接收者名称
    sendMsg.buy_reason      = recvMsg->buy_reason;          // 购买原因

    player->SendToGame(MSG_MALL_BUY_ACK, &sendMsg);
    return;

_ERROR:
    player->SendMallBuyNtf(errCode);
}

// 商城是否有折扣
bool MallManager::IsHaveDiscount(const PRODUCT_INFO* product, uint32  nowTime)
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
// ==================================================
// 获得产品信息
// ==================================================
uint16 MallManager::CalProductInfo(const PRODUCT_INFO* tmpl, PRODUCT_INFO& productInfo, uint32 nowTime)
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
        // 商品被抢信息
        limitIt = m_globalLimit.find(tmpl->product_id);
        if(limitIt != m_globalLimit.end())
        {
            // 抢购列表中有信息
            if(productInfo.global_limit_num <= limitIt->second)
            {
                errcode = ERR_MALL_PRODUCT_SELLOUT;
                goto _END;
            }
            else
            {
                productInfo.product_num = productInfo.global_limit_num - limitIt->second;
            }
        }
        else
        {
            productInfo.product_num = tmpl->product_num;
        }
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

// ==================================================
// 从数据库中加载商城数据
// ==================================================
bool MallManager::UpdateDBVersion()
{
    uint32 mall_version = 0;

    MallJob::Instance()->LoadSystemVersion(SYSTEM_VERSION_ID_MALL, mall_version);

    // 如果当前配置表中的版本本大于数据库中的版本，则更新数据库中的数据
    if(MallConfigManager::Instance()->GetVersion() <= mall_version)
    {
        return true;
    }

    // 删除DB中旧的商城数据
    MallJob::Instance()->DelModifyMallInfo();

    // 向DB填充新的商城数据
    if(!MallJob::Instance()->InsertNewMallInfo())
    {
        return false;
    }

    if(!MallJob::Instance()->UpdateSystemVersion(SYSTEM_VERSION_ID_MALL, MallConfigManager::Instance()->GetVersion()))
    {
        CnError("Update systemID:%u version:%u failed\n", SYSTEM_VERSION_ID_MALL, MallConfigManager::Instance()->GetVersion());
        return false;
    }

    return true;
}

// ==================================================
// 从数据库中加载商品信息
// ==================================================
void MallManager::LoadDBMallInfo()
{
    MallJob::Instance()->LoadDBMallInfo();
}

// ==================================================
// 通知GS商城的商品信息
// ==================================================
void MallManager::SendMallInfoToGameSvr(uint32 sessionID)
{
    MALL_LIST sendMsg;
    MallInfoMap::const_iterator mallInfoIt;

    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
    if(!regionID)
    {
        return;
    }

    sendMsg.opt_flag = OPT_INIT_FLAG;

    const MallInfoMap& mallInfoMap = MallConfigManager::Instance()->GetDBMallInfoMap();

    for(mallInfoIt = mallInfoMap.begin(); mallInfoIt != mallInfoMap.end(); ++mallInfoIt)
    {
        sendMsg.mall_list.push_back(mallInfoIt->second);
    }

    GameHandler::Instance()->SendByID(regionID, MSG_MALL_INFO_NOTIFY, &sendMsg);
}

void MallManager::PlayerBuyProductSuccess(uint64 playerID, uint32 productID, uint32 productNum)
{
    const PRODUCT_INFO* productInfo = MallConfigManager::Instance()->GetDBInfoByProductID(productID);
    if (productInfo->sell_type != MALL_LIMIT_SELL_TYPE && productInfo->sell_type != VIP_LIMIT_SELL_TYPE)
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    // 商品属于全部抢购
    if(productInfo->global_limit_num >= 0)
    {
        //// 商品被抢信息
        //std::map<uint32, int32>::iterator RushPurchaseIt = m_rushPurchase.find(productID);
        //// 抢购列表中有信息
        //if(RushPurchaseIt != m_rushPurchase.end())
        //{
        //    RushPurchaseIt->second += productNum;
        //}
        //else
        //{
        //    m_rushPurchase[productID] = productNum;
        //}
        ////保存数据
        //TradeJob::Instance()->SaveGlobalLimitSellDate();
    }

    // 商品属于个人限购
    if(productInfo->personal_limit_num >= 0)
    {
        //// 商品被抢信息
        //std::map<uint32, int32>::iterator RushPurchaseIt = m_rushPurchase.find(productID);
        //// 抢购列表中有信息
        //if(RushPurchaseIt != m_rushPurchase.end())
        //{
        //    RushPurchaseIt->second += productNum;
        //}
        //else
        //{
        //    m_rushPurchase[productID] = productNum;
        //}
        ////保存数据
        //TradeJob::Instance()->SaveGlobalLimitSellDate();
    }
}