#include "qtgameserver_pch.h"
#include "qtgameview.h"
#include "qtcreature.h"
#include "qtobject.h"
#include "qtinfo.h"
#include "dungeon_type.h"


float g_Scale = 0.1f;      // 缩放比例

QtGameView::QtGameView(QWidget *parent)
    : QGraphicsView(parent)
{
    m_SceneIndex = 0;

    m_pGraphicsScene = new QGraphicsScene(this);
    m_pGraphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    m_pGraphicsScene->setSceneRect(0, 0, 700, 700);

    setScene(m_pGraphicsScene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);

    m_pCoWorld = NULL;

    m_CellColor.setRgb(200, 200, 200);      // 剖分格颜色
    m_FontColor.setRgb(100, 100, 100);      // 文字颜色
    m_WallColor.setRgb(100, 100, 100);      // 墙颜色
    m_NavigateColor.setRgb(0, 200, 0);      // 导航网格颜色
    m_BoundingBoxColor.setRgb(230, 0, 0);   // 包围框颜色

    m_CellInfoType = 0;
    m_TriangleInfoType = 0;
    m_BoundingBoxType = 0;

    m_pInfo = new QtInfo;
    m_pGraphicsScene->addItem(m_pInfo);

    m_ChannelId = 1;
    m_DungeonId = 1;
    m_SceneId = 1;
}

QtGameView::~QtGameView()
{

}

void QtGameView::Update()
{
    UpdateCreatures();
	UpdateSceneObjects();   
}

Scene*  QtGameView::GetDrawScene()
{
    return DungeonManager::Instance()->GetScene(UNION_ID(m_ChannelId, m_DungeonId, m_SceneId));
}

void QtGameView::UpdateCreatures()
{
    m_drawCreatureCount = 0;

    Scene* pScene = GetDrawScene();
    if(pScene)
    {
        for(Scene::BattleUnitMap::iterator citer = pScene->GetBattleUnits().begin(); citer != pScene->GetBattleUnits().end(); citer++)
        {
            UpdateCreature(citer->second);
        }
    }

    // 隐藏多余
    for(int i=0; i<m_pAllCreature.size(); i++)
    {
        if(i>=m_drawCreatureCount)
        {
            m_pAllCreature[i]->hide();
            m_pAllCreature[i]->SetBattleUnit(NULL);
        }
    }

}

void QtGameView::UpdateCreature(BattleUnit* pBattleUnit)
{
    m_drawCreatureCount++;
    while(m_pAllCreature.size() < m_drawCreatureCount)
    {
        qtCreature *newCreature = new qtCreature;
        newCreature->SetGameView(this);
        m_pAllCreature.push_back(newCreature);
        m_pGraphicsScene->addItem(newCreature);
    }

    qtCreature* pQtCreature = m_pAllCreature[m_drawCreatureCount-1];

    POINT p;
    p.x = pBattleUnit->GetX();
    p.y = pBattleUnit->GetY();

    ToRenderPos(p);

    pQtCreature->SetBattleUnit(pBattleUnit);
    pQtCreature->setPos(p.x, p.y);
    pQtCreature->setRotation(-pBattleUnit->GetAngle());
	pQtCreature->setToolTip("tips");
    pQtCreature->show();

}

void QtGameView::UpdateSceneObjects()
{
	m_drawObjectCount = 0;

	Scene* pScene = GetDrawScene();
    if(!pScene)
        return;

	for(Scene::SceneObjectMap::iterator citer = pScene->GetObjects().begin(); citer != pScene->GetObjects().end(); citer++)
	{
		UpdateSceneObject(citer->second);
	}

    // 隐藏多余
    for(int i=0; i<m_pAllObject.size(); i++)
    {
        if(i>=m_drawObjectCount)
        {
            m_pAllObject[i]->hide();
            m_pAllObject[i]->SetObject(NULL);
        }
    }
}

void QtGameView::UpdateSceneObject(SceneObject* pObject)
{
	m_drawObjectCount++;
	while(m_pAllObject.size() < m_drawObjectCount)
	{
		qtObject *newTrigger = new qtObject;
        newTrigger->SetGameView(this);
		m_pAllObject.push_back(newTrigger);
		m_pGraphicsScene->addItem(newTrigger);
	}

	qtObject* pQtTrigger = m_pAllObject[m_drawObjectCount-1];

	POINT p;
	p.x = pObject->GetX();
	p.y = pObject->GetY();

	ToRenderPos(p);

	pQtTrigger->SetObject(pObject);
	pQtTrigger->setPos(p.x, p.y);
	pQtTrigger->setRotation(-pObject->GetAngle());
	pQtTrigger->setToolTip("trigger");
	pQtTrigger->show();
}

