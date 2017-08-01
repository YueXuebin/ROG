#ifndef horoscope_action_h
#define horoscope_action_h

#include "action_base.h"

class Horoscope_Action : public ActionBase
{
public:
    Horoscope_Action(Robot* pRobot,int32 id,int32 type);
    virtual ~Horoscope_Action(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    uint32 m_levelup_id;
    int32  m_type;//¿‡–Õ

};

#endif