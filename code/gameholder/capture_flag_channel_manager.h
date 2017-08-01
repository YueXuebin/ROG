#ifndef capture_flag_channel_manager_h__
#define capture_flag_channel_manager_h__

#include "channel_manager.h"

// 夺旗频道管理

class CaptureChannelManager : public ChannelManager
{
public:
    CaptureChannelManager(DungeonSet* pDungeonSet);
    virtual ~CaptureChannelManager();

    virtual uint32 GetType() {return CHANNEL_CAPTURE_FLAG;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);
private:
};

#endif // capture_flag_channel_manager_h__
