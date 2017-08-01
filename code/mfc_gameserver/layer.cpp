#include "stdafx.h"
#include "layer.h"
#include "canvas.h"

Gdiplus::Color Layer::m_CellColor = Gdiplus::Color(200, 200, 200);      // 剖分格颜色
Gdiplus::Color Layer::m_FontColor = Gdiplus::Color(100, 100, 100);      // 文字颜色
Gdiplus::Color Layer::m_WallColor = Gdiplus::Color(100, 100, 100);      // 墙颜色
Gdiplus::Color Layer::m_NavigateColor = Gdiplus::Color(0, 200, 0);      // 导航网格颜色
Gdiplus::Color Layer::m_BoundingBoxColor = Gdiplus::Color(230, 0, 0);   // 包围框颜色
Gdiplus::Color Layer::m_GridColor = Gdiplus::Color(0, 255, 255);          // 网格格子颜色

Gdiplus::Color Layer::m_DefaultColor = Gdiplus::Color(0, 0, 0);
Gdiplus::Color Layer::m_PlayerColor = Gdiplus::Color(0, 220, 0);
Gdiplus::Color Layer::m_MonsterColor = Gdiplus::Color(220, 0, 0);
Gdiplus::Color Layer::m_BossColor = Gdiplus::Color(255, 120, 0);
Gdiplus::Color Layer::m_NpcColor = Gdiplus::Color(0, 0, 220);
Gdiplus::Color Layer::m_PetColor = Gdiplus::Color(0, 100, 0);
Gdiplus::Color Layer::m_TriggerColor = Gdiplus::Color(0, 255, 255);
Gdiplus::Color Layer::m_TriggerActiveColor = Gdiplus::Color(50, 200, 200);


Gdiplus::Color Layer::m_RedColor = Gdiplus::Color(255, 0, 0);           // 红
Gdiplus::Color Layer::m_GreenColor = Gdiplus::Color(0, 255, 0);         // 绿
Gdiplus::Color Layer::m_BlueColor = Gdiplus::Color(0, 0, 255);          // 兰
Gdiplus::Color Layer::m_GreyColor = Gdiplus::Color(150, 150, 150);      // 灰



Layer::Layer(Canvas* canvas) :
m_Canvas(canvas),
m_RenderDirty(true),
m_pLayerBmp(NULL)
{
    m_pCoWorld = NULL;
}

Layer::~Layer()
{

}

void Layer::render( Gdiplus::Graphics* graph )
{
    m_pScene = m_Canvas->GetDrawScene();
    m_pCoWorld = m_pScene->GetCoWorld();
    
    if(m_RenderDirty)
    {
        ClearBmp();

        Gdiplus::Graphics* layerGraph = Gdiplus::Graphics::FromImage(m_pLayerBmp);

        OnRender(layerGraph);

        delete layerGraph;
    }

    // 渲染该图层的bitmap到画布上
    Gdiplus::Status retStatus = graph->DrawImage(m_pLayerBmp, 0, 0);
    _ASSERT(retStatus == Gdiplus::Ok);
}

void Layer::ToRenderPos(POINT& p)
{
    if(!m_pCoWorld)
        return;

    p.x -= m_pCoWorld->GetCellMinX() * m_pCoWorld->GetCellLength();
    p.y -= m_pCoWorld->GetCellMinY() * m_pCoWorld->GetCellLength();

    p.y = ( (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY() + 1) * m_pCoWorld->GetCellLength() ) - p.y;

    p.x *= m_Canvas->GetScale();
    p.y *= m_Canvas->GetScale();
}

void Layer::ToScenePos(POINT& p)
{
    if(!m_pCoWorld)
        return;

    p.x /= m_Canvas->GetScale();
    p.y /= m_Canvas->GetScale();

    p.y = ( (m_pCoWorld->GetCellMaxY() - m_pCoWorld->GetCellMinY() + 1) * m_pCoWorld->GetCellLength() ) - p.y;

    p.x += m_pCoWorld->GetCellMinX() * m_pCoWorld->GetCellLength();
    p.y += m_pCoWorld->GetCellMinY() * m_pCoWorld->GetCellLength();
}

void Layer::UpdateSize()
{
    ClearBmp();
}

void Layer::ClearBmp()
{
    if(m_pLayerBmp)
    {
        delete m_pLayerBmp;
        m_pLayerBmp = NULL;
    }

    m_pLayerBmp = new Gdiplus::Bitmap(m_Canvas->GetWidth(), m_Canvas->GetHeight());

    m_RenderDirty = true;
}

void Layer::DrawText( Gdiplus::Graphics* graph, const WCHAR* wstr, int x, int y, Gdiplus::Color *color )
{
    Gdiplus::SolidBrush  brush(*color);
    Gdiplus::FontFamily  fontFamily(L"Times New Roman");
    Gdiplus::Font        font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF      pointF(x, y);

    graph->DrawString(wstr, -1, &font, pointF, &brush);
}
