#include "gameholder_pch.h"
#include "dungeon.h"
#include "scene.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "game_define.h"
#include "player.h"
#include "monster.h"
#include "boss.h"
#include "pet.h"
#include "regular_dungeon.h"
#include "center_handler.h"
#include "event_define.h"
#include "dungeon_rule.h"
#include "performance_analysis.h"


Dungeon::Dungeon(DungeonSet* pSet, uint32 channelID):
m_pDungeonSet(pSet),
m_channelID(channelID),
m_pDefaultScene(NULL)
{
    m_DungeonRule = CnNew DungeonRule(this);

    m_IsVirgin = true;
    m_CanDestory = true;

    m_dungeonID = m_pDungeonSet->GetDungeonID();

    m_DungeonTime = 0;
    m_NoPlayerTimer = g_DungeonEndTime;
    m_SecondTimer = 0; 

    m_PlayerCount = 0;
    m_MonsterCount = 0;
    m_BossCount = 0;
    m_PetCount = 0;
    m_PeakPlayerNum = 0;

    m_invalide = false;

    //CnDbg("create dungeon id=%d channel=%d\n", m_pDungeonSet->GetDungeonID(), m_channelID);
}

Dungeon::~Dungeon()
{
    for(SceneMap::iterator iter=m_scenes.begin(); iter != m_scenes.end(); iter++)
    {
        CnDelete iter->second;
    }

    CnDelete m_DungeonRule;

    //CnDbg("delete dungeon id=%d channel=%d\n", m_pDungeonSet->GetDungeonID(), m_channelID);
}

bool Dungeon::Init()
{
    const DungeonTemplate* dungeonTmpl = m_pDungeonSet->GetDungeonTemplate();
    if(!dungeonTmpl)
    {
        return false;
    }

    for(std::list<SceneTemplate>::const_iterator sceneIt = dungeonTmpl->m_scene_list.begin();
            sceneIt != dungeonTmpl->m_scene_list.end(); ++sceneIt)
    {
        // 创建碰撞世界,读取map文件
        CoWorld* pCoWorld = MakeCoWorld(sceneIt->m_map_file);
        if(!pCoWorld)
        {
            CnFatal("map %s not exist!\n", sceneIt->m_map_file.c_str());
            CnAssert(false);
        }

        // 创建box2d物理世界
        b2World* pB2World = MakeB2World(sceneIt->m_map_file);
        if(!pB2World)
        {
            CnFatal("map %s not exist!\n", sceneIt->m_map_file.c_str());
            CnAssert(false);
        }

        // 创建场景
        Scene* pNewScene = CnNew Scene(this, sceneIt->m_scene_id, sceneIt->trigger_list, pCoWorld, pB2World, sceneIt->design_file, (sceneIt->m_close == 1));
        if(!m_pDefaultScene)
            m_pDefaultScene = pNewScene;

        m_scenes[sceneIt->m_scene_id] = pNewScene;
    }

    return true;
}

CoWorld* Dungeon::MakeCoWorld(const std::string& path)
{
    CoWorld* pWorld = MakeCollisionWorld(path.c_str());
    return pWorld;
}

b2World* Dungeon::MakeB2World(const std::string& path)
{
    b2World* pWorld = MakeBox2DWorld(path);
    return pWorld;
}

Scene*  Dungeon::GetScene(uint32 sceneID)
{
    if(m_scenes.find(sceneID) == m_scenes.end())
    {
        return NULL;
    }
    return m_scenes[sceneID];
}

void Dungeon::SetAllStateTime(int32 readyTime, int32 runTime, int32 endTime, int32 curTime)
{
    for(SceneMap::iterator sceneIt = m_scenes.begin(); sceneIt != m_scenes.end(); ++sceneIt)
    {
        Regular* pRegular = sceneIt->second->GetRegular();
        if(pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
        {
            RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pRegular);
            pRegularDungeon->SetAllStateTime(readyTime, runTime, endTime, curTime);
        }
    }
}

void Dungeon::SetSceneAllStateTime(int32 sceneID, int32 readyTime, int32 runTime, int32 endTime, int32 curTime)
{
    Scene* sceneIt = GetScene(sceneID);
    if(!sceneIt)
    {
        return;
    }

    Regular* pRegular = sceneIt->GetRegular();
    if(pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
    {
        RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pRegular);
        pRegularDungeon->SetAllStateTime(readyTime, runTime, endTime, curTime);
    }
}

void Dungeon::SetRegularTime(int32 time)
{
    for(SceneMap::iterator sceneIt = m_scenes.begin(); sceneIt != m_scenes.end(); ++sceneIt)
    {
        Regular* pRegular = sceneIt->second->GetRegular();
        if(pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
        {
            RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pRegular);
            pRegularDungeon->SetCurrentTime(time);
        }
    }
}

