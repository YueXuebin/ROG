#include "SDuPCH.h"
#include "sdrandom.h"
#include <time.h>
#include <cstdlib>

#ifdef WIN
#else
#include <stdarg.h>
#endif

namespace Crown
{

    void SDSetSeed(uint32 seed)
    {
        srand(seed);
    }

    int32 SDRandom()
    {
        return rand();
    }

    uint32 SDRandom(uint32 range)
    {
        if(range == 0)
            return 0;
        return uint32( SDRandomFloat((real)range) );     // rand() [0, RAND_MAX)
    }

    int32 SDRandom( int32 dwMin, int32 dwMax )         // [dwMin, dwMax)
    {
        if ( dwMin == dwMax )
        {
            return dwMax;
        }

        return SDRandom(dwMax - dwMin) + dwMin;
    }

    uint32 SDRandomRound(uint32 range)
    {
        if(range == 0)
            return 0;
        real randFloat = SDRandomFloat((real)range);
        return uint32(randFloat + 0.5f);            // 四舍五入+0.5
    }

    bool RandomPercent(uint32 percent)
    {
        uint32 randNumber = SDRandom(100); // [0-100)

        return (randNumber < percent) ? true : false;
    }

    uint32 RandomOneNumber(uint32 count, ...)
    {
        // 所有参数推进一个list
        std::vector<uint32> NumberList;
        va_list arg_ptr;
        uint32 temp_count = count;

        // 开始的位置.从number开始
        va_start (arg_ptr, count);
        while(count > 0)
        {
            uint32 temp = va_arg(arg_ptr,uint32);
            NumberList.push_back(temp);

            count--;
        }
        //结束
        va_end(arg_ptr);

        if(NumberList.empty())
        {
            CnAssert(false);
            return 3001;
        }

        uint32 size =  NumberList.size();
        
        // 如果传进来的数量对不上的话，永远返回3002
        if(size != temp_count)
        {
            CnAssert(false);
            return 3002;
        }

        uint32 rand = Crown::SDRandom(size);

        return NumberList[rand];
    }

    real SDRandomFloat(real range)
    {
        return range * ((real)rand() / (RAND_MAX+1.0f));            // rand() [0, RAND_MAX)
    }

    CSDRandom::CSDRandom()
    {
        m_seed = (uint32)time(nullptr);
    }

    uint32 CSDRandom::Random()
    {
        uint32 next = m_seed;
        uint32 result;

        next *= 1103515245;
        next += 12345;
        result = (uint32) (next / 65536) % 2048;

        next *= 1103515245;
        next += 12345;
        result <<= 10;
        result ^= (uint32) (next / 65536) % 1024;

        next *= 1103515245;
        next += 12345;
        result <<= 10;
        result ^= (uint32) (next / 65536) % 1024;

        m_seed = next;
        return result;
    }


    uint32 CSDRandom::Random(uint32 unRange)
    {
        if ( unRange == 0 )
        {
            return 0;
        }
        return Random() % unRange;
    }

    uint32 CSDRandom::Random(uint32 unMinRange, uint32 unMaxRange)
    {
        if ( unMinRange == unMaxRange )
        {
            return unMaxRange;
        }

        return Random(unMaxRange - unMinRange) + unMinRange;
    }
}

