#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "sdu.h"
#include <list>

class Player;
class Net_Packet;

/// 角色管理器
class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager();

    DECLARE_SINGLETON(PlayerManager)

    void Update();

    uint32 GetPlayerNum() {return m_players.size();}

    uint32 GetPlayerNumByGameServerIDAndState(uint32 gameServerID, uint32 state);     // 检测用

    bool AddSessionPlayer(uint32 sessionID, uint32 addrIP, uint16 addrPort);    ///< 添加角色
    void DelSessionPlayer(uint32 sessionID);        ///< 删除角色

    uint64 GetLoginID(uint32 sessionID);            ///< 获得玩家的登陆ID
    uint64 GetPlayerID(uint32 sessionID);           ///< 获得玩家的游戏ID

    uint32 GetSnIDByLoginID(uint64 loginID);        ///< 通过登陆ID获得会话ID
    uint32 GetSnIDByPlayerID(uint64 playerID);      ///< 通过角色ID获得会话ID

    uint32 GetState(uint32 sessionID);              ///< 获得玩家的游戏状态
    Player* GetPlayer(uint32 sessionID);            ///< 获得玩家
    Player* GetPlayerByLoginID(uint64 loginID);     ///< 获得玩家
    Player* GetPlayerByPlayerID(uint64 playerID);   ///< 获得玩家

    void CloseCenterServer();                       ///< 关闭中心服务器

    void SendToAll(uint32 msgID, void* data);
    void SendToAll(Net_Packet* packet);             ///< 发送给所有的玩家
private:
    std::map<uint32, Player*>   m_players;          // <session id, player>
    std::map<uint64, Player*>   m_loginIDMap;       // <login id, player>
};

#endif
