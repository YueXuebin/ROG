#ifndef point_touch_h__
#define point_touch_h__

#include "point_base.h"

/*
 *	采集点(采集蘑菇、采集矿石、开锁）
 */
class PointTouch : public PointBase
{
    CnDeclareRTTI
public:
    PointTouch(RegularBuilder* pRogRegular, uint32 CheckPointID);
    virtual ~PointTouch();

    virtual bool Build();
    virtual void Update(uint32 curr_tick) {}
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void SetShowID(uint32 showId);

    void SetTouchTime(uint32 time);

protected:
    virtual void OnEventTrigger();

private:
    TouchData m_TouchData;
    Touch*  m_pTouch;

};


#endif // point_touch_h__
