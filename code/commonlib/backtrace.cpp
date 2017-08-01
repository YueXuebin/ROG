#include "commonlib_pch.h"
#include "backtrace.h"
#ifdef WIN
#else
#include "execinfo.h"
#endif


/*

Linux下会生成如下的日志。
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_Z13ShowBackTracev+0x1a) [0x15a8346]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_Z13_SignalHanderi+0x46) [0x159bbce]
[FATAL][2016-12-08 19:46:11 583]: /lib64/libc.so.6() [0x30cb4329a0]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN11GmCommander14ParseGMCommandEP6PlayerRKSs+0x40f) [0xc3377f]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN11ChatManager9OnChatReqEPv+0x287) [0xe77ff7]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN11ChatManager6OnRecvEjPv+0x39) [0xe77d59]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN6Player6OnRecvEjPv+0x47e) [0xca136a]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN11GateHandler6OnRecvEyjjPvS0_+0xba) [0xc268ac]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN11GateHandler7RecvMsgEyjjPvS0_+0xbc) [0xc2674c]
[FATAL][2016-12-08 19:46:11 583]: ./gameserver(_ZN21AFTransferServerProxy4RecvEyjjPv+0x44) [0x15600ec]

可以使用addr2line查看具体的文件、函数、代码行数
如：
addr2line -e gameserver -f -C 0xc3377f
GmCommander::ParseGMCommand(Player*, std::string const&)
/root/work/mysg-server/code/gameholder/gm_commander.cpp:285

*/

void ShowBackTrace()
{
#ifdef WIN
    Crown::SdStackWalker sw;
    sw.Dump();
#else
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 10);
    strings = backtrace_symbols (array, size);

    for (i = 0; i < size; i++)
    {
        CnFatal("%s\n", strings[i]);
    }

    free (strings);
#endif
}
