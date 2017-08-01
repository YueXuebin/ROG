#include "gameholder_pch.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "game_util.h"
#include "player.h"
#include "regular.h"
#include "world_boss_manager.h"
#include "city_fight_manager.h"
#include "server_config.h"
#include "setting_manager.h"
#include "center_handler_helper.h"
#include "global_setting_manager.h"
#include "proclamation_manager.h"
#include "guild_manager.h"
#include "activity_manager.h"
#include "player_manager.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "dungeon.h"
#include "trigger.h"
#include "event_define.h"
#include "player_prop_notifier.h"
#include "team_relationship.h"
#include "game_define.h"
#include "daily_fashion_manager.h"
#include "team_manager.h"


IMPLEMENT_SINGLETON(CenterHandler)

CenterHandler::CenterHandler()
{
    m_CenterHelper = CnNew CenterHandlerHelper(this);

    m_SecondTimer = 0;
}

CenterHandler::~CenterHandler()
{
    CnDelete m_CenterHelper;
    m_CenterHelper = NULL;

    Clear();
}

void CenterHandler::Clear()
{
    for(CenterConnecterList::iterator it = m_centerList.begin(); it != m_centerList.end(); ++it)
    {
        if(it->m_centerServer)
        {
            CnDelete it->m_centerServer;
            it->m_centerServer = 0;
        }
    }

    m_centerList.clear();
}

void CenterHandler::Update()
{
    m_SecondTimer += g_ServerUpdateTime;
    if(m_SecondTimer > 2000)
    {
        m_SecondTimer = 0;
        Update2Second();
    }

}

void CenterHandler::Update2Second()
{
    // 踢走要断开的Center上的所有玩家，确保安全退出
    for(uint32 i=0; i<m_DeleteIDs.size();)
    {
        uint32 id = m_DeleteIDs[i];
        CenterConnecter* pConnecter = GetConnecterByID(id);
        CnAssert(pConnecter);

        uint32 centerId = pConnecter->m_centerID;

        uint32 playerNum = PlayerManager::Instance()->GetPlayerNumByCenterID(centerId);     // 检查属于该Center的玩家都安全退出了
        CnDbg("delete center index=%d id=%d player num=%d\n", pConnecter->m_id, centerId, playerNum);
        if(playerNum == 0)
        {   // 可以安全断开CenterServer了
            _DisconnectCenterServerByID(id);
            m_DeleteIDs.erase(m_DeleteIDs.begin()+i);
        }
        else
        {
            i++;

            if(pConnecter->m_IsSeal)       // center封闭后,才开始安全退出
            {
                PlayerManager::Instance()->KickOutPlayerByCenterID(ERR_CHANGE_DUNGEON_SEAL, centerId);       // 踢该Center的玩家下线
            }
        }
    }
}

bool CenterHandler::Init()
{
    CenterConnecter connecter;
    const std::vector<ServerInfo>& centerList = ServerConfig::Instance()->GetCenterList();

    // 先复制配置信息
    for(std::vector<ServerInfo>::const_iterator it = centerList.begin(); it != centerList.end(); ++it)
    {
        connecter.Clear();
        connecter.m_id = m_centerList.size() + 1;   // 连接中的逻辑 ID，这个 ID 不需要配置
        connecter.m_ip = it->m_outer_ip;        // 连接用外网IP
        connecter.m_port = it->m_port;
        connecter.m_centerType = it->m_type;     
        //connecter.m_centerID = it->m_center_id;
        m_centerList.push_back(connecter);
    }

    if(!ConnectCenterServer())
    {
        goto _ERR;
    }

    SetState(kStateInit);

    return true;

_ERR:
    Clear();
    return false;
}

bool CenterHandler::ConnectCenterServer()
{
    // 开始创建连接
    for(CenterConnecterList::iterator it = m_centerList.begin(); it != m_centerList.end(); ++it)
    {
        // 中心服务器
        if(it->m_centerServer)
            continue;       // 该CenterServer已经有连接对象了

        if(it->m_centerType != CENTERSVR_TYPE_MASTER && ServerConfig::Instance()->GetMyType() != GAMESVR_TYPE_CROSS)
            continue;       // 只有跨服GameServer才连接从CenterServer

        CnAssert(it->m_id != 0);         // id!=0,以免影响指针判空
        CnAssert(it->m_centerID == 0);      // 此时应该没有被告知centerID
        it->m_centerServer = CreateTransferClientProxy(it->m_ip.c_str(), it->m_port, &RecvMsg, (void*)it->m_id);
        if(!it->m_centerServer)
        {
            CnWarn("create connect to center %s:%d faile\n", it->m_ip.c_str(), it->m_port);
            if(it->m_centerType == CENTERSVR_TYPE_MASTER)
            {
                CnError("connect master center faile\n");
                return false;
            }
        }
        it->m_centerServer->Connect(true);
        CnDbg("connect center type:%d %s:%d\n", it->m_centerType, it->m_ip.c_str(), it->m_port);
    }
    return true;
}

void CenterHandler::RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_NET_CONNECT:
        Instance()->OnConnect(sessionID, data, userdata);
        break;
    case MSG_NET_CLOSE:
        Instance()->OnClose(sessionID, data, userdata);
        break;
    case MSG_NET_ERROR:
        Instance()->OnError(sessionID, data, userdata);
        break;
    default:
        Instance()->OnRecv(playerID, sessionID, msgID, data, userdata);
        break;
    }
}

bool CenterHandler::SendToAll(uint32 msgID, void* data, uint64 playerID)
{
    CnAssert(false);            // 暂不此功能需求
    bool errCode = true;
    bool ret = false;

    for(CenterConnecterList::iterator centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        CenterConnecter* connecter = &(*centerIt);
        ret = _SendToCenter(connecter, msgID, data, playerID);

        if(errCode && !ret)
        {
            errCode = ret;
        }
    }

    return errCode;
}

