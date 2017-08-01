#ifndef chaos_faction_channel_h__
#define chaos_faction_channel_h__

#include "channel_manager.h"

// 3V3匹配频道管理

class ChaosFactionChannelManager : public ChannelManager
{
public:
    ChaosFactionChannelManager(DungeonSet* pDungeonSet);
    virtual ~ChaosFactionChannelManager();

    virtual uint32 GetType() {return CHANNEL_CHAOS;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);

    uint32 GetCreateChannelID();
};

#endif // chaos_faction_channel_h__
