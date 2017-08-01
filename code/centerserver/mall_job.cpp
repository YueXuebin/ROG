#include "centerserver_pch.h"
#include "mall_job.h"
#include "dbdriver.h"
#include "mall_config_manager.h"

IMPLEMENT_SINGLETON(MallJob)

// ==================================================
// 构造函数
// ==================================================
MallJob::MallJob()
{
    m_dbExecutor = 0;
}

// ==================================================
// 析构函数
// ==================================================
MallJob::~MallJob()
{
    
}

// ==================================================
// 加载系统版本
// ==================================================
void MallJob::LoadSystemVersion(uint32 system_id, uint32& version)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("SELECT `version` FROM `system_version` WHERE `system_id`=%u", system_id);
    if(recorder.HasExecut() && (recorder.GetRowNum() > 0))
    {
        recorder.GetRecord(version);
    }
}

// ==================================================
// 更新数据库中的系统版本
// ==================================================
bool MallJob::UpdateSystemVersion(uint32 system_id, uint32 version)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO `system_version` VALUES('%u', '%u') "
        "ON DUPLICATE KEY UPDATE `version`=%u", system_id, version, version);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 删除商城表中可删除的数据
// ==================================================
bool MallJob::DelModifyMallInfo()
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM `mall_info` WHERE `modify_flag`=%d", 1);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 向DB中加入新的商城数据
// ==================================================
bool MallJob::InsertNewMallInfo()
{
    DBRecorder recorder;
    std::stringstream ss;
    MallInfoMap::const_iterator mallInfoIt;

    const MallInfoMap& mallInfoMap = MallConfigManager::Instance()->GetXmlMallInfoMap();
    for(mallInfoIt = mallInfoMap.begin(); mallInfoIt != mallInfoMap.end(); )
    {
        ss.str("");
        
        ss << "INSERT INTO `mall_info` ";

        for(uint32 i = 0;(mallInfoIt != mallInfoMap.end()) && (i < MAX_MALLINFO_SIZE); ++i)
        {

            if(i == 0)
            {
                ss << "VALUES(";
            }
            else
            {
                ss << ",(";
            }

            ss << mallInfoIt->second.product_id;
            ss << ",";
            ss << mallInfoIt->second.product_num;
            ss << ",";
            ss << (int32)mallInfoIt->second.money_type;
            ss << ",";
            ss << mallInfoIt->second.sell_type;
            ss << ",";
            ss << mallInfoIt->second.tag_type;
            ss << ",";
            ss << mallInfoIt->second.home_page;
            ss << ",";
            ss << mallInfoIt->second.discount;
            ss << ",";
            ss << (int32)mallInfoIt->second.show_flag;
            ss << ",";
            ss << (int32)mallInfoIt->second.hot_flag;
            ss << ",";
            ss << (int32)mallInfoIt->second.new_flag;
            ss << ",";
            ss << (int32)mallInfoIt->second.present_flag;
            ss << ",";
            ss << 1;                            ///< 允许修改此项数据
            ss << ",";
            ss << mallInfoIt->second.item_id;
            ss << ",";
            ss << mallInfoIt->second.item_num;
            ss << ",";
            ss << mallInfoIt->second.price;
            ss << ",";
            ss << mallInfoIt->second.show_price;
            ss << ",";
            ss << mallInfoIt->second.vip_price;
            ss << ",";
            ss << mallInfoIt->second.show_vip_price;
            ss << ",";
            ss << mallInfoIt->second.again_discount;
            ss << ",";
            ss << mallInfoIt->second.again_price;
            ss << ",";
            ss << mallInfoIt->second.show_again_price;
            //ss << ",";
            //ss << mallInfoIt->second.again_start_time;
            //ss << ",";
            //ss << mallInfoIt->second.again_end_time;

            if(mallInfoIt->second.again_start_time)
            {
                ss << ",FROM_UNIXTIME(" << mallInfoIt->second.again_start_time << ")";
            }
            else
            {
                ss << "," << mallInfoIt->second.again_start_time;
            }

            if(mallInfoIt->second.again_end_time)
            {
                ss << ",FROM_UNIXTIME(" << mallInfoIt->second.again_end_time << ")";
            }
            else
            {
                ss << "," << mallInfoIt->second.again_end_time /*<< ")"*/;
            }

            ss << " ,";
            ss << mallInfoIt->second.personal_limit_num;
            ss << ",";
            ss << mallInfoIt->second.global_limit_num;
            ss << ",";
            ss << 0;                            ///< 全局购买计数

            if(mallInfoIt->second.start_time)
            {
                ss << ",FROM_UNIXTIME(" << mallInfoIt->second.start_time << ")";
            }
            else
            {
                ss << "," << mallInfoIt->second.start_time;
            }

            if(mallInfoIt->second.end_time)
            {
                ss << ",FROM_UNIXTIME(" << mallInfoIt->second.end_time << "))";
            }
            else
            {
                ss << "," << mallInfoIt->second.end_time << ")";
            }

            ++mallInfoIt;
        }

        recorder = m_dbExecutor->Query(ss.str().c_str());
        if(!recorder.HasExecut())
        {
            CnError("Insert mall_info failed\n");
            return false;
        }
    }

    return true;
}

