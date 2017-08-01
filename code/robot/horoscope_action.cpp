#include "robot_pch.h"
#include "horoscope_action.h"
#include "robot.h"
#include "commin_const.h"
Horoscope_Action::Horoscope_Action(Robot* pRobot,int32 id,int32 type):ActionBase(pRobot)
{
    m_Name = "HoroscopeLeveup";
    m_levelup_id = id;
    m_type = type;
}

Horoscope_Action::~Horoscope_Action(void)
{

}

void Horoscope_Action::OnStart()
 {
     ActionBase::OnStart();

        NONE req;
        m_Robot->Send(MSG_HOROSCOPE_LEVELUP_REQ, &req);
 
     //else if(m_type == HOROSCOPE_LEVELUP_CMPL)
     //{
     //    NONE req;
     //    m_Robot->Send(MSG_HOROSCOPE_LEVELUP_CMPL_REQ, &req);
     //}
     End();
 }

 void  Horoscope_Action::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
    case MSG_HOROSCOPE_LEVELUP_ACK:
        {
            HOROSCOPE_LEVELUP_ACK* pData = (HOROSCOPE_LEVELUP_ACK*)data;
            if(pData->errcode ==  ERR_SUCCEED)
            {
                  CnDbg(" �ǿ�ͼ�����ɹ� %d \n",m_levelup_id);
                  End();
            }
            else if (pData->errcode ==  ERR_FAILED)
            {
                CnDbg(" �ǿ�ͼ����ʧ�� %d \n",m_levelup_id);
            }
            else if (pData->errcode ==  ERR_NO_ENOUGH_GAMEPOINT)
            {
                CnDbg(" �ǿ�ͼ����û���㹻��Ǯ %d \n",m_levelup_id);
            }
            else if (pData->errcode == ERR_NO_ENOUGH_SOUL)
            {
                CnDbg(" �ǿ�ͼ����û���㹻�ľ���%d \n",m_levelup_id);
            }
        }
        break;
    }
}

