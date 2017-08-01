/*
 *	世界对象基类
 */
#ifndef object_h__
#define object_h__

#include "event_receiver.h"
#include "rtti.h"

class Object : public EventReceiver
{
	CnDeclareRootRTTI(Object)
public:
	Object(uint32 objType);
	virtual ~Object();
};

#endif // object_h__
