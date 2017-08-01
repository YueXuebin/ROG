#pragma once


#include "action_base.h"


class PassiveSkillAction :public ActionBase
{
public:
    PassiveSkillAction(Robot* pRobot,int32 skillId,int32 type);
    virtual ~PassiveSkillAction();

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    int32  m_skillId;
    int32  m_type;   //发送升级的类型

};
