/*
 *	信号捕获器
 */
#ifndef signal_catcher_h__
#define signal_catcher_h__

typedef void (*pSignalHandler)(int32);

void SignalCatcher(pSignalHandler pHander);

#endif // signal_catcher_h__
