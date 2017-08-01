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
    // �����̳�����
    if (MALL_ENTRY::Instance()->LoadConfig(path))
    {
        const MALL_ROOT_STRUCT& RootStruct = MALL_ENTRY::Instance()->GetStruct();

        // �̳ǰ汾��
        m_version = RootStruct.version.number;
        PRODUCT_INFO product;

        for (std::vector<MALL_ROOT_LIST_PRODUCT_STRUCT>::const_iterator cIter = RootStruct.list.product_list.begin(); cIter != RootStruct.list.product_list.end(); ++cIter)
        {
            product.product_id = cIter->product_id;    // ��ƷID
            product.product_num = cIter->product_num;  // ��Ʒ����
            product.money_type = cIter->money_type;  // ��Ʒ����
            product.sell_type = cIter->sell_type;     // ��������
            product.tag_type = cIter->tag_type;      // ��ǩҳ����
            product.home_page = cIter->home_page;     // �Ƿ���ʾ����ҳ��
            product.discount = cIter->discount;      // ������ʾ��

            product.show_flag = cIter->show_flag;    // �Ƿ���ʾ���
            product.hot_flag = cIter->hot_flag;    // �������
            product.new_flag = cIter->new_flag;    // ����Ʒ���
            product.present_flag = cIter->present_flag;    // ���ͱ��

            product.item_id = cIter->item_id;    // �������ƷID
            product.item_num = cIter->item_num;    // �������Ʒ����
            product.price = cIter->price;    // ��ͨ����۸�
            product.show_price = cIter->show_price;    // ��ͨ��ʾ�۸�
            product.vip_price = cIter->vip_price;    // VIP����۸�
            product.show_vip_price = cIter->show_vip_price;    // VIP��ʾ�۸�

            product.again_discount = cIter->again_discount;    // �����۵���ʾ��
            product.again_price = cIter->again_price;    // �����۵Ĺ���۸�
            product.show_again_price = cIter->show_again_price;    // �����۵ĵ���ʾ�۸�
            //mallItem->QueryIntAttribute("again_start_time", &product.again_start_time);     //�����۵ĵĿ�ʼʱ��
            //mallItem->QueryIntAttribute("again_end_time", &product.again_end_time);         // �����۵ĵĽ���ʱ��

            std::string discountime = "";// �����۵�ʱ��
            discountime = cIter->again_start_time;
            if((discountime == "0000-00-00 00:00:00") || (discountime == "")|| (discountime == "0"))
            {
                product.again_start_time = 0;
            }
            else
            {
                product.again_start_time = Crown::TimeStamp(discountime);
            }


            product.global_limit_num = cIter->global_limit_num;       // ȫ���޹�����
            product.personal_limit_num = cIter->personal_limit_num;   // �����޹�����

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
                case MALL_NORMAL_SELL_TYPE: // 1.������Ʒ
                case MALL_NOT_SELL_TYPE:    // 2.������Ʒ
                case MALL_LIMIT_SELL_TYPE:  // 3.�޹���Ʒ
                case VIP_LIMIT_SELL_TYPE:   // 5.VIP�޹���Ʒ
                    break;
                default:
                    // ����������
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
  
    // ���ط�������
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
    // ����Ƿ����д���Ʒ
    if(m_dbMallMap.find(productInfo.product_id) != m_dbMallMap.end())
    {
        CnError("Duplicate ProductID:%u\n", productInfo.product_id);
        return;
    }

    // �����Ʒ
    m_dbMallMap[productInfo.product_id] = productInfo;
}

void MallConfigManager::ClearDBProductInfo()
{
    m_dbMallMap.clear();
}

// ==================================================
// ������ƷID��ѯ��Ʒ
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
// ���ݵ���ID��ѯ��Ʒ
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
