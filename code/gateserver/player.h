/*
 *	gateserver上的玩家对象
 */
#ifndef player_h__
#define player_h__

#include "sdu.h"

class Net_Packet;
class Encrypt;

class Player
{
public:
    Player();
    Player(const Player& player);
    ~Player();

    // LoginID和PlayerID都可以唯一的标识出某个玩家
    // 但当玩家刚连接到游戏还没有通过验证的时，这时还没有PlayerID
    // 这时需要通过LoginID来确保消息送回给指定的玩家
    void Clear()
    {
        m_sessionID = 0;        // 会话ID
        m_playerID = 0;         // 玩家ID（由CenterServer产生）
        m_loginID = 0;          // 登陆的唯一ID
        m_gameSvrID = 0;        // 游戏服务器ID
        m_loginTime = 0;        // 登陆时间
        m_addrIP = 0;           // 登陆地址IP
        m_addrPort = 0;         // 登陆地址端口
        m_state = kStateInit;   // 玩家的游戏状态
    }

    enum
    {
        kStateInit              = 0,    // 初始状态
        kStateSetPrivateKey     = 1,    // 设置私有密匙
        kStateLoginReq          = 2,    // 已发送登陆请求
        kStateLoginOk           = 3,    // 登陆成功
        kStateLoadReq           = 4,    // 角色加载请求
        kStateLoadOk            = 5,    // 角色加载成功
        kStateInGame            = 6,    // 在游戏中
        kStateChangeDungeon     = 7,    // 切换副本
        kStateChangePlayerName  = 8,    // 更改角色名称
        kStatelogout            = 9,    // 退出游戏
    };

    void Update();

    uint32 GetSessionID() { return m_sessionID; }
    void SetSessionID(uint32 val) { m_sessionID = val; }

    uint64 GetPlayerID() { return m_playerID; }
    void SetPlayerID(uint64 val) { m_playerID = val; }

    uint64 GetLoginID() { return m_loginID; }
    void SetLoginID(uint64 val) { m_loginID = val; }

    uint32 GetGameSvrID() { return m_gameSvrID; }
    void SetGameSvrID(uint32 val) { m_gameSvrID = val; }

    void EncryptPacket(Net_Packet* packet);
    void DecryptPacket(Net_Packet* packet);

    bool CompressPacket(Net_Packet* packet);

    void SetEncryptPrivateKey(uint32 val);
    uint32 GetEncryptPrivateKey() const;

    void SetEncryptPublicKey(uint32 val);
    uint32 GetEncryptPublicKey() const;

    uint32 GetLoginTime() { return m_loginTime; }
    void SetLoginTime(uint32 val) { m_loginTime = val; }

    uint32 GetAddrIP() { return m_addrIP; }
    void SetAddrIP(uint32 val) { m_addrIP = val; }

    uint16 GetAddrPort() { return m_addrPort; }
    void SetAddrPort(uint16 val) { m_addrPort = val; }

    uint32 GetState() { return m_state; }
    void SetState(uint32 val) { m_state = val; }

    void SendEncryptPublicKeyNtf();                     // 发送私有Key 给客户端

    bool Send2Client(uint32 msgID, void* data);

    void OnSendMessage(uint32 msgID);
    void OnRecvMessage(uint32 msgID);

    uint32 GetSendCount() {return m_SendCount;}
    uint32 GetRecvCount() {return m_RecvCount;}

    bool CanCompress() { return m_bCompress; }
    void EnableCompress() { m_bCompress = true; }

private:
    Encrypt* m_encrypt;

    bool    m_bCompress;

    uint32  m_sessionID;
    uint64  m_playerID;
    uint64  m_loginID;
    uint32  m_gameSvrID;
    uint32  m_centerID;
    uint32  m_loginTime;
    uint32  m_addrIP;
    uint16  m_addrPort;
    uint32  m_state;

    uint32  m_SendCount;    // 发送消息数
    uint32  m_RecvCount;    // 接收消息数

    int32   m_SecondTimer;
    uint32  m_RecvCountPerSecond;   // 每秒接收消息数

    
};

#endif // player_h__
