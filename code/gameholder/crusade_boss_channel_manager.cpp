#include "gameholder_pch.h"
#include "crusade_boss_channel_manager.h"
#include "dungeon_set.h"
#include "dungeon_data.h"
#include "dungeon.h"
#include "player.h"
#include "guild_config.h"

CrusadeBossChannelManager::CrusadeBossChannelManager( DungeonSet* pDungeonSet ) :
GuildChannelManager(pDungeonSet)
{
    SetGroupNum(1);
    SetGroupPlayerNum(GuildConfig::Instance()->GetCrusadeBossMaxPlayerNum());
}

CrusadeBossChannelManager::~CrusadeBossChannelManager()
{
    
}

void CrusadeBossChannelManager::Update()
{
    
}

void CrusadeBossChannelManager::SetReadyRunTime(Dungeon* dungeon)
{
    
}
