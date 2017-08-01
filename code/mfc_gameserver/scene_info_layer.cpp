#include "stdafx.h"
#include "scene_info_layer.h"
#include "canvas.h"


SceneInfoLayer::SceneInfoLayer( Canvas* canvas ) :
Layer(canvas)
{
    m_InfoType = NONE;
}

SceneInfoLayer::~SceneInfoLayer()
{

}

void SceneInfoLayer::OnRender( Gdiplus::Graphics* graph )
{
    drawCellInfo(graph);
}

void SceneInfoLayer::drawCellInfo(Gdiplus::Graphics* graph)
{
    if(m_InfoType == NONE)
        return;

    Gdiplus::SolidBrush  brush(m_FontColor);
    Gdiplus::FontFamily  fontFamily(L"Times New Roman");
    Gdiplus::Font        font(&fontFamily, 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF      pointF(0.0f, 0.0f);

    int CellLength = m_pCoWorld->GetCellLength();

    CellLength *= m_Canvas->GetScale();
    int LenY = (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY());
    int LenX = (m_pCoWorld->GetCellMaxX() - m_pCoWorld->GetCellMinX());

    CStringW str;
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

            // 三角数
            int triCount = pCellGrid->GetTrianglesCount();
            // 对象
            int dynCount = pCellGrid->GetDynObjects().size();
            // 碰撞
            int colCount = pCellGrid->GetCollObjects().size();

            
            if(m_InfoType == TRIANGLE)
            {   // 三角数
                str.Format(L"%d", triCount);
            }
            else if(m_InfoType == OBJECT)
            {   // 对象
                str.Format(L"%d", dynCount);
            }
            else if(m_InfoType == COLLISION)
            {   // 碰撞
                str.Format(L"%d", colCount);
            }
            else if(m_InfoType == VISITOR)
            {
                if(m_pScene->IsAOINotifier())
                {
                    SceneNotifierAOI* pNotifer = (SceneNotifierAOI*)m_pScene->GetNotifier();
                    AOIManager* pAOIManager = pNotifer->GetAOIManager();
                    CellVisitor* cellVisitor = pAOIManager->GetCellVisitor(dx, dy);
                    str.Format(L"%d %d", cellVisitor->GetAOIObjects().size(), cellVisitor->GetSeePlayers().size());
                }
            }

            pointF.X = p0.x;
            pointF.Y = p1.y;
            
            graph->DrawString(str, -1, &font, pointF, &brush);

            //sprintf_s(str, "[%d,%d]", x, y);
            //painter->drawText(p0.x, (p1.y-p0.y)/2+p0.y, str);
        }
    }
}

void SceneInfoLayer::SwitchCellInfo()
{
    if(m_InfoType == TRIANGLE)
        m_InfoType = OBJECT;
    else if(m_InfoType == OBJECT)
        m_InfoType = COLLISION;
    else if(m_InfoType == COLLISION)
        m_InfoType = VISITOR;
    else if(m_InfoType == VISITOR)
        m_InfoType = NONE;
    else if(m_InfoType == NONE)
        m_InfoType = TRIANGLE;
    
}
