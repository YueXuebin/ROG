/*
 *	≥°æ∞–≈œ¢≤„
 */
#ifndef scene_info_layer_h__
#define scene_info_layer_h__

#include "layer.h"

class SceneInfoLayer : public Layer
{
public:
    SceneInfoLayer(Canvas* canvas);
    virtual ~SceneInfoLayer();

    enum
    {
        NONE,
        TRIANGLE,
        OBJECT,
        COLLISION,
        VISITOR,
    };

    void    SwitchCellInfo();

protected:
    virtual void OnRender(Gdiplus::Graphics* graph);

    void drawCellInfo(Gdiplus::Graphics* graph);

private:
    int32   m_InfoType;
};


#endif // scene_info_layer_h__
