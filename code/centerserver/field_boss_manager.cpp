#include "centerserver_pch.h"
#include "field_boss_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player_manager.h"
IMPLEMENT_SINGLETON(FieldBossManager)

FieldBossManager::FieldBossManager()
{

}

FieldBossManager::~FieldBossManager()
{

}

void FieldBossManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_FIELD_BOSS_INFO_REQ:
        RecvFieldBossInfoReq(data);
        break;
    case MSG_FIELD_BOSS_PLAYER_IS_OFFLINE_REQ:
        RecPlayerIsOnLine(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void FieldBossManager::RecPlayerIsOnLine(void* data)
{
    FIELD_BOSS_PLAYER_IS_OFFLINE_REQ* recvMsg = (FIELD_BOSS_PLAYER_IS_OFFLINE_REQ*)data;
    if(!recvMsg) return;

    FIELD_BOSS_PLAYER_IS_OFFLINE_ACK sendMsg;
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
    if(!player)
    {
        sendMsg.player_id.id_h = recvMsg->player_id.id_h;
        sendMsg.player_id.id_l = recvMsg->player_id.id_l;

        GameHandler::Instance()->SendToAll(MSG_FIELD_BOSS_PLAYER_IS_OFFLINE_ACK, &sendMsg);
    }
}
void FieldBossManager::RecvFieldBossInfoReq( void* data )
{
    FIELD_BOSS_INFO_REQ* recvMsg = (FIELD_BOSS_INFO_REQ*)data;
    FIELD_BOSS_INFO_ACK  sendMsg;
    uint32 gameServerID = 0;

    if(!recvMsg)
        return;

    // 检查哪个GameServer拥有野外的地图
    gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(recvMsg->dungeon_id);
    if(gameServerID == 0)
    {   // 野外Boss的server不存在
        sendMsg.errcode = ERR_FAILED;
        GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_FIELD_BOSS_INFO_ACK, 
            &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        return;
    }

    GameHandler::Instance()->SendByID(gameServerID, MSG_FIELD_BOSS_INFO_REQ, data);
}
