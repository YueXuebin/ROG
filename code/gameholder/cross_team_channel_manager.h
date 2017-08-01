/*
 *  跨服组队副本的频道管理器
 */
#ifndef cross_team_channel_manager_h__
#define cross_team_channel_manager_h__

#include "channel_manager.h"

class DungeonSet;

class CrossTeamChannelManager : public ChannelManager
{
public:
    CrossTeamChannelManager(DungeonSet* pDungeonSet);
    virtual ~CrossTeamChannelManager();

    virtual uint32 GetType() {return CHANNEL_CROSS_TEAM;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID );

    virtual void    RegisterChannel(uint32 channelID);
    virtual void    UnregisterChannel(uint32 channelID);

    uint64  FindTeamIDByChannelID(uint32 channelID);

private:
    std::map<uint64, uint32>    m_TeamChannelMap;       // <team id, 频道id>
};



#endif // cross_team_channel_manager_h__
