#include "net_manager_pch.h"
#include "net_thread.h"
#include "net_manager.h"
#include "reactor.h"
#include "log.h"


// class Net_Thread
Net_Thread::Net_Thread(Net_Manager *net_manager, Reactor *reactor)
{
    m_is_run = false;
    m_notify_stop = false;
    m_net_manager = net_manager;
    m_reactor = reactor;
}

Net_Thread::~Net_Thread()
{
}

int Net_Thread::start()
{
    if (false != m_is_run) {
        return -1;
    }

    m_notify_stop = false;
    int rc = activate();
    if (rc != 0) {
        return -1;
    }

    m_is_run = true;
    return 0;
}

int Net_Thread::stop()
{
    m_notify_stop = true;
    wait();

    m_is_run = false;
    return 0;
}

int Net_Thread::svc()
{
    while (m_notify_stop != true)
    {
        // dispatch task
        int task_count = 0;
        while (true)
        {
            //LOG(FATAL)("Net_Thread::svc check send task quene");
            Net_Packet* send_packet = m_net_manager->get_send_packet();
            if (NULL == send_packet) {
                break;
            }

            send_packet->movetobegin();

            Event_Handler* event_handler = NULL;
            if (send_packet->id == 0)
            {
                switch (send_packet->m_cmd.cmd)
                {
                case Net_Packet::NET_TCP_ACCEPTOR_REGISTER:
                case Net_Packet::NET_UDP_REGISTER:
                    {
                        event_handler = reinterpret_cast<Event_Handler*>(send_packet->m_cmd.data);
                        m_reactor->register_handler(event_handler, send_packet->m_cmd.mask);
                    }
                    break;
                case Net_Packet::NET_TCP_CONNECTOR_REGISTER:
                    {
                        event_handler = reinterpret_cast<Event_Handler*>(send_packet->m_cmd.data);
                        m_reactor->register_handler(event_handler, send_packet->m_cmd.mask);
                        m_reactor->set_timeout(send_packet->m_cmd.id, send_packet->m_cmd.timeout);
                    }
                    break;
                case Net_Packet::NET_DELETE:
                    {
                        m_reactor->notify_close(send_packet->m_cmd.id);
                    }
                    break;
                default:
                    break;
                }


                delete send_packet;
                continue;
            }

            event_handler = m_reactor->get_event_handler(send_packet->id);
            if (event_handler == NULL) 
            {
                delete send_packet;
                continue;
            }

            int rc = event_handler->post_packet(send_packet);
            if(rc != 0)
            {
                delete event_handler;
                event_handler = NULL;
            }

            task_count++;
        }

        // network event loop
        int event_count = m_reactor->run_reactor_event_loop();
        if (event_count < 0)
        {
            // epoll错误, 线程退出
            m_is_run = false;
            m_net_manager->on_net_error();
            return -1;
        }

    }

    return 0;
}