bool CenterHandler::SendBySessionID(uint32 sessionID, uint32 msgID, void* data, uint64 playerID)
{
    CenterConnecter* connect = 0;

    connect = GetConnecterBySessionID(sessionID);
    if(!connect)
    {
        CnError("Can't send msg to session %u centerser\n", sessionID);
        CnAssert(false);
        return false;
    }
    return _SendToCenter(connect, msgID, data, playerID);
}

bool CenterHandler::SendToMasterCenter(uint32 msgID, void* data, Player* player)
{
    uint64 playerID = 0;
    if(player)
    {
        playerID = player->GetPlayerID();
    }

    return SendByCenterID(ServerConfig::Instance()->GetCenterID(), msgID, data, playerID);
}

bool CenterHandler::SendByPlayer(Player* player, uint32 msgID, void* data, uint64 playerID)
{
    uint32 CenterSvrID = player->GetCenterSvrID();
    if(CenterSvrID == 0)
    {
        CnAssert(false);
        return false;
    }

    return SendByCenterID(CenterSvrID, msgID, data, playerID);
}

bool CenterHandler::SendByID(uint32 id, uint32 msgID, void* data, uint64 playerID)
{
    CnAssert(id != 0);

    CenterConnecterList::iterator centerIt;
    for(centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        if(centerIt->m_id == id)
        {
            break;
        }
    }

    if(centerIt == m_centerList.end())
    {
        CnError("Can't send msg to centerser index=%d\n", id);
        CnAssert(false);
        return false;
    }

    CenterConnecter* connecter = &(*centerIt);
    return _SendToCenter(connecter, msgID, data, playerID);
}

bool CenterHandler::SendByCenterID(uint32 centerID, uint32 msgID, void* data, uint64 playerID)
{
    if(centerID == 0)
    {
        CnAssert(false);            // CenterServer已经断掉了
        return false;
    }

    CenterConnecterList::iterator centerIt;
    for(centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        if(centerIt->m_centerID == centerID)
        {
            break;
        }
    }

    if(centerIt == m_centerList.end())
    {
        CnError("Can't send msg to centerser %u\n", centerID);
        CnAssert(false);
        return false;
    }

    CenterConnecter* connecter = &(*centerIt);
    return _SendToCenter(connecter, msgID, data, playerID);
}

bool CenterHandler::_SendToCenter(CenterConnecter* pConnecter, uint32 msgID, void* data, uint64 playerID)
{
    CnAssert(pConnecter);

    if(pConnecter->m_state < HANDLER_STATE_CONNECTED)
    {   // CS没有连接
        CnError("Center centerID:%u not connected\n", pConnecter->m_centerID);
        return false;
    }

    return pConnecter->m_centerServer->Send(msgID, data, playerID);
}

void CenterHandler::OnConnect(uint32 sessionID, void* data, void* userdata)
{
    CenterConnecter* connect = NULL;
    uint32 id = 0;

    // 如果是空指针直接返回
    if(!userdata)
    {
        // 许愿传入用户参数，否则无法判断是哪个centerServer建立了连接
        CnError("Center server OnConnect userdata is null\n");
        CnAssert(false);
        return;
    }

    // 我知道传入是整形，所以直接强转了

    id = (uint32)(VOID_TO_UINT(userdata));

    connect = GetConnecterByID(id);
    if(!connect)
    {
        CnError("Can't find center connecter id:%u\n", id);
        CnAssert(false);
        return;
    }

    CnDbg("on connect center id=%d %s:%d\n", id, connect->m_ip.c_str(), connect->m_port);

    CnAssert(connect->m_sessionID == 0);
    connect->m_sessionID = sessionID;
    CnAssert(connect->m_state == HANDLER_STATE_INIT);
    connect->m_state = HANDLER_STATE_CONNECTED;

    // 如果是跨服的Game，需要先连接到主Center上，因为他监听Gate的信息是由MasterCenter发送过来的
    if(ServerConfig::Instance()->GetMyType() == GAMESVR_TYPE_CROSS)
    {
        // 主Center还未连接上，Game上没有Gate的监听信息，所以先不给连接上的WorkCenter发请求信息
        if((GetState() < kStateMasterActivateAck) && (connect->m_centerType != CENTERSVR_TYPE_MASTER))
        {
            return;
        }
    }

    SendServerInfoReq(connect);

    connect->m_state = HANDLER_STATE_SEND_INFO_REQ;
}

void CenterHandler::OnClose(uint32 sessionID, void* data, void* userdata)
{
    OnConnectClose(sessionID, NET_STATE_CLOSED, userdata);
}

void CenterHandler::OnError(uint32 sessionID, void* data, void* userdata)
{
    OnConnectClose(sessionID, NET_STATE_CLOSED, userdata);
}

void CenterHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    switch(msgID)
    {
    case MSG_SERVER_INFO_ACK:
        RecvServerInfoAck(sessionID, data);
        break;
    case MSG_SERVER_ACTIVE_ACK:
        RecvServerActiveAck(sessionID, data);
        break;
    case MSG_SERVER_ALL_READY:
        RecvServerReadyAck(data);
        break;
    case MSG_CROSS_SERVER_ADD:
        RecvCrossServerAdd(data);
        break;
    case MSG_CROSS_SERVER_REMOVE:
        RecvCrossServerRemove(data);
        break;
    case MSG_CROSS_SERVER_LIST_REQ:
        RecvCrossServerAsk(sessionID, data);
        break;
    case MSG_DELIVER_PLAYER_INFO_REQ:
        RecvDeliverPlayerInfoReq(playerID, data);
        break;
    case MSG_ENTER_DUNGEON_REQ:
        RecvEnterDungeonReq(playerID, sessionID, data);
        break;
    case MSG_EXIT_DUNGEON_REQ:
        RecvExitDungeonReq(playerID);
        break;
    case MSG_WRITEBACK_PLAYER_INFO_ACK:
        RecvWriteBackPlayerFirstInfoAck(playerID);
        break;
    case MSG_GAMESERVER_SEAL_ACK:
        RecvSealServerAck(sessionID, data);
        break;
    default:
        break;
    }

    m_CenterHelper->OnRecv(playerID, sessionID, msgID, data, userdata);
}

