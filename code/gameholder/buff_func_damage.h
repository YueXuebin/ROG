#ifndef buff_func_damage_h__
#define buff_func_damage_h__

#include "buff_func_base.h"

/*
 *	血量改变
 */
class BuffFuncDamage : public BuffFuncBase
{
public:
    BuffFuncDamage();
    virtual ~BuffFuncDamage();

    virtual void    Start();
    virtual void    Update(uint32 delta_time);
private:
    int32   m_SecondTimer;
};


#endif // buff_func_damage_h__
