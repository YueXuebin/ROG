#include "robot_pch.h"
#include "skill_action.h"
#include "robot.h"


SkillAction::SkillAction(Robot* pRobot,int32 rune_type)
:ActionBase(pRobot)
{
    m_Name = "Skill_action";
    m_type = rune_type;
}

SkillAction::~SkillAction()
{

}

void SkillAction::OnStart()
{
    ActionBase::OnStart();
    //SKILL_RUNEUP_REQ req;
    //req.rune_type = m_type;
    //m_Robot ->Send(MSG_SKILL_RUNEUP_REQ,&req);
    //End();
}

void SkillAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID,data);

    //switch (msgID)
    //{
    //case MSG_SKILL_RUNEUP_ACK:
    //    {
    //        SKILL_RUNEUP_ACK * pData = (SKILL_RUNEUP_ACK *)data;
    //        if(pData->errcode ==  ERR_SUCCEED)
    //        {
    //            CnDbg(" 技能升级成功 %d fire_level=%d ice_level=%d thunder_level=%d \n",m_type,pData->fire_level,pData->ice_level,pData->thunder_level);
    //            End();
    //        }
    //        else if (pData->errcode ==  ERR_FAILED)
    //        {
    //            CnDbg(" 技能升级失败 %d %d %d %d \n",m_type,pData->fire_level,pData->ice_level,pData->thunder_level);
    //        }
    //        else
    //        {
    //            CnDbg(" 技能升级失败 错误码:%d\n", pData->errcode);
    //        }
    //    }
    //	break;
    //}

}


