#ifndef multiserver_battle_channel_manager_h__
#define multiserver_battle_channel_manager_h__

#include "channel_manager.h"

// 跨服战斗频道管理
class MultiserverBattleChannelManager : public ChannelManager
{
public:
    MultiserverBattleChannelManager(DungeonSet* pDungeonSet);
    ~MultiserverBattleChannelManager(void);

    virtual uint32 GetType() {return CHANNEL_MULTISERVER_BATTLE;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID );

    uint32 GetCreateChannelID(uint32 battle_type);                                      // 创建获取频道ID
    uint32 JudgeDungeonStart(uint32 channelID);                                         // 副本是否开始

private:
    void NotifyAddChannel();
};

#endif
