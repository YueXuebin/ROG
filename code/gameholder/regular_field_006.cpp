#include "gameholder_pch.h"
#include "regular_field_006.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_006::RegularField_006(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_006::~RegularField_006()
{

}

void RegularField_006::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}