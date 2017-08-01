#include "SDuPCH.h"
#include "sdtime.h"
#include "sdtimer_impl.h"
#include <cmath>


static const uint32 MAX_ENABLE_TIMER = 0xFFFFFFFF - 1000;

namespace Crown
{


/////////////////////////////////////////////////////////////////////////////

    CSDTimerImpl::CSDTimerImpl( ) : m_oTimerPool()
    {
        //m_oTimerMap.clear();
        for(uint32 i=0; i<TIMERARRAY_COUNT; ++i)
        {
            m_aoSlotLevels[i].Init(i);
        }


#ifdef WIN32
        // WIN32不使用该变量
        m_dwMsPerTick = 1;
#else
        m_dwMsPerTick = 1000 / sysconf(_SC_CLK_TCK);
#endif

        m_dwLastTickCount = SDGetTickCount();
    }

    CSDTimerImpl::~CSDTimerImpl()
    {
        //m_oTimerMap.clear();
    }

    bool SDAPI CSDTimerImpl::SetTimer(ISDTimer* poTimerHandler, uint32 dwTimerId, uint32 dwElapse, uint32 dwLoopCount)
    {
        if (nullptr == poTimerHandler || (dwElapse > MAX_ENABLE_TIMER) || (0 == dwElapse))
        {
            return false;
        }

        if (LookupTimer(dwTimerId) != nullptr)
        {
            return false;
        }

        CTimerItem* poTimer = m_oTimerPool.Alloc();
        if (nullptr == poTimer)
        {
            return false;
        }

        poTimer->Init(this, poTimerHandler, dwElapse, dwTimerId,dwLoopCount);
        //m_oTimerMap[dwTimerId] = poTimer;
        return AddTimerForNextTrigger(poTimer);
    }

    bool CSDTimerImpl::AddTimerForNextTrigger(CTimerItem * poTimer)
    {
		uint32 dwMsFromLast = GetMsFromLast(m_dwLastTickCount) ;
		uint32 dwLevel = poTimer->SetupForNextTrigger(dwMsFromLast);
		m_aoSlotLevels[dwLevel].AddTimer(poTimer);
        return true;
    }

    bool SDAPI CSDTimerImpl::KillTimer(uint32 dwTimerId)
    {
        //CTimerMap::iterator iter = m_oTimerMap.find(dwTimerId);
        //if (iter == m_oTimerMap.end())
        //    return false;

        //CTimerItem* poTimer = iter->second;
        //m_oTimerMap.erase(iter);
        //m_aoSlotLevels[poTimer->m_timerLevel].DelTimer(poTimer);
        //m_oTimerPool.Free(poTimer);

        return true;
    }

    bool SDAPI CSDTimerImpl::Run()
    {
        for ( uint32 dwMsFromLast = GetMsFromLast(); dwMsFromLast>0; --dwMsFromLast)
        {
            TrigerTimersForCurSlot();
        }
        return true;
    }

    uint32 CSDTimerImpl::TrigerTimersForCurSlot()
    {
	    TimerSlot& oCurSlot = m_aoSlotLevels[0].GetCurSlot();
        uint32 dwTimeOutCount = 0;

        while( ! oCurSlot.empty() )
        {
            CTimerItem * poTimer = oCurSlot.front();
            poTimer->SetLastExpdTime(SDGetTickCount());
            poTimer->OnTimer();

            // 判断是否在OnTimer里面删除了该Timer
            if (oCurSlot.empty() || (oCurSlot.front() != poTimer))
                continue;

            // 循环定时器，需要再次插入到合适位置
            oCurSlot.pop_front();
            if (poTimer->CheckLoop())
            {
                AddTimerForNextTrigger(poTimer);
            }
            dwTimeOutCount++;
        }
        if( !m_aoSlotLevels[0].NextSlot(true) )
            ReloadSlotLevel();

        return dwTimeOutCount;
    }

    bool CSDTimerImpl::ReloadSlotLevel(byte dwLevel)
    {
        if(dwLevel >= TIMERARRAY_COUNT-1)
            return false;

        CTimerSlotLevel& oNextLevel = m_aoSlotLevels[dwLevel+1];

        if( !oNextLevel.NextSlot(true) )
            ReloadSlotLevel(dwLevel+1);

        m_aoSlotLevels[dwLevel].ReloadFromLevel(oNextLevel);
        return true;
    }

	std::string CSDTimerImpl::DumpTimerInfo()
    {
		std::stringstream info ; 
        for (byte level=0; level< TIMERARRAY_COUNT; level++)
        {
			info << m_aoSlotLevels[level].DumpTimerInfo() << std::endl;
        }
		return info.str(); 
    }

    uint32 CSDTimerImpl::GetMsFromLast()
    {
        uint32 dwCurTick = SDGetTickCount();
        uint32 dwTickFromLast;
        if (dwCurTick < m_dwLastTickCount)
        {
#ifdef WIN32
            dwTickFromLast = (uint32)0xFFFFFFFF - m_dwLastTickCount + dwCurTick + 1;
#else
            dwTickFromLast = (uint32)0x7FFFFFFF - m_dwLastTickCount + dwCurTick + 1;
#endif
        }
        else
        {
            dwTickFromLast = dwCurTick - m_dwLastTickCount;
        }
        m_dwLastTickCount = dwCurTick;
        return dwTickFromLast * m_dwMsPerTick;
    }

    uint32 CSDTimerImpl::GetMsFromLast(uint32 dwLastTime)
    {
        uint32 dwCurTick = SDGetTickCount();
        uint32 dwTickFromLast;
        if (dwCurTick < dwLastTime)
        {
#ifdef WIN32
            dwTickFromLast = (uint32)0xFFFFFFFF - dwLastTime + dwCurTick + 1;
#else
            dwTickFromLast = (uint32)0x7FFFFFFF - dwLastTime + dwCurTick + 1;
#endif
        }
        else
        {
            dwTickFromLast = dwCurTick - dwLastTime;
        }

        return dwTickFromLast * m_dwMsPerTick;

    }


    CTimerItem* CSDTimerImpl::LookupTimer( uint32 dwTimerId )
    {
        //CTimerMap::iterator iter = m_oTimerMap.find(dwTimerId);
        //if( iter == m_oTimerMap.end())
        //    return nullptr;
        //return iter->second;
		return NULL;
    }

} // end of namespace Crown

