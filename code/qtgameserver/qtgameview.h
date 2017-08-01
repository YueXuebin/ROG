#ifndef QTGAMEVIEW_H
#define QTGAMEVIEW_H

#include <QGraphicsView>

class qtCreature;
class qtObject;
class QtInfo;

class QtGameView : public QGraphicsView
{
public:
    QtGameView(QWidget *parent = 0);
    ~QtGameView();

    void Update();
    int m_CellInfoType;
    int m_TriangleInfoType;
    int m_BoundingBoxType;

    uint32  m_ChannelId;
    uint32  m_DungeonId;
    uint32  m_SceneId;

    Scene*  GetDrawScene();
protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    void drawInfo(QPainter *painter);
    void drawCoWorldCell(QPainter *painter);
    void drawTriangles(QPainter *painter);
    void drawNavigations(QPainter *painter);
    void drawBoundingBox(QPainter *painter);

    void ToRenderPos(POINT& p);
    void ToScenePos(POINT& p);

    void UpdateCreatures();
    void UpdateCreature(BattleUnit* pCreature);

	void UpdateSceneObjects();
	void UpdateSceneObject(SceneObject* pObject);

   

private:
    CoWorld*    m_pCoWorld;

    QGraphicsScene* m_pGraphicsScene;
    QtInfo* m_pInfo;
    std::vector<qtCreature*>    m_pAllCreature;
    int32   m_drawCreatureCount;
	std::vector<qtObject*>		m_pAllObject;
	int32	m_drawObjectCount;
    
    uint32  m_SceneIndex;

    QColor m_CellColor;
    QColor m_FontColor;
    QColor m_WallColor;
    QColor m_NavigateColor;
    QColor m_BoundingBoxColor;

    
};

#endif // QTGAMEVIEW_H
