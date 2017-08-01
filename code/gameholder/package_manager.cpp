#include "gameholder_pch.h"
#include "package_manager.h"
#include "player.h"
#include "global_setting_manager.h"
#include "package_config.h"

PackageManager::PackageManager(Player* player):
m_player(player)
{
    Clear();
}

PackageManager::~PackageManager(void)
{
}

void PackageManager::Clear()
{

}

void PackageManager::OnRecv( uint32 msgID, void* data )
{

}

void PackageManager::LoadPackageInfo( const PACKAGE_BAG& info )
{

    m_packageMap = info;
}

void PackageManager::FillPackageInfo( PACKAGE_BAG& info )
{
    info = m_packageMap;
}

const TREASURE_PACKAGE_ITEM* PackageManager::GetPackageItem(int32 packageid, uint32 type)
{
    return GetTreasurePackageItem(PackageConfig::Instance()->GetTreasurePackage(packageid), type);
}


const TREASURE_PACKAGE_ITEM* PackageManager::GetTreasurePackageItem( const TREASURE_PACKAGE_INFO* package , uint32 type)
{
    const TREASURE_PACKAGE_ITEM* packageItem = NULL;
    if(!package)
    {
        return NULL;
    }
    
    PACKAGE_INFO* packageInfo = GetPackageInfoToType(type);
    if(!packageInfo)
    {
        PACKAGE_INFO newPackageInfo;
        newPackageInfo.type = type;
        m_packageMap.package_info.push_back(newPackageInfo);
    }

    if(package->extract == 1)
    {
        PACKAGE_COUNT_INFO* countInfo = GetPackageInfoCount( type, package->package_id);
        if(!countInfo)
            return packageItem;

        countInfo->package_count += 1;

        if( countInfo->package_count >= package->times)
        {
            //次数达到直接获取物品
            for (std::vector<TREASURE_PACKAGE_ITEM>::const_iterator it = package->item_list.begin(); it != package->item_list.end(); it++)
            {
                if(it->extract == 1)
                {
                    packageItem = &(*it);
                    break;
                }
            }
        }
        else
        {
            //次数没有
            do 
            {
                packageItem = RandomPackageHandle(package, type);
            } 
            while (!packageItem);
        }
        if(packageItem->extract == 1)
        {
            //重置物品次数
            countInfo->package_count = 0;
        }
    }
    else
    {
        //不记录次数抽物品
        do
        {
            packageItem = RandomPackageHandle(package, type);
        }
        while (!packageItem);
    }

    if(packageItem->wrap == 1)
    {
        //物品是包裹继续抽
        return GetTreasurePackageItem(PackageConfig::Instance()->GetTreasurePackage(packageItem->item_id), type);
    }
    else
    {
        //物品不是包裹
        return packageItem;
    }

}


const TREASURE_PACKAGE_ITEM* PackageManager::RandomPackageHandle(const TREASURE_PACKAGE_INFO* package, uint32 type)
{
    const TREASURE_PACKAGE_ITEM* item;
    uint32 probability = Crown::SDRandom(BASE_RATE_NUM);
    uint32 lastProbability = 0;
    bool haveItem = false;
    for (std::vector<TREASURE_PACKAGE_ITEM>::const_iterator it = package->item_list.begin(); it != package->item_list.end(); it++)
    {
        if(probability > lastProbability  && probability <= it->all_probability)
        {
            //随机物品
            item = &(*it);
            haveItem = true;
            break;
        }
        lastProbability = it->all_probability;
    }

    if(!haveItem)
    {
        //没有物品
        return NULL;
    }

    Property itemTime;
    int32 curTime;
    bool perFlag = false;
    bool serFlag = false;
    curTime = (int32)Crown::SDNow().GetTimeValue();

    //判断时间
    perFlag = PackageJudgeTimeHandle(item, package->package_id, type);
    serFlag = GlobalSettingManager::Instance()->CheckPackageTime(package->package_id,item->item_id,item->ser_cd);

    //是否通过
    if(perFlag && serFlag)
    {
        //设置时间
        PackageSetTimeHandle(item, package->package_id, type);
        GlobalSettingManager::Instance()->SetPackageTime(package->package_id,item->item_id,item->ser_cd);
        return item;
    }
    else
    {
        //没有物品
        return NULL;
    }
}

