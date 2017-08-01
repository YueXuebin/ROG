#ifndef move_action_h__
#define move_action_h__


//  ÒÆ¶¯
#include "action_base.h"

class MoveAction : public ActionBase
{
public:
    MoveAction(Robot* pRobot, int32 x, int32 y);
    virtual ~MoveAction();

    virtual void OnRecv(uint32 msgID, void* data);
    virtual void Update(uint32 tickcount, uint32 deltacount);
    
    virtual void OnStart();

private:
    void SendMoveReq(void* data);
    void RecvMoveAck(void* data);

    void move();
    void stop();

protected:
    int32   m_srcX;
    int32   m_srcY;
    int32   m_desX;
    int32   m_desY;
    int32   m_Time;

};

#endif // move_action_h__
