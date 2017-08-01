#include "gameholder_pch.h"
#include "regular_home.h"
#include "player.h"



RegularHome::RegularHome(RegularDoc* pDoc) : 
RegularSingle(pDoc)
{

}

RegularHome::~RegularHome()
{

}

void RegularHome::OnCreatureEnter(BattleUnit* creature)
{
    RegularSingle::OnCreatureEnter(creature);

    creature->SetGroup(FRIEND_GROUP);           // 领地所有人都是队友

}

void RegularHome::FirstUpdate()
{
    RegularSingle::FirstUpdate();

}

