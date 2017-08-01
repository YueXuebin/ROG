#pragma once

/******************************************************************************
		Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/
#ifndef SDLIMIT_H
#define SDLIMIT_H

#include <climits>

/**
* @file sdlimit.h
* @author lw
* @brief 数据类型最大最小值定义
*
**/
namespace Crown
{

    /* Mechanisms to properly type numeric literals */
#ifndef __GNUC__
#define SDU_INT64_C(val) (val##i64)
#define SDU_UINT64_C(val) (val##Ui64)
#else
#define SDU_INT64_C(val) (val##LL)
#define SDU_UINT64_C(val) (val##ULL)
#endif

#ifdef INT16_MIN
#define SDU_INT16_MIN   INT16_MIN
#else
#define SDU_INT16_MIN   (-0x7fff - 1)
#endif

#ifdef INT16_MAX
#define SDU_INT16_MAX  INT16_MAX
#else
#define SDU_INT16_MAX   (0x7fff)
#endif

#ifdef UINT16_MAX
#define SDU_UINT16_MAX  UINT16_MAX
#else
#define SDU_UINT16_MAX  (0xffff)
#endif

#ifdef INT32_MIN
#define SDU_INT32_MIN   INT32_MIN
#else
#define SDU_INT32_MIN   (-0x7fffffff - 1)
#endif

#ifdef INT32_MAX
#define SDU_INT32_MAX  INT32_MAX
#else
#define SDU_INT32_MAX  (0x7fffffff)
#endif

#ifdef UINT32_MAX
#define SDU_UINT32_MAX  UINT32_MAX
#else
#define SDU_UINT32_MAX  (0xffffffffU)
#endif

#ifdef INT64_MIN
#define SDU_INT64_MIN   INT64_MIN
#else
#define SDU_INT64_MIN   (SDU_INT64_C(-0x7fffffffffffffff) - 1)
#endif

#ifdef INT64_MAX
#define SDU_INT64_MAX   INT64_MAX
#else
#define SDU_INT64_MAX   SDU_INT64_C(0x7fffffffffffffff)
#endif

#ifdef UINT64_MAX
#define SDU_UINT64_MAX  UINT64_MAX
#else
#define SDU_UINT64_MAX  SDU_UINT64_C(0xffffffffffffffff)
#endif


}//namespace Crown

#endif // 
