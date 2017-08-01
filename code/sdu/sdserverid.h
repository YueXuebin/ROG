#pragma once

/*
 *	游戏服务器中的服务器id处理函数
 */
#ifndef SDSERVERID_H
#define SDSERVERID_H
/**
* @file sdserverid.h
* @author wangkui
* @brief ServerID类
*
**/
#include "sdtype.h"
#include <string>


namespace Crown
{
    /**
    * @defgroup groupgametools GAME TOOLS
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief
    * 将ServerID从数字转化为字符串
    * @param dwServerID : 数字的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则数字的ServerID的31－24位代表字符串的ServerID的第一位part1，
    * 数字的ServerID的23－16代表字符串的ServerID的第二位part2，
    * 数字的ServerID的15－8代表字符串的ServerID的第三位part3，
    * 数字的ServerID的7－0代表字符串的ServerID的第四位part4，
    * @return 转化为字符串的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    std::string SDAPI SDServerIDUtoa(uint32 dwServerID, uint32 dwMask = 0x08080808);

    /**
    * @brief
    * 将ServerID从字符串转化为数字
    * @param pszServerID : 字符串的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则字符串的ServerID的第一位part1代表数字的ServerID的31－24位，
    * 字符串的ServerID的第一位part2代表数字的ServerID的23－16位，
    * 字符串的ServerID的第一位part3代表数字的ServerID的15－8位，
    * 字符串的ServerID的第一位part4代表数字的ServerID的7－0位，
    * @return 转化为数字的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    uint32 SDAPI SDServerIDAtou(const char *pszServerID, uint32 dwMask = 0x08080808);

//TODO 增加规范

    /**
    *@brief ServerID类
    */
    class CSDServerId
    {
    public:
        CSDServerId(uint32 dwId = 0, uint32 dwMask = 0x08080808);
        ~CSDServerId() {}

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的数字型的ID
        * @return void
        */
        void SDAPI SetID(uint32 dwID, uint32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的字符串型的ID
        * @return void
        */
        void SDAPI SetID(const char *pszID, uint32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置掩码
        * @param dwMask : 设置的掩码, 注意掩码数总和不能超过sizeof(uint32)
        * @return void
        */
        void SDAPI SetMask(uint32 dwMask);

        /**
        * @brief
        * 获取掩码
        * @param dwMask : 获取的掩码值
        * @return void
        */
        uint32 SDAPI GetMask();

        /**
        * @brief
        * 获取AreaID，即字符串型ServerID的第一位
        * @return 获取AreaID
        */
        uint32 SDAPI GetAreaId();

        /**
        * @brief
        * 获取GroupID，即字符串型ServerID的第二位
        * @return 获取GroupID
        */
        uint32 SDAPI GetGroupId();

        /**
        * @brief
        * 获取ServerType，及字符串型ServerID的第三位
        * @return 获取ServerType
        */
        uint32 SDAPI GetServerType();

        /**
        * @brief
        * 获取ServerIndex，及字符串型ServerID的第四位
        * @return 获取ServerIndex
        */
        uint32 SDAPI GetServerIndex();

        /**
        * @brief
        * 获取字符串型的ServerID
        * @return 字符串型的ServerID
        */
        std::string SDAPI AsString();

        /**
        * @brief
        * 获取数字型的ServerID
        * @return 数字型的ServerID
        */
        uint32 SDAPI AsNumber();

    private:
        uint32       m_maskLen[4] ; //mask的长度，用来移位
        uint32       m_subMask[4] ; //mask的值
        uint32       m_id;          // 数字ID
        uint32       m_mask;        //掩码,注意掩码数总和不能超过32
        std::string  m_strId;
    };


    /**
    * @brief
    * 将ServerID从数字转化为字符串
    * @param dwServerID : 数字的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则数字的ServerID的31－24位代表字符串的ServerID的第一位part1，
    * 数字的ServerID的23－16代表字符串的ServerID的第二位part2，
    * 数字的ServerID的15－8代表字符串的ServerID的第三位part3，
    * 数字的ServerID的7－0代表字符串的ServerID的第四位part4，
    * @return 转化为字符串的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    std::string SDAPI SDServerIDExUtoa(uint64 dwServerID, uint32 dwMask = 0x08080808);

    /**
    * @brief
    * 将ServerID从字符串转化为数字
    * @param pszServerID : 字符串的ServerID
    * @param dwMask : 掩码，代表字符串的ServerID的每一位占数字的ServerID的多少位
    * 如，如果mask为0x08080808，
    * 则字符串的ServerID的第一位part1代表数字的ServerID的31－24位，
    * 字符串的ServerID的第一位part2代表数字的ServerID的23－16位，
    * 字符串的ServerID的第一位part3代表数字的ServerID的15－8位，
    * 字符串的ServerID的第一位part4代表数字的ServerID的7－0位，
    * @return 转化为数字的ServerID
    * @remark : 字符串ServerID的格式为: part1-part2-part3-part4.
    */
    uint64 SDAPI SDServerIDExAtou(const char* pszServerID, uint32 dwMask = 0x08080808);

    class CSDServerIdEx
    {
    public:
        CSDServerIdEx(uint64 dwId = 0, uint32 dwMask = 0x08080808);
        ~CSDServerIdEx() {}

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的数字型的ID
        * @return void
        */
        void SDAPI SetID(uint64 dwID, uint32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置ID
        * @param pszID : 设置的字符串型的ID
        * @return void
        */
        void SDAPI SetID(const char* pszID, uint32 dwMask = 0x08080808);

        /**
        * @brief
        * 设置掩码
        * @param dwMask : 设置的掩码, 注意掩码数总和不能超过sizeof(uint32)
        * @return void
        */
        void SDAPI SetMask(uint32 dwMask);

        /**
        * @brief
        * 获取掩码
        * @param dwMask : 获取的掩码值
        * @return void
        */
        uint32 SDAPI GetMask();


        /**
        * @brief
        * 获取游戏中id
        * @return uint32
        */
        uint32 SDAPI GetGameId();


        /**
        * @brief
        * 获取AreaID，即字符串型ServerID的第一位
        * @return 获取AreaID
        */
        uint32 SDAPI GetAreaId();

        /**
        * @brief
        * 获取GroupID，即字符串型ServerID的第二位
        * @return 获取GroupID
        */
        uint32 SDAPI GetGroupId();

        /**
        * @brief
        * 获取ServerType，及字符串型ServerID的第三位
        * @return 获取ServerType
        */
        uint32 SDAPI GetServerType();

        /**
        * @brief
        * 获取ServerIndex，及字符串型ServerID的第四位
        * @return 获取ServerIndex
        */
        uint32 SDAPI GetServerIndex();

        /**
        * @brief
        * 获取字符串型的ServerID
        * @return 字符串型的ServerID
        */
        std::string SDAPI AsString();

        /**
        * @brief
        * 获取数字型的ServerID
        * @return 数字型的ServerID
        */
        uint64 SDAPI AsNumber();
    private:
        uint32        m_maskLen[4] ; //mask的长度，用来移位
        uint32        m_subMask[4] ; //mask的值
        uint32       m_mask ; //服务器ID掩码
        uint64       m_id;
        std::string  m_strId;
    };

    
}

#endif // 

