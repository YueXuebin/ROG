#pragma once

#ifndef _SDNETCONFIG_H_
#define _SDNETCONFIG_H_

#ifdef WIN
#include <WinSock2.h>
#include <Windows.h>
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#endif
#include <sdtype.h>



#define DEFAULT_SENDBUF_SIZE	65536
#define DEFAULT_RECVBUF_SIZE	65536

//允许发送或接收的最大包长度
#define MAX_PKG_LEN		(1024*1024)

extern int32 MAX_CONNECTION;		//最大网络连接数
extern int32 RECV_LOOP_BUF_SIZE;	//接收循环buffer大小，Epoll线程到主线程的数据管道大小
extern int32 SEND_LOOP_BUF_SIZE;	//发送循环buffer大小，主线线程到Epoll线程的数据管道大小
extern int32 MAX_NET_REQUEST;		//主线程到Epoll线程的请求队列大小
extern int32 MAX_NET_EVENT;			//Epoll线程到主线程的网络事件队列大小
extern int32 POP_TO_BUF_COUNT;		//每次向临时缓冲队列中弹出请求或事件的个数
extern int32 VAL_SO_SNDLOWAT;		//发送低潮，即setsockopt中的SO_SNDLOWAT
extern uint32 DELAY_RELEASE_CPSOCK_TIME;

#define MSG_MARK  __FUNCTION__, __LINE__



#endif // _SDNETCONFIG_H_
