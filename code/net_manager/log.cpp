// log.cpp
#include "net_manager_pch.h"
#include "log.h"

#ifdef _MULTI_THREAD
#include "thread_mutex.h"
namespace utils
{
    class Log::Lock : public Thread_Mutex {};
}
#else
namespace utils
{
    /// 一个空互斥锁
    class Log::Lock
    {
    public:
        void acquire() {};
        void release() {};
    };
}
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef WIN32
#include <WinSock2.h>
#if _MSC_VER <= 1400
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#else

#include <unistd.h>

#endif

namespace utils
{

#ifdef WIN32

#define DATE_START  0
    char Log::level_str_[][64] = {
        "2008-11-07 09:35:00 FATAL ", 
        "2008-11-07 09:35:00 ERROR ", 
        "2008-11-07 09:35:00 WARN  ", 
        "2008-11-07 09:35:00 INFO  ", 
        "2008-11-07 09:35:00 TRACE ", 
        "2008-11-07 09:35:00 DEBUG "
    };

    char Log::level_str_usec_[][64] = {
        "\033[1;31m2008-11-07 09:35:00.000000 FATAL ", 
        "\033[1;33m2008-11-07 09:35:00.000000 ERROR ", 
        "\033[1;35m2008-11-07 09:35:00.000000 WARN	", 
        "\033[1;32m2008-11-07 09:35:00.000000 INFO	", 
        "\033[0;00m2008-11-07 09:35:00.000000 TRACE ", 
        "\033[0;00m2008-11-07 09:35:00.000000 DEBUG "
    };
#else

#define DATE_START  7
    char Log::level_str_[][64] = {
        "\033[1;31m2008-11-07 09:35:00 FATAL ", 
        "\033[1;33m2008-11-07 09:35:00 ERROR ", 
        "\033[1;35m2008-11-07 09:35:00 WARN  ", 
        "\033[1;32m2008-11-07 09:35:00 INFO  ", 
        "\033[0;00m2008-11-07 09:35:00 TRACE ", 
        "\033[0;00m2008-11-07 09:35:00 DEBUG "
    };

    char Log::level_str_usec_[][64] = {
        "\033[1;31m2008-11-07 09:35:00.000000 FATAL ", 
        "\033[1;33m2008-11-07 09:35:00.000000 ERROR ", 
        "\033[1;35m2008-11-07 09:35:00.000000 WARN  ", 
        "\033[1;32m2008-11-07 09:35:00.000000 INFO  ", 
        "\033[0;00m2008-11-07 09:35:00.000000 TRACE ", 
        "\033[0;00m2008-11-07 09:35:00.000000 DEBUG "
    };

#endif

#define TIME_START  (DATE_START + 11)

    Log::Log()
    {
        memset(file_name_, 0, sizeof(file_name_));
        max_size_ = 500 * 1024 * 1024;  // 500M
        lock_ = CnNew Lock;
        file_ = NULL;
        max_level_ = L_INFO;
        enable_usec = false;
    }

    Log::~Log()
    {
        close();
        CnDelete lock_;
        lock_ = NULL;
    }

    int Log::set_file_name(const char *filename)
    {
        // 已经打开，不能设置了
        if (file_ != NULL) {
            return -1;
        }

        strcpy(file_name_, filename);
        return 0;
    }

    int Log::set_max_size(size_t maxsize)
    {
        max_size_ = maxsize;
        // 不立即生效
        return 0;
    }

    int Log::set_max_level(LogLevel level)
    {
        if (level < L_LEVEL_MAX) {
            max_level_ = level;
            return 0;
        }
        return 1;
    }

    int Log::set_usec(bool in_enable_usec)
    {
        //	    	printf("in_enable_usec:%d set.\n", in_enable_usec);
        enable_usec = in_enable_usec;
        return 1;
    }

    int Log::open()
    {
        if (file_ != NULL) {
            return -1;
        }


        char name[MAX_PATH_LEN];
        size_t len = 0;

        strncpy(name, file_name_, MAX_PATH_LEN);
        len = strlen(name);

        time_t t;
        time(&t);
        struct tm lt = *localtime(&t);
        strftime(name + len, MAX_PATH_LEN - len, "-%Y%m%d-%H%M%S.log", &lt);

        file_ = fopen(name, "a+");
        if (NULL == file_)
            return -1;

        // 填写日志记录中的日期，在一天之内就不用填写了
        strftime(name, 12, "%Y-%m-%d", &lt);
        for (int i = 0; i < L_LEVEL_MAX; i++) {
            memcpy(level_str_[i] + DATE_START, name, 10);
        }

        for (int i = 0; i < L_LEVEL_MAX; i++) {
            memcpy(level_str_usec_[i] + DATE_START, name, 10);
        }

        lt.tm_hour = lt.tm_min = lt.tm_sec = 0;
        mid_night_ = mktime(&lt);

        return 0;
    }

