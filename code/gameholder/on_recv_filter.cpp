#include "gameholder_pch.h"
#include "on_recv_filter.h"
#include "player.h"
#include "scene.h"

MessageTimeRecorder::MessageTimeRecorder()
{

}

MessageTimeRecorder::~MessageTimeRecorder()
{

}

void MessageTimeRecorder::Clear()
{
    ticks       = 0;
    count       = 0;
}

OnRecvFilter::OnRecvFilter(Player* player) :
m_player(player)
{

}

OnRecvFilter::~OnRecvFilter()
{

}

void OnRecvFilter::Update()
{
    for(MessageTimeMap::iterator it = m_MessageTimeMap.begin(); it != m_MessageTimeMap.end(); ++it)
    {
        if(it->second.count > 0)
        {
            it->second.ticks += g_ServerUpdateTime;

            switch(it->first)
            {
            case MSG_CHANGE_DUNGEON_REQ:
            case MSG_CHANGE_SCENE_REQ:
                {
                    if((it->second.ticks >= 1000) && 
                       (it->second.count > 0))
                    {
                        it->second.ticks = 0;
                        it->second.count--;
                    }
                }
                break;
            case MSG_MOVE_REQ:
                {
                    if((it->second.ticks >= g_ServerUpdateTime) && 
                        (it->second.count > 0))
                    {
                        it->second.ticks = 0;
                        it->second.count--;
                    }
                }
                break;
            }
        }
        else
        {
            if(it->second.ticks) it->second.ticks = 0;
            if(it->second.count) it->second.count = 0;
        }
    }
}

bool OnRecvFilter::OnRecv( uint32 msgID, void* data )
{
    MessageTimeRecorder         msgTimeRecorder;
    MessageTimeMap::iterator    msgTimeIt;
    // 场景的当前时间
    uint32 currentTime = m_player->GetScene()->GetSceneTime();

    // 协议发送时间间隔检验
    bool isValid = true;

    // 不用的协议直接跳过
    switch(msgID)
    {
    case MSG_CHANGE_DUNGEON_REQ:
    case MSG_CHANGE_SCENE_REQ:
    case MSG_MOVE_REQ:
        break;
    default:
        return isValid;
        break;
    }

    msgTimeIt = m_MessageTimeMap.find(msgID);

    if(msgTimeIt != m_MessageTimeMap.end())
    {
        switch(msgID)
        {
        // 跳场景和跳副本协议，必须间隔1秒
        case MSG_CHANGE_DUNGEON_REQ:
        case MSG_CHANGE_SCENE_REQ:
            {
                // 两秒内，最多只能跳转2次副本
                if(msgTimeIt->second.count >= 2)
                {
                    if(msgID == MSG_CHANGE_DUNGEON_REQ)
                        m_player->SendChangeDungeonErrorAck(ERR_CHANGE_DUNGEON_TOO_FAST);            // 跳副本失败
                    else if(msgID == MSG_CHANGE_SCENE_REQ)
                        m_player->SendChangeSceneAck(ERR_CHANGE_DUNGEON_TOO_FAST, 0, 0, 0, 0);  // 跳场景失败
                    isValid = false;
                }
                else
                {
                    msgTimeIt->second.count++;
                }
            }
            break;
        case MSG_MOVE_REQ:
            {
                // 3帧内，最多只能移动3次(暂时去掉)
                //MOVE_REQ* moveReq = (MOVE_REQ*)data;
                //if((msgTimeIt->second.count >= 3) &&
                //    moveReq->is_moving)
                //{
                //    isValid = false;    // 移动包，一帧只处理一个
                //}
                //else
                //{
                //    msgTimeIt->second.count++;
                //}
            }
            break;
        default:
            break;
        }
    }
    else
    {
        msgTimeRecorder.ticks = 0;
        msgTimeRecorder.count = 1;
        m_MessageTimeMap[msgID] = msgTimeRecorder;
    }

    return isValid;
}
