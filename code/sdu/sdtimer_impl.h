#ifndef SDTIMER_IMPL_H
#define SDTIMER_IMPL_H
#include "sdhashmap.h"
#include "sdobjectpool.h"
#include "sdtimer_detail.h"

namespace Crown
{
    /**
    *@brief 定时器模块类，采用类似LINUX的分层的定时器管理方法，通过在线程当中循环调用Run函数来驱动定时器
    */

    class CSDTimerImpl
    {
	//typedef CnHashMap<uint32, CTimerItem*> CTimerMap;

    public:
        CSDTimerImpl();
        ~CSDTimerImpl();

        bool SDAPI SetTimer(ISDTimer* poTimerHandler, uint32 dwTimerId, uint32 dwElapse, uint32 dwLoop= SDINFINITE);

        bool SDAPI KillTimer(uint32 dwTimerId);

        bool SDAPI Run();

		std::string SDAPI DumpTimerInfo();

    private:

        friend class CTimerItem;
        inline CTimerSlotLevel& GetSlotLevel( uint32 nLevel )
        {
            return m_aoSlotLevels[nLevel];
        }

        bool ReloadSlotLevel(byte dwLevel = 0);

        uint32 TrigerTimersForCurSlot();

        bool AddTimerForNextTrigger(CTimerItem *pTimer);

        uint32 GetMsFromLast();
        uint32 GetMsFromLast(uint32 dwLastTime);

        CTimerItem* LookupTimer( uint32 dwTimerId );

        CSDObjectPool<CTimerItem, CSDMutex > m_oTimerPool;
        CTimerSlotLevel m_aoSlotLevels[TIMERARRAY_COUNT];
        //CTimerMap   m_oTimerMap;
        uint32      m_dwLastTickCount;
        uint32      m_dwMsPerTick;
    };
	
}

#endif // 



