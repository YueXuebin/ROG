/*
 *	等待一段时间
 */
#ifndef wait_action_h__
#define wait_action_h__

#include "action_base.h"


class WaitAction : public ActionBase
{
public:
    WaitAction(Robot* pRobot, int32 millisecond);
    virtual ~WaitAction();

    virtual void Update(uint32 tickcount, uint32 deltacount);

private:
    int32   m_millisecond;
};

#endif // wait_action_h__
