#pragma once

// 配置文件读取类,使用xml或ini文件

#ifndef SDCONFIGURE_H
#define SDCONFIGURE_H

// 配置相关类
#include "sdxmlconfigure.h"
#include "sdiniconfigure.h"
#include "sdmapconfigure.h"

#include <sstream>

namespace Crown
{
    /**
    * @defgroup groupconfigure 配置文件读写，本类不是线程安全的
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief
    * 模板字符串转换
    * @param value : 输入的需要转换的字符串
    * @return 转换后的值
    */
//    template <typename T>
//    T SDConvterTrait( const std::string & value )
//    {
//        return value;
//    };

    /**
    * @brief
    * 字符串转换为int
    * @param value : 输入的需要转换的字符串
    * @return 转换后的值
    */
//    template <>
//    int32 SDConvterTrait<int32>( const std::string & value )
//    {
//        return atoi(value.c_str());
//    };

   // 配置文件读取类
    template<typename ConfigType>
    class CSDConfigureImpl
    {
    public:
//        CSDConfigureImpl()
//        {
//        }

        /**
        * @brief
        * 读入一个配置文件
        * @param pszFileName : 配置文件的路径
        * @param pLocate :文件的编码格式 默认为UTF-8
        * @return 读取成功返回true，错误返回false
        */

	bool Read( const char * /*pszFileName*/, const char* /*pLocate*/ = nullptr )
        {
            return true; // m_configure.ReadConfig(pszFileName);
        }


	bool Write( const char * /*pszFileName*/ )
        {
            return true; //m_configure.WriteConfig(pszFileName);
        }


        /**
        * @brief
        * 读取一个配置项
        * @param p : 配置项的名称
        * @param def : 默认值，如果没有此配置项，返回指定的默认值
        * @return 配置项的内容
        */
        template <typename T>
	T GetProperty( const char* /*pszProp*/, const T& def = T() )
        {
            // std::string result = m_configure.GetProperty(pszProp);
            // return result.empty() ? def : SDConvterTrait<T>(result);
            return def;
        }

	std::string  GetProperty( const char* /*pszProp*/ )
        {
            // return m_configure.GetProperty(pszProp);
            return "";
        }

        /**
        * @brief
        * 设置一个配置项，此接口暂时未实现
        * @param pProp : 配置项的名称
        * @param value : 配置项的值
        * @return oid
        */
        template <typename T>
	void SetProperty( const char* /*pszProp*/, const T& value )
        {
            std::stringstream strsteam;
            strsteam << value;
            // m_configure.SetProperty(pszProp, strsteam.str());
        }

    private:
        // ConfigType m_configure;
    };

    //typedef CSDConfigureImpl<CSDMapConfigure> CSDMapConfig;
    //typedef CSDConfigureImpl<CSDXmlConfigure> CSDXmlConfig;
    //typedef CSDConfigureImpl<CSDIniConfigure> CSDIniConfig;


}

#endif // SDCONFIGURE_H
