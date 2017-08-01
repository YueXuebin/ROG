#include "centerserver_pch.h"
#include "void_field_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(VoidFieldManager)

VoidFieldManager::VoidFieldManager()
{

}

VoidFieldManager::~VoidFieldManager()
{

}

void VoidFieldManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_CHANNEL_INFO_REQ:
        RecvVoidFieldChannelInfoReq(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void VoidFieldManager::RecvVoidFieldChannelInfoReq( void* data )
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    VOID_FIELD_CHANNEL_INFO_ACK  sendMsg;

    if(!recvMsg)
        return;

    // 检查哪个GameServer拥有虚空幻境的地图
    uint32 gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(recvMsg->dungeon_id);
    if(gameServerID == 0)
    {   // 虚空幻境的server不存在
        sendMsg.errcode = ERR_FAILED;
        GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_VOID_FIELD_CHANNEL_INFO_ACK, &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        return;
    }

    GameHandler::Instance()->SendByID(gameServerID, MSG_VOID_FIELD_CHANNEL_INFO_REQ, data);
}
