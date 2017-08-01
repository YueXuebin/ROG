#ifndef city_survival_channel_manager_h__
#define city_survival_channel_manager_h__

#include "channel_manager.h"

// 圣光生存频道管理

class CitySurvivalChannelManager : public ChannelManager
{
public:
    CitySurvivalChannelManager(DungeonSet* pDungeonSet);
    virtual ~CitySurvivalChannelManager();

    virtual uint32 GetType() {return CHANNEL_CITY_SURVIVAL;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);
private:
};

#endif // city_survival_channel_manager_h__
