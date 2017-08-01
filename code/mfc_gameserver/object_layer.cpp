#include "stdafx.h"
#include "object_layer.h"
#include "canvas.h"
#include "action_controller.h"

ObjectLayer::ObjectLayer( Canvas* canvas ) :
Layer(canvas)
{
    m_ShowCreature = true;
    m_ShowObject = false;
    m_ShowSkill = true;
    m_ShowAOI = true;

    m_SelectObjectID = 0;

    m_StartPos.Set(0, 0);
}

ObjectLayer::~ObjectLayer()
{

}

void ObjectLayer::OnRender( Gdiplus::Graphics* graph )
{
    Scene* pScene = m_Canvas->GetDrawScene();
    if(m_ShowCreature)
    {
        for(Scene::BattleUnitMap::iterator citer = pScene->GetBattleUnits().begin(); citer != pScene->GetBattleUnits().end(); citer++)
        {
            RenderCreature(graph, citer->second);
        }
    }
    
    if(m_ShowObject)
    {
        for(Scene::SceneObjectMap::iterator oiter = pScene->GetObjects().begin(); oiter != pScene->GetObjects().end(); oiter++)
        {
            RenderObject(graph, oiter->second);
        }
    }

    if(m_ShowSkill)
    {
        for(Scene::BattleUnitMap::iterator citer = pScene->GetBattleUnits().begin(); citer != pScene->GetBattleUnits().end(); citer++)
        {
            BattleUnit* pBattleUnit = citer->second;
            std::vector<SkillObject*>& skills = pBattleUnit->GetSkillController()->GetSkills();
            for(std::vector<SkillObject*>::iterator skillIter = skills.begin(); skillIter != skills.end(); skillIter++)
            {
                SkillObject* pSkill = dynamic_cast<SkillObject*>(*skillIter);
                if(!pSkill)
                    continue;

                RenderSkill(graph, pSkill);
            }
        }

        RenderRange(graph);
    }

    if(m_ShowAOI)
    {
        BattleUnit* pSelectBattleUnit = pScene->GetBattleUnit(m_SelectObjectID);
        //if(pSelectBattleUnit)
            //RenderAOISystem(graph, pSelectBattleUnit);
    }

    RenderPath(graph);


    //DrawBoundingBox(graph);
}

void ObjectLayer::RenderPath(Gdiplus::Graphics* graph)
{
    Gdiplus::Pen pen(m_GreenColor);
    
    Scene* pScene = m_Canvas->GetDrawScene();
    BattleUnit* pBattleUnit = pScene->GetBattleUnit(m_SelectObjectID);
    if(!pBattleUnit)
        return;

    ActionCreatureController* pController = (ActionCreatureController*)pBattleUnit->GetActionController();
    const std::vector<CoVec2>& m_Path = pController->GetPath();

    uint32 pathCounter = 0;
    CoVec2 a, b;
    for(std::vector<CoVec2>::const_iterator iter=m_Path.begin(); iter != m_Path.end(); iter++)
    {
        pathCounter++;
        
        if(pathCounter == 1)
        {
            a = pBattleUnit->GetPosition();
        }

        b = a;
        a = *iter;

        POINT p1;
        p1.x = a.x;
        p1.y = a.y;
        ToRenderPos(p1);

        POINT p2;
        p2.x = b.x;
        p2.y = b.y;
        ToRenderPos(p2);

        graph->DrawLine(&pen, (float)p1.x, (float)p1.y, (float)p2.x, (float)p2.y);
    }

}

