#include "gameholder_pch.h"
#include "path_mover.h"
#include "creature.h"
#include "scene.h"
#include "step_mover.h"
#include "performance_analysis.h"
#include "action_controller.h"


PathMover::PathMover(Creature* creature) :
m_Creature(creature),
m_StepMover(creature)
{
    m_StepMover.Init(this);
}

PathMover::~PathMover()
{
}

void PathMover::Stop()
{
    m_PathList.clear();
    m_StepMover.Stop();
}

void PathMover::MoveTo(int32 x, int32 y)
{
    if(m_PathList.size() >= 1 && ( (m_PathDes - CoVec2(x, y)).LengthSquared() < 200*200 ))
    {
        // 无需重新计算路径,仅修改最后一个路径点的位置
        CoVec2& lastPos = m_PathList[m_PathList.size()-1];
        lastPos.x = (real)x;
        lastPos.y = (real)y;

        if(m_PathList.size() == 1)
            m_StepMover.MoveTo((int32)lastPos.x, (int32)lastPos.y);
    }
    else
    {
        // 需要重新计算路径
        CoVec2 startPos(m_Creature->GetX(), m_Creature->GetY());
        CoVec2 endPos(x, y);
        m_PathList.clear();
        FindPath(startPos, endPos, m_PathList);
        CnAssert(m_PathList.size() >= 2);           // 路径中至少有2个点

        m_PathDes = m_PathList[m_PathList.size()-1];

        MoveToNextPathPos();
    }
}

void PathMover::MoveToNextPathPos()
{
    if(m_PathList.size() <= 0)
    {
        OnPathEnd();
        return;     // 没有路径
    }

    m_PathList.erase(m_PathList.begin());
    if(m_PathList.size() <= 0)
    {
        OnPathEnd();
        return;     // 没有下一个路径点了
    }

    const CoVec2& pos = m_PathList[0];
    m_StepMover.MoveTo((int32)pos.x, (int32)pos.y);
}
// 线段移动完成回调
void PathMover::OnStepMoveEnd()
{
    if(m_PathList.size() >= 2)
    {
        MoveToNextPathPos();
    }
    else
    {
        OnPathEnd();
    }
}
// 路径移动完成回调
void PathMover::OnPathEnd()
{
    m_Creature->GetActionController()->OnMoveEnd(MOVE_TYPE_MOVE);
}

void PathMover::FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath)
{
    Scene* pScene = m_Creature->GetScene();
    bool b_result = pScene->FindPath(start, end, pPath);
}
//更新移动
void PathMover::UpdateMove()
{
    if(m_PathList.size() == 0)
        return;
    
    if(m_PathList.size() >= 2 && m_StepMover.RemainDistance() < 100)
    {   // 已经移动到当前路径点了，再继续移动到下一个路径点(沿路径移动的优化处理)
        MoveToNextPathPos();
    }

    m_StepMover.UpdateMove();
}
