#ifndef buff_func_shock_h__
#define buff_func_shock_h__

#include "buff_func_base.h"

/*
 *	感电状态
 */
class BuffFuncShock : public BuffFuncBase
{
public:
    BuffFuncShock();
    virtual ~BuffFuncShock();

    virtual void    Start();
    virtual void    End();
};


#endif // buff_func_shock_h__
