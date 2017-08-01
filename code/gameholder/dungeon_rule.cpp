#include "gameholder_pch.h"
#include "dungeon_rule.h"
#include "dungeon.h"
#include "regular_dungeon.h"
#include "scene.h"
#include "player.h"
#include "player_manager.h"
#include "regular_single.h"

DungeonRule::DungeonRule(Dungeon* pDungeon) :
m_pDungeon(pDungeon)
{
    m_IsClear = false;
}

DungeonRule::~DungeonRule()
{

}

void DungeonRule::DungeonClear(bool success, std::vector<std::string>* list)
{
    if(m_IsClear)
        return;
    m_IsClear = true;       // 副本通关只有一次

    // 副本结算处理
    Dungeon::SceneMap& sceneMap = m_pDungeon->GetScenes();
    // 每个场景结束
    for(Dungeon::SceneMap::iterator sceneIt = sceneMap.begin(); sceneIt != sceneMap.end(); sceneIt++)
    {
        Scene* pScene = sceneIt->second;
        RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pScene->GetRegular());  // 临时副本才有结算
        if(!pRegularDungeon)
            continue;

        pRegularDungeon->OnDungeonClear(success, list);          // 场景结束
    }

    // 副本通关,玩家处理
    std::vector<Player*> players = m_pDungeon->GetPlayerMap();
    for(std::vector<Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = *it;
        // 任务结算副本
        player->OnTaskSettleDungeon(m_pDungeon->GetDungeonID(), success);
        // 为副本进入次数计数
        player->OnAddDungeonCount(m_pDungeon->GetDungeonID());
    }
}

void DungeonRule::OneSecondUpdate()
{
    // 延时处理玩家的进入与离开造成的难度变化

    // 进入玩家处理
    for(std::vector<PlayerRecordData>::iterator playerIt = m_EnterPlayerList.begin(); playerIt != m_EnterPlayerList.end(); playerIt++)
    {
        PlayerRecordData& enterData = *playerIt;
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(enterData.playerID);
        
        CnAssert(player);
        CnAssert(DUNGEON_ID(player->GetUnionMapID()) == m_pDungeon->GetDungeonID());

        // 通知所有场景,有玩家进入副本
        Dungeon::SceneMap& scenes = m_pDungeon->GetScenes();
        for(Dungeon::SceneMap::iterator iter = scenes.begin(); iter != scenes.end(); iter++)
        {
            Scene* pScene = iter->second;
            RegularSingle* pRegularSingle = CnDynamicCast(RegularSingle, pScene->GetRegular());
            if(pRegularSingle)
                pRegularSingle->OnPlayerEnterDungeonNtf(enterData.playerID, enterData.playerName);
        }
    }
    m_EnterPlayerList.clear();

    // 离开玩家处理
    for(std::vector<PlayerRecordData>::iterator playerIt = m_LeavePlayerList.begin(); playerIt != m_LeavePlayerList.end(); playerIt++)
    {
        PlayerRecordData& leaveData = *playerIt;

        // 通知所有场景,有玩家进入副本
        Dungeon::SceneMap& scenes = m_pDungeon->GetScenes();
        for(Dungeon::SceneMap::iterator iter = scenes.begin(); iter != scenes.end(); iter++)
        {
            Scene* pScene = iter->second;
            RegularSingle* pRegularSingle = CnDynamicCast(RegularSingle, pScene->GetRegular());
            if(pRegularSingle)
                pRegularSingle->OnPlayerLeaveDungeonNtf(leaveData.playerID, leaveData.playerName);
        }
    }
    m_LeavePlayerList.clear();
}

void DungeonRule::OnPlayerEnter(Player* player)
{
    for(std::vector<PlayerRecordData>::iterator iter = m_LeavePlayerList.begin(); iter != m_LeavePlayerList.end(); iter++)
    {
        PlayerRecordData& leaveData = *iter;
        if(leaveData.playerID == player->GetPlayerID())
        {
            m_LeavePlayerList.erase(iter);
            return;
        }
    }

    PlayerRecordData enterData;
    enterData.playerID = player->GetPlayerID();
    enterData.playerName = player->GetName();
    m_EnterPlayerList.push_back(enterData);

    // 记录第一个进入副本的玩家信息
    if(m_FirstEnterPlayer.playerID == 0)
    {
        m_FirstEnterPlayer.playerID = player->GetPlayerID();
        m_FirstEnterPlayer.playerName = player->GetName();
    }
}

void DungeonRule::OnPlayerLeave(Player* player)
{
    for(std::vector<PlayerRecordData>::iterator iter = m_EnterPlayerList.begin(); iter != m_EnterPlayerList.end(); iter++)
    {
        PlayerRecordData& enterData = *iter;
        if(enterData.playerID == player->GetPlayerID())
        {
            m_EnterPlayerList.erase(iter);
            return;
        }
    }

    PlayerRecordData leaveData;
    leaveData.playerID = player->GetPlayerID();
    leaveData.playerName = player->GetName();
    m_LeavePlayerList.push_back(leaveData);
}
