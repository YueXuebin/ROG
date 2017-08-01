#include "robot_pch.h"
#include "task_aciton.h"
#include "robot.h"
#include "commin_const.h"
Task_Aciton::Task_Aciton(Robot* pRobot, uint32 id, int32 type):ActionBase(pRobot)
{
    m_Name = "TaskAction";
    m_id = id;
    m_type = type;
}

Task_Aciton::~Task_Aciton(void)
{
}

 void Task_Aciton::OnStart()
 {
     ActionBase::OnStart();
     if (m_type == TASK_ASK_TYPE)
     {
         TAKE_TASK_REQ req;
         req.task_id = m_id;
         m_Robot->Send(MSG_TAKE_TASK_REQ,&req);
     }
     else if (m_type == TASK_TALK_NPC_TYPE)
     {
         TASK_TALK_NPC req;
         req.npc_id = m_id;
        m_Robot->Send(MSG_TASK_TALK_NPC,&req);
     }
     else if (m_type == TASK_CHANGE_DUNGEON_TYPE)
     {
         TASK_CHANGE_DUNGEON_REQ req;
         req.task_id = m_id;
        m_Robot->Send(MSG_TASK_CHANGE_DUNGEON_REQ,&req);
     }
     else if (m_type == TASK_REWARD_TYPE)
     {
         TASK_REWARD_REQ req;
         req.task_id = m_id;
         m_Robot->Send(MSG_TASK_REWARD_REQ,&req);
     }
 }
 void Task_Aciton::OnRecv(uint32 msgID, void* data)
 {
     ActionBase::OnRecv(msgID, data);

     switch(msgID)
     {
     case MSG_TASK_UPDATE_NTF:
         {
          TASK_UPDATE_INFO* pData = (TASK_UPDATE_INFO*)data;
          CnDbg(" 任务请求成功 或请求对话成功%d \n",m_id);
          End();
         }
         break;;
     case  MSG_TASK_REWARD_ACK:
         {
             TASK_REWARD_ACK* pData = (TASK_REWARD_ACK*)data;
             if(pData->errcode == ERR_UNFOUND_TASK)
             {
                  CnDbg(" 奖励失败,没有找到任务 %d \n",m_id);
             }
             else if (pData->errcode == ERR_SUCCEED)
             {
                 CnDbg(" 奖励成功 %d \n",m_id);
             }
             else if (pData->errcode == ERR_TASK_UNFINSH)
             {
                 CnDbg(" 没有完成任务 %d \n",m_id);
             }
             else if (pData->errcode == ERR_SUCCEED)
             {
                 CnDbg(" 奖励成功 %d \n",m_id);
                 End();
             }

         }
         break;
     case  MSG_CHANGE_DUNGEON_ACK:
         {
             CnDbg("任务跳副本成功%d \n",m_id);
             End();
             break;
         }
     }
 }