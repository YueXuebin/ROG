#include "gameholder_pch.h"
#include "object.h"

CnImplementRootRTTI(Object);

Object::Object( uint32 objType ) : 
EventReceiver(objType)
{

}

Object::~Object()
{

}

