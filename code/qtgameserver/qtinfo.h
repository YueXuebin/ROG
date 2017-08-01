#ifndef QTINFO_H
#define QTINFO_H

#include <QGraphicsItem>

extern uint32  g_FrameCount;
extern uint32  g_FrameUsedTime;

class QtInfo : public QGraphicsItem
{
public:
    QtInfo();
    ~QtInfo();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    
};

#endif // QTINFO_H
