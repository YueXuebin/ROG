#include "robot_pch.h"
#include "passive_skill.h"


#include "commin_const.h"
#include "robot.h"


PassiveSkillAction::PassiveSkillAction(Robot* pRobot,int32 skillId,int32 type)
:ActionBase(pRobot)
{
    m_Name = "PassiveSkillAction";
    m_skillId = skillId;
    m_type = type;
}

PassiveSkillAction::~PassiveSkillAction(void)
{

}

void PassiveSkillAction::OnStart()
{
    ActionBase::OnStart();

    if(m_type == PASSIVE_SKILL_OPEN_REQ)
    {
        SKILL_ID req;
        req.skill_id = m_skillId;
        m_Robot->Send(MSG_PASSIVE_SKILL_OPEN_REQ, &req);
    }
    else if(m_type == PASSIVE_SKILL_UP_REQ)
    {
        SKILL_ID req;
        req.skill_id = m_skillId;
        m_Robot->Send(MSG_PASSIVE_SKILL_UP_REQ, &req);
    }
}


void PassiveSkillAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
        case MSG_PASSIVE_SKILL_OPEN_ACK:
        {
            ERROR_CODE* pData = (ERROR_CODE*)data;
            if(pData->errcode == ERR_SUCCEED)
            {
                CnDbg("开启成功 \n");
            }
            else
            {
                CnDbg("SKILL_OPEN %d failed\n", pData->errcode);
            }
        }
        End();
        break;
        case MSG_PASSIVE_SKILL_UP_ACK:
        {
            ERROR_CODE* pData = (ERROR_CODE*)data;
            if(pData->errcode == ERR_SUCCEED)
            {
                CnDbg("升级成功 \n");
            }
            else if(pData->errcode == ERR_FAILED)
            {
                CnDbg("SKILL_OPEN %d failed\n", pData->errcode);
            }
        }
        End();
        break;
    }
}