void CenterHandler::OnConnectClose(uint32 sessionID, uint32 netState, void* userdata)
{
    CenterConnecter* connect = NULL;
    uint32 id = 0;

    if(!userdata)
    {
        CnAssert(false);
        return;
    }

    id = (uint32)(VOID_TO_UINT(userdata));

    connect = GetConnecterByID(id);
    if(!connect)
    {
        CnError("ConnectClose can't find center connecter sessionID:%u\n", sessionID);
        CnAssert(false);
        return;
    }

    if(connect->m_state != HANDLER_STATE_INIT)
    {
        CnInfo("disconnect center id=%d %s:%d\n", connect->m_id, connect->m_ip.c_str(), connect->m_port);
    }
    else
    {
        CnAssert(connect->m_sessionID == 0);
        CnDbg("disconnect center id=%d %s:%d again\n", connect->m_id, connect->m_ip.c_str(), connect->m_port);
        return;
    }

    // 如果是主服务器，则需要设置配置中的主服务器ID
    if(connect->m_centerType == CENTERSVR_TYPE_MASTER)
    {
        CnAssert(connect->m_id == id);
        CnAssert(ServerConfig::Instance()->GetCenterID() != 0);
        CnAssert(connect->m_centerID == ServerConfig::Instance()->GetCenterID());
        ServerConfig::Instance()->SetCenterID(0);
    }

    connect->m_state = HANDLER_STATE_INIT;
    connect->m_sessionID = 0;
    connect->m_closedtime = Crown::SDGetTickCount();

    if(connect->m_centerID)
    {
        PlayerManager::Instance()->OnCloseCenterServer(connect->m_centerID);
        OnCloseCenterServer(connect->m_centerID);

        connect->m_centerID = 0;
    }
}

// ==================================================
// 获得中心服务器配置
// ==================================================
CenterConnecter* CenterHandler::GetConnecterByID(uint32 id)
{
    CenterConnecterList::iterator centerIt;

    for(centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        if(centerIt->m_id == id)
        {
            break;
        }
    }

    if(centerIt != m_centerList.end())
    {
        return &(*centerIt);
    }

    return NULL;
}

// ==================================================
// 获得中心服务器配置
// ==================================================
CenterConnecter* CenterHandler::GetConnecterBySessionID(uint32 sessionID)
{
    CenterConnecterList::iterator centerIt;

    for(centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        if(centerIt->m_sessionID == sessionID)
        {
            break;
        }
    }

    if(centerIt != m_centerList.end())
    {
        return &(*centerIt);
    }

    return NULL;
}

CenterConnecter* CenterHandler::GetConnecterByCenterID(uint32 centerID)
{
    CenterConnecterList::iterator centerIt;

    for(centerIt = m_centerList.begin(); centerIt != m_centerList.end(); ++centerIt)
    {
        if(centerIt->m_centerID == centerID)
        {
            break;
        }
    }

    if(centerIt != m_centerList.end())
    {
        return &(*centerIt);
    }

    return NULL;
}

bool CenterHandler::IsMasterCenterBySessionID(uint32 sessionID)
{
    CenterConnecter* connecter = GetConnecterBySessionID(sessionID);
    if(!connecter)
        return false;

    if(connecter->m_centerType == CENTERSVR_TYPE_MASTER)
        return true;

    return false;
}

// ==================================================
// 设置中心服务器的区号
// ==================================================
void CenterHandler::SetCenterID(uint32 sessionID, uint32 centerID)
{
    CenterConnecter* connect = GetConnecterBySessionID(sessionID);
    if(!connect)
    {
        CnError("SetCenterRegionID can't find center connecter sessionID:%u\n", sessionID);
        CnAssert(false);
        return;
    }

    // 如果是主服务器，则需要设置配置中的主服务器ID
    if(connect->m_centerType == CENTERSVR_TYPE_MASTER)
    {
        CnAssert(ServerConfig::Instance()->GetCenterID() == 0);
        ServerConfig::Instance()->SetCenterID(centerID);
        CnAssert(ServerConfig::Instance()->GetCenterID() != 0);
    }

    connect->m_centerID = centerID;
}

void CenterHandler::OnCloseCenterServer(uint32 centerID)
{
    GuildManager::Instance()->OnCloseCenterServer(centerID);
}

// ==================================================
// 发送请求服务器信息
// ==================================================
void CenterHandler::SendServerInfoReq(CenterConnecter* connecter)
{
    if(connecter->m_centerType == CENTERSVR_TYPE_MASTER)
    {
        CnAssert(ServerConfig::Instance()->GetCenterID() == 0);
    }
    else if(connecter->m_centerType == CENTERSVR_TYPE_WORKER)
    {
        CnAssert(ServerConfig::Instance()->GetCenterID() != 0);
    }
    else
        CnAssert(false);

    SERVER_INFO_REQ sendMsg;
    sendMsg.server_type = SERVER_TYPE_GAME;
    sendMsg.server_id = ServerConfig::Instance()->GetMyID();
    sendMsg.game_type = ServerConfig::Instance()->GetMyType();
    sendMsg.server_port= ServerConfig::Instance()->GetListenGatePort();
    sendMsg.server_ip = ServerConfig::Instance()->GetListenGateIP();
    sendMsg.server_outer_ip = ServerConfig::Instance()->GetListenGateOuterIP();
    sendMsg.version = VERSION_FULL;
    sendMsg.center_id = ServerConfig::Instance()->GetCenterID();

    CnDbg("SendServerInfoReq to %s:%d info=%s(%s):%d\n", connecter->m_ip.c_str(), connecter->m_port, sendMsg.server_ip.c_str(), sendMsg.server_outer_ip.c_str(), sendMsg.server_port);

    SendBySessionID(connecter->m_sessionID, MSG_SERVER_INFO_REQ, &sendMsg);
}

