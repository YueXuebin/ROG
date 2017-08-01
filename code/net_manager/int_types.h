//! @file int_types.h
//! @brief 整数的数据类型


#ifndef _INT_TYPES_H_
#define _INT_TYPES_H_


#ifdef WIN32	// for windows

typedef __int8					int8_t;
typedef __int16					int16_t;
typedef __int32					int32_t;
typedef __int64					int64_t;

typedef unsigned __int8			uint8_t;
typedef unsigned __int16		uint16_t;
typedef unsigned __int32		uint32_t;
typedef unsigned __int64		uint64_t;

#ifdef _WIN64
  typedef long int 				intptr_t;
  typedef unsigned long int		uintptr_t;
#else
  typedef int					intptr_t;
  typedef unsigned int			uintptr_t;
#endif

#else			// for linux

#include <stdint.h>

#endif


#endif // _INT_TYPES_H_
