#ifndef point_treasure_h__
#define point_treasure_h__

#include "point_base.h"

/*
 *	宝箱
 */
class PointTreasure : public PointBase
{
    CnDeclareRTTI
public:
    PointTreasure(RegularBuilder* pRogRegular, uint32 CheckPointID);
    virtual ~PointTreasure();

    virtual bool Build();
    virtual void Update(uint32 curr_tick) {}
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void SetShowID(uint32 showId) {m_showID = showId;}

    uint32  GetTriggerID() {return m_TriggerID;}

protected:
    virtual void OnEventTrigger();

    void BuildTreasure();       // 创建宝箱

private:
    uint32  m_showID;

    uint32  m_TriggerID;
    Trigger*    m_pTrigger;


};

#endif // point_treasure_h__