void ObjectLayer::RenderRange(Gdiplus::Graphics* graph)
{
    Gdiplus::Pen pen(m_RedColor);

    for(std::vector<RangeRect>::iterator iter = m_Rects.begin(); iter != m_Rects.end(); iter++)
    {
        RangeRect& rect = *iter;
        graph->DrawLine(&pen, rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
        graph->DrawLine(&pen, rect.p2.x, rect.p2.y, rect.p3.x, rect.p3.y);
        graph->DrawLine(&pen, rect.p3.x, rect.p3.y, rect.p4.x, rect.p4.y);
        graph->DrawLine(&pen, rect.p4.x, rect.p4.y, rect.p1.x, rect.p1.y);
    }

    for(std::vector<RangeFan>::iterator iter = m_Fans.begin(); iter != m_Fans.end(); iter++)
    {
        RangeFan& fan = *iter;
        Gdiplus::Rect circleRc(fan.x-fan.radius, fan.y-fan.radius, fan.radius*2, fan.radius*2);
        graph->DrawPie(&pen, circleRc, fan.startAngle, fan.sweepAngle);
    }

    for(std::vector<RangeRect>::iterator iter = m_Rects.begin(); iter != m_Rects.end();)
    {
        RangeRect& rect = *iter;
        rect.time -= g_ServerUpdateTime;
        if(rect.time <= 0)
            iter = m_Rects.erase(iter);
        else
            iter++;
    }

    for(std::vector<RangeFan>::iterator iter = m_Fans.begin(); iter != m_Fans.end();)
    {
        RangeFan& fan = *iter;
        fan.time -= g_ServerUpdateTime;
        if(fan.time <= 0)
            iter = m_Fans.erase(iter);
        else
            iter++;
    }
}

void ObjectLayer::RenderCreature( Gdiplus::Graphics* graph, BattleUnit* pBattleUnit )
{
    CoDynamicObject* pDynObject = pBattleUnit->GetCoObject();
    if(!pDynObject)
        return;

    CoVec2 pos = pDynObject->GetPosition();
    POINT p;
    p.x = pos.x;
    p.y = pos.y;

    ToRenderPos(p);

    float angle = pDynObject->GetAngle();
    int32 radius = pDynObject->GetRadius() * m_Canvas->GetScale();

    // Body
    Gdiplus::Pen pen(m_DefaultColor);
    if (pBattleUnit->GetObjectType() == OBJ_MONSTER)
        pen.SetColor(m_MonsterColor);
    else if (pBattleUnit->GetObjectType() == OBJ_BOSS)
        pen.SetColor(m_BossColor);
    else if (pBattleUnit->GetObjectType() == OBJ_PLAYER)
        pen.SetColor(m_PlayerColor);
    else if (pBattleUnit->GetObjectType() == OBJ_PET)
        pen.SetColor(m_PetColor);
    else 
        pen.SetColor(m_DefaultColor);

    Gdiplus::Rect circleRc(p.x-radius, p.y-radius, radius*2, radius*2);
    graph->DrawEllipse(&pen, circleRc);

    // 方向
    graph->DrawLine(&pen, (float)p.x, (float)p.y, p.x + radius*2*cos(angle), p.y + radius*2*sin(angle));

    if(pBattleUnit->GetObjectType() == OBJ_PLAYER)
    {
        CoVec2 forsee = ((ActionPlayerController*)pBattleUnit->GetActionController())->GetCheckPos();
        
        p.x = forsee.x;
        p.y = forsee.y;

        ToRenderPos(p);

        Gdiplus::Rect circleRc(p.x-radius, p.y-radius, radius*2, radius*2);
        graph->DrawEllipse(&pen, circleRc);
    }

    CStringW str;
    // 状态
    uint32 state = pBattleUnit->GetState();
    str.Format(L"%d", state);
    DrawText(graph, str, p.x - radius, p.y, &m_BlueColor);

    // AI
    if(pBattleUnit->GetAIController())
    {
        uint32 aistate = pBattleUnit->GetAIController()->GetAIState();
        str.Format(L"%d", aistate);
        DrawText(graph, str, p.x - radius + 10, p.y, &m_RedColor);
    }
}

void ObjectLayer::RenderObject( Gdiplus::Graphics* graph, SceneObject* pSceneObject )
{
    CoDynamicObject* pDynObject = pSceneObject->GetCoObject();

    float angle = pSceneObject->GetAngle();

    int radius = 50;
    if(pDynObject)
        radius = pDynObject->GetRadius() *  m_Canvas->GetScale();
    else
        radius = 50 * m_Canvas->GetScale();

    CoVec2 pos = pSceneObject->GetPosition();
    POINT p;
    p.x = pos.x;
    p.y = pos.y;

    ToRenderPos(p);

    Gdiplus::Rect circleRc(p.x-radius, p.y-radius, radius*2, radius*2);

    if (pSceneObject->GetObjectType() == OBJ_TRIGGER)
    {
        Trigger* pTrigger = CnDynamicCast(Trigger, pSceneObject);
        Gdiplus::Pen pen(m_TriggerColor);
        graph->DrawEllipse(&pen, circleRc);
        
        // 激活范围
        Gdiplus::Pen apen(m_TriggerActiveColor);
        int32 activeRadius = pTrigger->GetRadius();
        activeRadius = activeRadius * m_Canvas->GetScale();
        Gdiplus::Rect activeCircleRc(p.x-activeRadius, p.y-activeRadius, activeRadius*2, activeRadius*2);
        graph->DrawEllipse(&apen, activeCircleRc);
    }
    else
    {
        Gdiplus::Pen pen(m_GreyColor);
        // 圆
        graph->DrawEllipse(&pen, circleRc);
        // 方向
        graph->DrawLine(&pen, (float)p.x, (float)p.y, p.x + radius*cos(angle), p.y + radius*sin(angle));
    }
}

void ObjectLayer::RenderSkill( Gdiplus::Graphics* graph, SkillObject* pSkill )
{
    // 弹道
    const SkillObject::SkillLogicList& allLogics = pSkill->GetAllLogics();

    for(SkillObject::SkillLogicList::const_iterator iter = allLogics.begin(); iter != allLogics.end(); iter++)
    {
        SkillLogicBase* pLogic = *iter;
        if(pLogic->GetLogicType() != SKILL_LOGIC_BULLET)
            continue;

        SkillLogicBullet* pBulletLogic = (SkillLogicBullet*)pLogic;
        if(!pBulletLogic->IsActive())
            continue;

        int radius = pBulletLogic->GetRadius() * m_Canvas->GetScale();
        radius = Crown::SDMax(radius, 1);
        const CoVec2& pos = pBulletLogic->GetPosition();
        POINT p;
        p.x = pos.x;
        p.y = pos.y;

        ToRenderPos(p);

        Gdiplus::Pen pen(m_RedColor);
        Gdiplus::Rect circleRc(p.x-radius, p.y-radius, radius*2, radius*2);
        graph->DrawEllipse(&pen, circleRc);
    }

    // 击中
    for(SkillObject::SkillLogicList::const_iterator iter = allLogics.begin(); iter != allLogics.end(); iter++)
    {
        SkillLogicBase* pLogic = *iter;
        if(pLogic->GetLogicType() != SKILL_LOGIC_HIT)
            continue;

        SkillLogicHit* pHitLogic = (SkillLogicHit*)pLogic;
        if(!pHitLogic->m_DebugShow)
            continue;
        pHitLogic->m_DebugShow = false;

        const SkillHitData* m_HitData = &pHitLogic->GetSkillData()->m_HitData;

        CoVec2 pos = pHitLogic->GetHitPosition();
        POINT p;
        p.x = pos.x;
        p.y = pos.y;

        ToRenderPos(p);

        int range = m_HitData->m_HitRange * m_Canvas->GetScale();

        real skillAngle = pHitLogic->GetHitAngle();
        if(m_HitData->m_IsRect)
        {
            CoVec2 l;
            l.SetAngle(m_HitData->m_HitRange, ANGLE_TO_RADIAN(skillAngle));

            CoVec2 w;
            w.SetAngle(m_HitData->m_HitAngle/2, ANGLE_TO_RADIAN(skillAngle + 90));

            CoVec2 p1 = pos + w;
            CoVec2 p2 = pos + w + l;
            CoVec2 p3 = pos - w + l;
            CoVec2 p4 = pos - w;

            POINT p;
            p.x = p1.x; p.y = p1.y;
            ToRenderPos(p);

            RangeRect rect;
            rect.p1.x = p.x;
            rect.p1.y = p.y;

            p.x = p2.x; p.y = p2.y;
            ToRenderPos(p);
            rect.p2.x = p.x;
            rect.p2.y = p.y;

            p.x = p3.x; p.y = p3.y;
            ToRenderPos(p);
            rect.p3.x = p.x;
            rect.p3.y = p.y;

            p.x = p4.x; p.y = p4.y;
            ToRenderPos(p);
            rect.p4.x = p.x;
            rect.p4.y = p.y;

            rect.time = 2000;
            m_Rects.push_back(rect);
        }
        else
        {   
            RangeFan fan;           // 半径线为方向的反方向
            fan.x = p.x;
            fan.y = p.y;
            fan.radius = range;
            fan.startAngle = -skillAngle - m_HitData->m_HitAngle/2;
            fan.sweepAngle = m_HitData->m_HitAngle;

            fan.time = 2000;
            m_Fans.push_back(fan);
        }

    }
}

void ObjectLayer::DrawBoundingBox(Gdiplus::Graphics* graph)
{
    Scene* pScene = m_Canvas->GetDrawScene();

    Gdiplus::Pen pen(m_RedColor);

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

        graph->DrawLine(&pen, p0.x, p0.y, p1.x, p0.y);
        graph->DrawLine(&pen, p1.x, p0.y, p1.x, p1.y);
        graph->DrawLine(&pen, p1.x, p1.y, p0.x, p1.y);
        graph->DrawLine(&pen, p0.x, p1.y, p0.x, p0.y);
    }
}

