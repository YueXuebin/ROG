#include "transfer.h"
#include "splitter_factory.h"

class AFTransferServerProxy : public TransferServerProxy
{
public:
    AFTransferServerProxy(const char* ip, uint32 port, TransferRecvFunc func, void* userdata);
    virtual ~AFTransferServerProxy();

    virtual void    SetRecvFunc(TransferRecvFunc, void* );
    virtual void    Recv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);
    virtual bool    Send(uint32 sessionID, uint32 msgID, void* data, uint64 playerID=0);
    virtual void    Close(uint32 sessionID);
    virtual bool    IsValid() {return m_serverID != 0;}
private:
    uint32              m_serverID;         ///< 服务器ID
    TransferRecvFunc    m_recvfunc;         ///< 接收函数
    void*               m_userdata;         ///< 客户数据
    AfPacketSplitterFactory m_splitter_factory; ///< 拆包器工厂
};