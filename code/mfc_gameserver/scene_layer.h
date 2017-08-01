/*
 *	³¡¾°Í¼²ã
 */
#ifndef scene_layer_h__
#define scene_layer_h__

#include "layer.h"

class SceneLayer : public Layer
{
public:
    SceneLayer(Canvas* canvas);
    virtual ~SceneLayer();

    virtual void OnRender(Gdiplus::Graphics* graph);

    void    ShowWall(bool show);
    bool    IsShowWall() {return m_ShowWall;}

    void    ShowNav(bool show);
    bool    IsShowNav() {return m_ShowNav;}

    void    ShowCell(bool show);
    bool    IsShowCell() {return m_ShowCell;}

private:
    void drawCoWorldCell(Gdiplus::Graphics* graph);         // »­ÆÊ·Ö¸ñ
    void drawWalls(Gdiplus::Graphics* graph);               // »­Ç½
    void drawNavigations(Gdiplus::Graphics* graph);         // »­µ¼º½Íø¸ñ

private:
    bool    m_ShowWall;
    bool    m_ShowNav;
    bool    m_ShowCell;

};


#endif // scene_layer_h__
