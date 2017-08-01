#include "gameholder_pch.h"
#include "city_survival_manager.h"
#include "player_manager.h"
#include "activity_manager.h"
#include "player.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "center_handler.h"
#include "gate_handler.h"
#include "game_util.h"
#include "server_config.h"

IMPLEMENT_SINGLETON(CitySurvivalManager)

// ==================================================
// 构造函数
// ==================================================
CitySurvivalManager::CitySurvivalManager()
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
CitySurvivalManager::~CitySurvivalManager()
{
}

// ==================================================
// 清除数据
// ==================================================
void CitySurvivalManager::Clear()
{
    m_open = false;
    m_activity_server = false;
    m_cur_time = 0;
    m_player_map.clear();
}

void CitySurvivalManager::Update()
{
    m_cur_time += g_ServerUpdateTime;

    //非活动所在服务器
    if (!m_activity_server)
        return;

    // 数据有变更,则广播当前信息
    if (m_city_survival_info_modify)
    {
        m_city_survival_info_modify = false;

        SendCitySurvivalInfoNtf();
    }
}

uint16 CitySurvivalManager::CanEnter(Player* player)
{
    if (!m_open)
        return ERR_ACTIVITY_NO_OPEN;

    SurvivalPlayerMap::iterator iter = m_player_map.find(player->GetPlayerID());
    if ((iter != m_player_map.end()) && iter->second > 0)
        return ERR_ACTIVITY_NO_ENOUGH_ENTER;

    return ERR_SUCCEED;
}

void CitySurvivalManager::PlayerEnter( Player* player )
{
    // 非活动服务器不会到这里
    if (!m_activity_server)
        return;

    if (!player)
        return;

    SurvivalPlayerMap::iterator iter = m_player_map.find(player->GetPlayerID());
    if (iter != m_player_map.end())
    {
        CnWarn("city survival repeat enter! \n");
        return;
    }
    else
    {
        m_player_map.insert(std::make_pair(player->GetPlayerID(), 1));
        m_city_survival_info_modify = true;
    }
    
    SendEnterAck(player, ERR_SUCCEED);
}

void CitySurvivalManager::PlayerLeave( Player* player )
{
}

// ==================================================
// 协议接收client
// ==================================================
void CitySurvivalManager::OnRecv(uint32 msgID, void* data, Player* player)
{
    switch (msgID)
    {
        case MSG_CITY_SURVIVAL_INFO_REQ:
            {
                OnInfoReq(data, player);
                break;
            }
        case MSG_CITY_SURVIVAL_ENTER_REQ:
            {
               OnEnterReq(data, player);
                break;
            }
        default:
            break;
    }
}

// ==================================================
// 协议接收centerserver
// ==================================================
void CitySurvivalManager::OnCSRecv( uint64 playerID, uint32 msgID, void* data )
{
    switch (msgID)
    {
    case MSG_CITY_SURVIVAL_START_NTF:
        {
            OnStart(data);
            break;
        }
    case MSG_CITY_SURVIVAL_END_NTF:
        {
            OnEnd(data);
            break;
        }
    case MSG_CITY_SURVIVAL_REALLY_START_NTF:
        {
            OnReallyStart();
            break;
        }
    case MSG_CITY_SURVIVAL_REALLY_END_NTF:
        {
            OnReallyEnd();
            break;
        }
    case MSG_CITY_SURVIVAL_INFO_NTF:
        {
            OnInfoNtf(data);
            break;
        }
    default:
        break;
    }

}

// ==================================================
// 开始
// ==================================================
void CitySurvivalManager::OnStart( void* data )
{
    // 先清空之前数据
    Clear();

    m_activity_server = IsActivityServer();
    if (m_activity_server)
    {
        OnReallyStart();

        NONE none;
        CenterHandler::Instance()->SendToMasterCenter(MSG_CITY_SURVIVAL_REALLY_START_NTF, &none);
    }
}

// ==================================================
// 结束
// ==================================================
void CitySurvivalManager::OnEnd( void* data )
{
    if (m_activity_server)
    {
        OnReallyEnd();

        NONE none;
        CenterHandler::Instance()->SendToMasterCenter(MSG_CITY_SURVIVAL_REALLY_START_NTF, &none);
    }
}

// ==================================================
// 开始
// ==================================================
void CitySurvivalManager::OnReallyStart()
{
    m_open = true;
    
    NONE none;
    GateHandler::Instance()->SendToAll(MSG_CITY_SURVIVAL_REALLY_START_NTF, &none);
}

// ==================================================
// 结束
// ==================================================
void CitySurvivalManager::OnReallyEnd()
{
    m_open = false;

    //DungeonManager::Instance()->StopAllRegular(CITY_SURVIVAL_MAP);

    NONE none;
    GateHandler::Instance()->SendToAll(MSG_CITY_SURVIVAL_REALLY_END_NTF, &none);
}

void CitySurvivalManager::OnInfoNtf(void* data)
{
    CITY_SURVIVAL_INFO_NTF* ntf = (CITY_SURVIVAL_INFO_NTF*)data;
    if (!ntf)
        return;

    uint64 playerId = 0;
    for (uint32 i = 0; i < ntf->player_list.size(); ++i)
    {
        playerId = U64ID(ntf->player_list[i].id_h, ntf->player_list[i].id_l);
        m_player_map.insert(std::make_pair(playerId, 1));
    }
}

// ==================================================
// 城市信息请求
// ==================================================
void CitySurvivalManager::OnInfoReq(void* data, Player* player)
{
    if (!player)
        return;

    CITY_SURVIVAL_INFO info;
    info.open = IsOpen();
    player->SendToGate(MSG_CITY_SURVIVAL_INFO_ACK, &info);
}

// ==================================================
// 进入请求
// ==================================================
void CitySurvivalManager::OnEnterReq(void* data, Player* player)
{
    if (!player)
        return;

    // 检查角色是否可进
    uint32 errCode = CanEnter(player);
    if (errCode != ERR_SUCCEED)
    {
        SendEnterAck(player, errCode);
        return;
    }

    // 跳副本
    //CHANGE_DUNGEON_REQ changeDungeonMsg;
    //changeDungeonMsg.dungeon_id = CITY_SURVIVAL_MAP;
    //player->SendToCenter(MSG_CHANGE_DUNGEON_REQ, &changeDungeonMsg);
}

void CitySurvivalManager::SendEnterAck(Player* player, uint16 errCode)
{
    ERROR_CODE ack;
    ack.errcode = errCode;
    player->SendToGate(MSG_CITY_SURVIVAL_ENTER_ACK, &ack);
}

bool CitySurvivalManager::IsActivityServer()
{
    //return DungeonManager::Instance()->GetServerIdByDungeonId(CITY_SURVIVAL_MAP) == ServerConfig::Instance()->GetMyID();
    return false;
}

void CitySurvivalManager::SendCitySurvivalInfoNtf()
{
    CITY_SURVIVAL_INFO_NTF ntf;
    BIGINT_ID playerId;

    SurvivalPlayerMap::iterator iter = m_player_map.begin();
    for (; iter != m_player_map.end(); ++iter)
    {
        playerId.Clear();
        playerId.id_h = U64ID_HIGH(iter->first);
        playerId.id_l = U64ID_LOW(iter->first);

        ntf.player_list.push_back(playerId);
    }

    CenterHandler::Instance()->SendToMasterCenter(MSG_CITY_SURVIVAL_INFO_NTF, &ntf);
}
