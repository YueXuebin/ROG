#ifndef QTTRIGGER_H
#define QTTRIGGER_H

#include <QGraphicsItem>

class QtGameView;

class qtObject : public QGraphicsItem
{
public:
	qtObject();
	~qtObject();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void SetObject(SceneObject* pObject);
    void SetGameView(QtGameView* pView) {m_pView = pView;}

private:
	SceneObject*       m_pObject;
    QtGameView*     m_pView;
};

#endif // QTTRIGGER_H