bool PackageManager::PackageJudgeTimeHandle(const TREASURE_PACKAGE_ITEM* item,int32 packageId, int32 type)
{
    int32 cd = 0;
    bool flag = false;
    int32 curTime = (int32)Crown::SDNow().GetTimeValue();
    cd = item->per_cd;

    if(cd > 0)
    {
        PACKAGE_INFO* packageInfo = GetPackageInfoToType(type);
        if(!packageInfo)
        {
            return true;
        }

        PACKAGE_TIME_INFO* timeInfo = GetPackageInfoTime(packageId, item->item_id, type);

        if(!timeInfo)
        {
            return true;
        }

        if(curTime - (int32)timeInfo->item_time > cd)
        {
            //物品cd时间到了
            flag = true;
        }
        else
        {
            //物品cd时间没到
            flag = false;
        }

    }
    else
    {
        flag = true;
    }
    return flag;
}

bool PackageManager::PackageSetTimeHandle(const TREASURE_PACKAGE_ITEM* item,int32 packageId, int32 type)
{
    int32 cd = 0;
    Property itemTime;
    int32 curTime = (int32)Crown::SDNow().GetTimeValue();
    bool have = false;

    cd = item->per_cd;

    if(cd > 0)
    {

        PACKAGE_TIME_INFO* timeInfo = GetPackageInfoTime(packageId, item->item_id, type);
        if(!timeInfo)
        {
            return true;
        }

        if(curTime - (int32)timeInfo->item_time > cd)
        {
            //物品cd时间到了
            timeInfo->item_time = curTime;
        }
    }
    return true;
}

PACKAGE_INFO* PackageManager::GetPackageInfoToType( int32 type )
{
    for (std::vector<PACKAGE_INFO>::iterator it = m_packageMap.package_info.begin(); it != m_packageMap.package_info.end(); it++)
    {
        if(it->type == type)
            return &(*it);
    }
    return NULL;
}

PACKAGE_COUNT_INFO* PackageManager::GetPackageInfoCount( uint32 type, uint32 id)
{

    PACKAGE_INFO* packageInfo = GetPackageInfoToType(type);
    if(!packageInfo)
        return NULL;

    bool countFlag = false;
    for (std::vector<PACKAGE_COUNT_INFO>::iterator icout = packageInfo->package_count.begin(); icout != packageInfo->package_count.end(); icout++)
    {
        if(icout->package_id == id)
        {
            countFlag = true;
            return &(*icout);
        }
    }
    
    if(!countFlag)
    {
        PACKAGE_COUNT_INFO newCountInfo;
        newCountInfo.package_id = id;
        newCountInfo.package_count = 0;
        packageInfo->package_count.push_back(newCountInfo);

        for (std::vector<PACKAGE_COUNT_INFO>::iterator icout = packageInfo->package_count.begin(); icout != packageInfo->package_count.end(); icout++)
        {
            if(icout->package_id == id)
            {
                return &(*icout);
            }
        }
    }
    return NULL;
}

PACKAGE_TIME_INFO* PackageManager::GetPackageInfoTime( uint32 packageid, uint32 itemid, uint32 type)
{
    PACKAGE_INFO* packageInfo = GetPackageInfoToType(type);
    if(!packageInfo)
        return NULL;

    bool countFlag = false;
    for (std::vector<PACKAGE_TIME_INFO>::iterator it = packageInfo->package_time.begin(); it != packageInfo->package_time.end(); it++)
    {
        if(it->package_id == packageid && it->item_id == itemid)
        {
            countFlag = true;
            return &(*it);
        }
    }

    if(!countFlag)
    {
            PACKAGE_TIME_INFO newTimetInfo;
            newTimetInfo.package_id = packageid;
            newTimetInfo.item_id = itemid;
            newTimetInfo.item_time = 0;
            packageInfo->package_time.push_back(newTimetInfo);

            for (std::vector<PACKAGE_TIME_INFO>::iterator it = packageInfo->package_time.begin(); it != packageInfo->package_time.end(); it++)
            {
                if(it->package_id == packageid && it->item_id == itemid)
                {
                    return &(*it);
                }
            }
        
    }

    return NULL;
}

void PackageManager::RandomPackageItem( int package_id, std::vector<REWARD_ITEM>* itemList, uint32 type )
{
    const TREASURE_PACKAGE_ITEM* packageItem = GetTreasurePackageItem(PackageConfig::Instance()->GetTreasurePackage(package_id), type);
    if(!packageItem)
    {
        return;
    }

    REWARD_ITEM rewardItem;
    int32 num = 0;
    if(packageItem->count_max > packageItem->count_min)
    {
       //随机物品数量
       num = packageItem->count_min + Crown::SDRandom(packageItem->count_max - packageItem->count_min);
    }
    else
    {
        num =  packageItem->count_min;
   }
   //添加抽到的物品
    rewardItem.id = packageItem->item_id;
    rewardItem.num = num;

    itemList->push_back(rewardItem);

}
