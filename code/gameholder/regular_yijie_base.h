#ifndef regular_yijie_base_h__
#define regular_yijie_base_h__

#include "regular_rog_dungeon.h"

/*
 *	冒险地图关卡基类
 */
class RegularYiJieBase : public RegularROGDungeon
{
    CnDeclareRTTI
public:
    RegularYiJieBase(RegularDoc *pDoc);
    virtual ~RegularYiJieBase();

    virtual void OneSecondUpdate(int32 deltatime);

    virtual void Update(uint32 curr_tick);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);



private:
    void OnBattleEventPlayerKill(uint32 attackerID, uint32 defencerID);         // 接收角色死亡事件
    void OnBattleEventMonsterDead(uint32 monsterID, uint32 killerID);           // 接收怪物死亡事件


};


#endif // regular_maoxian_base_h__
