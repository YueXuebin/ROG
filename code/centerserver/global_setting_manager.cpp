#include "centerserver_pch.h"
#include "global_setting_manager.h"
#include "global_setting_job.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(GlobalSettingManager)

GlobalSettingManager::GlobalSettingManager(void)
{
}

GlobalSettingManager::~GlobalSettingManager(void)
{
}

void GlobalSettingManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GLOBAL_SETTING_INFO_FILL:
        FillGlobalSettingList(data);
        break;
    case MSG_GLOBAL_SETTING_INFO_LOAD_REQ:
        SendGlobalSettingList(GameHandler::Instance()->GetRegionIDBySessionID(sessionID));
        break;
    default:
        CnAssert(false);
        break;
    }
}

void GlobalSettingManager::SaveGlobalSettingInfo()
{
    GlobalSettingJob::Instance()->SaveGlobalSettingInfo();
}

void GlobalSettingManager::Init()
{
    GlobalSettingJob::Instance()->LoadGlobalSettingInfo();
}

void GlobalSettingManager::FillGlobalSettingList(void* data)
{
    GLOBAL_CONFIG_BAG* recvMsg = (GLOBAL_CONFIG_BAG*)data;
    m_nextFashionInfoList.fashion_info.Clear();
    // 更新配置信息
    for(std::vector<PACKAGE_TIME_INFO>::iterator bossIt = recvMsg->fashion_info.package_time.begin(); bossIt != recvMsg->fashion_info.package_time.end(); ++bossIt)
    {
        PACKAGE_TIME_INFO& bossInfo = *bossIt;
        bool have = true;
        // 是否已经有这个配置信息
        for(std::vector<PACKAGE_TIME_INFO>::iterator nextIt = m_nextFashionInfoList.fashion_info.package_time.begin(); nextIt != m_nextFashionInfoList.fashion_info.package_time.end(); ++nextIt)
        {
            if(bossIt->package_id == nextIt->package_id && bossIt->item_time == nextIt->item_time)
            {
                nextIt->item_time = bossIt->item_time;
                break;
            }
        }

        // 不存在添加这个配置
        if(have)
        {
            m_nextFashionInfoList.fashion_info.package_time.push_back(*bossIt);
        }
    }

    // 保存数据库
    SaveGlobalSettingInfo();
    // 发送给客户端
    SendGlobalSettingList();
}

void GlobalSettingManager::SendGlobalSettingList(uint32 gameserverID)
{
    GLOBAL_CONFIG_BAG sendMsg;
    sendMsg.Clear();
    sendMsg = m_nextFashionInfoList;

    if(gameserverID == 0)
    {
        GameHandler::Instance()->SendToAll(MSG_GLOBAL_SETTING_INFO_LOAD, &sendMsg);
    }
    else
    {
        GameHandler::Instance()->SendByID(gameserverID, MSG_GLOBAL_SETTING_INFO_LOAD, &sendMsg);
    }
}
