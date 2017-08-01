#include "centerserver_pch.h"
#include "multiserver_battle_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player.h"
#include "global_db_operation.h"

IMPLEMENT_SINGLETON(MultiserverBattleManager)
// 跨服战斗匹配转发管理器
MultiserverBattleManager::MultiserverBattleManager(void)
{
}

MultiserverBattleManager::~MultiserverBattleManager(void)
{
}

void MultiserverBattleManager::OnRecv( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_MULTISERVER_BATTLE_APPLY_REQ:
        RecvMultiserverBattleApplyReq(data);
        break;
    case MSG_MULTISERVER_BATTLE_QUIT_REQ:
        RecvMultiserverBattleQuitReq(data);
        break;
    case MSG_MULTISERVER_BATTLE_WAIT_ACK:
        RecvWaitAck(data);
        break;
    case MSG_MULTISERVER_BATTLE_INFO_REQ:
        RecvMultiserverBattleInfoAboutRank(playerID);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void MultiserverBattleManager::RecvMultiserverBattleApplyReq( void* data )
{
    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;
    MULTISERVER_BATTLE_APPLY_ACK  sendMsg;
    uint32 gameServerID = 0;

    if(!recvMsg)
        return;

    if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_ONE)
    {
        // 检查哪个GameServer拥有跨服战斗的地图
        //gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(DUNGEON_MULTISERVER_BATTLE_ID);
        //if(gameServerID == 0)
        //{   // 跨服战斗的server不存在
        //    CnInfo("multi battle dungeon=%d game server not exist\n", DUNGEON_MULTISERVER_BATTLE_ID);
        //    sendMsg.errcode = ERR_MULTISERVER_NOT_SERVICE;
        //    sendMsg.battle_type = recvMsg->battle_type;
        //    GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_MULTISERVER_BATTLE_APPLY_ACK, &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        //    return;
        //}
    }
    else if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_THREE)
    {
        // 检查哪个GameServer拥有跨服战斗的地图
        //gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(CHAOS_MAP);
        //if(gameServerID == 0)
        //{   // 跨服战斗的server不存在
        //    CnInfo("multi battle dungeon=%d game server not exist\n", CHAOS_MAP);
        //    sendMsg.errcode = ERR_MULTISERVER_NOT_SERVICE;
        //    sendMsg.battle_type = recvMsg->battle_type;
        //    GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_MULTISERVER_BATTLE_APPLY_ACK, &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        //    return;
        //}
    }

    GameHandler::Instance()->SendByID(gameServerID, MSG_MULTISERVER_BATTLE_APPLY_REQ, data);
}

void MultiserverBattleManager::RecvMultiserverBattleQuitReq( void* data )
{
    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;
    MULTISERVER_BATTLE_APPLY_ACK  sendMsg;
    uint32 gameServerID = 0;

    if(!recvMsg)
        return;

    if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_ONE)
    {
        // 检查哪个GameServer拥有跨服战斗的地图
        //gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(DUNGEON_MULTISERVER_BATTLE_ID);
        //if(gameServerID == 0)
        //{   // 跨服战斗的server不存在
        //    sendMsg.errcode = ERR_FAILED;
        //    sendMsg.battle_type = recvMsg->battle_type;
        //    GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_MULTISERVER_BATTLE_QUIT_ACK, 
        //        &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        //    return;
        //}
    }
    else if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_THREE)
    {
        // 检查哪个GameServer拥有跨服战斗的地图
        //gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(CHAOS_MAP);
        //if(gameServerID == 0)
        //{   // 跨服战斗的server不存在
        //    sendMsg.errcode = ERR_FAILED;
        //    sendMsg.battle_type = recvMsg->battle_type;
        //    GateHandler::Instance()->SendByID(recvMsg->gate_id, MSG_MULTISERVER_BATTLE_QUIT_ACK, 
        //        &sendMsg, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
        //    return;
        //}
    }

    GameHandler::Instance()->SendByID(gameServerID, MSG_MULTISERVER_BATTLE_QUIT_REQ, data);
}

void MultiserverBattleManager::RecvMultiserverBattleInfoAboutRank(uint64 playerID)
{
    GlobalDbOperation::Instance()->SendMultiSrvInfoAboutRank(playerID);
}

void MultiserverBattleManager::RecvWaitAck( void* data )
{
    MULTISERVER_BATTLE_WAIT_ACK* recvMsg = (MULTISERVER_BATTLE_WAIT_ACK*)data;

    if(!recvMsg)
        return;

    GameHandler::Instance()->SendByID(recvMsg->game_server_id, MSG_MULTISERVER_BATTLE_WAIT_ACK, recvMsg);
}

void MultiserverBattleManager::onExitGame( Player* pPlayer )
{
    if(pPlayer->GetLevel() >= 30)
    {
        //跨服战斗清除匹配列表
        MULTISERVER_BATTLE_APPLY_REQ recvMsg;
        recvMsg.battle_type = MULTISERVER_BATTLE_APPLY_ONE;
        recvMsg.gate_id = pPlayer->GetCentGateID();
        recvMsg.player_id.id_h = U64ID_HIGH(pPlayer->GetPlayerID());
        recvMsg.player_id.id_l = U64ID_LOW(pPlayer->GetPlayerID());
        recvMsg.game_id = 0;
        recvMsg.apply_time = (int32)Crown::SDNow().GetTimeValue();
        recvMsg.channel_id = 0;
        RecvMultiserverBattleQuitReq(&recvMsg);
    }

}
