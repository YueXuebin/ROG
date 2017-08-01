#include "transfer.h"
#include "splitter_factory.h"

class Encrypt;

class AFTransferClientProxy : public TransferClientProxy
{
public:
    AFTransferClientProxy(const char* ip, uint32 port, TransferRecvFunc func, void* userdata, bool useBigQueue);
    virtual ~AFTransferClientProxy();

    virtual void SetRecvFunc(TransferRecvFunc, void* );
    virtual void Recv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);
    virtual bool Send(uint32 msgID, void* data, uint64 playerID = 0);
    virtual bool SendPacket(Net_Packet* packet);
    virtual bool    IsValid() {return m_clientID != 0;}

    // 设置加密器
    virtual void SetEncrpty(Encrypt* pEncryptor);

    virtual void BeforeDecode(Net_Packet* packet);
    virtual void AfterEncode(Net_Packet* packet);
    /**
     * @brief
     * 建立连接
     * @param ip: 网络地址
     * @param port: 端口号
     */
    virtual void Connect(bool AutoReconnect, uint32 ReconnectTime);

    /**
     * @brief
     * 重新建立连接
     */
    virtual void Reconnect();

    /**
     * @brief
     * 关闭连接
     */
    virtual void Close();

    virtual void Update();

private:
    char                m_ip[20];           ///< IP地址
    uint32              m_port;             ///< 端口号
    uint32              m_clientID;         ///< 客户端ID
    TransferRecvFunc    m_recvfunc;         ///< 接收函数
    void*               m_userdata;         ///< 用户数据
    AfPacketSplitterFactory m_splitterFactory;  ///< 拆包器工厂

    bool                m_NeedReconnect;
    int32               m_CloseTime;       // 重连倒计时
    bool                m_AutoReconnect;    ///< 是否自动重连(默认false)
    uint32              m_ReconnectTime;    ///< 重连时间间隔

    Encrypt*            m_Encryptor;        
};
