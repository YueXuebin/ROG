#ifndef faction_pk_channel_manager_h__
#define faction_pk_channel_manager_h__

#include "channel_manager.h"
class Dungeon;

// 圣剑争锋频道管理

class FactionPkChannelManager : public ChannelManager
{
public:
    FactionPkChannelManager(DungeonSet* pDungeonSet);
    virtual ~FactionPkChannelManager();

    virtual uint32 GetType() {return CHANNEL_FACTION_PK;}

    virtual void Update();

    virtual uint32  AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID);
private:

    virtual uint32  ReadyUnionID();
    virtual uint32  StartUnionID(uint32 channelID);

    void            SetAllStateTime(Dungeon* dungeon);  // 游戏准备设置时间
};

#endif // faction_pk_channel_manager_h__
