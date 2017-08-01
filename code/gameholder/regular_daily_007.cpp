#include "gameholder_pch.h"
#include "regular_daily_007.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularDaily_007::RegularDaily_007(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_007::~RegularDaily_007()
{

}

void RegularDaily_007::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}