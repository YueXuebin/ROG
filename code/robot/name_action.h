#ifndef name_action_h__
#define name_action_h__

// ¸ÄÃû
#include "action_base.h"

class NameAction : public ActionBase
{
public:
    NameAction(Robot* pRobot, const std::string& name);
    virtual ~NameAction();

    virtual void OnStart();

    virtual void OnRecv(uint32 msgID, void* data);

private:
    std::string m_NewName;
};

#endif // name_action_h__
