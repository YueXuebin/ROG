#ifndef world_map_channel_manager_h__
#define world_map_channel_manager_h__

//野外Boss频道分配

#include "dungeon_set.h"
#include "channel_manager.h"

class WorldMapChannelManager : public ChannelManager
{
public:
    WorldMapChannelManager(DungeonSet* pDungeonSet);
    virtual ~WorldMapChannelManager();

    virtual uint32 GetType() {return CHANNEL_ASHURA;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID );

private:
};

#endif // world_map_channel_manager_h__
