#include "gameholder_pch.h"
#include "regular_daily_005.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularDaily_005::RegularDaily_005(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_005::~RegularDaily_005()
{

}

void RegularDaily_005::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}