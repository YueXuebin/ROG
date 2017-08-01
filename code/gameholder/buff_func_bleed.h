#ifndef buff_func_bleed_h__
#define buff_func_bleed_h__

#include "buff_func_base.h"

/*
 *	流血
 */
class BuffFuncBleed : public BuffFuncBase
{
public:
    BuffFuncBleed();
    virtual ~BuffFuncBleed();

    virtual void    Start();
    virtual void    Update(uint32 delta_time);
private:
    int32   m_SecondTimer;
};



#endif // buff_func_bleed_h__
