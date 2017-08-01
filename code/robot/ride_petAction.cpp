#include "robot_pch.h"
#include "ride_petAction.h"
#include "robot.h"
#include "commin_const.h"
RidePetAction::RidePetAction(Robot* pRobot, int32 type, uint32 fieldid,uint32 modelid) :
ActionBase(pRobot)
{
    m_Name = "RidePet";

    // 1:ˢ������ ��ļ ����   2:��ļ         3:
    m_type = type;
    m_fieldid = fieldid;
    m_modeid = modelid;


}

RidePetAction::~RidePetAction(void)
{
}

void RidePetAction::OnStart()
{
    ActionBase::OnStart();

    switch(m_type)
    {
    case 1:
        {
            RefRidePet();
        }
        break;
    case RIDE_PET_RIGHT_NOW_TRAIN:
        {
            RightNowTrain(m_fieldid,20002);
            End();
        }
        break;
    case RIDE_PET_START_TRAIN:
        {
            StartTrain(20002, m_fieldid ,m_modeid);
            End();
        }
        break;
    case RIDE_PET_OPEN_TRAIN_POS:
        {
           SendTrainPos(m_fieldid);
           End();
        }
        break;
    }
}
void RidePetAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
    case MSG_RIDE_PET_UPDATE_ACK:
        {
            RIDE_PET_UPDATE_ACK* pData = (RIDE_PET_UPDATE_ACK*) data;
            for(std::vector<RIDE_PET_INFO>::iterator it = pData->ride_random_list.begin(); it != pData->ride_random_list.end(); ++it)
            {
                CnDbg("ˢ�·���  %d \n",it->pet_id);
                RecruitPet(it->pet_id);
                sendgifPet(it->pet_id);
            }
        }
        End();
        break;

    case MSG_RIDE_PET_SENDGIF_ACK:
        {
            RIDE_PET_SENDGIF_ACK* pData = (RIDE_PET_SENDGIF_ACK*) data;
            CnDbg("���ͷ���  %d \n", pData->object_random_list.size());
        }
        End();
        break;

    case MSG_RIDE_PET_LIST_NTF:
        {
            RIDE_PET_LIST* pData = (RIDE_PET_LIST*) data;
            // MSG_RIDE_PET_LIST_NTF
            if(pData->opt_flag == OPT_ADD_FLAG)
            {
                CnDbg("��ļ����  %d \n", pData->opt_flag);
            }
            else if (pData->opt_flag == OPT_UPDATE_FLAG)
            {
                std::vector<RIDE_PET_INFO>::iterator rideiter;
                for(rideiter = pData->ride_pet_list.begin(); rideiter !=  pData->ride_pet_list.end(); rideiter++) 
                {
                    if(rideiter->b_train == ON_IDLE)
                    {
                        CnDbg("������� %d \n",rideiter->pet_id);
                    }
                }
            }

        }
        End();
        break;
        //���ѵ���ķ���
        //case MSG_RIDE_PET_TRAIN_END_ACK:
        //    {
        //        END_TRAIN_ACK* pdata = (END_TRAIN_ACK*)data;
        //        if(pdata->opt_flag == TRAIN_FINISH)
        //        {
        //            CnDbg("ѵ��λ %d ��� %d \n",pdata->ride_train.id,pdata->ride_petinfo.pet_id);
        //        }
        //        End();
        //    }
    case MSG_RIDE_PET_TRAIN_INIT_LIST:
        {
            RIDE_PET_TRAIN_LIST* pData = (RIDE_PET_TRAIN_LIST*) data;
            if(pData->opt_flag == OPT_UPDATE_FLAG)
            {
                std::vector<RIDE_TRAIN>::iterator trainiter;
                for (trainiter = pData->ride_train_list.begin(); trainiter != pData->ride_train_list.end(); trainiter++)
                {
                    if (trainiter->state == OPEN)
                    {
                        CnDbg("ѵ��λ %d ����\n",trainiter->id);
                    }
                }
            }
            else if (pData->opt_flag == ERR_TRAIN_MODELS_NO_OPEN)
            {
                CnDbg("ѵ��ħʯû�п���\n");
            }
            else if(pData->opt_flag == ERR_NO_ENOUGH_GAMEPOINT)
            {
                CnDbg("û���㹻����Ϸ��\n");
            }
            else if (pData->opt_flag == PROP_MONEYPOINT)
            {
                CnDbg("û���㹻�������\n");
            }
        }
        End();
        break;
    }
}

////ˢ��
void RidePetAction::RefRidePet()
{
    RIDE_PET_UPDATE_REQ req;
    req.type = 2;//ʹ��ħʯ
    m_Robot ->Send(MSG_RIDE_PET_UPDATE_REQ,&req);
}
//��ļ
void RidePetAction::RecruitPet(int32 petID)
{
    SET_RIDE_PET_REQ req;
    req.pet_id = petID;
    m_Robot ->Send(MSG_RIDE_PET_CAPTURE_REQ,&req);
}
//����
void RidePetAction::sendgifPet(int32 petID)
{
    SET_RIDE_PET_REQ req;
    req.pet_id = petID;
    m_Robot ->Send(MSG_RIDE_PET_SENDGIF_REQ,&req);
}
//�㿪ѵ����λ
void RidePetAction::SendTrainPos(int32 fieldid)
{
    TRAIN_POST_REQ trainpos;
    trainpos.id = fieldid;
    m_Robot->Send(MSG_RIDE_PET_TRAIN_POS_REQ,&trainpos);
}
//��ʼѵ��
void RidePetAction::StartTrain(int32 petid ,uint32 m_fieldid ,uint32 m_modeid)
{
    RIDE_PET_SET_MODELS_REQ req;
    req.petid = petid;
    req.fieldid = m_fieldid;
    req.modeid = m_modeid;
    m_Robot ->Send(MSG_RIDE_PET_SET_MODELS_REQ,&req);
}
//�������ѵ��
void RidePetAction::RightNowTrain(uint32 m_fieldid,uint32 petid)
{
    NOW_TRAIN_REQ req;
    req.id = m_fieldid;
    req.pet_id = petid;
    m_Robot->Send(MSG_RIDE_PET_RIGHT_NOW_TRAIN_REQ, &req);
}
