#ifndef player_job_h__
#define player_job_h__


class DBExecutor;
class DBRecorder;
class Player;


class PlayerJob
{
    PlayerJob() { m_dbExecutor = 0; }
    ~PlayerJob() { }

    DECLARE_SINGLETON(PlayerJob)

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; }

    void LoginLoadAfAccount(uint32 gateSvrID, uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort);  // 加载账号数据

    void LoginLoadKLAccount(uint32 gateSvrID, uint64 loginID, std::string& uid, std::string& uname, uint32 addrIP, uint16 addrPort);  // 加载账号数据

    // 创建游戏角色
    void CreatePlayer(uint32 gateSvrID, uint64 loginID, const std::string& account, const std::string& name, 
        const std::string& uid, const std::string& uname, const std::string& platform, const std::string& platform_server_name,
        int32 region_id, uint8 sex, uint8 career);

    // 查询账号的角色
    void AskPlayerInfo(uint32 gateSvrID,  uint64 loginID, const std::string& account, const std::string& platform_server_name);

    void ChangePlayerName(uint64 playerID, std::string& name);  // 请求修改角色名称

    void SavePlayerDataAndDestory(Player* player);      // 保存角色数据,并销毁角色
    void AutoSavePlayerDataToDB(Player* player);        // 自动回存角色数据到DB

    void AddFriend(uint64 player_id_a, uint64 player_id_b);         // 添加好友
    void DelFriend(uint64 player_id_a, uint64 player_id_b);         // 删除好友

    void RefreshPlayerDoorList(Player* player, uint32 battle_power);                            // 获得离线玩家的列表
    void RefreshPlayerDoorListByPlayerID(Player* player, std::vector<uint64>& playerIDList);    // 根据离线玩家id列表获取离线玩家的列表
    void RequestOfflinePlayerRecord(uint64 playerID, uint32 GameServerID, uint8 reason);                      // 获得离线玩家数据
    void RequestOfflinePlayerRecordByName(const std::string& playerName, uint32 GameServerID, uint8 reason);  // 获得离线玩家数据(用名字查询)

    void GetAnswerTopRank(uint8 isend);                                             // 获得答题前10排名
    void GetAnswerPlayerRank(uint64 playerID, uint32 msgID);                        // 获得个人答题排名
    void SaveAnswerScore(uint64 playerID, std::string player_name, int32 score);    // 保存答题个人信息
    void GetAnswerTopRankEndNtf();                                                  // 获得答题前10排名


    void AddMoneyPoint(std::string account, int32 moneyPoint);  // 添加魔石
    void CBAddMoneyPoint(DBRecorder& res);                      // 回调添加魔石

    void SubMoneyPoint(std::string account, int32 moneyPoint);  // 减少魔石
    void CBSubMoneyPoint(DBRecorder& re);                       // 回调减少魔石

    bool LoadActivationCodeItemInfo(uint64 playerID, std::string activation_code);

    bool FindPlayerNameIsExit(const std::string& playername);

private:
    PlayerJob( const PlayerJob& );
    PlayerJob& operator=( const PlayerJob& );
    PlayerJob* operator&();
    const PlayerJob* operator&() const;

    void RecordStatistic(Player* player);

    void ParseOfflinePlayerInfo(DBRecorder& db, OFFLINE_PLAYER_INFO& player_info);

    // ================================================================================

    void RecordPlayerOnlineTime(uint64 playerID, uint32 beginTime, uint32 endTime);
    void CBRecordStatistic(DBRecorder& res);

    void CBAskPlayerInfo(DBRecorder& res, uint32 gateSvrID, uint64 loginID);        // 查询角色信息

    void CBChangePlayerName(DBRecorder& res, uint64 playerID, std::string name);    // 更改角色名称

    void CBLoginLoadAfAccount(DBRecorder& res, uint32 gateSvrID, uint64 loginID, uint32 addrIP, uint16 addrPort);
    void CBLoginLoadKlAccount(DBRecorder& res, uint32 gateSvrID, uint64 loginID, 
                                std::string uid, std::string uname, uint32 addrIP, uint16 addrPort);

    void CBDeletePlayerInfo(DBRecorder& res);                                       // 删除角色

    void CBRefreshPlayerDoorList(DBRecorder& res, uint64 playerID);                 // 刷新离线角色列表

    void CBRefreshPlayerDoorListByPlayerID(DBRecorder& res, uint64 playerID);       // 根据离线玩家id刷新离线玩家列表

    void CBRequestOfflinePlayerRecord(DBRecorder& res, uint32 GameServerID, uint8 reason);        // 获得离线角色信息

    void CBGetOfflinePlayerInfo(DBRecorder& res, uint64 playerID);                  // 获得离线角色信息

    void CBAddFriend(DBRecorder& res);                                              // 添加好友

    void CBDelFriend(DBRecorder& res);                                          // 删除好友

    void CBGetAnswerTopRank(DBRecorder& res, uint8 isend);                      // 获得前TOP答题排行

    void CBGetAnswerPlayerRank(DBRecorder& res, uint64 playerID, uint32 msgID); // 获得个人答题排行

    void CBSaveAnswerScore(DBRecorder& res);                                    // 保存个人答题数据

    void CBLoadPassiveSkillInfo(DBRecorder& res, uint64 playerID);              // 获得被动技能信息

    void CBLoadActivationCodeItemInfo(DBRecorder& res, uint64 playerID);        // 加载激活码信息

    void CBLoadSingleAnswerInfo(DBRecorder& res, uint64 playerID);              // 加载单人问答

private:
    static char m_dungeon_buff[MAX_SQLTXT_LEN];
    static char m_vip_buff[MAX_SQLTXT_LEN];
    static char m_open_system_buff[MAX_SQLTXT_LEN];
    static char m_guild_welfare_buff[MAX_SQLTXT_LEN];
    static char m_city_fight_buff[MAX_SQLTXT_LEN];
    static char m_fcm_buff[MAX_SQLTXT_LEN];
    static char m_vigor_buff[MAX_SQLTXT_LEN];

    DBExecutor*         m_dbExecutor;                                           ///< SQL执行器
};

#endif
