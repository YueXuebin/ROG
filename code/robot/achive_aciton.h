#ifndef achivepet_action_h
#define achivepet_action_h

#include "action_base.h"

class Achive_aciton : public ActionBase
{
public:
    Achive_aciton(Robot* pRobot);
   virtual ~Achive_aciton(void);
   virtual void OnStart();
   virtual void OnRecv(uint32 msgID, void* data);

private:

};


#endif
