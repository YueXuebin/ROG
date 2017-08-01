

/************************************************************************/
/*                           监控服务器管理                             */
/************************************************************************/


#ifndef SERVER_INFO_HANDLER_H
#define SERVER_INFO_HANDLER_H

class TransferServerProxy;

class ServerInfoHandler
{
public:
    ServerInfoHandler();
    ~ServerInfoHandler();

    DECLARE_SINGLETON(ServerInfoHandler)
    
    bool Init(const char* ip, uint32 port);

/************************************************************************/
/*                       接收网络消息                                   */
/************************************************************************/

    static void RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

private:

    
    void OnAccept(uint32 sessionID, void* data);
    void OnConnect(uint32 sessionID, void* data);
    void OnClose(uint32 sessionID, void* data);
    void OnError(uint32 sessionID, void* data);
    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data, void* userdata);


    void OnDoungenReq( uint32 sessionID );

    TransferServerProxy* m_InfoServer;
};

#endif