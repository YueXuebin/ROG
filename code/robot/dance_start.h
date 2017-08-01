#pragma once


// ÌøÎè
#include "action_base.h"

class DanceStart : public ActionBase
{
public:
    DanceStart(Robot* pRobot);
    ~DanceStart(void);

    virtual void OnRecv(uint32 msgID, void* data);
    virtual void OnStart();

private:

    void SendDanceReq(void * data);
    void RecvDanceAck(void * data);


};
