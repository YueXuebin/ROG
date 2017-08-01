/*
 *	����BOSS��Ƶ��������
 */
#ifndef world_boss_channel_manager_h__
#define world_boss_channel_manager_h__

#include "channel_manager.h"

// ����BOSS�淨����Ϸ״̬
enum
{
    WORLD_BOSS_CLOSE = 0,       // ����bossδ����
    WORLD_BOSS_OPEN,            // ����boss��������boss��δ����
    WORLD_BOSS_START,           // ����boss������
    WORLD_BOSS_END,             // ����boss������
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
    uint32              m_WorldBossState;       // ����Boss��Ϸ״̬
    int32               m_StateTime;            // ����Boss��Ϸ״̬ʱ��
    uint32              m_CreatureID;           // ����BossID

    std::vector<uint32>     m_CurrentChannels;
};


#endif // world_boss_channel_manager_h__
