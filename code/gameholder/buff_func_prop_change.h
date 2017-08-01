#ifndef buff_func_prop_up_h__
#define buff_func_prop_up_h__


#include "buff_func_base.h"


/*
 *	属性改变(基于参数)
 */
class BuffFuncPropChange : public BuffFuncBase
{
public:
    BuffFuncPropChange();
    virtual ~BuffFuncPropChange();

    virtual void    Start();
    virtual void    Update(uint32 delta_time);

    virtual void    OnUpdateValue() { m_FirstUpdate = true;}

private:
    bool    m_FirstUpdate;

};

#endif // buff_func_prop_up_h__
