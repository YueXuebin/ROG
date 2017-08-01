//! @file types.h
//! @brief 模块用到的各项基本数据类型定义

#ifndef _TYPES_H_
#define _TYPES_H_

#include "sdtype.h"

#ifdef WIN32	// for windows

#include <stddef.h>				// for size_t

typedef char*					pstr_t;
typedef const char*				pcstr_t;

typedef char					int8_t;
typedef __int16					int16_t;
typedef __int32					int32_t;
typedef __int64					int64_t;

typedef unsigned char			uint8_t;
typedef unsigned __int16		uint16_t;
typedef unsigned __int32		uint32_t;
typedef unsigned __int64		uint64_t;

typedef int						ssize_t;
typedef int						socklen_t;
typedef uint32_t				in_addr_t;
typedef uint16_t				in_port_t;

#else			// for linux

#include <stdint.h>
#include <stddef.h>				// for size_t
#include <unistd.h>				// for ssize_t and socklen_t
#include <netinet/in.h>			// for in_addr_t and in_port_t

typedef char*					pstr_t;
typedef const char*				pcstr_t;

#endif

//typedef char                  	int8;
//typedef int16_t                 int16;
//typedef int32_t                 int32;
//typedef int64_t                 int64;
//
//typedef unsigned char           uint8;
//typedef uint16_t                uint16;
//typedef uint32_t                uint32;
//typedef uint64_t                uint64;

#endif // _TYPES_H_
