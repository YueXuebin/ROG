#include "gameholder_pch.h"
#include "regular_city.h"
#include "point_monster.h"
#include "battle_unit.h"


RegularCity::RegularCity(RegularDoc* pDoc) : 
RegularWorld(pDoc)
{

}

RegularCity::~RegularCity()
{

}

void RegularCity::FirstUpdate()
{
    RegularWorld::FirstUpdate();

    SetMapLevel(75);

    PointMonster* pPointMonster = NULL;
    // 点1：npc1
    pPointMonster = CnNew PointMonster(this, 1);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMax(1);
    pPointMonster->SetMonster(3006, 1);
    pPointMonster->SetGroup(FRIEND_GROUP);
    pPointMonster->SetTalk(true);
    pPointMonster->SetCantMove(true);
    pPointMonster->SetAlwaysShow(true);
    

    // 点2：npc2
    pPointMonster = CnNew PointMonster(this, 2);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMax(1);
    pPointMonster->SetMonster(3008, 1);
    pPointMonster->SetGroup(FRIEND_GROUP);
    pPointMonster->SetTalk(true);
    pPointMonster->SetCantMove(true);
    pPointMonster->SetAlwaysShow(true);

    //// 点3：npc3
    //pPointMonster = CnNew PointMonster(this, 3);
    //pPointMonster->SetRadius(0);
    //pPointMonster->SetMax(1);
    //pPointMonster->SetMonster(3007, 1);
    //pPointMonster->SetGroup(FRIEND_GROUP);
    //pPointMonster->SetTalk(true);
    //pPointMonster->SetCantMove(true);
    //pPointMonster->SetAlwaysShow(true);

    //// 点4：npc4
    //pPointMonster = CnNew PointMonster(this, 4);
    //pPointMonster->SetRadius(0);
    //pPointMonster->SetMax(1);
    //pPointMonster->SetMonster(3008, 1);
    //pPointMonster->SetGroup(FRIEND_GROUP);
    //pPointMonster->SetTalk(true);
    //pPointMonster->SetCantMove(true);
    //pPointMonster->SetAlwaysShow(true);
}

void RegularCity::OnCreatureEnter(BattleUnit* creature)
{
    RegularWorld::OnCreatureEnter(creature);

    creature->SetGroup(FRIEND_GROUP);           // 主城所有人都是队友
}
