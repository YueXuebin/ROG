#include "centerserver_pch.h"
#include "activity_manager.h"
#include "game_handler.h"
#include "gate_handler.h"

IMPLEMENT_SINGLETON(ActivityManager)

ActivityManager::ActivityManager()
{

}

ActivityManager::~ActivityManager()
{

}

void ActivityManager::OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ACTIVITY_LIST_NTF:
        // 通知服务器
        GameHandler::Instance()->SendToAll(MSG_ACTIVITY_LIST_NTF, data);

        // 通知客户端
        GateHandler::Instance()->SendToAll(MSG_ACTIVITY_LIST_NTF, data, BROADCAST_PLAYER_ID);
        break;
    }
}
