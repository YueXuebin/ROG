/*
 *	暮光之战的频道管理器
 */
#ifndef twilight_war_channel_manager_h__
#define twilight_war_channel_manager_h__

#include "guild_channel_manager.h"

class TwilightWarChannelManager : public GuildChannelManager
{
public:
    TwilightWarChannelManager(DungeonSet* pDungeonSet);
    virtual ~TwilightWarChannelManager();

    virtual uint32 GetType() {return CHANNEL_TWILIGHT_WAR;}

    virtual void Update();

    virtual void SetReadyRunTime(Dungeon* dungeon);

private:
    
};


#endif // twilight_war_channel_manager_h__
