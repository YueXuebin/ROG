#include "centerserver_pch.h"
#include "city_fight_manager_gc.h"
#include "game_handler.h"
#include "player_manager.h"
#include "player.h"
#include "guild_config.h"
#include "player_net_handler.h"
#include "special_activity_job.h"

IMPLEMENT_SINGLETON(CityFightManager)

CityFightManager::CityFightManager()
{
    m_init_finish = false;
}

CityFightManager::~CityFightManager()
{

}

//gs->cs
void CityFightManager::OnGSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    if (!m_init_finish)
        return;

    switch(msgID)
    {
    case MSG_CITY_FIGHT_INFO_NTF:
        {
            CITY_FIGHT_INFO_NTF* ntf = (CITY_FIGHT_INFO_NTF*)data;
            if (!ntf)
                return;
            
            if (ntf->save_in_db)
            {
                // 存档gs发过来的数据
                SpecialActivityJob::Instance()->SaveCityFightActivityInfo(ntf);
            }

            // 覆盖cs上的数据
            m_city_fight_info = *ntf;

            // 同步到其他服务器上
            uint32 regionId = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
            GameHandler::Instance()->SendToAllBut(regionId, MSG_CITY_FIGHT_INFO_NTF_ACK, &m_city_fight_info);
            break;
        }
    case MSG_CITY_FIGHT_INFO_NTF_REQ:
        {
            uint32 regionId = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
            GameHandler::Instance()->SendByID(regionId, MSG_CITY_FIGHT_INFO_NTF_ACK, &m_city_fight_info);
            break;
        }
    default:
        break;
    }
}

//ts->cs
void CityFightManager::OnTSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_CITY_FIGHT_START_NTF: 
        {
            GameHandler::Instance()->SendToAll(msgID, data);
            break;
        }
    case MSG_CITY_FIGHT_END_NTF:
        {
            GameHandler::Instance()->SendToAll(msgID, data);
            break;
        }
    default:
        break;
    }
}

void CityFightManager::Init()
{
    // gs请求加载数据
    SpecialActivityJob::Instance()->GetCityFightActivityInfo();
}

void CityFightManager::InitFinish()
{
    m_init_finish = true;
}

CITY_FIGHT_INFO_NTF& CityFightManager::GetCityFightSaveInfo()
{ 
    return m_city_fight_info; 
}


