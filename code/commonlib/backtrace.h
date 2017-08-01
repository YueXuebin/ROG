#ifndef backtrace_h__
#define backtrace_h__


/*
 *	显示当前的堆栈信息
 *  注意：linux下需要带编译参数-rdynamic，才能看到函数信息
 */

void    ShowBackTrace();

#endif // backtrace_h__
