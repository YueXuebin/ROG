/*
 *	世界BOSS规则
 */
#ifndef regular_world_boss_h__
#define regular_world_boss_h__

#include "regular_multiple.h"

class RegularDoc;

class RegularWorldBoss : public RegularMultiple
{
public:
    RegularWorldBoss(RegularDoc* pDoc);
    virtual ~RegularWorldBoss();

    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);

    virtual void    OneSecondUpdate(int32 deltatime);

    virtual void    FillStartPostion(Player* player, uint32 triggerID=0);

    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void    OnDungeonClear(bool success, std::vector<std::string>* arglist);
    virtual bool    IsGameStart();

    void Start();
    void End();
    void CreateWorldBoss(uint32 creatureID, uint32 dungeonID);

private:
    BossData    m_WorldBossData;
    uint32      m_dungeonID;        // 副本ID
    uint32      m_KillerID;         // 世界Boss的击杀者ID
};

#endif // regular_world_boss_h__