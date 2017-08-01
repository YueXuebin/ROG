#ifndef buff_func_stun_h__
#define buff_func_stun_h__

#include "buff_func_base.h"

/*
 *	眩晕功能
 */
class BuffFuncStun : public BuffFuncBase
{
public:
    BuffFuncStun();
    virtual ~BuffFuncStun();

    virtual void    Start();
    virtual void    ResetLifeTime(int32 lifeTime);

private:
    void EnterStun(int32 time);
};


#endif // buff_func_stun_h__