void QtGameView::drawBackground( QPainter *painter, const QRectF &rect )
{
    Scene* pScene = GetDrawScene();
    if(!pScene) return;
    m_pCoWorld = pScene->GetCoWorld();

    drawInfo(painter);              // 信息
    drawCoWorldCell(painter);       // 画剖分格

    if(m_TriangleInfoType == 0)
        drawTriangles(painter);         // 画三角
    else if(m_TriangleInfoType == 1)
        drawNavigations(painter);       // 画导航网格
    else
    {  
        drawTriangles(painter);
        drawNavigations(painter);
    }
    
    if(m_BoundingBoxType)
        drawBoundingBox(painter);       // 画包围框
}

void QtGameView::drawInfo(QPainter *painter)
{
    painter->setPen(m_FontColor);

    char str[100];
    if(m_CellInfoType == 0)
        sprintf_s(str, "tri");
    else if(m_CellInfoType == 1)
        sprintf_s(str, "dyn");
    else if(m_CellInfoType == 2)
        sprintf_s(str, "col");
    else
        sprintf_s(str, "nav");

    painter->drawText(300, 0, str);

	// pawn info
	Scene* pScene = GetDrawScene();
    if(!pScene)
        return;
	painter->setPen(PlayerColor);
	sprintf_s(str, "Player: [%d]", pScene->GetPlayers().size());
	painter->drawText(-100, 30, str);

	painter->setPen(MonsterColor);
	sprintf_s(str, "Monster:[%d]", pScene->GetMonsters().size());
	painter->drawText(-100, 45, str);

    painter->setPen(BossColor);
    sprintf_s(str, "Boss:   [%d]", pScene->GetBosses().size());
    painter->drawText(-100, 60, str);

	painter->setPen(NpcColor);
	sprintf_s(str, "NPC:    [%d]", pScene->GetNpcs().size());
	painter->drawText(-100, 75, str);

    painter->setPen(PetColor);
    sprintf_s(str, "PET:    [%d]", pScene->GetPets().size());
    painter->drawText(-100, 90, str);

	painter->setPen(TriggerColor);
	sprintf_s(str, "Trigger:(%d)", pScene->GetTriggers().size());
	painter->drawText(-100, 105, str);
}

void QtGameView::drawBoundingBox(QPainter *painter)
{
    painter->setPen(m_BoundingBoxColor);

    Scene* pScene = GetDrawScene();
    if(!pScene)
        return;
    for(Scene::PlayerMap::const_iterator citer = pScene->GetPlayers().begin(); citer != pScene->GetPlayers().end(); citer++)
    {
        Player* pPlayer= citer->second;
        BoundingBoxInt bbox = pPlayer->GetCoObject()->GetBoundingBox();

        POINT p0, p1;
        p0.x = bbox.Pos().X;
        p0.y = bbox.Pos().Y;
        p1.x = bbox.Pos().X + bbox.Size().X;
        p1.y = bbox.Pos().Y + bbox.Size().Y;
        
        p0.x *= m_pCoWorld->GetCellLength();
        p0.y *= m_pCoWorld->GetCellLength();
        p1.x *= m_pCoWorld->GetCellLength();
        p1.y *= m_pCoWorld->GetCellLength();

        ToRenderPos(p0);
        ToRenderPos(p1);

        painter->drawLine(p0.x, p0.y, p1.x, p0.y);
        painter->drawLine(p1.x, p0.y, p1.x, p1.y);
        painter->drawLine(p1.x, p1.y, p0.x, p1.y);
        painter->drawLine(p0.x, p1.y, p0.x, p0.y);
    }

}

void QtGameView::drawNavigations(QPainter *painter)
{
    painter->setPen(m_NavigateColor);
    
    const std::vector<CoPolygon*> navs = m_pCoWorld->GetNavigations();
    for (std::vector<CoPolygon*>::const_iterator iter=navs.begin(); iter != navs.end(); iter++)
    {
        CoPolygon* pTriangle = *iter;
        const std::vector<CoVec2>& points = pTriangle->Points();

        POINT p0, p1, p2;
        p0.x = points[0].x;
        p0.y = points[0].y;
        p1.x = points[1].x;
        p1.y = points[1].y;
        p2.x = points[2].x;
        p2.y = points[2].y;

        ToRenderPos(p0);
        ToRenderPos(p1);
        ToRenderPos(p2);

        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
        painter->drawLine(p1.x, p1.y, p2.x, p2.y);
        painter->drawLine(p2.x, p2.y, p0.x, p0.y);
    }
}

