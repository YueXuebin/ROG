#pragma once

/*
*	会话管理器(会话代表的是一个连接)
*/
#ifndef SessionManager_h__
#define SessionManager_h__

#include "protocol_define.h"
#include "session.h"

class SessionManager : public Crown::ISDSessionFactory
{
    SessionManager();
    virtual ~SessionManager();
public:
    static SessionManager*	GetInstance() {return m_Instance;}
    static bool Init();         // 创建并初始化
    static void Release();

    RealTransferServer* GetServerProxy() { return m_pServerProxy; }
    void    SetServerProxy(RealTransferServer* pServerProxy) {m_pServerProxy = pServerProxy;}

    bool    StartListen(const char * ip, uint32 port);
    void    Update();
    bool    Send(uint32 sessionID, char* data, uint32 dataLen);

    bool    HasSession(uint32 sessionID);
    Session* GetSession(uint32 sessionID);

    virtual Crown::ISDSession* CreateSession(Crown::ISDConnection* poConnection);   // 连接时被调用,创建Session
    void    ReleaseSession(Session* poSessin);
    void    ReleaseSession(uint32 SessionId);

private:
    bool _Init();
    void _Release();

    // Session会话相关
    Crown::CSDObjectPool<Session>*  m_pCliSessionPool;                  // Session对象池
    Session*                        m_AllSessions[MAX_PLAYER_NUM];      // 创建出来的Session
    Crown::CSDMutex                 m_mutex;

    // 网络相关
    Crown::ISDNet*                  m_pNetModule;
    Crown::ISDListener*             m_pListener;                        // 监听器

    RealTransferServer*             m_pServerProxy;
    static SessionManager*          m_Instance;
};

#endif // SessionManager_h__
