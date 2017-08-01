#pragma once


#include "action_base.h"

class SkillAction : public ActionBase
{
public:
    SkillAction(Robot* pRobot,int32 rune_type);
    virtual ~SkillAction();

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    int32  m_type;//¿‡–Õ

};


