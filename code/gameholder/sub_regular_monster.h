/*
 *	大地图刷怪规则
 */
#ifndef sub_regular_monster_h__
#define sub_regular_monster_h__

#include "sub_regular.h"
#include "world_map_helper.h"

class Player;
class Scene;
class RegularDoc;
class RegularFiledBoss;

class SubRegularMonster : public SubRegular
{
public:
    SubRegularMonster(Regular* pRegular);
    virtual ~SubRegularMonster();

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);
private:
    //=====================================刷怪=====================================================
    void            OnBattleEventMonsterDead(uint32 monstercreateID, uint32 killerID,uint32 monseterid);///< 接收怪物死亡事件
    void            CheckDropItemTime();                                                                ///<.检查物品显示掉落

    RegularFiledBoss*            m_masterRegular;           ///< 管理者
};

#endif // sub_regular_monster_h__
