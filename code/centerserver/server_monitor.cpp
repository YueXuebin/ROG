#include "centerserver_pch.h"
#include "server_monitor.h"
#include "config_loader.h"

IMPLEMENT_SINGLETON(ServerMonitor)

ServerMonitor::ServerMonitor() :
m_state(SERVER_STATE_INIT)
{
    
}

ServerMonitor::~ServerMonitor()
{

}

void ServerMonitor::OnAllServerReady()
{
    if(m_state == SERVER_STATE_INIT)
        SetState(SERVER_STATE_RUNNING);
}

void ServerMonitor::OnGameServerClose( uint32 gameSvrID )
{
    ServerConfig* pGameConfig = ConfigLoader::Instance()->GetServerConfig(SERVER_TYPE_GAME, gameSvrID);
    bool isCrossGame = !pGameConfig->m_is_config;
    if(pGameConfig && isCrossGame)
    {   // 跨服GameServer断开属于正常行为，不记录异常
        return;
    }

    SetState(SERVER_STATE_EXCEPTION);
}

void ServerMonitor::OnGateServerClose( uint32 gateRegionID )
{
    SetState(SERVER_STATE_EXCEPTION);
}

void ServerMonitor::OnFlashServerClose()
{
    SetState(SERVER_STATE_EXCEPTION);
}

void ServerMonitor::OnTimerServerClose()
{
    SetState(SERVER_STATE_EXCEPTION);
}

void ServerMonitor::SetState(uint32 state)
{
    m_state = state;
    CnInfo("server state=%d\n", m_state);
}