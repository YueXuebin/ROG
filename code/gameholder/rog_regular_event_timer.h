#ifndef rog_regular_event_timer_h__
#define rog_regular_event_timer_h__

#include "rog_regular_event.h"

/*
 *	限时事件
 *  param1: 总时间
 *  param2: 经过时间
 */
class ROGRegularEvent_Timer : public ROGRegularEvent
{
public:
    ROGRegularEvent_Timer(RegularDungeon* pRegular, bool startActive = false);
    virtual ~ROGRegularEvent_Timer();
    
    void SetTime(uint32 val) { m_param1 = val; } // 设置计时时间，单位毫秒
    uint32 GetTime() { return m_param1; }

    virtual void OneSecondUpdate(int32 deltatime);

private:
    bool m_startActive;
};

#endif // rog_regular_event_timer_h__
