#include "commonlib_pch.h"
#include "signal_catcher.h"
#include "signal_try_catcher.h"
#include "error_reporter.h"
#include "backtrace.h"

bool justOnce = true;
pSignalHandler pSignalHander = NULL;    // 用户的信号处理函数
std::terminate_handler oldHandler = NULL;

void _SetSignalHandler();

void _SignalHander(int32 sig)
{
    // 需要显示堆栈信息的signal处理
#ifdef WIN
    if(sig == SIGILL || sig == SIGABRT || sig == SIGFPE || sig == SIGSEGV)
#else
    if(sig == SIGQUIT || sig == SIGQUIT || sig == SIGILL || sig == SIGTRAP || sig == SIGABRT || sig == SIGBUS || sig == SIGFPE || sig == SIGKILL || sig == SIGSEGV)
#endif
    {
        ShowBackTrace();
    }

    // signal作为异常抛出的处理
    if(g_InSetJmp)
    {
        _SetSignalHandler();       // longjmp前需要再次设置signal的处理函数
        // 用longjmp将signal作为异常处理
        SDLongJmp             
            (
            g_JmpBuffer,        // using this context to say where to jump to
            sig                 // and passing back the value of the signal.
            );
    }

    // log
    CnInfo("signal %d\n", sig);
    pSignalHander(sig);

#ifdef LINUX
    if(sig == SIGHUP)
    {
        // 终端断开
    }
    else if(sig == SIGINT)
    {
        // Ctrl+C
    }
    else if(sig == SIGPIPE)
    {
        // 管道破裂
        CnInfo("recv signal SIGPIPE\n");
    }
    else if(sig == SIGQUIT || sig == SIGQUIT || sig == SIGILL || sig == SIGTRAP || sig == SIGABRT
        || sig == SIGBUS || sig == SIGFPE || sig == SIGKILL || sig == SIGSEGV)
    {
        // 会生成core的信号
        signal(sig, SIG_DFL);
        raise(sig);
    }
    else
    {   // 需要执行默认处理的信号
        signal(sig, SIG_DFL);
        raise(sig);
    }
#else
    if(sig != SIGINT && sig != SIGBREAK)
    {
        raise(sig);
    }
#endif
}

void LinuxExit(void)
{
    if(pSignalHander)
    {
        //ErrorReporter("exit not normal\n");
    }
    else
    {
        // do nothing ...
    }
}

void TermFunction() 
{
    CnInfo("call terminate\n");
    abort();
}

// 设置各signal的处理函数
void _SetSignalHandler()
{
#ifdef LINUX
    signal(SIGHUP, _SignalHander);      // 终端断开
#endif

    signal(SIGINT, _SignalHander);      // Ctrl+C

#ifdef LINUX
    // linux
    signal(SIGQUIT, _SignalHander);   // core
    signal(SIGILL, _SignalHander);    // core
    signal(SIGTRAP, _SignalHander);   // core
    signal(SIGABRT, _SignalHander);   // core 6 调用abort函数生成的信号
    signal(SIGBUS, _SignalHander);    // core
    signal(SIGFPE, _SignalHander);    // core
    signal(SIGKILL, _SignalHander);   // 不可截获 9
    signal(SIGUSR1, _SignalHander);
    signal(SIGSEGV, _SignalHander);   // core
    signal(SIGUSR2, _SignalHander);
    signal(SIGPIPE, _SignalHander);     // 管道破裂
    signal(SIGALRM, _SignalHander);     // 时钟定时信号
    signal(SIGTERM, _SignalHander);     // 程序结束(terminate)信号
    //16
    signal(SIGCHLD, _SignalHander);     // 子进程结束
    signal(SIGCONT, _SignalHander);     // 让一个停止的进程继续执行
    signal(SIGSTOP, _SignalHander);     // 停止进程的执行
    signal(SIGTSTP, _SignalHander);     // 停止进程的运行
    signal(SIGTTIN, _SignalHander);     // 当后台作业要从用户终端读数据时, 该作业中的所有进程会收到SIGTTIN信号. 缺省时这些进程会停止执行
    signal(SIGTTOU, _SignalHander);     // 类似于SIGTTIN, 但在写终端(或修改终端模式)时收到
    signal(SIGURG, _SignalHander);      // 有"紧急"数据或out-of-band数据到达socket时产生
    signal(SIGXCPU, _SignalHander);     // 超过CPU时间资源限制. 这个限制可以由getrlimit/setrlimit来读取/改变
    signal(SIGXFSZ, _SignalHander);     // 超过文件大小资源限制
    signal(SIGVTALRM, _SignalHander);   // 虚拟时钟信号. 类似于SIGALRM, 但是计算的是该进程占用的CPU时间
    signal(SIGPROF, _SignalHander);     // 类似于SIGALRM/SIGVTALRM, 但包括该进程用的CPU时间以及系统调用的时间
    signal(SIGWINCH, _SignalHander);    // 窗口大小改变时发出
    signal(SIGIO, _SignalHander);       // 文件描述符准备就绪, 可以开始进行输入/输出操作
    signal(SIGPWR, _SignalHander);      // Power failure
#else
    // window
    signal(SIGTERM, _SignalHander);
    signal(SIGBREAK, SIG_IGN);          // 忽略
    signal(SIGBREAK, _SignalHander);    // console窗口叉掉
    signal(SIGABRT, _SignalHander);
    signal(SIGSEGV, _SignalHander);     // segment fault

#endif

    CnInfo("set signal handler\n");

}

void SignalCatcher( pSignalHandler pHander )
{
    pSignalHander = pHander;

    if(pSignalHander == NULL)
    {
        std::set_terminate(oldHandler);
        CnInfo("uncatch signal\n");
        return;
    }

    oldHandler = std::set_terminate(TermFunction);

    CnAssert(justOnce);
    justOnce = false;

    atexit(LinuxExit);

    _SetSignalHandler();           // 设置signal的处理函数
}


