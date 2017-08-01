#include "gameholder_pch.h"
#include "regular_field_005.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_005::RegularField_005(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_005::~RegularField_005()
{

}

void RegularField_005::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}