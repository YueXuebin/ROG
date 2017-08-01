#include "stdafx.h"
#include "scene_layer.h"
#include "canvas.h"

SceneLayer::SceneLayer( Canvas* canvas ) :
Layer(canvas)
{
    m_pCoWorld = NULL;

    m_ShowWall = true;
    m_ShowNav = false;
    m_ShowCell = true;
}

SceneLayer::~SceneLayer()
{

}

void SceneLayer::OnRender( Gdiplus::Graphics* graph )
{
    m_RenderDirty = false;

    Scene* pScene = m_Canvas->GetDrawScene();
    m_pCoWorld = pScene->GetCoWorld();

    if(m_ShowCell)
        drawCoWorldCell(graph);         // 画剖分格
    if(m_ShowWall)
        drawWalls(graph);               // 画墙
    if(m_ShowNav)
        drawNavigations(graph);         // 画导航网格


    Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 255));
    
    // 边框
    graph->DrawLine(&pen, 0 ,0, m_Canvas->GetWidth()-1, 0);
    graph->DrawLine(&pen, m_Canvas->GetWidth()-1, 0, m_Canvas->GetWidth()-1, m_Canvas->GetHeight()-1);
    graph->DrawLine(&pen, m_Canvas->GetWidth()-1, m_Canvas->GetHeight()-1, 0, m_Canvas->GetHeight()-1);
    graph->DrawLine(&pen, 0 ,m_Canvas->GetHeight()-1, 0, 0);
}

void SceneLayer::drawWalls(Gdiplus::Graphics* graph)
{
    Gdiplus::Pen wallPen(m_WallColor);

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

        graph->DrawLine(&wallPen, p0.x, p0.y, p1.x, p1.y);
        graph->DrawLine(&wallPen, p1.x, p1.y, p2.x, p2.y);
        graph->DrawLine(&wallPen, p2.x, p2.y, p0.x, p0.y);
    }
}

void SceneLayer::drawNavigations(Gdiplus::Graphics* graph)
{
    Gdiplus::Pen navPen(m_NavigateColor);

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

        graph->DrawLine(&navPen, p0.x, p0.y, p1.x, p1.y);
        graph->DrawLine(&navPen, p1.x, p1.y, p2.x, p2.y);
        graph->DrawLine(&navPen, p2.x, p2.y, p0.x, p0.y);
    }
}

void SceneLayer::drawCoWorldCell(Gdiplus::Graphics* graph)
{
    int CellLength = m_pCoWorld->GetCellLength();

    CellLength *= m_Canvas->GetScale();
    int LenY = (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY());
    int LenX = (m_pCoWorld->GetCellMaxX() - m_pCoWorld->GetCellMinX());

    Gdiplus::Pen cellPen(m_CellColor);

    for(int y = m_pCoWorld->GetCellMinY(); y <= m_pCoWorld->GetCellMaxY(); y++)
    {
        for(int x = m_pCoWorld->GetCellMinX(); x <= m_pCoWorld->GetCellMaxX(); x++)
        {
            // 画剖分格
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

            graph->DrawLine(&cellPen, p0.x, p0.y, p0.x, p1.y);
            graph->DrawLine(&cellPen, p0.x, p1.y, p1.x, p1.y);
            graph->DrawLine(&cellPen, p1.x, p1.y, p1.x, p0.y);
            graph->DrawLine(&cellPen, p1.x, p0.y, p0.x, p0.y);

            if(pCellGrid->GetState() == CELL_BLOCK)
            {
                graph->DrawLine(&cellPen, p0.x, p0.y, p1.x, p1.y);
                graph->DrawLine(&cellPen, p0.x, p1.y, p1.x, p0.y);
            }

            // 写字
            //painter->setPen(m_FontColor);

            //// 三角数
            //int triCount = pCellGrid->GetTrianglesCount();
            //// 对象
            //int dynCount = pCellGrid->GetDynObjects().size();
            //// 碰撞
            //int colCount = pCellGrid->GetCollObjects().size();

            //char str[100];
            //if(m_CellInfoType == 0)
            //{   // 三角数
            //    sprintf_s(str, "%d", triCount);
            //}
            //else if(m_CellInfoType == 1)
            //{   // 对象
            //    sprintf_s(str, "%d", dynCount);
            //}
            //else
            //{
            //    sprintf_s(str, "%d", colCount);
            //}
            //painter->drawText(p0.x, p0.y, str);

            //sprintf_s(str, "[%d,%d]", x, y);
            //painter->drawText(p0.x, (p1.y-p0.y)/2+p0.y, str);
        }
    }
}

void SceneLayer::ShowWall( bool show )
{
    m_ShowWall = show;
    m_RenderDirty = true;
}

void SceneLayer::ShowNav( bool show )
{
    m_ShowNav = show;
    m_RenderDirty = true;
}

void SceneLayer::ShowCell( bool show )
{
    m_ShowCell = show;
    m_RenderDirty = true;
}
