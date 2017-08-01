#ifndef BOOSTIOSERVICEPOOL_H
#define BOOSTIOSERVICEPOOL_H

#ifdef BOOST_NET

#include <boost/thread.hpp>
#include <boost/asio/io_service.hpp>

class BoostIOServicePool : private boost::noncopyable
{
public:
    typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

    BoostIOServicePool(uint32 serviceCount);
    virtual ~BoostIOServicePool();

    boost::asio::io_service& GetIOService();
    void Run();
    void Stop();

private:
    uint32 m_ServiceCount;
    uint32 m_NextService;

    std::vector<IOServicePtr> m_Services;
    std::vector<WorkPtr> m_Works;
    std::vector<boost::shared_ptr<boost::thread> > m_Threads;
};

#endif // BOOST_NET

#endif // BOOSTIOSERVICEPOOL_H
