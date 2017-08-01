/*
*   玩家角色管理
*/
#ifndef player_manager_h__
#define player_manager_h__

#include "player.h"
#include <map>
#include <string>

class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager();

    DECLARE_SINGLETON(PlayerManager)

    typedef std::map<uint64, Player*>   PlayerMap;

    void Update();
    void SyncFrame(uint32 frameCounter);

    Player* GetPlayerByObjID(uint32 objectID);
    Player* GetPlayerByPlayerID(uint64 playerID);
    Player* GetPlayerByLoginID(uint64 loginID);
    Player* GetPlayerByName(const std::string& name);
    const PlayerMap& GetPlayers(){ return m_players; }
    uint32  GetPlayerNum() {return m_players.size();}
    uint32  GetPlayerNumInGame();
    uint32  GetPlayerNumByCenterID(uint32 centerId);

    bool AddPlayer(Player* player);
    void DelPlayer(uint64 playerID);

    void KickOutPlayer(uint16 errCode, uint64 playerID);
    void KickOutPlayerByCenterID(uint16 errCode, uint32 CenterID);

    void OnCloseCenterServer(uint32 centerID);

    void ReloadTask();                  // 重新加载角色的节日任务

    void SendToAll(uint32 msgId, void* msg);    // 发送给当前GameServer的所有玩家
    void AddMailToAll(MAIL_INFO* mail);
    void DelExpiredMail(uint32 mail_id_h);      // 删除过期邮件
    void GmtUpdate(uint32 type);                // gmt数据更新

private:
    PlayerMap m_players;
    std::map<uint64, Player*> m_loginIDMap;
    std::map<uint32, Player*> m_objectIDMap;

    int32   m_SecondTimer;
};

#endif
