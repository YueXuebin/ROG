#include "qtgameserver_pch.h"
#include "qtinfo.h"

QtInfo::QtInfo()
    : QGraphicsItem()
{

}

QtInfo::~QtInfo()
{

}

QRectF QtInfo::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
        36 + adjust, 60 + adjust);
}

void QtInfo::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    painter->setPen(QColor(0,0,0));
    char str[100];
    sprintf_s(str, "frame:%d %0.1f%%", g_FrameCount, (real)g_FrameUsedTime/g_ServerUpdateTime*100);
    painter->drawText(0, 0, str);

    


}
