#ifndef buy_aciton_h
#define buy_aciton_h
#include "action_base.h"
class Buy_Action: public ActionBase
{
public:
    Buy_Action(Robot* pRobot,uint32 itemid,int32 num);
    ~Buy_Action(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);
private:
    uint32 m_curitemid;//��Ʒid
    int32  m_num;//��Ʒ����
};
#endif
