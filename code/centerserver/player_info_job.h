#ifndef player_info_job_h__
#define player_info_job_h__


class DBExecutor;
class DBRecorder;
class Player;

/*
*	创建角色的数据库操作
*/
class PlayerInfoJob
{
    PlayerInfoJob() { m_dbExecutor = 0; }
    ~PlayerInfoJob() { }

    DECLARE_SINGLETON(PlayerInfoJob)

    void CreatePlayerInfo(uint32 gateSvrID, uint64 loginID, uint64 playerID, 
        const std::string& uid, const std::string& uname, const std::string& platform, const PROPERTY_INFO& info);

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

private:
    bool OnCreatePlayerInfoEnd(uint32 gateSvrID, uint64 loginID, uint64 playerID, const std::string& uid, const std::string& uname, const PROPERTY_INFO& info);

    bool CreateOnlineActivity(uint64 playerID);

    bool CreateDailyActivity(uint64 playerID);                                                                                              // 创建普通活动

    bool CreateEquip(uint64 playerID);     // 创建装备

    bool CreatePlayerLog(uint64 loginID, uint64 playerID, const std::string& uname,const PROPERTY_INFO& propInfo);                          // 创建角色日志

    bool CreatePlayer(uint64 playerID, const std::string& uid, const std::string& uname, const std::string& platform, const PROPERTY_INFO& info);   // 创建玩家信息
 
    bool CreateHeadPortrait(uint64 playerID, const PROPERTY_INFO& propInfo);   // 创建玩家头像信息

    bool RechargeReturn(uint64 playerID, const std::string& uname);

    bool CreatePlayerDungeonInfo(uint64 playerID);

    bool CreateOfflinePlayerInfo(uint64 playerID, const PROPERTY_INFO& propInfo);

    bool CreateOfflineRankingInfo(uint64 playerID);

    bool CreateItem(uint64 playerID);

    bool CreateSkill(uint64 playerID);

    bool CreatePet(uint64 playerID);

private:
    DBExecutor*         m_dbExecutor;
};

#endif
