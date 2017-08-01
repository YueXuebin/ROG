#ifndef equip_action_h
#define equip_action_h
#include "action_base.h"
class Equip_Action  : public ActionBase
{
public:
    Equip_Action(Robot* pRobot, int32 equiptype, int32 type, int32 pos);
    virtual ~Equip_Action(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);
private:

    int32 m_equiptype;
    int32 m_type;   //发送升级的类型
    int32 m_pos;    //装载或卸载宝石的位置
};
#endif
