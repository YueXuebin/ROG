/*
 *	世界BOSS的频道管理器
 */
#ifndef world_boss_channel_manager_h__
#define world_boss_channel_manager_h__

#include "channel_manager.h"

// 世界BOSS玩法的游戏状态
enum
{
    WORLD_BOSS_CLOSE = 0,       // 世界boss未开启
    WORLD_BOSS_OPEN,            // 世界boss开启，但boss还未出现
    WORLD_BOSS_START,           // 世界boss出现了
    WORLD_BOSS_END,             // 世界boss死亡了
};


class WorldBossChannelManager : public ChannelManager
{
public:
    WorldBossChannelManager(DungeonSet* pDungeonSet);
    virtual ~WorldBossChannelManager();

    virtual uint32 GetType() {return CHANNEL_WORLD_BOSS;}

    virtual void Update();

    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);

    void    SetWorldBossID(uint32 creatureID) {m_CreatureID = creatureID;}

    void    OpenDungeon();
    void    CloseDungeon();

private:
    void    CreateWorldBoss(uint32 creatureID);

private:
    uint32              m_WorldBossState;       // 世界Boss游戏状态
    int32               m_StateTime;            // 世界Boss游戏状态时间
    uint32              m_CreatureID;           // 世界BossID

    std::vector<uint32>     m_CurrentChannels;
};


#endif // world_boss_channel_manager_h__
