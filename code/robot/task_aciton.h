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
    uint32 m_id;       //����id����npc id
    int32  m_type;    //������������� ����npc�Ի����л����������󽫽������������Ʒ������ �����������


};
#endif
