#ifndef followpet_aciton_h
#define followpet_aciton_h 
#include "action_base.h"
class Followpet_Aciton : public ActionBase
{
public:
    Followpet_Aciton(Robot* pRobot,int32 pet_id,int32 upType,int32 type);
    virtual ~Followpet_Aciton(void);
    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);
    virtual void Update( uint32 tickcount, uint32 deltacount );
private:
    uint32 m_petid;
    int32 m_type;//发送出战或休息
    int32 m_upType;
    uint32 starttime;
};
#endif