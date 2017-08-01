#include "gameholder_pch.h"
#include "regular_yijie_012.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularYiJie_012::RegularYiJie_012(RegularDoc* pDoc) :
RegularYiJieBase(pDoc)
{

}

RegularYiJie_012::~RegularYiJie_012()
{

}

void RegularYiJie_012::FirstUpdate()
{
    RegularYiJieBase::FirstUpdate();

    // 木头人
    PointMonster* pPointMonster = CnNew PointMonster(this, 1);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3055, 1);

    pPointMonster = CnNew PointMonster(this, 2);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3056, 1);
    
    pPointMonster = CnNew PointMonster(this, 3);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3057, 1);

    pPointMonster = CnNew PointMonster(this, 4);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3058, 1);

    pPointMonster = CnNew PointMonster(this, 5);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3059, 1);

    pPointMonster = CnNew PointMonster(this, 6);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3060, 1);

    pPointMonster = CnNew PointMonster(this, 7);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3061, 1);

    pPointMonster = CnNew PointMonster(this, 8);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3062, 1);

    pPointMonster = CnNew PointMonster(this, 9);
    pPointMonster->SetRadius(0);
    pPointMonster->SetLoop(true);
    pPointMonster->SetMax(1);
    pPointMonster->SetSpawnSpeed(1000);
    pPointMonster->SetMonster(3063, 1);


    // 点1:Npc
    //pPointMonster = CnNew PointMonster(this, 1);
    //pPointMonster->SetRadius(0);
    //pPointMonster->SetMonster(1005, 1);
    //pPointMonster->SetGroup(FRIEND_GROUP);

    // 点101：木桶
    PointBarrel* pPointBarrel = CnNew PointBarrel(this, 101);
    pPointBarrel->SetRadius(300);
    pPointBarrel->SetLoop(true);
    pPointBarrel->SetNumber(3);

    // 点301：宝箱
    PointTreasure* pPointTreasure = CnNew PointTreasure(this, 301);
    pPointTreasure->SetShowID(4001);

    // 点401：触摸石
    pPointTreasure = CnNew PointTreasure(this, 401);
    pPointTreasure->SetShowID(4002);

    // 点201: 采集点
    PointTouch* pPointTouch = CnNew PointTouch(this, 201);
    pPointTouch->SetShowID(4003);
    pPointTouch->SetTouchTime(4000);
    



}