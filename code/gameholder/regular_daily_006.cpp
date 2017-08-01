#include "gameholder_pch.h"
#include "regular_daily_006.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularDaily_006::RegularDaily_006(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_006::~RegularDaily_006()
{

}

void RegularDaily_006::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}