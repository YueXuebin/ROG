#pragma once

/******************************************************************************
		Copyright (C) Shanda Corporation. All rights reserved.

sdfifodqueue.h - 使用双队列的先进先出队列


******************************************************************************/


#ifndef SD_FIFODQUEUE_H
#define SD_FIFODQUEUE_H
#include "sdmutex.h"
#include "sdlock.h"
#include "sdcondition.h"
#include <deque>

namespace Crown
{
    /**
    * @defgroup groupmessagequeue 消息队列
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief 使用双队列的消息队列，一个用来读消息，
    * 一个用来写入消息，由于采用双队列，读和写消息不会相互影响（加锁）
    */
    template< typename T, typename ReadMT = CSDMutex, typename WriteMT = CSDMutex >
    class CSDFifoDqueue
    {
    public:

        typedef std::deque< T* > TypedDeque;

        /**
        * @brief
        * 从消息队列获取消息
        * @param ms : 获取消息的超时时间
        * 当ms等于零时，检查一次，无论是否能获取结果，立即返回
        * 当ms大于零时，等待相应的时间
        * @return 获取的消息指针，如果超时，返回NULL
        */
        T * Pop(uint32 ms = 0)
        {
            for (;;)
            {

                {
                    // if there are only one thread to read the queue
                    // we needn't to lock it .
                    CSDLock<ReadMT> rlock(m_readMutex);
                    if (!m_readQueue.empty())
                    {
                        T * pItem = m_readQueue.front();
                        m_readQueue.pop_front();
                        return pItem;
                    }
                }

                {
                    CSDLock<WriteMT> wlock(m_writeMutex);
                    if (m_writeQueue.empty())
                    {
                        if (ms == 0)
                        {
                            return nullptr;
                        }

                        bool ret = m_fifoNotEmpty.Wait(m_writeMutex, ms);
                        if (!ret)
                        {
                            return nullptr;
                        }
                    }

                    {
                        CSDLock<ReadMT> rlock(m_readMutex);
                        if (m_readQueue.empty() && !m_writeQueue.empty())
                        {
                            m_writeQueue.swap(m_readQueue);
                        }
                    }
                }
            }
        }

        /**
        * @brief
        * 向消息队列写入消息
        * @param pItem : 写入的消息指针
        * @return void
        */
        void Push(T * pItem)
        {
            CSDLock<WriteMT> lock (m_writeMutex);
            m_writeQueue.push_back(pItem);
            m_fifoNotEmpty.Broadcast();
        }

        /**
        * @brief
        * 当前在读队列中的可读的消息数
        * @return void
        */
        size_t ReadSize()
        {
            return m_readQueue.size();
        }

        /**
        * @brief
        * 当前写队列中的消息个数
        * @return 已经写的消息个数
        */
        size_t WriteSize()
        {
            return m_writeQueue.size();
        }

    private:
        TypedDeque    m_readQueue;
        TypedDeque    m_writeQueue;
        WriteMT       m_writeMutex;
        ReadMT        m_readMutex;
        CSDCondition  m_fifoNotEmpty;
    };

};
#endif // 
