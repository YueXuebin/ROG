/*
 *	永久地图的频道管理器
 */
#ifndef city_channel_manager_h__
#define city_channel_manager_h__

#include "channel_manager.h"

class CityChannelManager : public ChannelManager
{
public:
    CityChannelManager(DungeonSet* pDungeonSet);
    virtual ~CityChannelManager();

    virtual uint32 GetType() {return CHANNEL_CITY;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID );

private:
    void NotifyAddChannel();
};


#endif // city_channel_manager_h__
