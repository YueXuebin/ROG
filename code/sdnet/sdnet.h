#pragma once

/******************************************************************************
 sdnet.h  - High perforcement net module classes, macros, types and definitions.
            Include file for sdcore.dll. This library is designed to provide
            large scale network connection management.

 Class    - ISDConnection
          - ISDConnector
          - ISDListener
          - ISDNet
          - ISDPacketParser
          - ISDSession
          - ISDSessionFactory


 Remarks  - SDNet module provides functionality of high performance TCP 
            connection management and high throughput data transmission. SDNet 
            module also defines a simple network programming framework. User
            only need to follow the framework to implement high performance 
            network functionality.

            SDNet is pure virtual C++ function based. It contains a set of pure
            virtual C++ class, include both interfaces implemented inside SDNet
            and callback interfaces which to be implemented by user.
            
            To improvement performance, SDNet adopts high performance thread
            module inside. To simplify usage, SDNet provides single thread 
            module for users. Almost all the functions and callback functions
            are in a single thread, which is called main thread. All the 
            functions provided by SDNet are non-blocked. 

******************************************************************************/

#ifndef SGDP_SDNET_H_20070614
#define SGDP_SDNET_H_20070614

#include "sdbase.h"
#include "sdnet_ver.h"
#include "sdconfig.h"
//#include "sdlogger.h"


#ifdef WIN32
// windows socket
#include "win/sdasyncsocket.h"
#include "win/sdasyncsocketmgr.h"
#endif

namespace Crown{

//
// SDNet module name
//
const char SDNET_MODULENAME[] = "SDNet";

//
// SDNet version
//
const SSGDPVersion	SDNET_VERSION = SDNET_MODULE_VERSION;

//
// Error code of SDNet
//
enum ESDNetErrCode{
	NET_BIND_FAIL	= -7,
    NET_CONNECT_FAIL    = -6,
    NET_SYSTEM_ERROR    = -5, 
    NET_RECV_ERROR      = -4, 
    NET_SEND_ERROR      = -3, 
    NET_SEND_OVERFLOW   = -2,
    NET_PACKET_ERROR    = -1,
    NET_SUCCESS         = 0
};

//
// When create ISDConnector or ISDListener, user need to specify the net I/O
// module. 
//

//
// I/O Completion port module, which only fits for Windows.
//
const uint32 NETIO_COMPLETIONPORT   = 1;
const uint32 NETIO_COMPLETIONPORT_GATE   = 11;

//
// I/O AsyncSelect module, which only fits for Windows and client application.
//
const uint32 NETIO_ASYNCSELECT      = 2;

//
// epoll I/O module, which only fits for linux
//
const uint32 NETIO_EPOLL            = 10;

const uint32 NETIO_EPOLL_GATE       = 101;

const uint32 INVALID_IO_TYPE        = 0xFFFFFFFF;

const uint32 UNKNOWN_SIZE	= 0xFFFFFFFF;

class ISDPacketParser;
class ISDSession;
class ISDSessionFactory;

// 
// Name     : ISDConnection
// Function : An ISDConnection object represent an abstraction of a TCP connection.
//
class ISDConnection
{
public:
    virtual ~ISDConnection() {}
    //
    // Name     : IsConnected
    // Function : Check if the connection is connected.
    //
    virtual bool SDAPI IsConnected(void) = 0;

    //
    // Name     : Send
    // Function : Send data on the connection.
    //
    virtual void SDAPI Send(const char* pBuf,uint32 dwLen) = 0;

    //
    // Name     : SetOpt
    // Function : Set extension options.
    //
    virtual void SDAPI SetOpt(uint32 dwType, void* pOpt) = 0;

    //
    // Name     : Disconnect
    // Function : Close the TCP connection.
    //
    virtual void SDAPI Disconnect(void) = 0;

    //
    // Name     : GetRemoteIP
    // Function : Retrieves remote IP address for the connection.
    //
    virtual const uint32 SDAPI GetRemoteIP(void) = 0;