void Dungeon::StopAllRegular(bool success)
{
    for(SceneMap::iterator sceneIt = m_scenes.begin(); sceneIt != m_scenes.end(); ++sceneIt)
    {
        Regular* pRegular = sceneIt->second->GetRegular();
        if(pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
        {
            RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pRegular);
            DungeonClear(success);
        }
    }
}

void Dungeon::Update(uint32 frame_count)
{
    PERFORM_HELPER;
    m_DungeonTime += g_ServerUpdateTime;

    for(SceneMap::iterator iter = m_scenes.begin(); iter != m_scenes.end(); iter++)
    {
        iter->second->Update(frame_count);
    }

    // 无玩家时间计时
    if(m_PlayerCount > 0)
    {
        m_NoPlayerTimer = g_DungeonEndTime;        // 10秒
    }
    else
    {
        m_NoPlayerTimer -= g_ServerUpdateTime;
    }

    // 秒Update更新时间
    if(m_SecondTimer < 1000)
    {
        m_SecondTimer += g_ServerUpdateTime;
    }
    else
    {
        m_SecondTimer -= 1000;
        OneSecondUpdate();
    }
}

void Dungeon::OneSecondUpdate()
{
    m_DungeonRule->OneSecondUpdate();
}

void Dungeon::OnCreatureEnter( BattleUnit* creature )
{
    if(creature->IsKindOf(&Player::ms_RTTI))
    {
        m_PlayerCount++;
        m_PeakPlayerNum = Crown::SDMax(m_PeakPlayerNum, m_PlayerCount);
        m_IsVirgin = false;
        OnPlayerEnter((Player*)creature);
    }
    else if(creature->IsKindOf(&Monster::ms_RTTI))
        m_MonsterCount++;
    else if (creature->IsKindOf(&Boss::ms_RTTI))
        m_BossCount++;
    else if(creature->IsKindOf(&Pet::ms_RTTI))
        m_PetCount++;
}

void Dungeon::OnCreatureLeave( BattleUnit* creature )
{
    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        ((Player*)creature)->OnExitDungeon();
    }

    if(creature->IsKindOf(&Player::ms_RTTI))
    {
        m_PlayerCount--;
        CnAssert(m_PlayerCount >= 0);
        OnPlayerLeave((Player*)creature);
    }
    else if(creature->IsKindOf(&Monster::ms_RTTI))
    {
        m_MonsterCount--;
        CnAssert(m_MonsterCount >= 0);
    }
    else if (creature->IsKindOf(&Boss::ms_RTTI))
    {
        m_BossCount--;
        CnAssert(m_BossCount >= 0);
    }
    else if(creature->IsKindOf(&Pet::ms_RTTI))
    {
        m_PetCount--;
        CnAssert(m_PetCount >= 0);
    }

}
bool Dungeon::IsClose()
{
    return m_DungeonRule->IsClear();
}

bool Dungeon::NeedDestory()
{
    if(m_pDungeonSet->GetType() == DUNGEON_TYPE_CITY || m_pDungeonSet->GetType() == DUNGEON_TYPE_WORLD)
        return false;       // 主城和世界地图不会自动销毁

    if(m_IsVirgin)
        return false;

    if(!m_CanDestory)
        return false;

    return  m_NoPlayerTimer<0;
}

uint32 Dungeon::GetType()
{
    return m_pDungeonSet->GetType();
}

void Dungeon::SendEvent( uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime )
{
    for(SceneMap::iterator iter=m_scenes.begin(); iter != m_scenes.end(); iter++)
    {
        iter->second->SendEvent(0, eventId, arg1, arg2, delayTime);
    }
}

uint32 Dungeon::GetPlayerNum()
{
    uint32 playerNum = 0;
    for(SceneMap::iterator iter = m_scenes.begin(); iter != m_scenes.end(); iter++)
    {
        playerNum += iter->second->GetPlayerCount();
    }
    return playerNum;
}

std::vector<Player*> Dungeon::GetPlayerMap()
{
    std::vector<Player*> playerMap;
    playerMap.clear();
    for(SceneMap::iterator iter = m_scenes.begin(); iter != m_scenes.end(); iter++)
    {
        Scene::PlayerMap& sceneList = iter->second->GetPlayers();
        for(Scene::PlayerMap::iterator sceneIt = sceneList.begin(); sceneIt != sceneList.end(); ++sceneIt)
        {
            playerMap.push_back(sceneIt->second);
        }
    }

    return playerMap;
}

void Dungeon::OnPlayerEnter(Player* player)
{
    m_DungeonRule->OnPlayerEnter(player);
}

void Dungeon::OnPlayerLeave(Player* player)
{
    m_DungeonRule->OnPlayerLeave(player);
}

void Dungeon::DungeonClear(bool success, std::vector<std::string>* list)
{
    m_DungeonRule->DungeonClear(success, list);
}

