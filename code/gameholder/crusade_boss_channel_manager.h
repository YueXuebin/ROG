/*
 *	争夺公会BOSS的频道管理器
 */
#ifndef crusade_boss_channel_manager_h__
#define crusade_boss_channel_manager_h__

#include "guild_channel_manager.h"

class CrusadeBossChannelManager : public GuildChannelManager
{
public:
    CrusadeBossChannelManager(DungeonSet* pDungeonSet);
    virtual ~CrusadeBossChannelManager();

    virtual uint32 GetType() {return CHANNEL_CRUSADE_BOSS;}

    virtual void Update();

    virtual void SetReadyRunTime(Dungeon* dungeon);
};


#endif // city_fight_channel_manager_h__
