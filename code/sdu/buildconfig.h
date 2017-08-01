#pragma once

/*
 *	预定义
 */
#ifndef buildconfig_h__
#define buildconfig_h__

#if defined(DEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__) || defined(_M_X64)
#define BITS64
#endif

// 平台定义
#if defined  WIN32 || WIN64
// windows平台
#define WIN
#ifdef BITS64
#define WIN64
#endif

#elif defined __linux__
// linux平台
#define LINUX

#ifdef BITS64
#define LINUX64
#else
#define LINUX32
#endif

#elif defined __APPLE__
#define APPLE

#ifdef BITS64
#define APPLE64
#else
#define APPLE32
#endif

#else
#pragma error "unknown system"
#endif

#ifdef WIN
#include <crtdbg.h>
#endif

// Assert 定义
#ifdef WIN
#define __CnAssert(condition) _ASSERT(condition)
#elif defined(LINUX) || defined(APPLE)
#define __CnAssert(condition) assert(condition)
#endif

//
#if defined(_DEBUG) && defined(WIN)
#define	CnNew		new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define CnDelete	delete
#else
#define CnNew		new
#define CnDelete	delete
#endif

#if defined(_DEBUG) && defined(WIN)
#define CnMalloc(x) _malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
#define CnFree(x)	_free_dbg(x, _NORMAL_BLOCK)
#else
#define CnMalloc(x) malloc(x)
#define CnFree(x)	free(x)
#endif

#if defined(BITS64)
#define VOID_TO_UINT(x)     ((uint64)x)
#else
#define VOID_TO_UINT(x)     ((uint32)x)
#endif


//这里利用了模板 特型 的特性，来实现编译期检查。
//Refer: http://blogs.msdn.com/b/abhinaba/archive/2008/10/27/c-c-compile-time-asserts.aspx
namespace compile_assertion
{
    template <bool> struct STATIC_ASSERT_FAILURE;
    template <> struct STATIC_ASSERT_FAILURE<true> { enum { value = 1 }; };

    template <int x> struct static_assert_test{};
}

#define COMPILE_ASSERT(x) \
    typedef ::compile_assertion::static_assert_test<\
        sizeof(::compile_assertion::STATIC_ASSERT_FAILURE< (bool)( x ) >)>\
            _static_assert_typedef_

#endif // buildconfig_h__
