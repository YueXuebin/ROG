#pragma once

#ifndef SDTIME_H
#define SDTIME_H

#include "sdtype.h"
#include <ctime>
#include <string>


/**
* @file sdtime.h
* @author lw
* @brief 时间处理系列
*
**/
namespace Crown
{
    /**
    * @defgroup grouptime 时间操作
    * @ingroup  Crown
    * @{
    */
    class CSDDateTime;

    /**
    * @brief 获取代表当前日期时间(调用此函数的此时此刻)的CSDDateTime
    * @return 代表当前日期时间(调用此函数的此时此刻)的CSDDateTime
    */

    CSDDateTime SDNow();

    /**
    * @brief 根据字符串返回时间戳
    * @param 日期时间字符串
    * @return 时间戳
    */
    uint32 TimeStamp(const std::string& str);

    std::string TimeString(time_t t);

    // 获得每日时间的秒数
    uint32 TimeToUint(const std::string& str);

    /**
    * @brief 日期时间类,代表1970年午夜到2037年之间的某一个时间
    *
    */
    class  CSDDateTime
    {
        friend uint32 TimeStamp(const std::string& str);
    public:
        /**
        * @brief 构造一个CSDDateTime类,构造出来的时间为1970-1-1午夜
        */
        CSDDateTime(time_t t = 0);

        CSDDateTime(const std::string& str);


        /**
        * @brief 获取本类所代表的日期时间的time_t值(unix时间戳)
        * @return 返回的time_t值
        */
        time_t GetTimeValue();

        //static time_t GetTimeZoneDiff();

        /**
        * @brief 设置本类所代表的日期时间
        * @param t 设置的日期时间
        * @return void
        */
        void SetTimeValue(time_t t);

