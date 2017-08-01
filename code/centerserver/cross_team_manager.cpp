#include "centerserver_pch.h"
#include "cross_team_manager.h"
#include "player_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "player.h"
#include "game_handler.h"


IMPLEMENT_SINGLETON(CrossTeamManager)

CrossTeamManager::CrossTeamManager()
{

}

CrossTeamManager::~CrossTeamManager()
{

}

void CrossTeamManager::OnRecv( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;

    ReSendToGame(player, msgID, data);
}

void CrossTeamManager::ReSendToGame(Player* player, uint32 msgID, void* data)
{
    ReSendToGame(player->GetPlayerID(), player->GetCentGateID(), msgID, data);
}

void CrossTeamManager::ReSendToGame(uint64 playerID, uint32 gateSvrID, uint32 msgID, void* data)
{
    // 查找跨服大厅所在的跨服Server
    uint32 gameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(DUNGEON_CROSS_TEAM_ID);
    if(gameServerID == 0)
    {   // 跨服Server不存在
        CnInfo("cross team dungeon=%d game server not exist\n", DUNGEON_CROSS_TEAM_ID);

        MULTISERVER_BATTLE_APPLY_ACK  sendMsg;
        sendMsg.errcode = ERR_MULTISERVER_NOT_SERVICE;
        if(gateSvrID)
            GateHandler::Instance()->SendByID(gateSvrID, MSG_MULTISERVER_BATTLE_APPLY_ACK, &sendMsg, playerID);
        return;
    }

    // 转发给对应的Game
    GameHandler::Instance()->SendByID(gameServerID, msgID, data, playerID);
}

void CrossTeamManager::LeaveTeam(uint64 playerID)       // 玩家下线,离开队伍
{
    return;
    CROSS_TEAM_MEMBER_DEL_REQ sendMsg;
    sendMsg.player_id_h = U64ID_HIGH(playerID);
    sendMsg.player_id_l = U64ID_LOW(playerID);

    ReSendToGame(playerID, 0, MSG_CROSS_TEAM_MEMBER_DEL_REQ, &sendMsg);
}