    //
    // Name     : GetRemoteIPStr
    // Function : Retrieves remote IP address in text format for the connection.
    //
    virtual const char* SDAPI GetRemoteIPStr(void) = 0;

    //
    // Name     : GetRemotePort
    // Function : Retrieves remote port number for the connection.
    // 
    virtual uint16 SDAPI GetRemotePort(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local IP address for the connection.
    // 
    virtual const uint32 SDAPI GetLocalIP(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local IP address in text format for the connection.
    // 
    virtual const char* SDAPI GetLocalIPStr(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local port number for the connection.
    // 
    virtual uint16 SDAPI GetLocalPort(void) = 0;


	//
	// Name		: GetSendBufFree
	// Function	: Retrieves local send buffer's free size
	// 
	virtual uint32 SDAPI GetSendBufFree(void) = 0;
};


//
// Name     : ISDListener
// Function : The interface provides functionality for TCP listening.
//
class ISDListener
{
public:
    virtual ~ISDListener() {}
    //
    // Name     : SetPacketParser
    // Function : Set an user implemented ISDPacketParser object, which process
    //            packet parse logic of the connection accepted by the ISDListener.
    //
    virtual void SDAPI SetPacketParser(ISDPacketParser* poPacketParser) = 0;

    //
    // Name     : SetSessionFactory
    // Function : Set the user implemented ISDSessionFactory object to ISDListener.
    //            When a TCP connection is accepted by ISDListener, 
    //            ISDSessionFactory object will be asked to create a ISDSession.
    //
    virtual void SDAPI SetSessionFactory(ISDSessionFactory* poSessionFactory) = 0;

    //
    // Name     : SetBufferSize
    // Function : Set the send and receive buffer size of the connection accepted
    //            by the ISDListener object.
    //
    virtual void SDAPI SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) = 0;

    //
    // Name     : SetOpt
    // Function : Set extension options.
    //
    virtual void SDAPI SetOpt(uint32 dwType, void* pOpt) = 0;

    //
    // Name     : Start
    // Function : Listen at specified IP and port.
    // 
    virtual bool SDAPI Start(const char* pszIP, uint16 wPort, bool bReUseAddr = true) = 0;

    //
    // Name     : Stop
    // Function : Stop listen.
    //
    virtual bool SDAPI Stop(void) = 0;

    //
    // Name     : Release
    // Function : Release the ISDListener object.
    //
    virtual void SDAPI Release(void) = 0;
};

//
// Name     : ISDConnector
// Function : The interface provides functionality for initiative TCP connection.
//
class ISDConnector
{
public:
    virtual ~ISDConnector() {}
    //
    // Name     : SetPacketParser
    // Function : Set the user implemented ISDPacketParser object, which process
    //            packet parse logic of the connection connected by the ISDConnector
    //            object.
    //
    virtual void SDAPI SetPacketParser(ISDPacketParser* poPakcetParser) = 0;

    //
    // Name     : SetSession
    // Function : Set the user implemented ISDSession object, which will be bind
    //            to ISDConnection object when connection establishes.
    //
    virtual void SDAPI SetSession(ISDSession* poSession) = 0;

    //
    // Name     : SetBufferSize
    // Function : Set the send and receive buffer size for the connection connected
    //            by the ISDConnector object.
    //
    virtual void SDAPI SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) = 0;

    //
    // Name     : Connect
    // Function : Connect to the specified address.
    //
    virtual bool SDAPI Connect(const char* pszIP, uint16 wPort) = 0;

    //
    // Name     : ReConnect
    // Function : Reconnect to the address which is specified by the last Connect
    //            call.
    //
    virtual bool SDAPI ReConnect(void) = 0;

    //
    // Name     : Release
    // Function : Release the ISDConnector object.
    //
    virtual void SDAPI Release(void) = 0;


	//
	// Name     : SetOpt
	// Function : Set extension options.
	//
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt) = 0;


};

//
// Name     : ISDNet
// Function : Interface of SDNet module.
//
class ISDNet : public ISDBase
{
public:
	 virtual ~ISDNet() {};
    //
    // Name     : CreateConnector
    // Function : Create an ISDConnector object.
    //
    virtual ISDConnector* SDAPI CreateConnector(uint32 dwNetIOType) = 0;

