/*
 *	城市争夺战的频道管理器
 */
#ifndef city_fight_channel_manager_h__
#define city_fight_channel_manager_h__

#include "guild_channel_manager.h"

class CityFightChannelManager : public GuildChannelManager
{
public:
    CityFightChannelManager(DungeonSet* pDungeonSet);
    virtual ~CityFightChannelManager();

    virtual uint32 GetType() { return CHANNEL_CITY_FIGHT; }

    virtual void Update();

    virtual void SetReadyRunTime(Dungeon* dungeon);
};

#endif // city_fight_channel_manager_h__
