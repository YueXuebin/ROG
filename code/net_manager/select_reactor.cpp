#include "net_manager_pch.h"

#include "select_reactor.h"
#include "event_handler.h"
#include "log.h"



// class Select_Reactor
Select_Reactor::Select_Reactor()
{
}

Select_Reactor::~Select_Reactor()
{
}

int Select_Reactor::open_reactor()
{
    return 0;
}

int Select_Reactor::close_reactor()
{
    return 0;
}

int Select_Reactor::run_reactor_event_loop()
{
    int fd_size = FD_SETSIZE;     // 看看FD_SETSIZE是多少

    fd_set read_set;
    fd_set write_set;
    fd_set exception_set;

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_ZERO(&exception_set);

    SOCKET max_fd = 0;

    if(m_HandlerCount == 0)     // 无socket
    {
        Sleep(10);
        return 0;
    }

    list_head* head;
    list_head* pos, *n;

    for (int nIndex = 0; nIndex < HANDLER_TABLE_SIZE; nIndex++)
    {
        head = &HandlerTable[nIndex];
        list_for_each(pos, head)
        {
            Event_Handler* pEventHandler = list_entry(pos, Event_Handler, hashitem);
            SOCKET fd = pEventHandler->get_handle();

            if (pEventHandler->read)
            {
                FD_SET(fd, &read_set);      // 读socket
            }
            if (pEventHandler->write)
            {
                FD_SET(fd, &write_set);     // 写socket
            }
            if (true) 
                FD_SET(fd, &exception_set); // 所有socket都加入异常
            if (fd > max_fd) 
                max_fd = fd;
        }
    }

    max_fd++;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;     // 1毫秒超时

    // select事件
    int count = select((int)max_fd, &read_set, &write_set, &exception_set, &timeout);
    if (count < 0)
    {   // 错误SOCKET_ERROR
        return -1;
    }
    if (count == 0)
    {   // 超时
        count = 0;
    }

    for (int nIndex = 0; nIndex < HANDLER_TABLE_SIZE; nIndex++)
    {
        head = &HandlerTable[nIndex];
        list_for_each_safe(pos, n, head)
        {
            Event_Handler *event_handler = list_entry(pos, Event_Handler, hashitem);
            SOCKET fd = event_handler->get_handle();

            if (true)
            {
                if (FD_ISSET(fd, &exception_set))
                {
                    removeHandler(event_handler);
                    event_handler->handle_exception();
                    CnDelete event_handler;
                    continue;
                }
            }
            // 读
            if (event_handler->read)
            {
                if (FD_ISSET(fd, &read_set))
                {
                    int rc = event_handler->handle_input();

                    if (-1 == rc)
                    {
                        removeHandler(event_handler);
                        event_handler->handle_close();
                        CnDelete event_handler;
                        continue;
                    }
                    if (-2 == rc)
                    {
                        removeHandler(event_handler);
                        event_handler->handle_exception();
                        CnDelete event_handler;
                        continue;
                    }
                }
            }
            // 写
            if (event_handler->write)
            {
                if (FD_ISSET(fd, &write_set))
                {
                    int type = event_handler->get_type();
                    int rc = event_handler->handle_output();
                    
                    if(type == CONNECTOR_HANDLER)
                        continue;       // connector会在handle_output中删除自己

                    if (-1 == rc)
                    {
                        // 连接关闭
                        removeHandler(event_handler);
                        event_handler->handle_close();
                        CnDelete event_handler;
                        continue;
                    }
                    if (-2 == rc)
                    {
                        // 连接异常
                        removeHandler(event_handler);
                        event_handler->handle_exception();
                        CnDelete event_handler;
                        continue;
                    }
                }
            }
        }
    }

    return count;
}

int Select_Reactor::end_reactor_event_loop()
{
    return Reactor::end_reactor_event_loop();
}

int Select_Reactor::register_handler(Event_Handler *event_handler, uint32_t masks)
{
    event_handler->reactor(this);
    uint32_t id = event_handler->get_id();

    Event_Handler* pEventHandler = get_event_handler(id);
    if (pEventHandler == NULL)
    {
        event_handler->notify_close = false;
        event_handler->timeout = 0;
        // add
        if (masks & MASK_READ) {
            event_handler->read = true;
        }
        else {
            event_handler->read = false;
        }

        if (masks & MASK_WRITE) {
            event_handler->write = true;
        }
        else {
            event_handler->write = false;
        }

        putHandler(event_handler, event_handler->get_id());
    }
    else
    {
        // mod
        if (masks & MASK_READ)
            pEventHandler->read = true;
        if (masks & MASK_WRITE)
            pEventHandler->write = true;
    }
    return 0;
}

int Select_Reactor::remove_handler(Event_Handler *event_handler, uint32_t masks)
{
    CnAssert(event_handler->reactor() == this);
    uint32_t id = event_handler->get_id();

    Event_Handler* pEventHandler = get_event_handler(id);
    if (NULL == pEventHandler) 
    {
        CnAssert(false);        // 必存在
        return 0;
    }
    CnAssert(event_handler == pEventHandler);

    // 移除读写标志位
    if (masks & MASK_READ)
        pEventHandler->read = false;
    if (masks & MASK_WRITE) 
        pEventHandler->write = false;

    return 0;
}

int Select_Reactor::set_timeout(uint32_t id, int timeout)
{
    Event_Handler* pEventHandler = get_event_handler(id);
    if (NULL == pEventHandler)
    {
        return -1;
    }

    pEventHandler->timeout = time(NULL) + timeout;
    return 0;
}

int Select_Reactor::cancel_timeout(uint32_t id)
{
    Event_Handler* pEventHandler = get_event_handler(id);
    if (NULL == pEventHandler)
    {
        return 0;
    }

    pEventHandler->timeout = 0;
    return 0;
}

int Select_Reactor::notify_close(uint32_t id)
{
    Event_Handler* pEventHandler = get_event_handler(id);
    if (NULL == pEventHandler) {
        return -1;
    }

    pEventHandler->notify_close = true;
    removeHandler(pEventHandler);
    pEventHandler->handle_close();
    CnDelete pEventHandler;

    return 0;
}

Event_Handler* Select_Reactor::get_event_handler(uint32_t id)
{
    return getHandler(id);
}

int Select_Reactor::delete_handler(Event_Handler *event_handler)
{
    CnAssert(event_handler->reactor() == this);
    removeHandler(event_handler);
    return 0;
}




