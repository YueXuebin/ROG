#pragma once

#ifndef SHAREDRESOUCES_H
#define SHAREDRESOUCES_H

#include <sdsingleton.h>

#include <boost/asio.hpp>

class SharedResouces : public Crown::Singleton<SharedResouces, Crown::WITH_DEFAULT_CONSTRUCTOR>
{
public:
	SharedResouces();

	boost::asio::io_service& IOService();

    std::string GSID;
};

#endif // SHAREDRESOUCES_H
