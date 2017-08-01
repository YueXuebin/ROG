#include "centerserver_pch.h"
#include "activecode_manager.h"
#include "player.h"
#include "player_manager.h"
#include "config_loader.h"
#include "activecode_job.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(ActiveCodeManager)

ActiveCodeManager::ActiveCodeManager()
{

}

ActiveCodeManager::~ActiveCodeManager()
{

}

// ==================================================
// 接收网络消息
// ==================================================
void ActiveCodeManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ACTIVECODE_USE_REQ:
        {   // 使用兑换码
            OnActiveCodeUseReq(playerID, data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 使用兑换码请求
// ==================================================
void ActiveCodeManager::OnActiveCodeUseReq(uint64 playerID, void* data)
{
    ACTIVATION_CODE* recvMsg = (ACTIVATION_CODE*)data;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player || (player->GetState()!=Player::kStateInGame))
    {
        return;
    }

    ActiveCodeJob::Instance()->UseGiftCode(player->GetPlayerID(), recvMsg->activation_code);
}

