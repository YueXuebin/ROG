/*
 *	世界BOSS管理
 */
#ifndef world_boss_manager_h__
#define world_boss_manager_h__

#include "message_encoder.h"

class WorldBossManager
{
public:
    WorldBossManager();
    ~WorldBossManager();

    DECLARE_SINGLETON(WorldBossManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

    void SaveWorldBossInfo();
    void Init();

    WORLD_BOSS_INFO_LIST& GetWorldBossList() {return m_nextBossList;}
private:
    void RecvWorldBossStart(void* data);
    void RecvWorldBossEnd(void*  data);

    void FillWorldBoss(void* data);
    void SendWorldBossList(uint32 gameserverID = 0);

    WORLD_BOSS_INFO_LIST m_nextBossList;
};


#endif // world_boss_manager_h__
