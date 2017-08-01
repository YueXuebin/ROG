#include "centerserver_pch.h"
#include "void_field_solo_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(VoidFieldSoloManager)

VoidFieldSoloManager::VoidFieldSoloManager()
{

}

VoidFieldSoloManager::~VoidFieldSoloManager()
{

}

void VoidFieldSoloManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ:
        RecvVoidFieldChannelInfoReq(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void VoidFieldSoloManager::RecvVoidFieldChannelInfoReq( void* data )
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    VOID_FIELD_CHANNEL_INFO_ACK  sendMsg;
    uint32 gameServerID = 0;

    if(!recvMsg)
        return;

    // 检查哪个GameServer拥有虚空幻境的地图
    //gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(DUNGEON_VOID_FIELD_SOLO_ID);
    //if(gameServerID == 0)
    //{   // 虚空幻境的server不存在
    //    sendMsg.errcode = ERR_FAILED;
    //    GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_VOID_FIELD_SOLO_CHANNEL_INFO_ACK, 
    //        &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
    //    return;
    //}

    //GameHandler::Instance()->SendByID(gameServerID, MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ, data);
}