    //
    // Name     : CreateListener
    // Function : Create an ISDListener object.
    // 
    virtual ISDListener* SDAPI CreateListener(uint32 dwNetIOType)  = 0;

    //
    // Name     : Run
    // Function : Process network event.
    //
    virtual bool SDAPI Run(int32 nCount = -1) = 0;
};

//
// Name     : ISDPacketParser
// Function : Break packet from TCP data stream. The interface need to be
//            implemented by user.
//
class ISDPacketParser
{
public:
    virtual ~ISDPacketParser() {}
    //
    // Name     : ParsePacket
    // Function : Return the length in bytes of the packet in TCP data stream.
    //
    virtual int32 SDAPI ParsePacket(const char* pBuf, uint32 dwLen) = 0;
};

//
// Name     : ISDSession
// Function : When network event happens, the related function of ISDSession 
//            will be callback. User need to implement ISDSession.
//
class ISDSession
{
public:
    virtual ~ISDSession() {}
    //
    // Name     : SetConnection
    // Function : Set the ISDConnection object to the ISDSession object.
    //
    virtual void SDAPI SetConnection(ISDConnection* poConnection) = 0;

    //
    // Name     : OnEstablish
    // Function : Callback when the connection is established.
    //
    virtual void SDAPI OnEstablish(void) = 0;

    //
    // Name     : OnTerminate
    // Function : Callback when the connection is closed.
    //
    virtual void SDAPI OnTerminate(void) = 0;

    //
    // Name     : OnError
    // Function : Callback when TCP connection has error.
    //
    virtual bool SDAPI OnError(int32 nModuleErr, int32 nSysErr) = 0;

    //
    // Name     : OnRecv
    // Function : Callback when receive a packet, which is parsed by ISDPacketParser.
    //            
    //
    virtual void SDAPI OnRecv(const char* pBuf, uint32 dwLen) = 0;

#ifdef SDNET_HAS_SEND_REPORT
	//
	// Name     : OnSend
	// Function : Callback when send  success, used in asynch mode .
	//           pBuf  : left data in buffer, 
	//           dwLen : left data length  in buffer
	//   				usage:  don't send the data again . 
	virtual void SDAPI OnSend(const char* pBuf, uint32 dwLen) 
	{
	}
#endif

    //
    // Name     : Release
    // Function : Release ISDSession object. It will be callback when SDNet no
    //            more use it.
    //
    virtual void SDAPI Release(void) = 0;
};

//
// Name     : ISDSessionFactory
// Function : Create user implemented ISDSession object. This interface need to
//            be implemented by user too.
//
class ISDSessionFactory
{
public:
    virtual ~ISDSessionFactory() {}
    //
    // Name     : CreateSession
    // Function : Create an user implemented ISDSession object.
    //
    virtual ISDSession* CreateSession(ISDConnection* poConnection) = 0;
};

//
// Name     : SDNetGetModule
// Function : Get an SDNet object.
//
ISDNet* SDAPI SDNetGetModule(const SSGDPVersion* pstVersion);

//
// Function pointer definition of SDNetGetModule
//
typedef ISDNet* (SDAPI *PFN_SDNetGetModule)(const SSGDPVersion* pstVersion);

//
// Name     : SDNetSetLogger
// Function : Set ISDLogger object to the SDNet. The log of SDNet will export
//            to the ISDLogger object.
//
//bool SDAPI SDNetSetLogger(ISDLogger* poLogger, uint32 dwLevel);

//
// Function pointer definition of SDNetSetLogger
//
//typedef bool (SDAPI *PFN_SDNetSetLogger)(ISDLogger* poLogger, uint32 dwLevel);

//
// Name     : SDNetSetOpt
// Function : Set extension options.
//
void SDAPI SDNetSetOpt(uint32 dwType, void* pOpt);

}

#endif
