/*
*   玩家角色管理
*/
#ifndef player_manager_h__
#define player_manager_h__

class Player;

class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager();

    DECLARE_SINGLETON(PlayerManager)

    void Update();

    Player* GetPlayerByPlayerID(uint64 id);
    Player* GetPlayerByLoginID(uint64 loginID);
    Player* GetPlayerByAccount(const std::string& account, const std::string& loginServerName);
    Player* GetPlayerByName(const std::string& name);

    uint64 GenPlayerUUID();     // 创建角色唯一ID

    // 检查重复登录
    bool CheckDuplicateLogin(uint32 gateID, uint64 loginID, const std::string& account, const std::string& loginServerName);

    // 添加验证玩家
    bool AddVerifyPlayer(uint32 centGateID, uint64 loginID, const std::string& origin_account, 
                            uint32 ip, uint32 port, 
                            const std::string& uid, const std::string& uname, const std::string& platform, const std::string& loginServerName);
    
    bool AddPlayerForLoaded(Player* player);

    void DelPlayer(uint64 playerID);
    void DelPlayerByLoginID(uint64 loginID);

    uint32 GetAccountNum();                                                     // 得到在线账户数
    uint32 GetPlayerNum();                                                      // 得到在线玩家数
    std::map<std::string, Player*> GetAccountMap(){return m_accountMap;}        // 得到在线玩家信息
    uint32 GetLevelNum();                                                       // 得到指定等级玩家数

    void ChangePlayerName(uint64 playerID, const std::string& newName);                // 修改玩家的角色名

    void ReloadPlayerRechargeInfo(const std::string& account, const std::string& loginServerName);     // 重新加载角色充值信息

    void AddSystemMail(MAIL_INFO& mail_info);                                   // 添加系统邮件
    void DelExpiredMail(uint32 mail_h_id);                                      // 删除过期邮件
    void SendGameMail(MAIL_INFO& mail_info);                                    // 通知GameServer邮件

    std::map<uint64, Player*> GetPlayerMap() { return m_players; }              // 得到玩家的映射

    void LogoutGame(Player* player);

    void GetQuickFriendList(std::list<Player*>& list, Player* player);

    void OnCloseGateServer(uint32 gateSvrID);
    void OnCloseGameServer(uint32 gameSvrID);

    void ReMapPlayerAccount(Player* player, const std::string& oldAccount, const std::string& newAccount);

    int32 OnLikeState(const std::string& user_id);                              // 获取Fb点赞的状态

    bool OnSubItem(uint64 user_id, uint32 itemid, uint32 itemnum);  // 删除物品

private:
    bool AddPlayerForLogin(Player* player);

    void CheckError();

private:
    std::map<uint64, Player*> m_players;
    std::map<uint64, Player*> m_loginIDMap;             // <loginID, player
    std::map<std::string, Player*> m_accountMap;        // < union(loginSrverName, account), player>
    std::map<std::string, Player*> m_nameMap;           //

    int32   m_LogCounter;
};

#endif
