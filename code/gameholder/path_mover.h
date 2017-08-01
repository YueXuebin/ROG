#ifndef path_mover_h_
#define path_mover_h_

#include "step_mover.h"

class Creature;
class ActionController;

/*
*	路径控制器
*/
class PathMover
{
    friend class StepMover;
public:
    PathMover(Creature* creature);
    ~PathMover();

    void UpdateMove();

    void MoveTo(int32 x, int32 y);                                          // 移动至某位置

    void Stop();

    const std::vector<CoVec2>&   GetPath() {return m_PathList;}              // 获取路径

private:
    void FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath);      //查找一条可行走的路径

    void MoveToNextPathPos();

    void OnStepMoveEnd();

    void OnPathEnd();

private:
    StepMover m_StepMover;

    Creature* m_Creature;

    std::vector<CoVec2> m_PathList;     //记录路径,[0]为当前Stepper正在前往的位置

    CoVec2  m_PathDes;          // 寻路的路径的终点

};

#endif // action_controller_h__