void QtGameView::drawTriangles(QPainter *painter)
{
    painter->setPen(m_WallColor);

    const std::vector<CoPolygon*> walls = m_pCoWorld->GetTriangles();
    for (std::vector<CoPolygon*>::const_iterator iter=walls.begin(); iter != walls.end(); iter++)
    {
        CoPolygon* pTriangle = *iter;
        const std::vector<CoVec2>& points = pTriangle->Points();

        POINT p0, p1, p2;
        p0.x = points[0].x;
        p0.y = points[0].y;
        p1.x = points[1].x;
        p1.y = points[1].y;
        p2.x = points[2].x;
        p2.y = points[2].y;

        ToRenderPos(p0);
        ToRenderPos(p1);
        ToRenderPos(p2);

        painter->drawLine(p0.x, p0.y, p1.x, p1.y);
        painter->drawLine(p1.x, p1.y, p2.x, p2.y);
        painter->drawLine(p2.x, p2.y, p0.x, p0.y);
    }

}

void QtGameView::drawCoWorldCell(QPainter *painter)
{
    int CellLength = m_pCoWorld->GetCellLength();

    CellLength *= g_Scale;
    int LenY = (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY());
    int LenX = (m_pCoWorld->GetCellMaxX() - m_pCoWorld->GetCellMinX());

    for(int y = m_pCoWorld->GetCellMinY(); y <= m_pCoWorld->GetCellMaxY(); y++)
    {
        for(int x = m_pCoWorld->GetCellMinX(); x <= m_pCoWorld->GetCellMaxX(); x++)
        {
            // 画剖分格
            painter->setPen(m_CellColor);
            int dx = x - m_pCoWorld->GetCellMinX();
            int dy = y - m_pCoWorld->GetCellMinY();

            CoWorld::Cell* pCellGrid = m_pCoWorld->GetCellGrid(x, y);
            _ASSERT(pCellGrid);

            POINT p0, p1;
            p0.x = pCellGrid->m_LeftTop.x;
            p0.y = pCellGrid->m_LeftTop.y;
            p1.x = pCellGrid->m_RightBottom.x;
            p1.y = pCellGrid->m_RightBottom.y;
            
            ToRenderPos(p0);
            ToRenderPos(p1);

            painter->drawLine(p0.x, p0.y, p0.x, p1.y);
            painter->drawLine(p0.x, p1.y, p1.x, p1.y);
            painter->drawLine(p1.x, p1.y, p1.x, p0.y);
            painter->drawLine(p1.x, p0.y, p0.x, p0.y);

            if(pCellGrid->GetState() == CELL_BLOCK)
            {
                painter->drawLine(p0.x, p0.y, p1.x, p1.y);
                painter->drawLine(p0.x, p1.y, p1.x, p0.y);
            }

            // 写字
            painter->setPen(m_FontColor);
            
            // 三角数
            int triCount = pCellGrid->GetTrianglesCount();
            // 对象
            int dynCount = pCellGrid->GetDynObjects().size();
            // 碰撞
            int colCount = pCellGrid->GetCollObjects().size();

            char str[100];
            if(m_CellInfoType == 0)
            {   // 三角数
                sprintf_s(str, "%d", triCount);
            }
            else if(m_CellInfoType == 1)
            {   // 对象
                sprintf_s(str, "%d", dynCount);
            }
            else
            {
                sprintf_s(str, "%d", colCount);
            }
            painter->drawText(p0.x, p0.y, str);

			//sprintf_s(str, "[%d,%d]", x, y);
			//painter->drawText(p0.x, (p1.y-p0.y)/2+p0.y, str);
        }
    }
}

void QtGameView::ToRenderPos(POINT& p)
{
    if(!m_pCoWorld)
        return;

    p.x -= m_pCoWorld->GetCellMinX() * m_pCoWorld->GetCellLength();
    p.y -= m_pCoWorld->GetCellMinY() * m_pCoWorld->GetCellLength();

    p.y = ( (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY() + 1) * m_pCoWorld->GetCellLength() ) - p.y;

    p.x *= g_Scale;
    p.y *= g_Scale;
}

void QtGameView::ToScenePos(POINT& p)
{
    if(!m_pCoWorld)
        return;

    p.x /= g_Scale;
    p.y /= g_Scale;

    p.x += m_pCoWorld->GetCellMinX() * m_pCoWorld->GetCellLength();
    p.y += m_pCoWorld->GetCellMinY() * m_pCoWorld->GetCellLength();
}