// ==================================================
// 接收服务器信息
// ==================================================
void CenterHandler::RecvServerInfoAck(uint32 sessionID, void* data)
{
    SERVER_INFO_ACK* recvMsg = (SERVER_INFO_ACK*)data;
    CenterConnecter* connect = NULL;

    if(!recvMsg) 
        return;

    CnDbg("RecvServerInfoAck %s(%s):%d\n", recvMsg->server_ip.c_str(), recvMsg->server_outer_ip.c_str(), recvMsg->server_port);

    // 查找是否有这个会话连接
    connect = GetConnecterBySessionID(sessionID);
    if(!connect)
    {
        CnError("%s can't find center connect by sessionID:%u\n", __FUNCTION__, sessionID);
        return;
    }

    CnDbg("%s from connect %s:%d\n", __FUNCTION__, connect->m_ip.c_str(), connect->m_port);

    CnAssert(connect->m_state == HANDLER_STATE_SEND_INFO_REQ);      // 必然经过此状态

    if(recvMsg->errcode != ERR_SUCCEED)
    {
        if(recvMsg->errcode == ERR_VERSION_ERROR)
        {   // 版本错误
            if(connect->m_centerType == CENTERSVR_TYPE_MASTER)
            {
                CnError("master center version failed\n");
                CnExit();
            }
            else
            {
                CnDbg("slave center version failed\n");
            }
        }
        // 如果是主Server，可能是因为未找到配置项
        else if(connect->m_centerType == CENTERSVR_TYPE_MASTER)
        {   // 如果是主Server，可能是配置错误
            CnError("Get gs info from master cs failed\n");
            CnExit();
        }
        else
        {
            CnError("Get gs info from worker cs failed\n");
        }

        connect->m_state = HANDLER_STATE_CONNECTED;
        return;
    }

    connect->m_state = HANDLER_STATE_SEND_INFO_ACK;

    // CenterID不能重复
    CenterConnecter* sameCenter = GetConnecterByCenterID(recvMsg->center_id);
    if(sameCenter)
    {
        CnError("active center id=%d old %s:%d duplicate\n", recvMsg->center_id, sameCenter->m_ip.c_str(), sameCenter->m_port );
        CnError("active center id=%d new %s:%d duplicate\n", recvMsg->center_id, connect->m_ip.c_str(), connect->m_port );
        return;
    }

    // 设置中心服务器
    connect->m_centerID = recvMsg->center_id;

    // 检查是否为主Center
    if(connect->m_centerType == CENTERSVR_TYPE_MASTER)
    {
        CnAssert(recvMsg->center_id != 0);
        CnAssert(ServerConfig::Instance()->GetCenterID() == 0);
        if(GateHandler::Instance()->IsValid())
        {   // 对Gate的监听端口已经创建过了,主Center重启时会执行
            CnAssert(GateHandler::Instance()->GetListenIp() == recvMsg->server_ip);
            CnAssert(GateHandler::Instance()->GetListenPort() == recvMsg->server_port);
        }
        else if(!GateHandler::Instance()->Init(recvMsg->server_ip.c_str(), recvMsg->server_port))
        {
            CnError("Create gate listener failed: %s, %u\n", recvMsg->server_ip.c_str(), recvMsg->server_port);
            return;
        }

        ServerConfig::Instance()->SetListenGatePort(recvMsg->server_port);
        ServerConfig::Instance()->SetListenGateIP(recvMsg->server_ip);
        ServerConfig::Instance()->SetListenGateOuterIP(recvMsg->server_outer_ip);
    }

    // 请求激活服务器
    CnDbg("SendServerActiveReq %s:%d\n", connect->m_ip.c_str(), connect->m_port);
    SendServerActiveReq(sessionID, connect->m_centerType);

    // 设置发送激活状态
    connect->m_state = HANDLER_STATE_SEND_ACTIATE_REQ;
}

// ==================================================
// 发送请求服务器激活
// ==================================================
void CenterHandler::SendServerActiveReq(uint32 sessionID, uint8 centerType)
{
    if(centerType == CENTERSVR_TYPE_MASTER)
    {
        CnAssert(ServerConfig::Instance()->GetCenterID() == 0);
    }

    SERVER_ACTIVE_REQ sendMsg;
    sendMsg.server_type = SERVER_TYPE_GAME;
    sendMsg.server_id = ServerConfig::Instance()->GetMyID();
    sendMsg.center_id = ServerConfig::Instance()->GetCenterID();
    SendBySessionID(sessionID, MSG_SERVER_ACTIVE_REQ, &sendMsg);
}

// ==================================================
// 接收服务器激活
// ==================================================
void CenterHandler::RecvServerActiveAck(uint32 sessionID, void* data)
{
    SERVER_ACTIVE_ACK* recvMsg = (SERVER_ACTIVE_ACK*)data;
    if(!recvMsg) return;

    CenterConnecter* connect = NULL;

    if(!recvMsg) return;

    // 查找是否有这个会话连接
    connect = GetConnecterBySessionID(sessionID);
    if(!connect)
    {
        CnError("%s can't find center connect by sessionID:%u\n", __FUNCTION__, sessionID);
        return;
    }

    CnDbg("%s %s:%d\n", __FUNCTION__, connect->m_ip.c_str(), connect->m_port);

    CnAssert(connect->m_state == HANDLER_STATE_SEND_ACTIATE_REQ);       // 必经过此状态
    connect->m_state = HANDLER_STATE_SEND_ACTIATE_ACK;

    if(recvMsg->errcode != ERR_SUCCEED)
    {
        CnDbg("server active error=%d : %s:%d\n", recvMsg->errcode, connect->m_ip.c_str(), connect->m_port);
        switch(recvMsg->errcode)
        {
        case ERR_SERVER_ACTIVATED:
            CnFatal("Active server duplicate activated\n");
            break;
        case ERR_UNFOUND_CONFIG:
            CnFatal("Active server unfound config\n");
            break;
        default:
            CnFatal("Active server failed\n");
            break;
        }
        CnExit();
    }

    SetCenterID(sessionID, recvMsg->center_id);

    // 如果是跨服的Game，需要先连接到主Center上，因为他监听Gate的信息是由主Center发送过来的
    if(ServerConfig::Instance()->GetMyType() == GAMESVR_TYPE_CROSS)
    {
        if((GetState() < kStateMasterActivateAck) && (connect->m_centerType == CENTERSVR_TYPE_MASTER))
        {
            SetState(kStateMasterActivateAck);
            // 开始给WorkCenter发送信息请求
            SendInfoReqToWorkCenter();
        }
    }
    else
    {
        SetState(kStateMasterActivateAck);
    }

    CnInfo("Active server succeed\n");

    // 发送这个 GameServer 开启副本的ID号给 CenterServer
    CnDbg("send dungeon list to %s:%d\n", connect->m_ip.c_str(), connect->m_port);
    DungeonManager::Instance()->SendServerDungeonListNtf(sessionID);
}

