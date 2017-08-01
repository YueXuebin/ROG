#ifndef push_mover_h__
#define push_mover_h__


class Creature;

class PushMover
{
public:
    PushMover(Creature* creature);
    ~PushMover();

    void PushTo(int32 x, int32 y, int32 time, bool isPassive);
    void Stop();

    void UpdatePush();

    bool IsPassivePush() {return m_IsPassivePush;}

private:
    real ProjectOnLine(int32 x, int32 y);

private:
    Creature* m_Creature;

    bool    m_IsPassivePush;        // 是否被动推移
    int32   m_PushTime;             // 总推动时间
    int32   m_PushUseTime;          // 已经推动的时间

    int32   m_StartX;               // 推动的开始位置
    int32   m_StartY;
    int32   m_DesX;                 // 推动的目的位置
    int32   m_DesY;
    CoVec2  m_PushLine;
    CoVec2  m_PushSpeed;

    bool    m_IsPushing;
    uint32  m_OldMask;

};

#endif // push_mover_h__
