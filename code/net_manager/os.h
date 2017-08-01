//! @file os.h
//! @brief 定义socket相关类型
#ifndef _OS_H_
#define _OS_H_




#ifdef WIN32	// for windows

#include <winsock2.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef int socklen_t;

//! 设置为非阻塞模式
inline int set_socket_nonblocking(SOCKET socket)
{
	int iMode = 1;
	int nret = ioctlsocket(socket, FIONBIO, (u_long FAR*)&iMode);

	return nret;
}

//! 关闭socket
inline int close(SOCKET socket)
{
	return closesocket(socket);
}

//! 错误码
inline int error_no()
{
	return WSAGetLastError();
}


#define NETMANAGER_EAGAIN		WSAEWOULDBLOCK		//<! 无数据可读或可写
#define NETMANAGER_EINPROGRESS	WSAEWOULDBLOCK			//<! 正在连接
#define NETMANAGER_EWOULDBLOCK	WSAEWOULDBLOCK

/*
//! 睡眠，改为毫秒
inline void sleep(unsigned long _m)
{
	Sleep(_m);
}
*/

//! 创建文件夹
inline int mkdir(const char *dirname, int)
{
	return _mkdir(dirname);
}


#define F_OK 0
#define W_OK 2
#define R_OK 4


//! 检测文件
inline int access(const char *path, int mode)
{
	return _access(path, mode);
}




#else			// for linux

#define NETMANAGER_EAGAIN		EAGAIN
#define NETMANAGER_EINPROGRESS	EINPROGRESS
#define NETMANAGER_EWOULDBLOCK	EWOULDBLOCK

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <netinet/tcp.h>

//! @typedef SOCKET
//! @brief socket类型
typedef int SOCKET;


//! @def INVALID_SOCKET
//! @brief 无效socket
#define INVALID_SOCKET -1

//! 睡眠，改为毫秒
inline void Sleep(unsigned long _m)
{
	struct timespec req;
	req.tv_sec = (int)_m/1000;
	req.tv_nsec = (_m-req.tv_sec*1000)*1000000L;
	nanosleep(&req, NULL);
}

//! 设置为非阻塞模式
inline int set_socket_nonblocking(SOCKET socket)
{
	int val = fcntl(socket, F_GETFL, 0);
	int nret = fcntl(socket, F_SETFL, val | O_NONBLOCK);

	return nret > -1 ? 0 : nret;
}

//! 错误码
inline int error_no()
{
	return errno;
}


#endif

//设置socket接收或者发送缓冲区大小
inline int set_socket_bufsize(SOCKET sock, bool isRead, int netbufsize)
{
	int opt_name = isRead ? SO_RCVBUF : SO_SNDBUF;
	int nBufSize = netbufsize;
	return ::setsockopt(sock, SOL_SOCKET, opt_name, reinterpret_cast<char*>(&nBufSize), sizeof(nBufSize));
}

inline int get_socket_bufsize(SOCKET sock, bool isRead, int& netbufsize)
{
	int opt_name = isRead ? SO_RCVBUF : SO_SNDBUF;
	socklen_t nsize = sizeof(netbufsize);
	return ::getsockopt(sock, SOL_SOCKET, opt_name, reinterpret_cast<char*>(&netbufsize), &nsize);
}

inline int set_socket_tcp_nodelay(SOCKET sock)
{
    int on = 1;
    return ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
}

inline int get_socket_tcp_nodelay(SOCKET sock, bool* ret)
{
    char off;
    socklen_t offlen = sizeof(off);
    return ::getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &off, &offlen);
}

#endif // _OS_H_