// ==================================================
// 给从Center服务器发送请求信息
// ==================================================
void CenterHandler::SendInfoReqToWorkCenter()
{
    for (CenterConnecterList::iterator it = m_centerList.begin(); it != m_centerList.end(); ++it)
    {
        if(it->m_centerType == CENTERSVR_TYPE_MASTER)
            continue;
        if(it->m_state == HANDLER_STATE_INIT)
        {
            CnAssert(it->m_sessionID == 0);
            continue;
        }

        SendServerInfoReq(&(*it));

        CnAssert(it->m_state == HANDLER_STATE_CONNECTED);
        it->m_state = HANDLER_STATE_SEND_INFO_REQ;
    }
}

void CenterHandler::RecvServerReadyAck(void* data)
{
    /* WorldBossManager::Instance()->OnServerReady();

    ActivityManager::Instance()->OnServerReady();

    CityFightManager::Instance()->OnServerReady();

    GlobalSettingManager::Instance()->OnServerReady();

    ProclamationManager::Instance()->OnServerReady();

    DailyFashionManager::Instance()->OnServerReady();*/
    CnInfo("ready\n");
}

void CenterHandler::RecvSealServerAck(uint32 sessionID, void* data)
{
    CenterConnecter* pConnecter = GetConnecterBySessionID(sessionID);
    if(!pConnecter)
    {
        CnDbg("seal center sessionid=%d no exist\n", sessionID);
        return;
    }

    CnInfo("sealed center index=%d id=%d %s:%d\n", pConnecter->m_id, pConnecter->m_centerID, pConnecter->m_ip.c_str(), pConnecter->m_port);
    pConnecter->m_IsSeal = true;
}

void CenterHandler::RecvCrossServerAdd(void* data)
{
    SERVER_INFO* serverInfo = (SERVER_INFO*)data;
    if(!serverInfo) 
        return;

    // 检查添加的CenterServer不应在删除列表中
    for(uint32 i=0; i<m_DeleteIDs.size(); i++)
    {
        uint32 delID = m_DeleteIDs[i];
        CenterConnecter* connecter = GetConnecterByID(delID);
        CnAssert(connecter);
        if(connecter->m_ip == serverInfo->server_ip && connecter->m_port == serverInfo->server_port)
        {
            CnDbg("add CrossServer is Delete %s:%d\n", connecter->m_ip.c_str(), connecter->m_port);
            return;
        }
    }

    // 检查添加的CenterServer是否有重复
    bool isDuplicate = false;
    for(uint32 i=0; i<m_centerList.size(); i++)
    {
        CenterConnecter& testConnecter = m_centerList[i];
        if(serverInfo->server_ip == testConnecter.m_ip && serverInfo->server_port == testConnecter.m_port)
        {
            isDuplicate = true;
            CnWarn("add CrossServer duplicate %s:%d\n", testConnecter.m_ip.c_str(), testConnecter.m_port);
        }
    }

    if(isDuplicate)
        return;

    CenterConnecter connecter;
    connecter.m_id = FindValidID();   // 连接中的逻辑 ID，这个 ID 不需要配置
    connecter.m_ip = serverInfo->server_ip;
    connecter.m_port = serverInfo->server_port;
    connecter.m_centerType = CENTERSVR_TYPE_WORKER;
    m_centerList.push_back(connecter);

    CnDbg("add cross center index=%d %s:%d\n", connecter.m_id, connecter.m_ip.c_str(), connecter.m_port);

    // 连接新CenterServer
    ConnectCenterServer();
}

void CenterHandler::RecvCrossServerRemove(void* data)
{
    SERVER_INFO* serverInfo = (SERVER_INFO*)data;
    if(!serverInfo) 
        return;

    // 检查删除的CenterServer不应在删除列表中
    for(uint32 i=0; i<m_DeleteIDs.size(); i++)
    {
        uint32 delID = m_DeleteIDs[i];
        CenterConnecter* connecter = GetConnecterByID(delID);
        CnAssert(connecter);
        if(connecter->m_ip == serverInfo->server_ip && connecter->m_port == serverInfo->server_port)
        {
            CnDbg("remove CrossServer is Delete %s:%d\n", connecter->m_ip.c_str(), connecter->m_port);
            return;
        }
    }

    // 查找该ip:port对应的CenterID
    int32 remove_index = -1;
    for(uint32 i=0; i<m_centerList.size(); i++)
    {
        CenterConnecter& testConnecter = m_centerList[i];
        if(serverInfo->server_ip == testConnecter.m_ip && serverInfo->server_port == testConnecter.m_port)
        {
            if(testConnecter.m_centerType == CENTERSVR_TYPE_MASTER)
            {
                CnDbg("can't remove master Center index=%d %s:%d\n", testConnecter.m_id, testConnecter.m_ip.c_str(), testConnecter.m_port);
                continue;
            }

            remove_index = i;
            break;
        }
    }

    if(remove_index < 0)
    {
        CnDbg("remove CrossServer %s:%d no exist\n", serverInfo->server_ip.c_str(), serverInfo->server_port);
        return;
    }

    CenterConnecter& removeConnecter = m_centerList[remove_index];

    CnDbg("remove cross center index=%d %s:%d\n", removeConnecter.m_id, removeConnecter.m_ip.c_str(), removeConnecter.m_port);

    SafeDeleteCenterServerByID(removeConnecter.m_id);
}

