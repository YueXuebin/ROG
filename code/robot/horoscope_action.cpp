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
                  CnDbg(" ÐÇ¿ÕÍ¼Éý¼¶³É¹¦ %d \n",m_levelup_id);
                  End();
            }
            else if (pData->errcode ==  ERR_FAILED)
            {
                CnDbg(" ÐÇ¿ÕÍ¼Éý¼¶Ê§°Ü %d \n",m_levelup_id);
            }
            else if (pData->errcode ==  ERR_NO_ENOUGH_GAMEPOINT)
            {
                CnDbg(" ÐÇ¿ÕÍ¼Éý¼¶Ã»ÓÐ×ã¹»µÄÇ® %d \n",m_levelup_id);
            }
            else if (pData->errcode == ERR_NO_ENOUGH_SOUL)
            {
                CnDbg(" ÐÇ¿ÕÍ¼Éý¼¶Ã»ÓÐ×ã¹»µÄ¾ª»ê%d \n",m_levelup_id);
            }
        }
        break;
    }
}