    int Log::close()
    {
        if (NULL == file_) {
            return -1;
        }

        fclose(file_);
        file_ = NULL;

        return 0;
    }

    int Log::log(LogLevel level, const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(level, fmt, ap); // not safe
        va_end(ap);
        return ret;
    }

    int Log::log_fatal(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_FATAL, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_error(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_ERROR, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_warn(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_WARN, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_info(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_INFO, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_trace(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_TRACE, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_debug(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_DEBUG, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::vlog(int level, const char * fmt, va_list ap)
    {
        if (level > max_level_ || NULL == file_)
            return -1;

        lock_->acquire();
        struct tm tm_now;
        struct timeval tv;
        tv.tv_sec = tv.tv_usec = 0;
#ifdef WIN32
        time_t now = time(NULL);
        tv.tv_sec = (long)now;
#else
        struct timezone tz;
        gettimeofday(&tv, &tz);
        time_t now = tv.tv_sec;
#endif
        int t_diff = (int)(now - mid_night_);
        if (t_diff > 24 * 60 * 60) {
            close();
            open();
            t_diff -= 24 * 60 * 60;
        }
#ifdef WIN32
        localtime_s(&tm_now, &now);
#else
        localtime_r(&now, &tm_now);
#endif
        if(enable_usec)
        {
            //				printf("%s enableusec:%d, -----\n", 
            //						level_str_[level], enable_usec, tv.tv_usec, tv.tv_usec);
            sprintf(((char*)level_str_usec_[level]+TIME_START), "%02d:%02d:%02d.%06lld",					
                tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, tv.tv_usec);
            level_str_usec_[level][strlen(level_str_usec_[level])] = ' ';
            //				printf("%s enableusec:%d, %06ld, %ld\n", 
            //						level_str_usec_[level], enable_usec, tv.tv_usec, tv.tv_usec);
            fputs(level_str_usec_[level], file_);
        }
        else
        {
            sprintf(((char*)level_str_[level]+TIME_START), "%02d:%02d:%02d",					
                tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
            level_str_[level][strlen(level_str_[level])] = ' ';
            //	 			printf("%s enableusec:%d, %06ld, %ld\n", 
            //	 				level_str_[level], enable_usec, tv.tv_usec, tv.tv_usec);
            fputs(level_str_[level], file_);
        }
        // strftime消耗大，改为直接格式化

        //time_t t;
        //struct tm lt;
        //char time_str[32];
        //time(&t);
        //lt = *localtime(&t);
        //strftime(time_str, 10, "%H:%M:%S", &lt);
        //memcpy(level_str_[level] + TIME_START, time_str, 8);

        //			register int hour = t_diff / 3600;
        //			register int minute = t_diff % 3600;
        //			register int second = minute % 60;
        //			minute /= 60;
        //	
        //			level_str_[level][TIME_START] = hour / 10 + '0';
        //			level_str_[level][TIME_START + 1] = hour % 10 + '0';
        //			level_str_[level][TIME_START + 3] = minute / 10 + '0';
        //			level_str_[level][TIME_START + 4] = minute % 10 + '0';
        //			level_str_[level][TIME_START + 6] = second / 10 + '0';
        //			level_str_[level][TIME_START + 7] = second % 10 + '0';
        //// set color
        //fputs(color_str_[level], file_);
        //// write time
        //fputs(time_str, file_);
        //// write level
        // write msg
        vfprintf(file_, fmt, ap);
        // reset color
        //fputs(color_str_[L_LEVEL_MAX], file_);
        if (fmt[strlen(fmt) - 1] != '\n')
            fputc('\n', file_);

        /*
        char buf[2048];
        snprintf(buf, sizeof(buf), "%s%s%s%s\n",color_str_[level] , time_str, level_str_[level], fmt);
        vfprintf(file_, buf, ap);
        */

        if ((size_t)ftell(file_) > max_size_) {
            close();
            open();
        }

        lock_->release();

        return 0;
    }


    //	    int Log::vlog(int level, const char * fmt, va_list ap)
    //	    {
    //	        if (level > max_level_ || NULL == file_)
    //	            return -1;
    //	
    //	        lock_->acquire();
    //	
    //	        int t_diff = (int)(time(NULL) - mid_night_);
    //	        if (t_diff > 24 * 60 * 60) {
    //	            close();
    //	            open();
    //	            t_diff -= 24 * 60 * 60;
    //	        }
    //	
    //	        // strftime消耗大，改为直接格式化
    //	
    //	        //time_t t;
    //	        //struct tm lt;
    //	        //char time_str[32];
    //	        //time(&t);
    //	        //lt = *localtime(&t);
    //	        //strftime(time_str, 10, "%H:%M:%S", &lt);
    //	        //memcpy(level_str_[level] + TIME_START, time_str, 8);
    //	
    //	        register int hour = t_diff / 3600;
    //	        register int minute = t_diff % 3600;
    //	        register int second = minute % 60;
    //	        minute /= 60;
    //	
    //	        level_str_[level][TIME_START] = hour / 10 + '0';
    //	        level_str_[level][TIME_START + 1] = hour % 10 + '0';
    //	        level_str_[level][TIME_START + 3] = minute / 10 + '0';
    //	        level_str_[level][TIME_START + 4] = minute % 10 + '0';
    //	        level_str_[level][TIME_START + 6] = second / 10 + '0';
    //	        level_str_[level][TIME_START + 7] = second % 10 + '0';
    //	
    //	        //// set color
    //	        //fputs(color_str_[level], file_);
    //	        //// write time
    //	        //fputs(time_str, file_);
    //	        //// write level
    //	        fputs(level_str_[level], file_);
    //	        // write msg
    //	        vfprintf(file_, fmt, ap);
    //	        // reset color
    //	        //fputs(color_str_[L_LEVEL_MAX], file_);
    //	        if (fmt[strlen(fmt) - 1] != '\n')
    //	            fputc('\n', file_);
    //	
    //	        /*
    //	        char buf[2048];
    //	        snprintf(buf, sizeof(buf), "%s%s%s%s\n",color_str_[level] , time_str, level_str_[level], fmt);
    //	        vfprintf(file_, buf, ap);
    //	        */
    //	
    //	        if ((size_t)ftell(file_) > max_size_) {
    //	            close();
    //	            open();
    //	        }
    //	
    //	        lock_->release();
    //	
    //	        return 0;
    //	    }

    static const char chex[] = "0123456789ABCDEF";

    int Log::log_hex_prefix(
        unsigned char * prefix,
        unsigned char * data, 
        size_t len, 
        LogLevel level)
    {
        log(level, "%s", prefix);
        return log_hex(data, len, level);
    }

    int Log::log_hex(
        unsigned char * data, 
        size_t len, 
        LogLevel level)
    {
        size_t i, j, k, l;

        if (level > max_level_ || NULL == file_)
            return -1;


        char msg_str[128] = {0};

        msg_str[0] = '[';
        msg_str[3] = '0';
        msg_str[4] = ']';
        msg_str[57] = ' ';
        msg_str[58] = '|';
        msg_str[75] = '|';
        msg_str[76] = 0;
        k = 4;
        for (j = 0; j < 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                msg_str[++k] = ' ';
            }
            k += 3;
            msg_str[k] = ' ';
        }
        for (i = 0; i < len / 16; i++)
        {
            msg_str[1] = chex[i >> 4];
            msg_str[2] = chex[i &0xF];
            k = 5;
            l = i * 16;
            memcpy(msg_str + 59, data + l, 16);
            for (j = 0; j < 16; j++)
            {
                if ((j & 0x03) == 0)
                {
                    k++;
                }
                msg_str[k++] = chex[data[l] >> 4];
                msg_str[k++] = chex[data[l++] & 0x0F];
                k++;
                if (!isgraph(msg_str[59 + j]))
                    msg_str[59 + j]= '.';
            }
            msg_str[127] = 0;
            lock_->acquire();
            fprintf(file_, "# %s\n", msg_str);
            lock_->release();
        }
        msg_str[1] = chex[i >> 4];
        msg_str[2] = chex[i &0xF];
        k = 5;
        l = i * 16;
        memcpy(msg_str + 59, data + l, len % 16);
        for (j = 0; j < len % 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                k++;
            }
            msg_str[k++] = chex[data[l] >> 4];
            msg_str[k++] = chex[data[l++] & 0x0F];
            k++;
            if (!isgraph(msg_str[59 + j]))
                msg_str[59 + j]= '.';
        }
        for (; j < 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                k++;
            }
            msg_str[k++] = ' ';
            msg_str[k++] = ' ';
            k++;
            msg_str[59 + j]= ' ';
        }
        msg_str[127] = 0;
        lock_->acquire();
        fprintf(file_, "# %s\n", msg_str);
        lock_->release();

        return 0;
    }

    Log Log::global_log;

}
