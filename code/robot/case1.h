/*
 *	用例1：什么都不做
 */
#ifndef case1_h__
#define case1_h__

#include "case_base.h"

class Case1 : public CaseBase
{
public:
    Case1(Robot* pRobot);
    virtual ~Case1();

    virtual void OnRecv(uint32 msgID, void* data);

    virtual void Update(uint32 tickcount, uint32 deltacount);

};


#endif // case1_h__
