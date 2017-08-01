#ifndef QTCREATURE_H
#define QTCREATURE_H

#include <QGraphicsItem>

class QtGameView;

class qtCreature : public QGraphicsItem
{
public:
    qtCreature();
    ~qtCreature();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void SetBattleUnit(BattleUnit* pBattleUnit);
    void SetGameView(QtGameView* pView) {m_pView = pView;}
    
private:
    QtGameView*     m_pView;
    BattleUnit*     m_pBattleUnit;
};

#endif // QTCREATURE_H
