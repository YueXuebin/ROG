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
    uint32 m_curitemid;//物品id
    int32  m_num;//物品个数
};
#endif
