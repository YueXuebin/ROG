#ifndef point_trigger_h__
#define point_trigger_h__

#include "point_base.h"

/*
 *	触摸点(触摸石)
 */
class PointTrigger : public PointBase
{
    CnDeclareRTTI
public:
    PointTrigger(RegularBuilder* pRogRegular, uint32 CheckPointID);
    virtual ~PointTrigger();

    virtual bool Build();
    virtual void Update(uint32 curr_tick) {}
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void SetShowID(uint32 showId) {m_showID = showId;}

    uint32  GetTriggerID() {return m_TriggerID;}
    Trigger* GetTrigger() {return m_pTrigger;}

protected:
    virtual void OnEventTrigger();

private:
    void BuildTrigger();        // 创建关卡
private:
    uint32  m_showID;

    uint32  m_TriggerID;
    Trigger*    m_pTrigger;


};

#endif // point_trigger_h__
