#include "qtgameserver_pch.h"
#include "qtcreature.h"
#include "qtgameview.h"

qtCreature::qtCreature()
    : QGraphicsItem()
{

}

qtCreature::~qtCreature()
{

}

QRectF qtCreature::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
        36 + adjust, 60 + adjust);
}


void qtCreature::SetBattleUnit( BattleUnit* pBattleUnit )
{
    m_pBattleUnit = pBattleUnit;
}

void qtCreature::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if(!m_pView->GetDrawScene())
        return;

    if(!m_pBattleUnit)
        return;

    CoDynamicObject* pDynObject = m_pBattleUnit->GetCoObject();
    if(!pDynObject)
        return;

    int radius = pDynObject->GetRadius() * g_Scale;
    // Body
    if (m_pBattleUnit->GetObjectType() == OBJ_MONSTER)
	    painter->setPen(MonsterColor);
    else if (m_pBattleUnit->GetObjectType() == OBJ_BOSS)
        painter->setPen(BossColor);
	else if (m_pBattleUnit->GetObjectType() == OBJ_PLAYER)
        painter->setPen(PlayerColor);
	else if (m_pBattleUnit->GetObjectType() == OBJ_NPC)
		painter->setPen(NpcColor);
    else if (m_pBattleUnit->GetObjectType() == OBJ_BATTLE_PET)
        painter->setPen(PetColor);
	else 
		painter->setPen(DefaultColor);

    painter->drawLine(-radius, -radius, radius, -radius);
    painter->drawLine(radius, -radius, radius, radius);
    painter->drawLine(radius, radius, -radius, radius);
    painter->drawLine(-radius, radius, -radius, -radius);
    
    // Direction
    painter->drawLine(0, 0, radius*2, 0);


    char str[100];
    // 状态
    painter->setPen(QColor(255, 0, 0));
    uint32 state = m_pBattleUnit->GetState();
    sprintf_s(str, 100, "%d", state);
    painter->drawText(-5,-5,str);

    // AI
    if(m_pBattleUnit->GetAIController())
    {
        painter->setPen(QColor(0, 0, 255));
        uint32 aistate = m_pBattleUnit->GetAIController()->GetAIState();
        sprintf_s(str, 100, "%d", aistate);
        painter->drawText(5,-5,str);
    }

    // 文字
    //if (m_pCreature->GetObjectType() == OBJ_MONSTER)
    //{
    //    Monster* pMonster = (Monster*)m_pCreature;

    //    // AI状态
    //    uint32 aiState = pMonster->GetAIState();

    //    char str[100];
    //    sprintf_s(str, 100, "ai=%d", aiState);
    //    painter->drawText(-5,-5,str);
    //}
/*	else if (m_pCreature->GetObjectType() == OBJ_NPC)
	{
		Npc* pNpc = (Npc *)m_pCreature;

		uint32 aiState = pNpc->GetAIState();

		char str[100];
		sprintf_s(str, 100, "%d", pNpc->GetId());
		painter->drawText(-5,-5,str);
	}
*/
}

