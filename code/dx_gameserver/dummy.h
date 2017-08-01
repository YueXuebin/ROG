#ifndef dummy_h__
#define dummy_h__

#include "object.h"

class Dummy : public Object
{
public:
    Dummy() {}
    virtual ~Dummy() {}

protected:
    virtual void OnLostDevice() {}
    virtual void OnResetDevice() {}

};


#endif // dummy_h__
