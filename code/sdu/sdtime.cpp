#include "SDuPCH.h"
#include "sdtime.h"
#include "Assertion.h"
#include "sdstring.h"
#include <sstream>
#include <iomanip>


#if !(defined(WIN32) || defined(WIN64))
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#endif

namespace Crown
{
    CSDDateTime  SDNow()
    {
        return time(nullptr);
    }

    // 字符串时间->unix时间戳
    uint32 TimeStamp(const std::string& str)
    {
        struct tm time_tm;
        uint32 time_stamp = 0;

        memset(&time_tm, 0, sizeof(time_tm));

        sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", 
            &time_tm.tm_year, &time_tm.tm_mon, &time_tm.tm_mday, 
            &time_tm.tm_hour, &time_tm.tm_min, &time_tm.tm_sec);

        if(time_tm.tm_year >= 1900) time_tm.tm_year -= 1900;
        if(time_tm.tm_mon >= 1) time_tm.tm_mon -= 1;
        time_tm.tm_isdst = -1;

        time_stamp = (uint32)(mktime(&time_tm));

        return time_stamp;
    }

    // unix时间戳->字符串时间
    std::string TimeString(time_t t)
    {
        tm* local = localtime(&t);

        char str[100];
        strftime(str, 100, "%y-%m-%d %H:%M:%S", local);
        return str;
    }

    // 获得每日时间的秒数
    uint32 TimeToUint(const std::string& str)
    {
        int32 hour = 0;
        int32 minute = 0;
        int32 second = 0;
        sscanf(str.c_str(), "%d:%d:%d", &hour, &minute, &second);
        return (hour*3600 + minute*60 + second);
    }

    // 字符串时间构造
    CSDDateTime::CSDDateTime(const std::string& str)
    {
        time_t t = TimeStamp(str);
        m_time = *localtime(&t);
    }

    // unix时间戳构造
    CSDDateTime::CSDDateTime(time_t t)
    {
        m_time = *localtime(&t);
    }

    void CSDDateTime::ToString(char* buffer, int32 len)
    {
        SDSnprintf(buffer, len, "%d-%d-%d %d:%d:%d", m_time.tm_year+1900, m_time.tm_mon+1, m_time.tm_mday, m_time.tm_hour, m_time.tm_min, m_time.tm_sec);
    }

    std::string CSDDateTime::ToString()
    {
        char t[200];
        ToString(t, 200);
        return t;
    }

    void CSDDateTime::FromString(const std::string& timeString)
    {
        time_t t = TimeStamp(timeString);
        m_time = *localtime(&t);
    }

    time_t CSDDateTime::GetTimeValue()
    {
        return (mktime(&m_time));
    }

    void CSDDateTime::SetTimeValue(time_t t)
    {
        m_time = *localtime(&t);
    }

    int64 CSDDateTime::operator - (const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return (int64)difftime(mktime(&m_time), mktime(&t_tm));
    }

    int64 CSDDateTime::DiffSecond(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return (int64)difftime(mktime(&m_time), mktime(&t_tm));
    }

    int64 CSDDateTime::DiffMinute(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((int64)difftime(mktime(&m_time), mktime(&t_tm)))/60;
    }

    int64 CSDDateTime::DiffHour(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((int64)difftime(mktime(&m_time), mktime(&t_tm)))/(60*60);
    }

    int64 CSDDateTime::DiffTimeNoDst(tm& day1, tm& day2)
    {
        int32 oldDst1 = day1.tm_isdst;
        int32 oldDst2 = day2.tm_isdst;
        day1.tm_isdst = 0;              // 取消夏令时设置
        day2.tm_isdst = 0;
        
        int64 ret = ((int64)difftime(mktime(&day1), mktime(&day2)));

        day1.tm_isdst = oldDst1;        // 还原夏令时设置
        day2.tm_isdst = oldDst2;
        return ret;
    }

    int64 CSDDateTime::DiffDay(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return (int64)difftime(mktime(&m_time), mktime(&t_tm))/(60*60*24);
    }

    int64 CSDDateTime::DiffDayNoDst(const CSDDateTime &datetime)
    {
        tm t_tm = datetime.m_time;
        return DiffTimeNoDst(m_time, t_tm)/(60*60*24);          // 计算日差,排除夏令时干扰
    }

    int64 CSDDateTime::DiffWeek(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((int64)difftime(mktime(&m_time), mktime(&t_tm)))/(60*60*24*7);        // 计算周差,排除夏令时干扰
    }

    int64 CSDDateTime::DiffWeekNoDst(const CSDDateTime &datetime)
    {
        tm t_tm = datetime.m_time;
        return DiffTimeNoDst(m_time, t_tm)/(60*60*24*7);        // 计算周差,排除夏令时干扰
    }

    int32 CSDDateTime::DiffMonth(const CSDDateTime & datetime)
    {
        return (m_time.tm_year - datetime.m_time.tm_year) * 12 + (m_time.tm_mon - datetime.m_time.tm_mon);
    }

    int32 CSDDateTime::DiffYear(const CSDDateTime & datetime)
    {
        return m_time.tm_year - datetime.m_time.tm_year;
    }

    CSDDateTime & CSDDateTime::IncYear(uint32 year)
    {
        m_time.tm_year += year;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecYear(uint32 year)
    {
        m_time.tm_year -= year;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncMonth(uint32 month)
    {
        m_time.tm_mon += month;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecMonth(uint32 month)
    {
        m_time.tm_mon -= month;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncDay(uint32 day)
    {
        m_time.tm_mday += day;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecDay(uint32 day)
    {
        m_time.tm_mday -= day;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncHour(uint32 hour)
    {
        m_time.tm_hour += hour;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecHour(uint32 hour)
    {
        m_time.tm_hour -= hour;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncMinute(uint32 minute)
    {
        m_time.tm_min += minute;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecMinute(uint32 minute)
    {
        m_time.tm_min -= minute;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncSecond(uint32 second)
    {
        m_time.tm_sec += second;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecSecond(uint32 second)
    {
        m_time.tm_sec -= second;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncWeek(uint32 week)
    {
        m_time.tm_mday += 7 * week;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecWeek(uint32 week)
    {
        m_time.tm_mday -= 7 * week;
        m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    bool CSDDateTime::SetDateTime(uint32 year, uint32 month, uint32 day, uint32 hours, uint32 minutes, uint32 seconds)
    {
        if(CheckDate(year, month, day) && CheckTime(hours, minutes, seconds))
        {
            m_time.tm_year = year - 1900;
            m_time.tm_mon = month - 1;
            m_time.tm_mday = day;

            m_time.tm_hour = hours;
            m_time.tm_min = minutes;
            m_time.tm_sec = seconds;
            m_time.tm_isdst = -1;
            mktime(&m_time);
            return true;
        }
        return false;
    }

    bool CSDDateTime::SetDate(uint32 year, uint32 month, uint32 day)
    {
        if(CheckDate(year, month, day))
        {
            m_time.tm_year = year - 1900;
            m_time.tm_mon = month - 1;
            m_time.tm_mday = day;
            m_time.tm_isdst = -1;
            mktime(&m_time);
            return true;
        }
        return false;
    }

    bool CSDDateTime::SetTime(uint32 hours, uint32 minutes, uint32 seconds)
    {
        if(CheckTime(hours, minutes, seconds))
        {
            m_time.tm_hour = hours;
            m_time.tm_min = minutes;
            m_time.tm_sec = seconds;
            m_time.tm_isdst = -1;
            mktime(&m_time);
            return true;
        }
        return false;
    }

    uint32 CSDDateTime::GetYear()
    {
        return m_time.tm_year + 1900;
    }

    uint32 CSDDateTime::GetMonth()
    {
        return m_time.tm_mon + 1;
    }

    uint32 CSDDateTime::GetDay()
    {
        return m_time.tm_mday;
    }

    uint32 CSDDateTime::GetHour() const
    {
        return m_time.tm_hour;
    }

    uint32 CSDDateTime::GetMinute() const
    {
        return m_time.tm_min;
    }

    uint32 CSDDateTime::GetSecond() const
    {
        return m_time.tm_sec;
    }

    uint32 CSDDateTime::GetWeek()
    {
        return m_time.tm_wday;
    }

    bool CSDDateTime::CheckDate(uint32 year, uint32 month, uint32 day)
    {
        if(year < 1970 || year >= 2037)
        {
            return false;
        }
        if(month < 1 || month > 12)
        {
            return false;
        }
        if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
        {
            if(day < 1 || day > 31)
            {
                return false;
            }
        }
        else if(month == 4 || month == 6 || month == 9 || month == 11)
        {
            if(day < 1 || day > 30)
            {
                return false;
            }
        }
        else if(month == 2)
        {
            if(SDIsLeapYear(year))
            {
                if(day < 1 || day > 29)
                {
                    return false;
                }
            }
            else
            {
                if(day < 1 || day > 28)
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool CSDDateTime::CheckTime(uint32 hours, uint32 minutes, uint32 seconds)
    {
        if(hours >= 24)
        {
            return false;
        }
        if(minutes >= 60)
        {
            return false;
        }
        if(seconds >= 60)
        {
            return false;
        }
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SDIsLeapYear(uint32 year)
    {
        return (bool)(year % 400 == 0 || (year % 100 != 0 && year % 4 == 0));
    }

#define EPOCH_1970_1601 116444736000000000ULL

    static uint64 SDAPI SDGetSystemTime()
    {
        CnVerify( sizeof(uint64) == 64/8 );
        uint64 time=0;
#if defined(WIN32)
        FILETIME ft;
        GetSystemTimeAsFileTime( &ft);
        ULARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        time = (li.QuadPart - EPOCH_1970_1601) /10;
#else
        struct timeval now;
        gettimeofday( &now , nullptr );
        //CnVerify( now );
        time = now.tv_sec;
        time = time*1000000;
        time += now.tv_usec;
#endif
        return time;
    }

    void SDAPI SDSleep(uint32 milliseconds)
    {
#if (defined(WIN32) || defined(WIN64))
        Sleep((DWORD)milliseconds);
#else
        usleep(milliseconds * 1000);
#endif
    }

    uint64 SDAPI SDTimeMicroSec()
    {
        return SDGetSystemTime();
    }

    uint64 SDAPI SDTimeMilliSec()
    {
        return SDGetSystemTime()/1000LL;
    }

    uint64 SDAPI SDTimeSecs()
    {
        return SDGetSystemTime()/1000000LL;
    }

#define SD_CLOCK_PER_SEC 1000;
    /**
    * 计算机系统启动到目前为止所经历的时间毫秒（此函数有效时间为系统启动后49.7天内）
    * @return 计算机系统启动到目前为止所经历的时间，如果该值不可获得，返回－1
    */
    uint32 SDAPI SDGetTickCount()
    {
#if (defined(WIN32) || defined(WIN64))
        return GetTickCount();
#else
        static uint64 begintick = 0;

        struct timeval cateq_v;
        struct timezone cateq_z;

        if(!begintick)
        {
            gettimeofday(&cateq_v, &cateq_z);
            begintick = (1000 * cateq_v.tv_sec) + (cateq_v.tv_usec / 1000);
        }

        gettimeofday(&cateq_v, &cateq_z);

        uint64 nowtick = (1000 * cateq_v.tv_sec) + (cateq_v.tv_usec / 1000);

        return (uint32)(nowtick - begintick);

#endif //
    }

    // 获取时区
    int32 SDAPI SDGetTimeZone()
    {
        // Get the UTC time
        time_t time_utc;
        time(&time_utc);

        // Get the local time  
        struct tm* pTm = localtime(&time_utc);
        struct tm tm_local = *pTm;

        // Change it to GMT tm  
        pTm = gmtime(&time_utc);
        struct tm tm_gmt = *pTm;

        //int64 time_zone = Crown::DiffTimeNoDst(tm_local, tm_gmt);

        int32 time_zone = tm_local.tm_hour - tm_gmt.tm_hour; 

        if(time_zone < -12)
            time_zone += 24;
        else if(time_zone > 12)
            time_zone -= 24;

        return time_zone;
    }

}
