#pragma once


#ifndef SDMATH_H
#define SDMATH_H

#include "math2d.h"
#ifdef WIN
//#include <boost/math/special_functions/fpclassify.hpp>
#endif
/**
* @file sdmath.h
* @author lw
* @brief 数学计算类
*
**/
namespace Crown
{
    /**
    * @defgroup 取最大最小值
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief
    * 比较两个数据，返回最小值
    * @param const T & a ,const T & b : 进行比较的连个值
    * @return  T &  : 返回最小值
    */
    template<typename T >
    inline const T & SDMin(const T & __a, const T & __b)
    {
        return (__b < __a ) ? __b: __a;
    }

    /**
    * @brief
    * 比较两个数据，返回最大值
    * @param const T & a ,const T & b : 进行比较的连个值
    * @return T &  : 返回最大值
    */
    template<typename T >
    inline const T & SDMax(const T & __a, const T & __b)
    {
        return (__a < __b)? __b : __a;
    }

    template<typename T >
    inline const T & SDRange(const T & __min, const T & __val, const T & __max)
    {
        if (__val < __min)
        {
            return __min;
        }

        if (__val > __max)
        {
            return __max;
        }

        return __val;
    }

    template <typename T>
    inline bool isnan(T a)
    {
#ifdef WIN
        return _isnan(a);
#else
        return std::isnan(a);
#endif
    }

} //namespace Crown

#endif // 

