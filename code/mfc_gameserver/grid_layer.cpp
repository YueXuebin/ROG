#include "stdafx.h"
#include "grid_layer.h"
#include "canvas.h"


GridLayer::GridLayer(Canvas* canvas) :
Layer(canvas)
{
    m_ShowGrid = false;
}

GridLayer::~GridLayer()
{

}

void GridLayer::OnRender(Gdiplus::Graphics* graph)
{
    if(!m_ShowGrid)
        return;

    Scene* pScene = m_Canvas->GetDrawScene();

    GridWorld* pGridWord = pScene->GetGridWorld();
    uint32 cellLength = pGridWord->GetCellLength();

    GridCell* root = pGridWord->GetRoot();

    Gdiplus::Pen cellPen(m_GridColor);

    // 画出所有有单位的格子
    GridCell* hereX = root->right;
    while(hereX)
    {
        GridCell* hereY = hereX->down;
        while (hereY)
        {
            CnAssert(hereY->HasObject());

            // 画格子对应的矩形

            int32 cellX = hereY->GetX();
            int32 cellY = hereY->GetY();

            POINT p0, p1;
            p0.x = cellX * cellLength;
            p0.y = cellY * cellLength;
            p1.x = (cellX+1) * cellLength;
            p1.y = (cellY+1) * cellLength;

            ToRenderPos(p0);
            ToRenderPos(p1);

            graph->DrawLine(&cellPen, p0.x, p0.y, p0.x, p1.y);
            graph->DrawLine(&cellPen, p0.x, p1.y, p1.x, p1.y);
            graph->DrawLine(&cellPen, p1.x, p1.y, p1.x, p0.y);
            graph->DrawLine(&cellPen, p1.x, p0.y, p0.x, p0.y);

            hereY = hereY->down;
        }
        hereX = hereX->right;
    }



}
