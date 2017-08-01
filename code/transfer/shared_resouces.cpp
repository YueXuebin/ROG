#include "transfer_pch.h"

#ifdef BOOST_NET
#include "SharedResouces.h"


SharedResouces::SharedResouces()
{
}

boost::asio::io_service &SharedResouces::IOService()
{
	static boost::asio::io_service service;
	return service;
}

#endif // BOOST_NET