void CenterHandler::RecvCrossServerAsk(uint32 sessionID, void* data)
{
    if(ServerConfig::Instance()->GetMyType() != GAMESVR_TYPE_CROSS)
    {
        CnWarn("cross server ask error, not to cross server\n");
    }

    if(!IsMasterCenterBySessionID(sessionID))
    {
        CnWarn("cross server ask error, not from master center\n");
    }

    SERVER_LIST_ACK serverInfoList;
    for(uint32 i=0; i<m_centerList.size(); i++)
    {
        CenterConnecter& connecter = m_centerList[i];
        if(!connecter.m_centerServer)
            continue;       // 未创建连接对象的不算

        SERVER_INFO sInfo;
        sInfo.server_ip = connecter.m_ip;
        sInfo.server_port = connecter.m_port;
        sInfo.center_id = connecter.m_centerID;
        serverInfoList.list.push_back(sInfo);
    }

    SendBySessionID(sessionID, MSG_CROSS_SERVER_LIST_ACK, &serverInfoList);
}

int32 CenterHandler::FindValidID()
{
    int32 maxID = 1;
    for(uint32 i=0; i<m_centerList.size(); i++)
    {
        CenterConnecter& connecter = m_centerList[i];
        maxID = Crown::SDMax((int32)connecter.m_id, maxID);
    }
    return maxID+1;
}

// ==================================================
// 收到角色的基本信息
// ==================================================
void CenterHandler::RecvDeliverPlayerInfoReq(uint64 playerID, void* data)
{
    DELIVER_PLAYER_INFO_REQ* recvMsg = (DELIVER_PLAYER_INFO_REQ*)data;

    Player* player = CnNew Player;
    uint16 errCode = ERR_SUCCEED;

    if(!recvMsg)
    {
        CnAssert(false);
        return;
    }

    player->PlayerClear();          // 初始化角色数据

    // 加载角色数据
    player->Load(playerID, recvMsg);

    // 初始角色初始设置
    player->InitNewRoleDefaultSet();

    // 计算每日刷新值
    player->CalDailyRefresh();

    // 每天8点刷新
    player->OnTimeFresh();

    // 计算角色属性
    player->CalcProperty();

    // 角色属性计算完成后，再设置hp和mp
    player->SetHP(recvMsg->prop_info.hp);
    player->SetMP(recvMsg->prop_info.mp);

    // 所有加载、计算等都做完后要设置的一些东西。比如佣兵的血量
    player->OnLastSet();

    if(!PlayerManager::Instance()->AddPlayer(player))
    {
        CnDelete player;
        errCode = ERR_FAILED;
        goto _END;
    }

_END:
    SendDeliverPlayerFirstInfoAck(recvMsg->prop_info.centersvr_id, errCode, playerID);
}

void CenterHandler::FillPlayerInfo(Player* player, WRITEBACK_PLAYER_INFO_REQ& sendMsg)
{
    sendMsg.Clear();

    player->Save(sendMsg);
}

// ==================================================
// 和Center同步角色信息
// ==================================================
void CenterHandler::SendWriteBackPlayerInfoReq(Player* player)
{
    CnAssert(player);
    if(!player) return;

    // 玩家下线后不应该在序章关卡中
    //if(player->GetScene() && player->GetScene()->GetRegular() && player->GetScene()->GetRegular()->GetType() == REGULAR_TYPE_TEASER)
    //    CnAssert(false);

    WRITEBACK_PLAYER_INFO_REQ sendMsg;
    FillPlayerInfo(player, sendMsg);

    //CnDbg("SendWriteBackPlayerInfoReq %s\n", player->GetName().c_str());

    SendByCenterID(player->GetCenterSvrID(), MSG_WRITEBACK_PLAYER_INFO_REQ, &sendMsg, player->GetPlayerID());
}

// ==================================================
// 自动回存角色信息
// ==================================================
void CenterHandler::AutoSavePlayerInfoReq(Player* player)
{
    CnAssert(player);
    CnAssert(player->GetNetState() == Player::kStateInGame);
    if(!player)
        return;

    // 如果玩家在序章里不回存玩家数据
    //if(player->GetScene() && player->GetScene()->GetRegular() && player->GetScene()->GetRegular()->GetType() == REGULAR_TYPE_TEASER)
    //    return;

    WRITEBACK_PLAYER_INFO_REQ sendMsg;
    FillPlayerInfo(player, sendMsg);

    SendByCenterID(player->GetCenterSvrID(), MSG_AUTO_SAVE_PLAYER_INFO_REQ, &sendMsg, player->GetPlayerID());
}
// ==================================================
// 确认角色的基本信息
// ==================================================
void CenterHandler::SendDeliverPlayerFirstInfoAck(uint32 regionID, uint16 errCode, uint64 playerID)
{
    PLAYER_BASE_INFO_ACK sendMsg;
    sendMsg.errcode = errCode;
    SendByCenterID(regionID, MSG_DELIVER_PLAYER_INFO_ACK, &sendMsg, playerID);
}

