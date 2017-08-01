#include "centerserver_pch.h"
#include "random_package_manager.h"
#include "player.h"
#include "player_manager.h"
#include "config_loader.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(RandomPackageManager)

RandomPackageManager::RandomPackageManager()
{

}

RandomPackageManager::~RandomPackageManager()
{

}

// ==================================================
// 接收网络消息
// ==================================================
void RandomPackageManager::OnRecv( uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GLOBAL_SETTING_INFO_FILL:
        {
            FillRecordInfo(data);
        }
        break;
    default:
        break;
    }
}

void RandomPackageManager::SendRecordInfo(const uint32 sessionID)
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
    if(!regionID)
    {
        return;
    }

    GameHandler::Instance()->SendByID(regionID, MSG_SERVER_PACKAGE_RECORD_LOAD, &m_RecordInfo);
}

void RandomPackageManager::FillRecordInfo(void* data)
{
    RANDOM_PACKAGE_RECORD_INFO* pInfo = (RANDOM_PACKAGE_RECORD_INFO*)data;
    m_RecordInfo = *pInfo;

    GameHandler::Instance()->SendToAll(MSG_SERVER_PACKAGE_RECORD_LOAD, &m_RecordInfo);
}
