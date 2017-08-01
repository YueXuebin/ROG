#include "server_info_pch.h"
#include "server_info_handler.h"
#include "server_info.h"

extern bool IsRunning;

IMPLEMENT_SINGLETON(ServerInfoHandler)

void OnNetRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    ((ServerInfoHandler*)userdata)->OnRecv(playerID, sessionID, msgID, data);
}

ServerInfoHandler::ServerInfoHandler()
{
    m_state = NET_STATE_INIT;
    m_closedtime = 0;
    m_pInfoClient = NULL;
}

ServerInfoHandler::~ServerInfoHandler()
{
    if (m_pInfoClient)
    {
        CnDelete m_pInfoClient;
        m_pInfoClient = NULL;
    }
}

bool ServerInfoHandler::Init(const char* ip, uint32 port)
{
    m_pInfoClient = CreateTransferClientProxy(ip, port, OnNetRecv, this);
    if(!m_pInfoClient)
    {
        return false;
    }
    return true;
}

void ServerInfoHandler::Update()          ///< 检查状态
{
    TransferUnpdate();
}

void ServerInfoHandler::OnAccept(uint32 sessionID, void* data)
{
        CnDbg("Accept from gateserver\n");
}
//连接
void ServerInfoHandler::OnConnect(uint32 sessionID, void* data)
{

}
//关闭网络
void ServerInfoHandler::OnClose(uint32 sessionID, void* data)
{

}
//错误
void ServerInfoHandler::OnError(uint32 sessionID, void* data)
{

}
//消息处理
void ServerInfoHandler::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_NET_CONNECT:
        {
            int32 i = ServerInfo::Instance()->GetOption();
            switch (i)
            {
            case kNumber:
            case kInformation:
            case kLevelPlayNumber:
            case kPlayerDungeon:
                {
                    MONITOR_INFO_REQ req;
                    req.test = 10;
                    m_pInfoClient->Send(MSG_MONITOR_INFO_REQ, &req);
                }
                break;
            case kDungeonInfo:
                {
                    NONE noneReq;
                    m_pInfoClient->Send(MSG_MONITOR_DUNGEON_INFO_REQ, &noneReq);
                }
                break;
            default:
                break;
            }
        }
        break;
    case MSG_NET_CLOSE:
    case MSG_NET_ERROR:
        CnInfo("connect failed\n");
        IsRunning = false;
        break;
    case MSG_MONITOR_INFO_ACK:
        {
            //centerserver信息处理
            MONITOR_INFO_ACK* Center_Info = (MONITOR_INFO_ACK*) data;
            Print(Center_Info);

            IsRunning = false;
        }
        break;
    case MSG_MONITOR_DUNGEON_INFO_ACK:
        {
            MONITOR_DUNGEON_INFO_ACK* dungeonInfo = (MONITOR_DUNGEON_INFO_ACK*)data;
            std::vector<DUNGEON_INFO> allDugeonInfo;
            allDugeonInfo.clear();
            allDugeonInfo = dungeonInfo->dungeon_list;
            DungeonInfoPrint(&allDugeonInfo);

            IsRunning = false;
        }
        break;
    default:
        break;
    }

}

void ServerInfoHandler::Print(MONITOR_INFO_ACK* Center_Info)
{
    int32 i = 0;
    i = ServerInfo::Instance()->GetOption();
    switch (i)
    {
    case kNumber:
        printf("Number of online gamers: %d \n", Center_Info->player_list.size());
        break;
    case kInformation:
        {
            printf("player account\t player name \n");
            for (std::vector<MONITOR_PLAYER_INFO>::iterator it = Center_Info->player_list.begin();it != Center_Info->player_list.end();++it)
            {
                printf("%s   \t %s\n",it->acount.c_str(),it->name.c_str());
            }
        }
        break;
    case kLevelPlayNumber:
        {
            printf(" LV %d  players Number: %d \n");
        }
        break;
    case kPlayerDungeon:
        {
            printf("%-20s %-20s %-20s %-20s\n", "playerName", "dungeonID", "channelID", "sceneID");
            for (std::vector<MONITOR_PLAYER_INFO>::iterator it = Center_Info->player_list.begin();it != Center_Info->player_list.end(); ++it)
            {
                printf("%-20s %-20d %-20d %-20d\n", it->name.c_str(), DUNGEON_ID(it->dungeon_id), CHANNEL_ID(it->dungeon_id), SCENE_ID(it->dungeon_id));
            }
        }
        break;
    default:
        break;
    }
}

void ServerInfoHandler::DungeonInfoPrint( std::vector<DUNGEON_INFO>* allDungeonInfo )
{
    uint32  AllChannelNum = 0;
    uint32  AllPlayerNum = 0;
    printf("%20s %20s %20s\n", "dungeonID", "channel number", "player number");
    for (std::vector<DUNGEON_INFO>::iterator it = allDungeonInfo->begin(); it != allDungeonInfo->end(); it++)
    {
        if(it->player_count == 0)
            continue;
        printf("%20d %20d %20d\n", it->dungeon_id, it->channel_count, it->player_count);
        AllChannelNum += it->channel_count;
        AllPlayerNum += it->player_count;
    }

    printf("dungeonNum=%d playerNum=%d\n", AllChannelNum, AllPlayerNum);
}
