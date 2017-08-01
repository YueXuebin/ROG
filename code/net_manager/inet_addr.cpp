#include "net_manager_pch.h"
#include "inet_addr.h"


// class INET_Addr
INET_Addr::INET_Addr()
{
	m_addr = 0;
	m_port = 0;
}

INET_Addr::~INET_Addr()
{
}

INET_Addr::INET_Addr(const INET_Addr& rhs)
{
	m_addr = rhs.m_addr;
	m_port = rhs.m_port;
}

INET_Addr& INET_Addr::operator=(const INET_Addr& rhs)
{
	m_addr = rhs.m_addr;
	m_port = rhs.m_port;
	return *this;
}

void INET_Addr::set_addr(in_addr_t addr)
{
	m_addr = addr;
}

in_addr_t INET_Addr::get_addr() const
{
	return m_addr;
}

void INET_Addr::set_port(in_port_t port)
{
	m_port = port;
}

in_port_t INET_Addr::get_port() const
{
	return m_port;
}

void INET_Addr::clear()
{
	m_addr = 0;
	m_port = 0;
}

