/*
 *	∂‘œÛÕº≤„
 */
#ifndef object_layer_h__
#define object_layer_h__

#include "layer.h"

class RangeRect
{
public:
    int32   time;
    POINT p1;
    POINT p2;
    POINT p3;
    POINT p4;
};

class RangeFan
{
public:
    int32   time;
    int32   x;
    int32   y;
    int32   radius;
    float   startAngle;
    float   sweepAngle;
};

class ObjectLayer : public Layer
{
public:
    ObjectLayer(Canvas* canvas);
    virtual ~ObjectLayer();

    void    ShowObject(bool show) {m_ShowObject = show;}
    bool    IsShowObject() {return m_ShowObject;}

    void    ShowCreature(bool show) {m_ShowCreature = show;}
    bool    IsShowCreature() {return m_ShowCreature;}

    void    ShowSkill(bool show) {m_ShowSkill = show;}
    bool    IsShowSkill() {return m_ShowSkill;}

    virtual bool OnLButtonDown(int32 px, int32 py);

    uint32  GetSelectObjectID() {return m_SelectObjectID;}

protected:
    virtual void OnRender(Gdiplus::Graphics* graph);

    void RenderCreature(Gdiplus::Graphics* graph, BattleUnit* pBattleUnit);
    void RenderObject(Gdiplus::Graphics* graph, SceneObject* pSceneObject);
    void RenderSkill(Gdiplus::Graphics* graph, SkillObject* pSkill);
    void RenderRange(Gdiplus::Graphics* graph);
    void DrawBoundingBox(Gdiplus::Graphics* graph);
    void RenderPath(Gdiplus::Graphics* graph);

    uint32  SelectObject(int32 px, int32 py);

private:
    bool    m_ShowCreature;
    bool    m_ShowObject;
    bool    m_ShowSkill;
    bool    m_ShowAOI;

    std::vector<RangeRect>      m_Rects;
    std::vector<RangeFan>       m_Fans;

    uint32  m_SelectObjectID;

    // —∞¬∑≤‚ ‘
    CoVec2  m_StartPos;
    //std::vector<CoVec2> m_Path;

};






#endif // object_layer_h__
