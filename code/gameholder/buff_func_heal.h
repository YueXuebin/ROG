#ifndef buff_func_heal_h__
#define buff_func_heal_h__


#include "buff_func_base.h"

/*
 *	回复生命值
 */
class BuffFuncHeal : public BuffFuncBase
{
public:
    BuffFuncHeal();
    virtual ~BuffFuncHeal();

    virtual void    Start();
    virtual void    Update(uint32 delta_time);
private:
    int32   m_SecondTimer;
};


#endif // buff_func_heal_h__
