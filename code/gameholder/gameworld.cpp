#include "gameholder_pch.h"
#include "gameholder.h"
#include "gameworld.h"
#include "scene.h"
#include "player.h"
#include "player_manager.h"
#include "regular_manager.h"
#include "dungeon_manager.h"
#include "offline_player_manager.h"
#include "ashura_manager.h"
#include "world_boss_manager.h"
#include "faction_manager.h"
#include "tower_defence_manager.h"
#include "city_fight_manager.h"
#include "city_survival_manager.h"
#include "twilight_war_manager.h"
#include "performance_analysis.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "multiserver_battle_manager.h"


GameWorld::GameWorld()
{

}

GameWorld::~GameWorld()
{

}

bool GameWorld::Init()
{
    return true;
}

void GameWorld::Uninit()
{
    
}

void GameWorld::Update(uint32 frame_count)
{
    PERFORM_HELPER;
    // 副本管理器
    DungeonManager::Instance()->Update(frame_count);

    // 离线角色管理器
    OfflinePlayerManager::Instance()->Update();

    // 玩家管理
    PlayerManager::Instance()->Update();

    // 阿修罗管理器
    AshuraManager::Instance()->Update();

    // 世界BOSS管理器
    WorldBossManager::Instance()->Update();

    // 派别管理器
    FactionManager::Instance()->Update();

    // 塔防管理器
    TowerDefenceManager::Instance()->Update();

    // 城市争夺战管理器
    CityFightManager::Instance()->Update();

    // 圣光生存战管理器
    CitySurvivalManager::Instance()->Update();

    // 暮光之战管理器
    TwilightWarManager::Instance()->Update(frame_count);

    // 跨服pk管理器
    MultiserverBattleManager::Instance()->Update(frame_count);

    VoidFieldManager::Instance()->Update();

    VoidFieldSoloManager::Instance()->Update();


}

// 账户登录失败
void GameWorld::SendEnterGameAck(uint32 sid, uint16 errCode)
{
    LOGIN_GAME_ACK sendData;
    sendData.errcode = errCode;
    //GateHandler::Instance()->Send(sid, MSG_LOGIN_GAME_ACK, &sendData);
}

