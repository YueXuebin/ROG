#include "robot_pch.h"
#include "robot.h"
#include "achive_aciton.h"

Achive_aciton::Achive_aciton(Robot* pRobot):ActionBase(pRobot)
{
    m_Name = "AchiveAction";
}

Achive_aciton::~Achive_aciton(void)
{
}
void Achive_aciton::OnStart()
{
    ActionBase::OnStart();

    NONE req;
    m_Robot->Send(MSG_INST_ACHIVE_REQ,&req);
}
void Achive_aciton::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
    case  MSG_INST_ACHIVE_ACK:
        {
            ACHIVE_STATE_LIST* pData = (ACHIVE_STATE_LIST*) data;
            if (pData->opt_flag == OPT_ADD_FLAG)
            {
                uint32 curid = pData->achive_list.at(0).id;
                printf("添加了成就 num=%d\n", curid);
            }
            else if (pData->opt_flag == OPT_ADD_FLAG)
            {
                for (std::vector<ACHIVE_INFO>::iterator _iter = pData->achive_list.begin(); _iter!= pData->achive_list.end(); _iter++)
                {
                    if (_iter->b_finsh == FINISH_ACHIVE)
                    {
                        printf("完成了成就类型为 type=%d id=%d\n", _iter->type,_iter->id);
                    }
                }
               
            }else if (pData->opt_flag == OPT_INIT_FLAG)
            {
                for (std::vector<ACHIVE_INFO>::iterator _iter = pData->achive_list.begin(); _iter!= pData->achive_list.end(); _iter++)
                {
                    if (_iter->b_finsh == FINISH_ACHIVE)
                    {
                        printf("完成了成就类型为 type=%d id=%d\n", _iter->type,_iter->id);
                    }
                }
            }
             End();
        }
        break;
    }
}