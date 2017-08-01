
#ifndef buff_func_immortal_h__
#define buff_func_immortal_h__

#include "buff_func_base.h"

/*
 *	免疫物理伤害
 */
class BuffFuncImmortal : public BuffFuncBase
{
public:
    BuffFuncImmortal();
    virtual ~BuffFuncImmortal();

    virtual void    Start();
    virtual void    End();
};


#endif // buff_func_immortal_h__
