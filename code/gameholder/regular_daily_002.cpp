#include "gameholder_pch.h"
#include "regular_daily_002.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularDaily_002::RegularDaily_002(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_002::~RegularDaily_002()
{

}

void RegularDaily_002::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}