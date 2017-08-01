//------------------------------------------------------------------------------
//
//    Copyright (C) DoinGame. All rights reserved.
//
//    File Name:   team_manager.h
//    Author:      Tomren
//    Create Time: 2013-09-16
//    Description: 
//
//    Version history:
//
//
//------------------------------------------------------------------------------

#include "centerserver_pch.h"
#include "adventure_manager.h"
#include "player.h"
#include "player_manager.h"
#include "activity_job.h"

IMPLEMENT_SINGLETON(AdventureManager)

// ==================================================
// 构造函数
// ==================================================
AdventureManager::AdventureManager()
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
AdventureManager::~AdventureManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void AdventureManager::Clear()
{
    //m_player = 0;
    //m_team_info.Clear();
}

// ==================================================
// 接收网络消息
// ==================================================
void AdventureManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ADVENTURER_LIST_REQ:
        OnAdventureListReq(playerID, data);         // 请求冒险者信息列表
        break;
    case MSG_ADVENTURE_ROB_REQ:
        OnAdventureRobReq(playerID, data);          // 请求打劫冒险者
        break;
    case MSG_ADVENTURE_ADD_REQ:
        OnAdventureAddReq(playerID, data);          // 添加大冒险信息
        break;
    default:
        break;
    }
}

// ==================================================
// 请求冒险者列表
// ==================================================
void AdventureManager::OnAdventureListReq(uint64 playerID, void* data)
{
    ActivityJob::Instance()->GetAdventurerList(playerID);
}

// ==================================================
// 请求打劫大冒险
// ==================================================
void AdventureManager::OnAdventureRobReq(uint64 playerID, void* data)
{
    ADVENTURE_ROB_REQ* recvMsg = (ADVENTURE_ROB_REQ*)data;
    Player* player = 0;
    uint64 roberID = 0;

    if(!recvMsg) return;

    // 获取请求者的角色指针
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    roberID = U64ID(recvMsg->player_id_h, recvMsg->player_id_l);

    ActivityJob::Instance()->RobAdventurer(playerID, roberID);
}

// ==================================================
// 添加大冒险信息
// ==================================================
void AdventureManager::OnAdventureAddReq(uint64 playerID, void* data)
{
    ADVENTURE_ADD_REQ* recvMsg = (ADVENTURE_ADD_REQ*)data;
    Player* player = 0;

    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    ActivityJob::Instance()->UpdateAdventure(playerID, *recvMsg);
}
