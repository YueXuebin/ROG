#ifndef regular_first_h__
#define regular_first_h__

#include "regular_rog_dungeon.h"

/*
 *	新手地图规则
 */
class RegularFirst : public RegularROGDungeon
{
public:
    RegularFirst(RegularDoc* pDoc);
    virtual ~RegularFirst();

    virtual void FirstUpdate();
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

private:
    void OnBattleEventPlayerKill(uint32 attackerID, uint32 defencerID);         // 接收角色死亡事件
    void OnPlayerResurgenceReq(int32 arg1); // 接受玩家复活事件

    void FirstDrop(Player* pPlayer, Monster* pMonster);

};



#endif // regular_first_h__
