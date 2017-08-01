#pragma once

/*
 *	处理原子操作的函数系列
 */
#ifndef SDATOMIC_H
#define SDATOMIC_H

/**
* @file sdatomic.h
* @author lw
* @brief 处理原子操作的函数系列
*
**/
#include "sdtype.h"

namespace Crown
{
    /**
    * @defgroup groupatomic 原子操作
    * @ingroup  Crown
    * @{
    */

    /*
    * 原子操作32位数值,注意不要在Win32模式下使用64位的原子操作函数。内部使用了64位的API，
		需要Windows Vista以上的版本
    * 需要注意的是：每个函数的内部实现都是由操作系统自己确认的
    * 在本组函数中，
    * Add代表加法
    * Sub代表减法
    * Inc代表加1
    * Dec代表减1
    */


    /**
    * @brief
    * 从内存中原子读取一个32位值
    * @param pdwMem : 指向需要被读取值的指针
    * @return mem的值
    */
    int32 SDAPI SDAtomicRead32(volatile int32 *pdwMem);

	/**
	* @brief
	* 从内存中原子读取一个64位值, 需要Windows Vista以上操作系统
	* @param pqwMem : 指向需要被读取值的指针
	* @return mem的值
	*/
    int64 SDAPI SDAtomicRead64(volatile int64 *pqwMem);

    /**
    * @brief
    * 在内存中原子设置一个32位的值
    * @param pdwMem : 指向需要设置值的指针
    * @param dwValue : 需要设置的值
    * @return void
    */
    void SDAPI SDAtomicSet32(volatile int32 *pdwMem, int32 dwValue);

	/**
	* @brief
	* 在内存中原子设置一个64位的值, 需要Windows Vista以上操作系统
	* @param pqwMem : 指向需要设置值的指针
	* @param dwValue : 需要设置的值
	* @return void
	*/
    void SDAPI SDAtomicSet64(volatile int64 *pqwMem, int64 qwValue);

    /**
    * @brief
    * 32位原子加法，将原始值添加一个val值
    * @param pdwMem : 指向被加数的指针
    * @param dwValue : 加数
    * @return mem中被加前的原始值
    */
    int32 SDAPI SDAtomicAdd32(volatile int32 *pdwMem, int32 dwValue);

	/**
	* @brief
	* 64位原子加法，将原始值添加一个val值, 需要Windows Vista以上操作系统
	* @param pdwMem : 指向被加数的指针
	* @param dwValue : 加数
	* @return mem中被加前的原始值
	*/
    int64 SDAPI SDAtomicAdd64(volatile int64 *pqwMem, int64 qwValue);

    /**
    * @brief
    * 32位原子减法，将原始值减去一个val值
    * @param pdwMem : 指向被减数的指针
    * @param dwValue : 减数
    * @return 被减前的原值
    */
    int32 SDAPI SDAtomicSub32(volatile int32 *pdwMem, int32 dwValue);

	/**
	* @brief
	* 64位原子减法，将原始值减去一个val值, 需要Windows Vista以上操作系统
	* @param pdwMem : 指向被减数的指针
	* @param dwValue : 减数
	* @return 被减前的原值
	*/
    int64 SDAPI SDAtomicSub64(volatile int64 *pqwMem, int64 qwValue);

    /**
    * @brief
    * 32位原子加1算法，将原始值加1
    * @param pdwMem : 指向被加数的指针
    * @return mem中被加前的原始值
    */
    int32 SDAPI SDAtomicInc32(volatile int32 *pdwMem);
	
	/**
	* @brief
	* 64位原子加1算法，将原始值加1, 需要Windows Vista以上操作系统
	* @param pdwMem : 指向被加数的指针
	* @return mem中被加前的原始值
	*/
    int64 SDAPI SDAtomicInc64(volatile int64 *pqwMem);

    /**
    * @brief
    * 32位原子减1算法，将原始值减1
    * @param pdwMem : 指向被减数的指针
    * @return mem中被减前的原始值
    */
    int32 SDAPI SDAtomicDec32(volatile int32 *pdwMem);

	/**
	* @brief
	* 64位原子减1算法，将原始值减1, 需要Windows Vista以上操作系统
	* @param pdwMem : 指向被减数的指针
	* @return mem中被减前的原始值
	*/
    int64 SDAPI SDAtomicDec64(volatile int64 *pqwMem);

    /**
    * @brief
    * 32位原子的CAS(Compare and Swap)算法，将32位值pdwMem指向的值与dwCmp比较，
    * 如果一致，则将dwValue的值赋值给*pdwMem
    * @param pdwMem : 指向被比较的值的指针
    * @param dwValue : 若mem指向的值与cmp一致，被赋值的值
    * @param dwCmp : 比较的值
    * @return mem中被赋值前的原始值
    */
    int32 SDAPI SDAtomicCas32(volatile int32 *pdwMem, int32 dwValue, int32 dwCmp);

	/**
	* @brief
	* 64位原子的CAS(Compare and Swap)算法，将64位值mem指向的值与cmp比较, 需要Windows Vista以上操作系统
	* 如果一致，则将with的值赋值给*pdwMem
	* @param pdwMem : 指向被比较的值的指针
	* @param dwValue : 若mem指向的值与cmp一致，被赋值的值
	* @param dwCmp : 比较的值
	* @return mem中被赋值前的原始值
	*/
    int64 SDAPI SDAtomicCas64(volatile int64 *pqwMem, int64 qwValue, int64 qwCmp);

    /**
    * @brief
    * 交换算法，交换val与mem指向的值
    * @param pdwMem : 指向被交换的值的指针
    * @param dwValue : 交换的值
    * @return mem中被交换前的原始值
    */
    int32 SDAPI SDAtomicXchg32(volatile int32 *pdwMem, int32 dwValue);

	/**
	* @brief
	* 交换算法，交换val与mem指向的值, 需要Windows Vista以上操作系统
	* @param pdwMem : 指向被交换的值的指针
	* @param dwValue : 交换的值
	* @return mem中被交换前的原始值
	*/
    int64 SDAPI SDAtomicXchg64(volatile int64 *pqwMem, int64 qwValue);

    /**
    * @brief
    * 原子的指针的CAS(Compare and Swap)算法，将mem指向的指针与cmp指针比较，
    * 如果一致，则将with指针与mem指向的指针交换
    * @param pPtr : 指向被比较的指针的指针
    * @param dwValue : 若mem指向的指针与cmp一致，被赋值的指针
    * @param pCmp : 比较的指针
    * @return mem中原始的指针
    */
    void* SDAPI SDAtomicCasptr(volatile void **pPtr, void *pWith, const void *pCmp);

    /**
    * @brief
    * 交换指针算法
    * @param pPtr : pointer to the pointer指向被交换的指针的指针
    * @param pWith : what to swap it with被交换的指针
    * @return mem中被交换的原始指针
    */
    void* SDAPI SDAtomicXchgptr(volatile void **pPtr, void *pWith);

}

#endif

