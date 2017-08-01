#include "SDuPCH.h"
#ifdef WIN
#include "sdstackwalker.h"
#include "StackWalker.h"

void Crown::SdStackWalker::Dump()
{
	StackWalker sw;
	sw.ShowCallstack();
}

Crown::SdStackWalker::SdStackWalker()
{

}

#else


#endif