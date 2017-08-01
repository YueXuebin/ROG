#include "gameholder_pch.h"
#include "regular_field_004.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_004::RegularField_004(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_004::~RegularField_004()
{

}

void RegularField_004::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;





}