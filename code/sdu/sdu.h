#ifndef sdu_h__
#define sdu_h__

// system define
#include "buildinclude.h"


// sdu interface

#include "sdtype.h"
#include "sdlog.h"
#include "sdnetutils.h"

#ifndef __APPLE__
#include "sdatomic.h"
#endif

#include "Assertion.h"
#include "sdcondition.h"
#include "sdconfigure.h"
#include "sddataqueue.h"
#include "sddatastream.h"
#include "sddir.h"
#include "sderrno.h"
#include "sdfifo.h"
#include "sdfile.h"
#include "sdfilemapping.h"
#include "sdcontainers.h"
#include "sdindexer.h"
#include "sdlimit.h"
#include "sdlock.h"
#include "sdloopbuffer.h"
#include "sdmath.h"
#include "sdmemorypool.h"
#include "sdmutex.h"
#include "sdobjectpool.h"
#include "sdobjectfifo.h"
#include "sdpoint.h"
#include "sdprocess.h"
#include "sdrandom.h"
#include "sdregex.h"
#include "sdserverid.h"
#include "sdshmem.h"
#include "sdsingleton.h"
#include "sdstring.h"
#include "sdthread.h"
#include "sdthreadpool.h"
#include "sdnamedsemaphore.h"
#include "sdtime.h"
#include "sdtimer.h"
#include "sdutil.h"
#include "httputils.h"


#include "NiQuickProf.h"    // 性能分析
#include "sdstackwalker.h"  // windows core dump
#include "getopt.h"         // 命令行解析
#include "colorlog.h"       // 彩色日志

// Version structure define of namespace Crown.
// Every module of Crown has its own version information.Make sure to get the
// right version DLL and Head file.Otherwise there may be some exceptions
// while using the modules.
// Crown version information has the following part:
// Major Version     :This field will increase by 1 when the module was changed
//                    from beta version to final version.
// Minor Version     :This field will increase by 1 every time the module's
//                    interface changed and conflicted with the last version.
// Compatible Version:This field will increase by 1 every time the module's
//                    interface changed but compatible with the last version.
// Build Number      :This field will increase by 1 every official build.And
//                    never back to zero.
//
struct SSDUVersion
{
    uint16 wMajorVersion;
    uint16 wMinorVersion;
    uint16 wCompatibleVersion;
    uint16 wBuildNumber;
};

namespace Crown
{
    const SSDUVersion SDU_VERSION = { 1, 0, 0, 2};

    inline SSDUVersion SDUGetVersion()
    {
        return SDU_VERSION;
    }

	inline std::string SDUGetVersionStr()
	{
		static char szVerStr[128] = {0}; 
#ifdef SDSTRING_H
		SDSprintf(szVerStr,"SDU v%d-%d-%d-%d",SDU_VERSION.wMajorVersion,
			SDU_VERSION.wMinorVersion,
			SDU_VERSION.wCompatibleVersion,
			SDU_VERSION.wBuildNumber); 
		return szVerStr; 
#endif
	}

}//end namespace

#endif // sdu_h__