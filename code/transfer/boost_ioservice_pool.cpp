#include "transfer_pch.h"
#ifdef BOOST_NET
#include "transfer.h"
#include "boost_ioservice_pool.h"

BoostIOServicePool::BoostIOServicePool(uint32 serviceCount) :
    m_ServiceCount(serviceCount),
    m_NextService(0)
{
    CnAssert(serviceCount > 0);

    for (std::size_t i = 0; i < serviceCount; ++i)
    {
        IOServicePtr service(CnNew boost::asio::io_service);
        WorkPtr work(CnNew boost::asio::io_service::work(*service));
        m_Services.push_back(service);
        m_Works.push_back(work);
    }
}

BoostIOServicePool::~BoostIOServicePool()
{
}

boost::asio::io_service &BoostIOServicePool::GetIOService()
{
    boost::asio::io_service& service = *m_Services[m_NextService];
    ++m_NextService;
    if (m_NextService == m_Services.size())
        m_NextService = 0;

    return service;
}

void BoostIOServicePool::Run()
{
    for (std::size_t i = 0; i < m_Services.size(); ++i)
    {
        boost::shared_ptr<boost::thread> thread(CnNew boost::thread(
            boost::bind(&boost::asio::io_service::run, m_Services[i])));
        m_Threads.push_back(thread);
    }
}

void BoostIOServicePool::Stop()
{
    for (std::size_t i = 0; i < m_Services.size(); ++i)
        m_Services[i]->stop();

    for (std::size_t i = 0; i < m_Threads.size(); ++i)
        m_Threads[i]->join();
}

#endif // BOOST_NET
