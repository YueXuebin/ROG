#ifndef task_aciton_h
#define task_aciton_h
#include "action_base.h"
class Task_Aciton :public ActionBase
{
public:
    Task_Aciton(Robot* pRobot, uint32 id, int32 type);
    virtual ~Task_Aciton(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);
private:
    uint32 m_id;       //任务id或者npc id
    int32  m_type;    //请求接任务任务 ，和npc对话，切换副本，请求将奖励，提高任务品质请求 立即完成任务


};
#endif
