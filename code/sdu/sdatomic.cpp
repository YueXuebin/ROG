#include "SDuPCH.h"
#include "sdatomic.h"
//#include "sddebug.h"
#if (defined(WIN32) || defined(WIN64))
#pragma warning(disable: 4146)
#endif

namespace Crown
{


    int32 SDAPI SDAtomicAdd32(volatile int32 *mem, int32 val)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedExchangeAdd((volatile long *)mem, val);
#else
        asm volatile ("lock; xaddl %0,%1"
                      : "=r" (val), "=m" (*mem)
                      : "0" (val), "m" (*mem)
                      : "memory", "cc");
        return val;
#endif
    }


    int64 SDAPI SDAtomicAdd64(volatile int64 *mem, int64 val)
    {
#if (defined(WIN64))
        return InterlockedExchangeAdd64(mem, val);
#elif (defined(LINUX64))
		asm volatile ("lock; xaddq %0,%1"
			: "=r" (val), "=m" (*mem)
			: "0" (val), "m" (*mem)
			: "memory", "cc");
		return val;
#endif
		CnVerify(false); 
		return *mem; 
    }


    int32 SDAPI SDAtomicSub32(volatile int32 *mem, int32 val)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedExchangeAdd((volatile long *)mem, -val);
#elif defined(LINUX) || defined(APPLE)
        asm volatile ("lock; subl %1, %0"
                      : /* no output */
                      : "m" (*(mem)), "r" (val)
                      : "memory", "cc");
        return *mem + 1;
#endif
    }


    int64 SDAPI SDAtomicSub64(volatile int64 *mem, int64 val)
    {
#if (defined(WIN64))
        return InterlockedExchangeAdd64(mem, -val);
#elif (defined(LINUX64))
		asm volatile ("lock; subq %1, %0"
			: /* no output */
			: "m" (*(mem)), "r" (val)
			: "memory", "cc");
		return *mem + 1;
#endif
		CnVerify(false); 
		return *mem; 
    }


    int32 SDAPI SDAtomicInc32(volatile int32 *mem)
    {
        /* we return old value, win32 returns new value :( */
#if (defined(WIN32) || defined(WIN64))
        return InterlockedIncrement((volatile long *)mem) - 1;
#elif defined(LINUX) || defined(APPLE)
        return SDAtomicAdd32(mem, 1);
#endif
    }



    int64 SDAPI SDAtomicInc64(volatile int64 *mem)
    {
        /* we return old value, win32 returns new value :( */
#if (defined(WIN64))
        return InterlockedIncrement64(mem) - 1;
#elif defined(LINUX64) || defined(APPLE64)
        return SDAtomicAdd64(mem, 1);
#endif
		CnVerify(false); 
		return *mem; 
    }


    int32 SDAPI SDAtomicDec32(volatile int32 *mem)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedDecrement((volatile long *)mem) + 1;
#elif defined(LINUX) || defined(APPLE)
        unsigned char prev;
        asm volatile ("lock; decl %0; setnz %1"
                      : "=m" (*mem), "=qm" (prev)
                      : "m" (*mem)
                      : "memory");

        return prev + 1;
#endif
    }


    int64 SDAPI SDAtomicDec64(volatile int64 *mem)
    {
#if (defined(WIN64))
        return InterlockedDecrement64(mem) + 1;
#elif defined(LINUX64) || defined(APPLE64)
        return SDAtomicSub64(mem, 1);
#endif
		CnVerify(false); 
		return *mem; 
    }


    void SDAPI SDAtomicSet32(volatile int32 *mem, int32 val)
    {
#if (defined(WIN32) || defined(WIN64))
        InterlockedExchange((volatile long *)mem, val);
#else
        *mem = val;
#endif
    }


    void SDAPI SDAtomicSet64(volatile int64 *mem, int64 val)
    {
#if (defined(WIN64))
        InterlockedExchange64((int64*)mem, val);
#elif (defined(LINUX64))
        *mem = val;
#endif
    }


    int32 SDAPI SDAtomicRead32(volatile int32 *mem)
    {
        return *mem;
    }


    int64 SDAPI SDAtomicRead64(volatile int64 *mem)
    {
        return *mem;
    }


    int32 SDAPI SDAtomicCas32(volatile int32 *mem, int32 with, int32 cmp)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedCompareExchange((volatile long *)mem, with, cmp);
#elif defined(LINUX) || defined(APPLE)
        uint32 prev;
        asm volatile ("lock; cmpxchgl %1, %2"
                      : "=a" (prev)
                      : "r" (with), "m" (*(mem)), "0"(cmp)
                      : "memory", "cc");
        return prev;
#endif
    }


    int64 SDAPI SDAtomicCas64(volatile int64 *mem, int64 with, int64 cmp)
    {
#if (defined(WIN64))
        return InterlockedCompareExchange64(mem,with,cmp);
#elif (defined(LINUX64))
		uint32 prev;
		asm volatile ("lock; cmpxchgq %1, %2"
			: "=a" (prev)
			: "r" (with), "m" (*(mem)), "0"(cmp)
			: "memory", "cc");
		return prev;
#endif
		CnVerify(false); 
		return *mem; 
    }


    int32 SDAPI SDAtomicXchg32(volatile int32 *mem, int32 val)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedExchange((volatile long *)mem, val);
#elif defined(LINUX) || defined(APPLE)
        int32 prev = val;
        asm volatile ("xchgl %0, %1"
                      : "=r" (prev), "+m" (*mem)
                      : "0" (prev));
        return prev;
#endif
    }


    int64 SDAPI SDAtomicXchg64(volatile int64 *mem, int64 val)
    {
#if (defined(WIN64))
        return InterlockedExchange64(mem, val);
#elif defined(LINUX64) || defined(APPLE64)
		int32 prev = val;
		asm volatile ("xchg %0, %1"
			: "=r" (prev), "+m" (*mem)
			: "0" (prev));
		return prev;
#endif
		CnVerify(false); 
		return *mem; 
    }


    void* SDAPI SDAtomicCasptr(volatile void **mem, void *with, const void *cmp)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedCompareExchangePointer((void* volatile*)mem, with, (void*)cmp);
#elif defined(LINUX64) || defined(APPLE64)
        void * prev;
       asm volatile ("lock; cmpxchgq %q2, %1"
                      : "=a" (prev), "=m" (*mem)
                      : "r" ((unsigned long)with), "m" (*mem),
                      "0" ((unsigned long)cmp));
        return prev;
#else
        void * prev;
        asm volatile ("lock; cmpxchgl %2, %1"
                      : "=a" (prev), "=m" (*mem)
                      : "r" (with), "m" (*mem), "0" (cmp));
        return prev;
#endif
    }


    void* SDAPI SDAtomicXchgptr(volatile void **mem, void *with)
    {
#if (defined(WIN32) || defined(WIN64))
        return InterlockedExchangePointer((void**)mem, with);
#elif defined(LINUX64)
        void * prev;
        asm volatile ("xchgq %q2, %1"
                      : "=a" (prev), "+m" (*mem)
                     : "r" ((unsigned long)with));
        return prev;
#else
        void * prev;
        asm volatile ("xchgl %2, %1"
                      : "=a" (prev), "+m" (*mem)
                      : "0" (with));
        return prev;
#endif
    }

}
