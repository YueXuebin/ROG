#include "gameholder_pch.h"
#include "action_factory.h"
#include "player.h"
#include "action_creature_controller.h"
#include "action_player_controller.h"


ActionController* CreateAction(BattleUnit* pUnit)
{
    uint32 objType = pUnit->GetObjectType();

    // 为不同对象创建不同的控制器
    if(objType == OBJ_PLAYER)
        return CnNew ActionPlayerController((Player*)pUnit);
    else if(objType == OBJ_MONSTER || objType == OBJ_BOSS || objType == OBJ_PET || objType == OBJ_OFFLINE_PLAYER)
        return CnNew ActionCreatureController((Creature*)pUnit);
    else
        return CnNew ActionController(pUnit);
}
