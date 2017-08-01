#pragma once

#include "action_base.h"

class TouchStartAction : public ActionBase
{
public:
    TouchStartAction(Robot* pRobot,int32 touchId);
    ~TouchStartAction(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    int32  m_touchId;

};
