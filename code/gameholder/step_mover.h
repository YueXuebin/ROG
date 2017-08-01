#ifndef step_mover_h__
#define step_mover_h__


const int32 DELTA_ANGLE = 30;           // 盘绕角度30
const int32 MAX_TWINE_ANGLE = 120;      // 盘绕最大角度120


class Creature;
class BattleUnit;
class PathMover;


class StepMover
{
public:
    StepMover(Creature* creature);
    ~StepMover();

    void Init(PathMover* pathMover);

    void UpdateMove();                     //盘绕算法

    void MoveTo(int32 x, int32 y);
    void Stop();

    int32 GetDesX() {return m_DesX;}
    int32 GetDesY() {return m_DesY;}

    real RemainDistance();

private:
    void Init();            // 初始化

    real ProjectOnLine(int32 x, int32 y);
private:
    // 移动
    int32   m_StartX;
    int32   m_StartY;
    int32   m_DesX;
    int32   m_DesY;
    CoVec2  m_Line;
    bool    m_IsMoving;

    // 盘绕计算用数据
    int32   m_TwineStartAngle;      // 开始盘绕时的目标点角度
    int32   m_TwineAngle;           // 盘绕角度
    int32   m_TwineCounter;
    real    m_TwineLeaveDistance;   // 离开盘绕时的距离终点距离

    Creature* m_Creature;
    PathMover* m_PathMover;
};


#endif // step_mover_h__
