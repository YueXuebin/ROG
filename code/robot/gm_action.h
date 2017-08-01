#ifndef gm_action_h__
#define gm_action_h__


#include "action_base.h"

class GM_Action : public ActionBase
{
public:
    GM_Action(Robot* pRobot, const std::string& command);
    virtual ~GM_Action(void);
    virtual void OnStart();
    virtual void Update( uint32 tickcount, uint32 deltacount );

private:
    std::string     m_command;
    int32   m_Timer;
};

#endif // gm_action_h__
