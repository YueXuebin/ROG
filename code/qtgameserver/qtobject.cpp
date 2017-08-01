#include "qtgameserver_pch.h"
#include "qtobject.h"
#include "qtgameview.h"

qtObject::qtObject()
: QGraphicsItem()
{

}

qtObject::~qtObject()
{

}

QRectF qtObject::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
        36 + adjust, 60 + adjust);
}


void qtObject::SetObject(SceneObject* pObject)
{
    m_pObject = pObject;
}

void qtObject::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if(!m_pView->GetDrawScene())
        return;

    if(!m_pObject)
        return;

    CoDynamicObject* pDynObject = m_pObject->GetCoObject();

    int radius = 50;
    if(pDynObject)
        radius = pDynObject->GetRadius() * g_Scale;
    else
        radius = 50 * g_Scale;

    if (m_pObject->GetObjectType() == OBJ_TRIGGER)
        painter->setPen(TriggerColor);
    else 
        painter->setPen(DefaultColor);

    // Ô²
    painter->drawEllipse(-radius, -radius, radius*2, radius*2);

    // ·½Ïò
    painter->drawLine(0, 0, radius, 0);

    // ¼¤»î·¶Î§
    if(m_pObject->GetObjectType() == OBJ_TRIGGER)
    {
        painter->setPen(TriggerActiveColor);
        
        Trigger* pTrigger = CnDynamicCast(Trigger, m_pObject);
        int32 activeRadius = pTrigger->GetActiveRadius();
        activeRadius = activeRadius * g_Scale;

        painter->drawEllipse(-activeRadius, -activeRadius, activeRadius*2, activeRadius*2);

    }

}

