/*
 *	默认的频道管理器(临时副本)
 */
#ifndef default_channel_manager_h__
#define default_channel_manager_h__

#include "channel_manager.h"

class DefaultChannelManager : public ChannelManager
{
public:
    DefaultChannelManager(DungeonSet* pDungeonSet);
    virtual ~DefaultChannelManager();

    virtual uint32 GetType() {return CHANNEL_DEFAULT;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);
};

#endif // default_channel_manager_h__
