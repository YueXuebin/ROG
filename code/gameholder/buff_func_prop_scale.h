#ifndef buff_func_prop_scale_h__
#define buff_func_prop_scale_h__

#include "buff_func_base.h"


/*
 *	属性改变(基于人物属性按百分比增减)
 */
class BuffFuncPropScale : public BuffFuncBase
{
public:
    BuffFuncPropScale();
    virtual ~BuffFuncPropScale();

    virtual void    Start();
    virtual void    Update(uint32 delta_time);

    virtual void    OnUpdateValue() { m_FirstUpdate = true;}

private:
    bool    m_FirstUpdate;
};


#endif // buff_func_prop_scale_h__
