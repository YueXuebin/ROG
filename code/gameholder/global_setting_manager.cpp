#include "gameholder_pch.h"
#include "global_setting_manager.h"
#include "center_handler.h"
#include "player.h"
#include "gate_handler.h"

IMPLEMENT_SINGLETON(GlobalSettingManager)

GlobalSettingManager::GlobalSettingManager(void)
{
}

GlobalSettingManager::~GlobalSettingManager(void)
{
}


void GlobalSettingManager::OnServerReady()
{
    // 加载配置信息
    GlobalSettingLoadReq();
}

void GlobalSettingManager::GlobalSettingLoadReq()
{
    NONE sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_GLOBAL_SETTING_INFO_LOAD_REQ, &sendMsg);        // call -> MSG_WORLD_BOSS_INFO_LOAD
}

void GlobalSettingManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GLOBAL_SETTING_INFO_LOAD:
        {
            // 更新配置信息
            GLOBAL_CONFIG_BAG* recvMsg = (GLOBAL_CONFIG_BAG*)data;
            LoadGlobalSettingInfo(*recvMsg);
        }
        break;
    default:
        break;
    }

}

void GlobalSettingManager::LoadGlobalSettingInfo(GLOBAL_CONFIG_BAG& info)
{
    //m_configInfo.Clear();

    //m_configInfo.fashion_info = info.fashion_info;

    
    if (info.fashion_info.package_time.size() > 0)
    {
        bool have = false;
        m_serverTime.clear();
        Property itemTime;
        for (std::vector<PACKAGE_TIME_INFO>::iterator timei = info.fashion_info.package_time.begin(); timei != info.fashion_info.package_time.end(); ++timei)
        {
            if(m_serverTime.find(timei->package_id) == m_serverTime.end())
            {
                itemTime.Clear();
                itemTime.type = timei->item_id;
                itemTime.value = timei->item_time;
                m_serverTime[timei->package_id].push_back(itemTime);
            }
            else
            {
                 have= true;
                for (std::vector<Property>::iterator it = m_serverTime[timei->package_id].begin(); it != m_serverTime[timei->package_id].end(); it++)
                {
                    if(it->type == timei->item_id)
                    {
                        it->value = timei->item_time;
                        have= false;
                    }
                }
                if(have)
                {
                    itemTime.Clear();
                    itemTime.type = timei->item_id;
                    itemTime.value = timei->item_time;
                    m_serverTime[timei->package_id].push_back(itemTime);
                }
            }
        }
     }
    
}


void GlobalSettingManager::SendGlobalSettingInfoToCenter()
{
    GLOBAL_CONFIG_BAG sendMsg;
    FillGlobalSetting(sendMsg);
    CenterHandler::Instance()->SendToMasterCenter(MSG_GLOBAL_SETTING_INFO_FILL, &sendMsg);
}

void GlobalSettingManager::FillGlobalSetting(GLOBAL_CONFIG_BAG& info)
{
    //info.fashion_info = m_configInfo.fashion_info;

    PACKAGE_TIME_INFO time_info;
    for(std::map<uint32, PropertyList>::iterator timei = m_serverTime.begin(); timei != m_serverTime.end(); ++timei)
    {
        for (std::vector<Property>::iterator it = m_serverTime[timei->first].begin(); it != m_serverTime[timei->first].end(); it++)
        {
            time_info.Clear();
            time_info.package_id = timei->first;
            time_info.item_id = it->type;
            time_info.item_time = it->value;
            info.fashion_info.package_time.push_back(time_info);
        }
    }
}

bool GlobalSettingManager::CheckPackageTime( int32 packageid, int32 itemid,int32 cd )
{
    bool flag = false;
    if(cd > 0)
    {
        bool have = false;

        if(m_serverTime.find(packageid) == m_serverTime.end())
        {
            flag = true;
        }
        else
        {
            have= false;
            for (std::vector<Property>::iterator it = m_serverTime[packageid].begin(); it != m_serverTime[packageid].end(); it++)
            {
                if(it->type == itemid)
                {
                    have = true;
                    int32 curTime = (int32)Crown::SDNow().GetTimeValue();
                    if(curTime - (int32)it->value > cd)
                    {
                        //物品cd时间到了
                        flag = true;
                    }
                    else
                    {
                        //物品cd时间没到
                        have = false;
                        flag = false;
                    }
                }
            }
            if(have)
            {
                //没有找到物品cd
                flag = true;
            }
        }
    }
    else
    {
        flag = true;
    }
    return flag;
}

bool GlobalSettingManager::SetPackageTime( int32 packageid, int32 itemid,int32 cd )
{
    if(cd > 0)
    {
        Property itemTime;
        bool updateflag = false;
        bool have = false;
        int32 curTime = (int32)Crown::SDNow().GetTimeValue();
        if(m_serverTime.find(packageid) == m_serverTime.end())
        {
            itemTime.type = itemid;
            itemTime.value = curTime;
            m_serverTime[packageid].push_back(itemTime);
            updateflag = true;
        }
        else
        {
            have = true;
            for (std::vector<Property>::iterator it = m_serverTime[packageid].begin(); it != m_serverTime[packageid].end(); it++)
            {
                if(it->type == itemid)
                {
                    have = false;
                    if(curTime - (int32)it->value > cd)
                    {
                        //物品cd时间到了
                        it->value = curTime;
                        updateflag = true;
                    }
                }
            }
            if(have)
            {
                //没有找到物品cd
                itemTime.type = itemid;
                itemTime.value = curTime;
                m_serverTime[packageid].push_back(itemTime);
                updateflag = true;
            }
            
        }
        if(updateflag)
        {
            SendGlobalSettingInfoToCenter();
        }
    }
    return true;
}
