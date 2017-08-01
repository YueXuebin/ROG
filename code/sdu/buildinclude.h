#pragma once

/*
 *	编译环境的include 
 */
#ifndef buidinclude_h__
#define buidinclude_h__

#include "buildconfig.h"

#ifdef WIN

#ifdef WIN32

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
	#endif

	#ifndef WINVER
	#define WINVER 0x0501
	#endif

	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501
	#endif						

	#ifndef _WIN32_WINDOWS
	#define _WIN32_WINDOWS 0x0410
	#endif
	
#endif // WIN32

// windows
#include <WinSock2.h>			// win sock 2.0
#include <mswsock.h>
#include <Windows.h>
#include <Tlhelp32.h>
#include <process.h>
#include <direct.h>	
#include <sys/stat.h>
#include <sqlext.h>

// C++
#include <conio.h>
#include <io.h>

#pragma warning(disable:4996) // suppress VS 2005 deprecated function warnings
#pragma warning(disable:4100)

#elif defined(LINUX) || defined(APPLE)
#include <unistd.h>
#include <time.h>

#endif // WIN & LINUX & APPLE

#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <signal.h>

#include <limits>
#include <string>
#include <vector>
#include <functional>
//#include <unordered_map>
#include <stdexcept>
#include <set>
#include <list>
#include <map>
//#include <hash_map>

#endif // buidinclude_h__
