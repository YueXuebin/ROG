#pragma once

/******************************************************************************
		Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SD_FIFOQUEUE_H
#define SD_FIFOQUEUE_H
#include "sdmutex.h"
#include "sdlock.h"
#include "sdcondition.h"
#include <deque>
#define MULTI_THREAD_READ

namespace Crown
{
    /**
    * @defgroup groupmessagequeue 消息队列
    * @ingroup  Crown
    * @{
    */

    /**
    *@brief  使用单队列的消息队列，从队列头读数据，从队列尾写入数据
    */
    template< typename T, typename MT = CSDMutex >
    class CSDFifoQueue
    {
    public:

        typedef std::deque< T* > TypedDeque;

        /**
        * @brief
        * 从消息队列获取消息
        * @param ms : 获取消息的超时时间，单位毫秒
        * 当ms小于零时，无限等待
        * 当ms等于零时，检查一次，无论是否能获取结果，立即返回
        * 当ms大于零时，等待相应的时间
        * @return :  获取的消息指针，如果超时，返回NULL
        */
        T * Pop(int ms = 0)
        {
            for (;;)
            {
                {
                    CSDLock<MT> lock (m_queueMutex);
                    // if there are only one thread to read the queue
                    // we needn't to lock it .
                    if (!m_queue.empty())
                    {
                        T * pItem = m_queue.front();
                        m_queue.pop_front();
                        return pItem ;
                    }
                }

                if (ms == 0)
                {
                    return nullptr;
                }
                if (ms < 0)
                {
                    CSDLock<MT> lock (m_queueMutex);
                    m_fifoNotEmpty.Wait(m_queueMutex);
                }
                else
                {
                    CSDLock<MT> lock (m_queueMutex);
                    bool ret = m_fifoNotEmpty.Wait(m_queueMutex,ms);
                    if (!ret)
                    {
                        return nullptr;
                    }
                }
            }
            return nullptr;
        }

        /**
        * @brief
        * 向消息队列写入消息
        * @param pItem : 写入的消息指针
        * @return void
        */
        void Push(T * pItem)
        {
            CSDLock<MT> lock (m_queueMutex);
            m_queue.push_back(pItem);
            m_fifoNotEmpty.Broadcast();
        }

        /**
        * @brief
        * 当前在读队列中的可读的消息数
        * @return : 消息队列中的消息个数
        */
        size_t ReadSize()
        {
            return m_queue.size();
        }

        size_t WriteSize()
        {
            return m_queue.size();
        }

    private:

        TypedDeque  m_queue;
        MT m_queueMutex;
        CSDCondition  m_fifoNotEmpty;
    };

}
#endif // 
