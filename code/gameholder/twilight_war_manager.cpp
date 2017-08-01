#include "gameholder_pch.h"
#include "twilight_war_manager.h"
#include "player_manager.h"
#include "guild_manager.h"
#include "guild_config.h"
#include "activity_manager.h"
#include "player.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "item_manager.h"
#include "center_handler.h"

IMPLEMENT_SINGLETON(TwilightWarManager)

// ==================================================
// 构造函数
// ==================================================
TwilightWarManager::TwilightWarManager()
{
    m_currTime = 0;
    m_isStart = false;
}

// ==================================================
// 析构函数
// ==================================================
TwilightWarManager::~TwilightWarManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void TwilightWarManager::Clear()
{
    
}

// ==================================================
// 暮光之战更新
// ==================================================
void TwilightWarManager::Update(uint32 frame_count)
{
    m_currTime += g_ServerUpdateTime;
}

// ==================================================
// 协议接收
// ==================================================
void TwilightWarManager::OnRecv(uint32 msgID, void* data, Player* player)
{
    switch (msgID)
    {
    case MSG_GUILD_TWILIGHT_WAR_OPEN_NTF:
        OnTwilightWarStart(data);
        break;
    case MSG_GUILD_TWILIGHT_WAR_CLOSE_NTF:
        OnTwilightWarEnd(data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

// ==================================================
// 开始
// ==================================================
void TwilightWarManager::OnTwilightWarStart( void* data )
{
    StartTwilightWar();
}

// ==================================================
// 结束
// ==================================================
void TwilightWarManager::OnTwilightWarEnd( void* data )
{
    // 当暮光之战结束后，通知副本集中的所有频道可以销毁
    StopTwilightWar();
}

// 开启暮光之战
void TwilightWarManager::StartTwilightWar()
{
    m_currTime = 0;
    m_isStart = true;
}

// 关闭副本
void TwilightWarManager::StopTwilightWar()
{
    DungeonSet* dungeonSet = 0;

    if(!m_isStart) return;

    m_isStart = false;

    //dungeonSet = DungeonManager::Instance()->GetDungeonSet(TWILIGHT_WAR_MAP_ID);

    //if(!dungeonSet) return;

    dungeonSet->StopAllRegular(false);

    ActivityManager::Instance()->CloseActivityByType(ACTIVITY_TYPE_TWILIGHT_WAR);

    std::map<uint32, Dungeon*>& dungeons = dungeonSet->GetDungeons();
    for (std::map<uint32, Dungeon*>::iterator dungeonIt = dungeons.begin();
            dungeonIt != dungeons.end(); ++dungeonIt)
    {
        dungeonIt->second->SetCanDestory(true);
    }
}
