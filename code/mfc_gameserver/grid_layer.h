#ifndef grid_layer_h__
#define grid_layer_h__

#include "layer.h"

/*
 *	网格场景管理
 */
class GridLayer : public Layer
{
public:
    GridLayer(Canvas* canvas);
    virtual ~GridLayer();

    void ShowGrid(bool show) {m_ShowGrid = show;}
    bool IsShowGrid() {return m_ShowGrid;}

protected:
    virtual void OnRender(Gdiplus::Graphics* graph);

private:
    bool    m_ShowGrid;
};


#endif // grid_layer_h__
