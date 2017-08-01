/*
 *	预编译头
 */
#ifndef net_manager_pch_h__
#define net_manager_pch_h__

#if defined WIN32 || WIN64
// 重定义select的fd_set数
#undef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#include <assert.h>
#include <vector>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <list>
#include <sstream>

#include "types.h"
#include "sdu.h"

#endif // net_manager_pch_h__
