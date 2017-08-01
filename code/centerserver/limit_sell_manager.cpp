#include "centerserver_pch.h"
#include "trade_job.h"
#include "player.h"
#include "mall_config_manager.h"
#include "limit_sell_manager.h"

LimitSellManager::LimitSellManager(Player* player) : 
m_player(player)
{
    Clear();
}

LimitSellManager::~LimitSellManager()
{

}

void LimitSellManager::Clear()
{
    m_limitSellList.clear();
}

bool LimitSellManager::CheckIfCanBuy( uint32 productID, uint32 productNum )
{
    const PRODUCT_INFO* productInfo = MallConfigManager::Instance()->GetDBInfoByProductID(productID);
    if (productInfo->sell_type != MALL_LIMIT_SELL_TYPE && productInfo->sell_type != VIP_LIMIT_SELL_TYPE)
        return true;

    return true;
}

void LimitSellManager::AddSellNum( uint32 productID, uint32 productNum )
{
    const PRODUCT_INFO* productInfo = MallConfigManager::Instance()->GetDBInfoByProductID(productID);
    if (productInfo->sell_type != MALL_LIMIT_SELL_TYPE && productInfo->sell_type != VIP_LIMIT_SELL_TYPE)
        return;

    // m_player非空,表明是属于玩家的对象,只处理个人限购
    // m_player为空,表明是属于服务器的对象,只处理全服限购
    if (m_player)
    {

    }
    else
    {

    }
}