uint32 ObjectLayer::SelectObject(int32 px, int32 py)
{
    POINT p;
    p.x = px;
    p.y = py;
    ToScenePos(p);

    // 搜索选中的单位
    uint32 SelectObjectID = 0;
    Scene* pScene = m_Canvas->GetDrawScene();
    if(!pScene)
        return 0;

    for(Scene::BattleUnitMap::iterator citer = pScene->GetBattleUnits().begin(); citer != pScene->GetBattleUnits().end(); citer++)
    {
        BattleUnit* pBattleUnit = citer->second;
        CoVec2 pos = pBattleUnit->GetPosition();
        int32 radius = pBattleUnit->GetRadius();
        if( (pos.x - radius) <= p.x && (pos.x + radius) >= p.x
            && (pos.y - radius) <= p.y && (pos.y + radius) >= p.y )
        {
            SelectObjectID = pBattleUnit->GetId();
        }
    }

    if(SelectObjectID)
        return SelectObjectID;

    for(Scene::SceneObjectMap::iterator oiter = pScene->GetObjects().begin(); oiter != pScene->GetObjects().end(); oiter++)
    {
        SceneObject* pSceneObject = oiter->second;
        CoVec2 pos = pSceneObject->GetPosition();
        int32 radius = pSceneObject->GetRadius();
        if( (pos.x - radius) <= p.x && (pos.x + radius) >= p.x
            && (pos.y - radius) <= p.y && (pos.y + radius) >= p.y )
        {
            SelectObjectID = pSceneObject->GetId();
        }
    }

    if(SelectObjectID)
        return SelectObjectID;

    return SelectObjectID;
}

bool ObjectLayer::OnLButtonDown( int32 px, int32 py )
{
    uint32 selectObjectID = SelectObject(px, py);
    if(selectObjectID)
    {
        m_SelectObjectID = selectObjectID;
        return true;
    }

    // 寻路路径测试
    Scene* pScene = m_Canvas->GetDrawScene();
    if(!pScene)
        return true;

    POINT p;
    p.x = px;
    p.y = py;
    ToScenePos(p);
    CoVec2 endPos(p.x, p.y);
    // pScene->FindPath(m_StartPos, endPos, m_Path);

    BattleUnit*  battleUnit = pScene->GetBattleUnit(m_SelectObjectID);
    if(battleUnit)
    {
        ActionController*  actionController= battleUnit->GetActionController();
        if(actionController)
        {
            actionController->RunTo(endPos.x, endPos.y);
        }

    }
    m_StartPos = endPos;

    return true;
}




