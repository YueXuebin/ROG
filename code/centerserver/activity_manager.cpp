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
        // ֪ͨ������
        GameHandler::Instance()->SendToAll(MSG_ACTIVITY_LIST_NTF, data);

        // ֪ͨ�ͻ���
        GateHandler::Instance()->SendToAll(MSG_ACTIVITY_LIST_NTF, data, BROADCAST_PLAYER_ID);
        break;
    }
}
