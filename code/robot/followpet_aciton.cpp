#include "robot_pch.h"
#include "followpet_aciton.h"
#include "robot.h"
#include "commin_const.h"


Followpet_Aciton::Followpet_Aciton(Robot* pRobot,int32 pet_id,int32 upType,int32 type):ActionBase(pRobot)
{
    m_Name = "FollowShoworHide";
    m_type = type;
    m_petid = pet_id;
    m_upType = upType;
    starttime = 0;
}

Followpet_Aciton::~Followpet_Aciton(void)
{
}

 void Followpet_Aciton::OnStart()
{
    ActionBase::OnStart();
    
    //if(m_type == FOLLOW_PET_SHOW)
    //{
    //    FOLLOW_PET_SHOW_REQ req;
    //    req.pet_id = m_petid;
    //    m_Robot->Send(MSG_FOLLOW_PET_SHOW_REQ,&req);
    //}
    //else if(m_type == FOLLOW_PET_HIDE)
    //{
    //    FOLLOW_PET_CANLESHOW_REQ req;
    //    req.pet_id = m_petid;
    //    m_Robot->Send(MSG_FOLLOW_PET_CANLESHOW_REQ,&req);
    //}

    //else if(m_type == FOLLOW_PET_CAN_RECUIT)                    //宠物解锁
    //{
    //  FOLLOW_PET_CAN_RECUIT_REQ req;
    //  req.pet_id = m_petid;
    //    m_Robot->Send(MSG_FOLLOW_PET_CAN_RECUIT_REQ,&req);
    //}

    //else if(m_type == FOLLOW_PET_RANDOM)                    //宠物升级
    //{
    //   starttime = (uint32)Crown::SDNow().GetTimeValue();
    //    FOLLOW_PET_RANDOM_EXP req;
    //    req.pet_id = m_petid;
    //    req.type   = m_upType;
    //    m_Robot->Send(MSG_FOLLOW_PET_RANDOM_EXP_REQ,&req);
    //    CnDbg("宠物升级 followPetId =%d \n",m_petid);
    //}
}

 void Followpet_Aciton::Update( uint32 tickcount, uint32 deltacount )
 {
     ActionBase::Update(tickcount, deltacount);
     uint32 finishtime = (uint32)Crown::SDNow().GetTimeValue();
     if(finishtime > starttime && starttime)
     {
         if(finishtime - starttime >1)
         {
             End();
             starttime= 0;
         }
     }
 }
 void Followpet_Aciton:: OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);

    //switch(msgID)
    //{
    //    //出战
    //case MSG_FOLLOW_PET_SHOW_ACK:
    //    End();
    //    break;
    //    //未出战
    //case  MSG_FOLLOW_PET_CANLESHOW_ACK:
    //    {
    //        FOLLOW_PET_CANLESHOW_ACK* pData = (FOLLOW_PET_CANLESHOW_ACK*) data;
    //        if (pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg(" 未出战失败宠物id %d failed\n",m_petid);
    //        }
    //        else if(pData->errcode == ERR_SUCCEED)
    //        {
    //            CnDbg(" 宠物未出战成功id %d failed\n",m_petid);
    //            End();
    //        }
    //    }
    //    break;

    //case MSG_FOLLOW_PET_CAN_RECUIT_ACK:                 //解锁
    //    {
    //        FOLLOW_PET_CAN_RECUIT_ACK* pData = (FOLLOW_PET_CAN_RECUIT_ACK*) data;
    //        CnDbg("解锁成功 followPetId =%d \n", pData->follow_pet.pet_id);
    //    }
    //    End();
    //    break;

    //case MSG_FOLLOW_PET_RANDOM_EXP_ACK:                 //增加经验
    //    {
    //        FOLLOW_PET_RANDOM_EXP_ACK* pData = (FOLLOW_PET_RANDOM_EXP_ACK*) data;
    //        if(pData)
    //        CnDbg("宠物 %d  当前等级 %d  当前经验 %d\n",pData->follow_pet.pet_id ,pData->follow_pet.cur_level , pData->follow_pet.cur_express);
    //    }
    //    End();
    //    break;
    //}

}
