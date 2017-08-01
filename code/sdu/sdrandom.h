#pragma once

#ifndef SDRANDOM_H
#define SDRANDOM_H
/**
* @file sdrandom.h
* @author lw
* @brief 随机数系列
*
**/
#include "sdtype.h"
#include <cstdlib>

namespace Crown
{
    /**
    * @defgroup grouprandom 随机数操作
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief
    * C-API方式，设置随机数种子
    * @param dwSeed ：随机数种子
    * @return void
    */
    void SDSetSeed(uint32 dwSeed);

    /**
    * @brief
    * C-API方式，获取随机数
    * @return 返回随机数
    */
    int32 SDRandom();

    /**
    * @brief
    * C-API方式，获取随机数
    * @return 返回指定范围内的随机数
    */
    uint32 SDRandom(uint32 dwRange);

    /**
    * @brief
    * 获取随机数，范围是[unMinRange, unMaxRange - 1]
    * @param dwMin ：最小值
    * @param dwMax ：最大值
    * @return 返回随机数
    */
    int32 SDRandom(int32 dwMin, int32 dwMax);



    // 四舍五入
    uint32 SDRandomRound(uint32 range);

    /**
    * @brief
    * 触发随机概率
    * @param percent : 概率值
    * @return 是否满足概率设置
    */
    bool RandomPercent(uint32 percent);

    /**
    * @brief
    * 从指定id的List中，随机选出一个id (概率相同)
    * @param count  可变参数的个数
    * @return 是否满足概率设置
    */
    uint32 RandomOneNumber(uint32 count, ...);

    /**
    * @brief
    * 获取vector中得到其中一个
    * @return 返回随机数
    */
    //template <class T>
    //T Random(std::vector<T> dwRange)
    //{
    //    int32 count = dwRange.size();
    //    CnAssert(count != 0);
    //    count = SDRandom(count);

    //    std::vector<T>::iterator it;
    //    for (it = dwRange.begin(); it != dwRange.end(); it++)
    //    {
    //        if (count-- <= 0)
    //        {
    //            break;
    //        }
    //    }
    //    return *it;
    //}

    float SDRandomFloat(float range);
    /**
    *@brief 随机数操作接口
    */
    class CSDRandom
    {
    public:
        CSDRandom();

        /**
        * @brief
        * 设置随机数种子
        * @param dwSeed ：随机数种子
        * @return void
        */
        inline void SetSeed(uint32 dwSeed)
        {
            m_seed = dwSeed;
        }

        /**
        * @brief
        * 获取随机数
        * @return 返回随机数
        */
        uint32 Random();


        /**
        * @brief
        * 获取随机数，范围是[0, unRange - 1]
        * @param unRange ：最大值
        * @return 返回随机数
        */
        uint32 Random(uint32 dwRange);


        /**
        * @brief
        * 获取随机数，范围是[unMinRange, unMaxRange - 1]
        * @param dwMin ：最小值
        * @param dwMax ：最大值
        * @return 返回随机数
        */
        uint32 Random(uint32 dwMin, uint32 dwMax);

    private:
        uint32 m_seed;
    };

}

#endif