        /**
        * @brief 设置本类所在的时区
        * @param t 时区
        * @return void
        */
        //static void SetTimeZone(int32 timezone);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的秒数
        * @param datetime 一个日期时间
        * @return 差距的秒数
        */
        int64 operator - (const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的秒数
        * @param datetime 一个日期时间
        * @return 差距的秒数
        */
        int64 DiffSecond(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的分钟数,差距少于1分钟的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的分钟数
        */
        int64 DiffMinute(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的小时数,差距少于1小时的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的小时数
        */
        int64 DiffHour(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的天数,差距少于1天的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的天数
        */
        int64 DiffDay(const CSDDateTime &datetime);
        int64 DiffDayNoDst(const CSDDateTime &datetime);        // 不考虑夏令时的比较(一般使用此函数)

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的星期数,差距少于1星期的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的星期数
        */
        int64 DiffWeek(const CSDDateTime &datetime);
        int64 DiffWeekNoDst(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的月份,差距少于1个月的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的星期数
        */
        int32 DiffMonth(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的年数,差距少于1年的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的年数
        */
        int32 DiffYear(const CSDDateTime &datetime);

        /**
        * @brief 增加本类所代表的日期时间的年数
        * @param year 增加的年数
        * @return 新的日期时间
        */
        CSDDateTime & IncYear(uint32 year = 1);

        /**
        * @brief 减少本类所代表的日期时间的年数
        * @param year 减少的年数
        * @return 新的日期时间
        */
        CSDDateTime & DecYear(uint32 year = 1);

        /**
        * @brief 增加本类所代表的日期时间的月数
        * @param month 增加的月数
        * @return 新的日期时间
        */
        CSDDateTime & IncMonth(uint32 month = 1);

        /**
        * @brief 减少本类所代表的日期时间的月数
        * @param month 减少的月数
        * @return 新的日期时间
        */
        CSDDateTime & DecMonth(uint32 month = 1);

        /**
        * @brief 增加本类所代表的日期时间的天数
        * @param day 增加的天数
        * @return 新的日期时间
        */
        CSDDateTime & IncDay(uint32 day = 1);

        /**
        * @brief 减少本类所代表的日期时间的天数
        * @param day 减少的天数
        * @return 新的日期时间
        */
        CSDDateTime & DecDay(uint32 day = 1);

        /**
        * @brief 增加本类所代表的日期时间的小时数
        * @param hour 增加的小时数
        * @return 新的日期时间
        */
        CSDDateTime & IncHour(uint32 hour = 1);

        /**
        * @brief 减少本类所代表的日期时间的小时数
        * @param hour 减少的小时数
        * @return 新的日期时间
        */
        CSDDateTime & DecHour(uint32 hour = 1);

        /**
        * @brief 增加本类所代表的日期时间的分钟数
        * @param minute 增加的分钟数
        * @return 新的日期时间
        */
        CSDDateTime & IncMinute(uint32 minute = 1);

        /**
        * @brief 减少本类所代表的日期时间的分钟数
        * @param minute 减少的分钟数
        * @return 新的日期时间
        */
        CSDDateTime & DecMinute(uint32 minute = 1);

        /**
        * @brief 增加本类所代表的日期时间的秒数
        * @param second 增加的秒数
        * @return 新的日期时间
        */
        CSDDateTime & IncSecond(uint32 second = 1);

        /**
        * @brief 减少本类所代表的日期时间的秒数
        * @param second 减少的秒数
        * @return 新的日期时间
        */
        CSDDateTime & DecSecond(uint32 second = 1);

        /**
        * @brief 增加本类所代表的日期时间的星期数
        * @param week 增加的星期数
        * @return 新的日期时间
        */
        CSDDateTime & IncWeek(uint32 week = 1);

        /**
        * @brief 减少本类所代表的日期时间的星期数
        * @param week 减少的星期数
        * @return 新的日期时间
        */
        CSDDateTime & DecWeek(uint32 week = 1);



        /**
        * @brief 设置本类所代表的日期时间
        * @param year 设置的年份[1970-2037]
        * @param month 设定的此年的哪个月[1-12]
        * @param day 设定的此月的第几天[1-31]
        * @param hours 设定的此天的第几小时[0-23]
        * @param minutes 设定的此小时的第几分钟[0-59]
        * @param seconds 设定的此分钟的第几秒[0-59]
        * @return 是否设置成功,false为没有设置成功
        */
        bool SetDateTime(uint32 year, uint32 month, uint32 day, uint32 hours, uint32 minutes, uint32 seconds);

        /**
        * @brief 设置本类所代表的日期,时间不变
        * @param year 设置的年份[1970-2037]
        * @param month 设定的此年的哪个月[1-12]
        * @param day 设定的此月的第几天[1-31]
        * @return 是否设置成功,false为没有设置成功
        */
        bool SetDate(uint32 year, uint32 month, uint32 day);

        /**
        * @brief 设置本类所代表的时间,日期不变
        * @param hours 设定的此天的第几小时[0-23]
        * @param minutes 设定的此小时的第几分钟[0-59]
        * @param seconds 设定的此分钟的第几秒[0-59]
        * @return 是否设置成功,false为没有设置成功
        */
        bool SetTime(uint32 hours, uint32 minutes, uint32 seconds);

        /**
        * @brief 获取本类所代表的年份
        * @return 获取的年份
        */
        uint32 GetYear();

        /**
        * @brief 获取本类所代表的日期时间所在的年份的第几个月
        * @return 获取的月份
        */
        uint32 GetMonth();

        /**
        * @brief 获取本类所代表的日期时间所在的月份的第几天
        * @return 获取的天数
        */
        uint32 GetDay();

        /**
        * @brief 获取本类所代表的日期时间所在的天的第几个小时
        * @return 获取的小时数
        */
        uint32 GetHour() const;

        /**
        * @brief 获取本类所代表的日期时间所在的小时的分钟数
        * @return 获取的分钟数
        */
        uint32 GetMinute() const;

        /**
        * @brief 设置本类所代表的日期时间所在的分钟的秒数
        * @return 获取的秒数
        */
        uint32 GetSecond() const;

		/**
        * @brief 设置本类所代表的日期时间所在的星期数
        * @return 获取的星期数
		*		0为星期天Sunday
		*		1为星期一Monday
		*		... ...
		*		6为星期六Saturday
        */
		uint32 GetWeek();

        /**
        * @brief 获取格式化后的日期时间字符串
        * @param buffer 格式化后的日期时间字符串
        */
        void ToString(char* buffer, int32 len);

        std::string ToString();

        /**
        * @brief 根据格式化后的日期时间字符串设置本类的时间
        * @param pDateTime 格式化后的日期时间字符串
        * @param format 格式化的格式
        * 具体格式:
        * ......
        * ......
        * @return 是否解析成功,true为成功
        */
        void FromString(const std::string& timeString);

    private:
        /**
        * @brief 不允许操作+
        */
        CSDDateTime & operator + (const CSDDateTime & datetime);

        bool CheckDate(uint32 year, uint32 month, uint32 day);

        bool CheckTime(uint32 hours, uint32 minutes, uint32 seconds);

        int64 DiffTimeNoDst(tm& day1, tm& day2);            // 不考虑夏令时的时间差值计算


    private:

        static time_t g_timezone;       // 时区

        tm m_time;
    };



    /**
    * @brief 检测此年是否为闰年
    * @param year 检测的年
    * @return 检测的结构,true为是闰年,false不为闰年
    */
    bool SDIsLeapYear(uint32 year);

    /**
    * @brief
    * 计算机系统启动到目前为止所经历的时间（此函数有效时间为系统启动后49.7天内）
    * @return 计算机系统启动到目前为止所经历的时间，如果该值不可获得，返回－1
    */
    uint32 SDAPI SDGetTickCount();

    /**
    * @brief
    * 挂起当前线程的执行直到milliseconds毫秒后
    * @param milliseconds : 指示睡眠的时间
    * @return void
    */
    void SDAPI SDSleep(uint32 milliseconds);

    /**
    * @brief
    * 获取当前的微妙时间
    * @return 当前的微妙时间
    */
    uint64 SDAPI SDTimeMicroSec();

    /**
    * @brief
    * 获取当前的毫秒时间
    * @return 当前的毫秒时间
    */
    uint64 SDAPI SDTimeMilliSec();

    /**
    * @brief
    * 获取当前的秒时间
    * @return 当前的秒时间
    */
    uint64 SDAPI SDTimeSecs();

    /*
    * 获取时区
    */
    int32 SDAPI SDGetTimeZone();

}//

#endif
