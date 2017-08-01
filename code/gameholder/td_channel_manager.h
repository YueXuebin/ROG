#ifndef td_channel_manager_h__
#define td_channel_manager_h__

#include "channel_manager.h"

// 守卫地宫频道管理

class TdChannelManager : public ChannelManager
{
public:
    TdChannelManager(DungeonSet* pDungeonSet);
    virtual ~TdChannelManager();

    virtual uint32 GetType() {return CHANNEL_TD;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);
private:
};

#endif // td_channel_manager_h__
