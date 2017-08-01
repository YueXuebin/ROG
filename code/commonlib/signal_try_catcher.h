#ifndef signal_try_catcher_h__
#define signal_try_catcher_h__

/*
 *	使signal以异常方式抛出(使代码可以在崩溃时，通过捕获异常而继续执行)
 */

#include "setjmp.h"

extern jmp_buf g_JmpBuffer;        // setjmp和longjmp使用
extern bool    g_InSetJmp;          // 标记是否在setjmp的栈中

#ifdef WIN
#define SDSetJmp(x) setjmp(x)
#define SDLongJmp(x, y) longjmp(x, y)
#else
#define SDSetJmp(x) sigsetjmp(x, 1)
#define SDLongJmp(x, y) longjmp(x, y)
#endif


#endif // signal_try_catcher_h__
