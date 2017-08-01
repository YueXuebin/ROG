#include "commonlib_pch.h"
#include "mall_config.h"
#include "mall_config_manager.h"

IMPLEMENT_SINGLETON(MallConfigManager)

MallConfigManager::MallConfigManager(void)
{
    MALL_ENTRY::CreateInstance();
    SERVICE_ENTRY::CreateInstance();
    m_version = 0;
    m_xmlMallMap.clear();
}

MallConfigManager::~MallConfigManager(void)
{
    MALL_ENTRY::DestroyInstance();
    SERVICE_ENTRY::DestroyInstance();
}

bool MallConfigManager::LoadConfig(const char* path)
{
    // 加载商城配置
    if (MALL_ENTRY::Instance()->LoadConfig(path))
    {
        const MALL_ROOT_STRUCT& RootStruct = MALL_ENTRY::Instance()->GetStruct();

        // 商城版本号
        m_version = RootStruct.version.number;
        PRODUCT_INFO product;

        for (std::vector<MALL_ROOT_LIST_PRODUCT_STRUCT>::const_iterator cIter = RootStruct.list.product_list.begin(); cIter != RootStruct.list.product_list.end(); ++cIter)
        {
            product.product_id = cIter->product_id;    // 商品ID
            product.product_num = cIter->product_num;  // 商品数量
            product.money_type = cIter->money_type;  // 商品数量
            product.sell_type = cIter->sell_type;     // 贩卖类型
            product.tag_type = cIter->tag_type;      // 标签页类型
            product.home_page = cIter->home_page;     // 是否显示在首页上
            product.discount = cIter->discount;      // 打折显示数

            product.show_flag = cIter->show_flag;    // 是否显示标记
            product.hot_flag = cIter->hot_flag;    // 热卖标记
            product.new_flag = cIter->new_flag;    // 新物品标记
            product.present_flag = cIter->present_flag;    // 赠送标记

            product.item_id = cIter->item_id;    // 购买的物品ID
            product.item_num = cIter->item_num;    // 购买的物品数量
            product.price = cIter->price;    // 普通购买价格
            product.show_price = cIter->show_price;    // 普通显示价格
            product.vip_price = cIter->vip_price;    // VIP购买价格
            product.show_vip_price = cIter->show_vip_price;    // VIP显示价格

            product.again_discount = cIter->again_discount;    // 折上折的显示数
            product.again_price = cIter->again_price;    // 折上折的购买价格
            product.show_again_price = cIter->show_again_price;    // 折上折的的显示价格
            //mallItem->QueryIntAttribute("again_start_time", &product.again_start_time);     //折上折的的开始时间
            //mallItem->QueryIntAttribute("again_end_time", &product.again_end_time);         // 折上折的的结束时间

            std::string discountime = "";// 折上折的时间
            discountime = cIter->again_start_time;
            if((discountime == "0000-00-00 00:00:00") || (discountime == "")|| (discountime == "0"))
            {
                product.again_start_time = 0;
            }
            else
            {
                product.again_start_time = Crown::TimeStamp(discountime);
            }


            product.global_limit_num = cIter->global_limit_num;       // 全服限购数量
            product.personal_limit_num = cIter->personal_limit_num;   // 个人限购数量

            std::string time = "";
            time = cIter->start_time;
            if((time == "0000-00-00 00:00:00") || (time == "") || (time == "0"))
            {
                product.start_time = 0;
            }
            else
            {
                product.start_time = Crown::TimeStamp(time);
            }

            time = cIter->end_time;
            if((time == "0000-00-00 00:00:00") || (time == "") || (time == "0"))
            {
                product.end_time = 0;
            }
            else
            {
                product.end_time = Crown::TimeStamp(time);
            }

            if(product.end_time < product.start_time)
            {
                CnInfo("product.end_time < product.start_time\n");
            }

            switch(product.sell_type)
            {
                case MALL_NORMAL_SELL_TYPE: // 1.售卖商品
                case MALL_NOT_SELL_TYPE:    // 2.非卖商品
                case MALL_LIMIT_SELL_TYPE:  // 3.限购商品
                case VIP_LIMIT_SELL_TYPE:   // 5.VIP限购商品
                    break;
                default:
                    // 非正常类型
                    CnAssert(false);
                    CnWarn("Unknow sell_type:%d by productID:%u\n", product.sell_type, product.product_id);
                    break;
            }

            if(m_xmlMallMap.find(product.product_id) != m_xmlMallMap.end())
            {
                CnWarn("mall config is exist! product id: %d\n", product.product_id);
                continue;
            }

            m_xmlMallMap[product.product_id] = product;
        }
    }
    else
    {
        return false;
    }
  
    // 记载服务配置
    if (SERVICE_ENTRY::Instance()->LoadConfig(path))
    {

    }
    else
    {
        return false;
    }

    return true;
}

void MallConfigManager::AddDBProductInfo(const PRODUCT_INFO& productInfo)
{
    // 检查是否已有此商品
    if(m_dbMallMap.find(productInfo.product_id) != m_dbMallMap.end())
    {
        CnError("Duplicate ProductID:%u\n", productInfo.product_id);
        return;
    }

    // 添加商品
    m_dbMallMap[productInfo.product_id] = productInfo;
}

void MallConfigManager::ClearDBProductInfo()
{
    m_dbMallMap.clear();
}

// ==================================================
// 根据商品ID查询商品
// ==================================================
const PRODUCT_INFO* MallConfigManager::GetDBInfoByProductID(uint32 productID)
{
    MallInfoMap::iterator productIt = m_dbMallMap.find(productID);
    if(productIt == m_dbMallMap.end())
    {
        return NULL;
    }

    return &productIt->second;
}


// ==================================================
// 根据道具ID查询商品
// ==================================================
const PRODUCT_INFO* MallConfigManager::GetDBInfoByItemID(uint32 itemID)
{

    MallInfoMap::iterator productIt = m_dbMallMap.begin();
    for(; productIt != m_dbMallMap.end(); productIt++)
    {
        if (productIt->second.item_id == itemID)
        {
            return &productIt->second;
        }
    }

    return NULL;
}

const SERVICE_ROOT_SERVICE_STRUCT* MallConfigManager::FindServiceTplById(uint32 serviceId)
{
    const SERVICE_ROOT_STRUCT& service = SERVICE_ENTRY::Instance()->GetStruct();

    std::vector<SERVICE_ROOT_SERVICE_STRUCT> service_list = service.service_list;

    for (std::vector<SERVICE_ROOT_SERVICE_STRUCT>::iterator cIter= service_list.begin(); cIter != service_list.end(); cIter++)
    {
        if(cIter->id == serviceId)
            return &(*cIter);
    }

    return NULL;
}
