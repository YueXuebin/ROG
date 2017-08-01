#pragma once

/******************************************************************************
			Copyright (C) Shanda Corporation. All rights reserved.

	sdregex.h - 正规表达式操作,使用了boost的xpressive头文件库,不用链接第boost库

******************************************************************************/
#ifndef SDREGEX_H
#define SDREGEX_H
/**
* @file sdregex.h
* @author wk
* @brief 正则表达式类
*
**/
#include "sdmacros.h"
#include "sdtype.h"
#include <string>
#include <vector>

#if SDU_WITH_BOOST
namespace Crown
{
    typedef std::vector<std::string> SMatchResult;
    /**
    * @brief
    * 从字符串中查找匹配的字符串 如  SDRegexMatch("hell world!",match,"(\\w+) (\\w+)!" );
    * @param pzStr：  原始字符串
    * @param SMatchResult ： 匹配后的结果
    * @param pattern ： 需要匹配的正规表达式
    * @return 返回是否匹配成功
    */
    bool SDRegexMatch(const char * pzStr,  SMatchResult & result, const char * pattern);

    /**
    * @brief
    * 从字符串中查找匹配的字符串 如  SDRegexMatch("hell world!",match,"(\\w+) (\\w+)!" );
    * @param pzStr：  原始字符串,以stl的表示
    * @param SMatchResult ： 匹配后的结果
    * @param pattern ： 需要匹配的正规表达式
    * @return 返回是否匹配成功
    */
    bool SDRegexMatch(const std::string & pzStr,  SMatchResult & result, const std::string&  pattern);

    /**
    * @brief
    * 通过正规表达式查找字符串中的子串，并用字符串进行替换
    *  如  SDRegexReplace("This is his face","his","her" ); 结果为"Ther is her face";
    * @param pzStr：  原始字符串
    * @param pattern ：匹配原始串中的正规表达式
    * @param format ： 匹配后需要替换的内容
    * @return 返回替换后的字符串
    */
    std::string SDRegexReplace(const char * pzStr , const char * pattern ,const char * format);

    /**
    * @brief
    * 通过正规表达式查找字符串中的子串，并用字符串进行替换
    *  如  SDRegexReplace("This is his face","his","her" ); 结果为"Ther is her face";
    * @param pzStr：  原始字符串, 以stl的表示
    * @param pattern ：匹配原始串中的正规表达式
    * @param format ： 匹配后需要替换的内容
    * @return 返回替换后的字符串
    */
    std::string SDRegexReplace(const std::string &  pzStr , const std::string &  pattern ,const std::string &  format);
}

#endif // SDU_WITH_BOOST


#endif // 