// ==================================================
// 从数据库中读取商品信息
// ==================================================
void MallJob::LoadDBMallInfo()
{
    DBRecorder recorder;
    PRODUCT_INFO productInfo;

    recorder = m_dbExecutor->Query("SELECT `product_id`, `product_num`,`money_type`, `sell_type`, `tag_type`, `home_page`, "
        "`discount`, `show_flag`, `hot_flag`, `new_flag`, `present_flag`, "
        "`item_id`, `item_num`, `price`, `show_price`, `vip_price`, `show_vip_price`,`again_discount`,`again_price`,`show_again_price`,UNIX_TIMESTAMP(`again_start_time`),UNIX_TIMESTAMP(`again_end_time`), "
        "`personal_limit_num`, `global_limit_num`, `global_limit_counter`, UNIX_TIMESTAMP(`start_time`), UNIX_TIMESTAMP(`end_time`) "
        "FROM `mall_info`");

    for(uint32 i=0; i<recorder.GetRowNum(); ++i)
    {
        productInfo.Clear();

        int64 again_start_time = 0;
        int64 again_end_time = 0;
        int64 start_time = 0;
        int64 end_time = 0;

        recorder.GetRecord(productInfo.product_id);
        recorder.GetRecord(productInfo.product_num);
        recorder.GetRecord(productInfo.money_type);
        recorder.GetRecord(productInfo.sell_type);
        recorder.GetRecord(productInfo.tag_type);
        recorder.GetRecord(productInfo.home_page);
        recorder.GetRecord(productInfo.discount);
        recorder.GetRecord(productInfo.show_flag);
        recorder.GetRecord(productInfo.hot_flag);
        recorder.GetRecord(productInfo.new_flag);
        recorder.GetRecord(productInfo.present_flag);
        recorder.GetRecord(productInfo.item_id);
        recorder.GetRecord(productInfo.item_num);
        recorder.GetRecord(productInfo.price);
        recorder.GetRecord(productInfo.show_price);
        recorder.GetRecord(productInfo.vip_price);
        recorder.GetRecord(productInfo.show_vip_price);
        recorder.GetRecord(productInfo.again_discount);
        recorder.GetRecord(productInfo.again_price);
        recorder.GetRecord(productInfo.show_again_price);
        recorder.GetRecord(again_start_time);
        productInfo.again_start_time = (uint32)again_start_time;
        recorder.GetRecord(again_end_time);
        productInfo.again_end_time = (uint32)again_end_time;

        recorder.GetRecord(productInfo.personal_limit_num);
        recorder.GetRecord(productInfo.global_limit_num);
        recorder.GetRecord(productInfo.global_limit_counter);

        recorder.GetRecord(start_time);
        productInfo.start_time = (uint32)start_time;
        //if(productInfo.start_time <= 0)
        //{
        //    productInfo.start_time = 315504000;//1980年1月1日
        //}

        recorder.GetRecord(end_time);
        productInfo.end_time = (uint32)end_time;

        if(productInfo.end_time <= 0)
        {
            productInfo.end_time = 4070880000; //2099年1月1日
        }

        if(productInfo.end_time < productInfo.start_time)
        {
            CnInfo("productInfo.end_time < productInfo.start_time\n");
        }


        MallConfigManager::Instance()->AddDBProductInfo(productInfo);
    }

}
