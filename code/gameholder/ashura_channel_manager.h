#ifndef ashura_channel_manager_h__
#define ashura_channel_manager_h__

// 修罗场频道分配

#include "dungeon_set.h"
#include "channel_manager.h"

class AshuraChannelManager : public ChannelManager
{
public:
    AshuraChannelManager(DungeonSet* pDungeonSet);
    virtual ~AshuraChannelManager();

    virtual uint32 GetType() {return CHANNEL_ASHURA;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID );

private:
};

#endif // ashura_channel_manager_h__