// ==================================================
// 请求进入场景信息
// ==================================================
void CenterHandler::RecvEnterDungeonReq(uint64 playerID, uint32 sessionID, void* data)
{
    ENTER_DUNGEON_REQ* recvMsg = (ENTER_DUNGEON_REQ*)data;
    if(!recvMsg) return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {   // 遇到这种情况，直接将这个祸害从center上干掉
        KICKOUT_GAME kickout;
        kickout.errcode = ERR_PLAYER_NOT_EXIT;
        CenterHandler::Instance()->SendBySessionID(sessionID, MSG_GS_KICKOUT_GAME, &kickout, playerID);
        return;
    }

    if(recvMsg->jump_type != DUNGEON_JUMP_TRIGGER
        && recvMsg->jump_type != DUNGEON_JUMP_TEAM
        && recvMsg->jump_type != DUNGEON_JUMP_POS
        && recvMsg->jump_type != DUNGEON_JUMP_PLAYER)
    {
        CnAssert(false);        // 跳副本类型错误
        recvMsg->jump_type = DUNGEON_JUMP_TRIGGER;
    }

    // 从之前的副本中删除角色
    if(recvMsg->is_sameserver)
    {
        Scene* scene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
        if(scene)
        {
            // 大地图跳转地图前保存城市地图的X,Y
            Dungeon* dungeon = scene->GetDungeon();
            if(IsWorldMap(dungeon->GetDungeonID()))
            {
                player->SetCityX(player->GetX());
                player->SetCityY(player->GetY());
            }
            scene->DelPlayer(player->GetId());
        }
    }

    // 查找副本集
    DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(recvMsg->dungeon_id);
    if(!pDungeonSet)
    {
        CnAssert(false);        // 进入的副本集不存在
        PlayerManager::Instance()->KickOutPlayer(ERR_CHANGE_DUNGEON_NO_DUNGEON, playerID);
        return;
    }

    // 判断进入该副本的哪个频道
    uint32 union_id = 0;
    if(recvMsg->jump_type == DUNGEON_JUMP_TEAM)
    {   
        union_id = UNION_ID(recvMsg->channel_id, recvMsg->dungeon_id, recvMsg->scene_id);
        if(!DungeonManager::Instance()->GetScene(union_id))
            union_id = 0;           // 要飞去的场景不存在

        // 进入队友所在频道
        if (recvMsg->channel_id == 0)
        {
            recvMsg->channel_id = TeamManager::Instance()->GetTeamChannelIDByPlayer(player, recvMsg->dungeon_id);
        }
    }

    if(!union_id)
        union_id = pDungeonSet->AskEnterUnionID(player, recvMsg->scene_id, recvMsg->channel_id);

    Scene* pScene = DungeonManager::Instance()->GetScene(union_id);
    if(!pScene)
    {
        CnAssert(false);    // 进入的地图不存在
        // 如果实在发生了，则踢出该玩家
        PlayerManager::Instance()->KickOutPlayer(ERR_CHANGE_DUNGEON_NO_DUNGEON, playerID);
        return;
    }

    Dungeon* dungeon = pScene->GetDungeon();

    // 设置角色的地图和坐标,当保存的时候回写
    // ==================================================
    player->SetUnionMapID(pScene->GetUnionID());

    bool isPlayerSetPosition = false;           // 是否设置了位置
    // 有跳转trigger点时，设置trigger点为坐标
    if(recvMsg->jump_type == DUNGEON_JUMP_TRIGGER || recvMsg->jump_type == DUNGEON_JUMP_TEAM)
    {
        // 获得目标位置
        Trigger* pTargetTirgger = pScene->GetTriggerByID(recvMsg->object_id);
        if(pTargetTirgger)
        {
            int32 newX = pTargetTirgger->GetX();
            int32 newY = pTargetTirgger->GetY();
            int32 newAngle = pTargetTirgger->GetAngle();
            player->SetX(newX);
            player->SetY(newY);
            player->SetAngle(newAngle);
            isPlayerSetPosition = true;
        }
    }
    else if(recvMsg->jump_type == DUNGEON_JUMP_POS)
    {
        int32 newX = recvMsg->pos_x;
        int32 newY = recvMsg->pos_y;
        CoVec2 newPos = pScene->FindValidPositionInCircle(CoVec2(newX, newY), g_TeamFlyNearDistance, player->GetRadius());
        player->SetX(int32(newPos.x));
        player->SetY(int32(newPos.y));
        isPlayerSetPosition = true;
    }
    else if(recvMsg->jump_type == DUNGEON_JUMP_PLAYER)
    {
        Player* flyPlayer = pScene->GetPlayerByPlayerId(U64ID(recvMsg->fly_player_id.id_h, recvMsg->fly_player_id.id_l));
        if(flyPlayer)
        {
            int32 newX = flyPlayer->GetX();
            int32 newY = flyPlayer->GetY();
            CoVec2 newPos = pScene->FindValidPositionInCircle(CoVec2(newX, newY), g_TeamFlyNearDistance, player->GetRadius());
            int32 newAngle = flyPlayer->GetAngle();
            player->SetX(int32(newPos.x));
            player->SetY(int32(newPos.y));
            player->SetAngle(newAngle);
            isPlayerSetPosition = true;
        }
    }

    if(!isPlayerSetPosition)
    {
        if(IsWorldMap(dungeon->GetDungeonID()))
        {
            player->SetX(player->GetCityX());
            player->SetY(player->GetCityY());
        }
        else
        {
            pScene->GetRegular()->FillStartPostion(player);
        }
    }

    // 如果新进入的地图为城镇，则保存地图ID和X,Y
    SavePlayerDungeonPos(dungeon, player);

    // 根据地图跳转次数判断角色是否为第一次登陆服务器
    // 第一次进入地图需要向客户端发送角色信息，其他情况则为跳转地图
    bool isFirstEnterGame = (player->GetChangeDungeonCount() == 0);
    if(isFirstEnterGame)
    {
        player->SendPlayerInfo(pScene);                         // 通知客户端，角色登陆成功
    }
    else
    {
        GateHandler::Instance()->SendChangeDungeonAck(ERR_SUCCEED, player, pScene);     // 通知客户端，跳地图成功
    }

    // 设置角色的状态为进入游戏
    CnAssert((player->GetNetState() == Player::kStateInit) || 
        (player->GetNetState() == Player::kStateInGame) || 
        (player->GetNetState() == Player::kStateJoinTeam));
    player->SetNetState(Player::kStateInGame);

    // 最后执行Add
    pScene->AddPlayer(player);

    // 成功进入副本相关处理
    player->OnEnterDungeon(dungeon, recvMsg->dungeon_id);

    // 对角色的跳转副本计数累加
    player->SetChangeDungeonCount(player->GetChangeDungeonCount() + 1);

    // 玩家队伍信息通知CenterServer
    player->SendToCenterTeamMemberProp();

    // 更新组队关系
    TeamRelationShip::Instance()->OnPlayerChangeDungeon(player);


    ////////////////////////////////////////////////////////////
    // 第一次进入游戏处理
    if(isFirstEnterGame)
    {
        CnDbg("player %s first enter game\n", player->GetName().c_str());
        if(player->GetGuildID())
        {
            player->SendTaskEvent(TASK_EVENT_JOIN_GUILD, 0, 0);
        }

        // 第一次加载需要计算充值任务
        //player->SendRechargeEvent();

        player->FirstEnterGame();

        NONE msg;
        player->SendToGate(MSG_PLAYER_ENTER_GAME, &msg);            // 通知客户端，玩家进入游戏
    }
}
// ==================================================
// 请求退出场景
// ==================================================
void CenterHandler::RecvExitDungeonReq(uint64 playerID)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    if((player->GetNetState() == Player::kStateInGame) || (player->GetNetState() == Player::kStateJoinTeam))
    {
        CnAssert(player->GetScene());
        Scene* pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
        if(pScene)
        {
            // 跳转地图前保存城市地图的X,Y
            Dungeon* dungeon = pScene->GetDungeon();
            if (IsWorldMap(dungeon->GetDungeonID()))
            {
                player->SetCityX(player->GetX());
                player->SetCityY(player->GetY());
            }

            // 从场景中删除角色
            pScene->DelPlayer(player->GetId());
        }
        else
            CnAssert(false);
    }
    else
    {
        CnAssert(player->GetScene() == NULL);       // 角色还未进入场景，就要退出场景了。因此 GetScene==NULL
    }

    if(player->GetNetState() == Player::kStateSyncData)
    {   // 玩家已经进入回存流程了
        CnDbg("player %s is already save\n", player->GetName().c_str());
    }
    else
    {
        player->SetNetState(Player::kStateSyncData);
        SendWriteBackPlayerInfoReq(player);
    }
}

