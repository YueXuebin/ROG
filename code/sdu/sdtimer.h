#pragma once

#ifndef SDTIMER_H
#define SDTIMER_H
/**
* @file sdtimer.h
* @author lw
* @brief 定时器系列
*
**/
#include "sdtype.h"
#include <string>

namespace Crown
{
    /**
    * @defgroup grouptimer 定时器组件
    * @ingroup  Crown
    * @{
    */
    class CSDTimerImpl;

    /**
    * @brief The ISDTimer class is an application-defined timer completion
    *       routine. Specify this address when calling the SetTimer function.
    *       then it will be called when the timer was timeout.
    *       Especially this OnTimer function can't be blocked.
    */
    class ISDTimer
    {
    public:

        virtual SDAPI ~ISDTimer() {}

        /**
        * @brief
        * 超时函数。当超时时，调用此定时器。
        * @param dwTimerID : 超时时的定时器IDTimer
        * @return void
        */
        virtual void SDAPI OnTimer(uint32 dwTimerID) = 0;
    };

    /**
    * @brief 定时器模块类，采用类似LINUX的分层的定时器管理方法，通过在线程当中循环调用Run函数来驱动定时器
    */
    class CSDTimer
    {

    public:
        CSDTimer();
        ~CSDTimer();

        /**
        * @brief
        * 创建一个时钟并设置time-out值
        * @param poTimerHandler : 用户传入时钟回调接口类
        * @param dwTimerId : 设置时钟ID
        * @param dwElapse : 设置time-out值，以毫秒为单位
        * @param dwLoop : 每个加入的定时器都会执行一次，dwLoop表示定时器超时后继续超时的次数，如果为0xFFFFFFFF，表示永久循环定时器
        * @return 返回一个bool值,true表示成功,false表示失败
        */
        bool SDAPI SetTimer(ISDTimer *pTimer, uint32 dwTimerId, uint32 dwElapse, uint32 dwLoop = 0xFFFFFFFF);

        /**
        * @brief
        * 取消时钟
        * @param dwTimerId : uint32 时钟ID，表示要取消的时钟
        * @return 返回一个bool值,true表示成功,false表示失败
        */
        bool SDAPI KillTimer(uint32 dwTimerId);

        /**
        * @brief
        * 时钟消息处理
        * @param nCount : 表示处理验证事件的数量上限，-1表示处理所有的事件
        * @return 如果还有事件没处理完返回true，如果所有事件都处理完了，返回false
        */
        bool SDAPI Run();

        /**
        * @brief
        * 打印定时器信息
        * @return void
        */
		std::string SDAPI DumpTimerInfo();

    protected:
		CSDTimer(const CSDTimer & ){};
        CSDTimer& operator = (const CSDTimer & );
    private:
        CSDTimerImpl *  m_pTimerImpl;
    };

    
}

#endif
