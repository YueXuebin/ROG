#pragma once

#ifndef SDNETDATA_H
#define SDNETDATA_H
/**
* @file sdnetdata.h
* @author wangkui
* @brief 网络数据转化系列
*
**/
#include <stdio.h>
#include "sdtype.h"

namespace Crown
{
    /**
    * @defgroup groupnetdata 网络传输封装
    * @ingroup  Crown
    * @{
    */

    /**
    *@brief 网络传输数据封装类实现
    */
    class CSDDataStream
    {
    public:
        CSDDataStream();
        virtual ~CSDDataStream();

        /**
        * @brief
        * 初始化net data
        * @param pszBuf : 输入数据buffer
        * @param dwBufLen : buffer的长度
        * @return void
        */
        void SDAPI Prepare(char *pszBuf, int32 dwBufLen);

        /**
        * @brief
        * 对数据区重置
        * @return void
        */
        void SDAPI Reset();

        /**
        * @brief
        * 得到封装好的数据缓冲区
        * @return 封装好的数据缓冲区指针
        */
        inline char* SDAPI GetData()
        {
            return m_pBuf;
        }

        /**
        * @brief
        * 得到封装好的数据缓冲区长度
        * @return 封装好的数据缓冲区长度
        */
        inline int32 SDAPI GetDataLen()
        {
            return m_pos;
        }

        /**
        * @brief
        * 数据包添加byte类型数据（1字节）
        * @param byValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        inline int32 SDAPI AddByte(byte byValue);

        /**
        * @brief
        * 数据包获取并删除byte类型数据（1字节）
        * @param byValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelByte( byte  &byValue);

        /**
        * @brief
        * 数据包添加char类型数据（1字节）
        * @param chChar : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddChar(char  chChar);

        /**
        * @brief
        * 数据包获取并删除char类型数据（1字节）
        * @param chChar : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelChar(char &chChar);

        /**
        * @brief
        * 数据包添加USHORT类型数据（2字节）
        * @param wValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddWord(uint16 wValue);

        /**
        * @brief
        * 数据包获取并删除USHORT类型数据（1字节）
        * @param wValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelWord(uint16 &wValue);

        /**
        * @brief
        * 数据包添加short类型数据（2字节）
        * @param sValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddShort(int16 sValue);

        /**
        * @brief
        * 数据包获取并删除short类型数据（2字节）
        * @param sValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelShort(int16 &sValue);

        /**
        * @brief
        * 数据包添加unsigned int类型数据
        * @param dwValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddDword(uint32 dwValue);

        /**
        * @brief
        * 数据包获取并删除unsigned int类型数据
        * @param dwValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelDword(uint32 &dwValue);

        /**
        * @brief
        * 数据包添加int类型数据
        * @param iValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddInt(int32 iValue);

        /**
        * @brief
        * 数据包获取并删除int类型数据
        * @param iValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelInt(int32 &iValue);

        /**
        * @brief
        * 数据包添加uint64类型数据(8字节)
        * @param uullValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddUint64(uint64 uullValue);

        /**
        * @brief
        * 数据包获取并删除uint64类型数据(8字节)
        * @param uullValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelUint64(uint64& uullValue);

        /**
        * @brief
        * 数据包添加int64类型数据(8字节)
        * @param llValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddInt64(int64 llValue);

        /**
        * @brief
        * 数据包获取并删除int64类型数据(8字节)
        * @param llValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelInt64(int64 &llValue);

        /**
        * @brief
        * 数据包添加real类型数据
        * @param fValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddFloat(real fValue);

        /**
        * @brief
        * 数据包获取并删除real类型数据
        * @param fValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelFloat(real &fValue);

        /**
        * @brief
        * 数据包添加double类型数据
        * @param dbValue : 添加的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddDouble(double dbValue);

        /**
        * @brief
        * 数据包获取并删除double类型数据
        * @param dbValue : 得到的数据内容
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelDouble(double &dbValue);

        /**
        * @brief
        * 数据包添加string类型数据
        * @param pszStr : 添加的数据内容
        * @param dwBufLen : 添加的数据长度
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddString(const char *pszStr, int32 dwBufLen);

        /**
        * @brief
        * 数据包获取并删除字符串类型数据
        * @param pszOut :得到的数据内容
        * @param dwBufLen : 得到的数据长度
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelString(char *pszOut, int32 dwBufLen);

        /**
        * @brief
        * 数据包添加一块buffer数据
        * @param pszBuf : 添加的buf地址
        * @param dwBufLen : 添加的buf长度
        * @return 数据包当前所指的位置
        */
        int32 SDAPI AddBuf(const byte *pszBuf, int32 dwBufLen);

        /**
        * @brief
        * 数据包获取并删除一块buffer数据
        * @param pszBuf :输入缓冲区得到buffer内容
        * @param dwBufLen : 得到的数据长度
        * @return 数据包当前所指的位置
        */
        int32 SDAPI DelBuf(byte *pszBuf, int32 dwBufLen);

    private:
        char    *m_pBuf;
        int32   m_bufLen;
        int32   m_pos;
    };

}
#endif