// 记录角色的副本和位置
void CenterHandler::SavePlayerDungeonPos(Dungeon* dungeon, Player* player)
{
    if(!dungeon || !player)
    {
        CnAssert(false);
        return;
    }
    // 如果新进入的地图为城镇，则保存地图ID和X,Y
    if(IsWorldMap(dungeon->GetDungeonID()))
    {
        player->SetCityMapID(player->GetUnionMapID());
        player->SetCityX(player->GetX());
        player->SetCityY(player->GetY());
    }
}

// ==================================================
// 收到角色的基本信息
// ==================================================
void CenterHandler::RecvWriteBackPlayerFirstInfoAck(uint64 playerID)
{
    // 查找角色
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    PlayerManager::Instance()->DelPlayer(playerID);
}

void CenterHandler::TellCenterSealGame(uint32 id)
{
    NONE none;
    SendByID(id, MSG_GAMESERVER_SEAL_REQ, &none);
}

void CenterHandler::SafeDeleteAllCenterServer()
{
    for(CenterConnecterList::iterator iter = m_centerList.begin(); iter != m_centerList.end(); iter++)
    {
        CenterConnecter& connecter = *iter;
        SafeDeleteCenterServerByID(connecter.m_id);
    }
}

void CenterHandler::SafeDeleteCenterServerByID(uint32 id)
{
    CenterConnecter* connecter = GetConnecterByID(id);
    if(!connecter)
    {
        CnDbg("delete center index=%d not exist\n", id);
        return;
    }

    for(uint32 i=0; i<m_DeleteIDs.size(); i++)
    {
        if(m_DeleteIDs[i] == connecter->m_id)
        {
            CnWarn("already delete center index=%d\n", connecter->m_id);
            return;
        }
    }

    m_DeleteIDs.push_back(id);

    if(connecter->m_centerID > 0)
        TellCenterSealGame(id);         // 通知Center封闭该Game
}

void CenterHandler::_DisconnectCenterServerByID(uint32 ID)
{
    CenterConnecter* pConnecter = NULL;
    for(CenterConnecterList::iterator iter = m_centerList.begin(); iter != m_centerList.end(); iter++)
    {
        CenterConnecter& connecter = *iter;
        if(connecter.m_id == ID)
        {
            pConnecter = &connecter;
            break;
        }
    }

    if(!pConnecter)
    {
        CnDbg("disconnect center index=%d not exist\n", ID);
        return;
    }

    if(!pConnecter->m_centerServer)
    {
        CnDbg("disconnect center index=%d not connected\n", pConnecter->m_id);
        return;
    }

    CnDbg("delete center index=%d id=%d %s:%d\n", pConnecter->m_id, pConnecter->m_centerID, pConnecter->m_ip.c_str(), pConnecter->m_port);
    pConnecter->m_centerServer->Close();

    DestoryTransferClientProxy(pConnecter->m_centerServer);
    pConnecter->m_centerServer = NULL;

    // 断开对应的Gate
    if(pConnecter->m_centerID > 0)
        GateHandler::Instance()->Close(pConnecter->m_centerID);

    // 从连接列表中删除
    for(CenterConnecterList::iterator iter = m_centerList.begin(); iter != m_centerList.end(); iter++)
    {
        if(iter->m_id == ID)
        {
            m_centerList.erase(iter);
            break;
        }
    }
}
