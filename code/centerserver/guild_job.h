class DBExecutor;
class DBRecorder;

class Player;

#ifndef guild_job_h__
#define guild_job_h__

class GuildJob
{
    GuildJob() { m_dbExecutor = 0; }
    ~GuildJob() { }

    DECLARE_SINGLETON(GuildJob)

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; }

    /// 检查同名公会是否存在
    void CheckGuildExist(const std::string& guild_name, uint64 playerID);

    /// 创建公会
    void CreateGuild(Player* player, const std::string& guildName);

    /// 删除公会
    void DestroyGuild(Player* player);

    /// 添加公会
    bool AddGuildInfo(uint64 guildID, const std::string& guildName, int32 memberCount, int32 level, int32 battle_power);

    /// 删除公会
    bool DelGuildInfo(uint64 guildID);

    /// 更新角色的公会ID
    bool SetPlayerGuildID(uint64 playerID, uint64 guildID);

    /// 保存到公会排行榜尾部
    bool SaveGuildRanking(uint64 guildID);

    /// 重置公会成员公会ID
    bool ResetMemberGuildID(uint64 guildID);

    /// 设置公会标记
    void SetGuildFlag(uint64 guildID, uint32 flag);

    /// 设置公会的等级和经验
    void SetGuildLeveAndExp(uint64 guildID, int32 guildLevel, int32 guildExp);

    /// 设置公会科技的加成
    void SetGuildAddition(uint64 guildID, std::vector<GUILD_ADDITION> addition_list);

    /// 设置工会科技加成的玩家信息（游戏币和魔石加成次数）
    //void SetGuildPlayerAddition(uint64 playerID, std::vector<GUILD_PLAYER_ADDITION> addition_list);

    /// 设置公会BOSS的创建和死亡时间
    void SetGuildBossTime(uint64 guildID, uint32 startTime, uint32 createTime, uint32 deadTime);

    /// 设置公会的等级和经验
    void SetGuildBossLeveAndExp(uint64 guildID, int32 bossLevel, int32 bossExp);

    /// 设置公会战斗力
    void SetMemberCountAndBattlpower(uint64 guildID, int32 member_count, int32 battlePower);

    /// 添加公会成员
    bool AddGuildMember(uint64 guildID, uint64 playerID, int32 officeID);

    /// 删除公会成员
    bool DelGuildMember(uint64 guildID);

    /// 删除公会成员
    bool DelGuildMember(uint64 guildID, uint64 playerID);

    /// 申请加入公会请求列表
    void GetGuildApplyList(uint64 playerID, uint64 guildID);

    /// 获得公会列表
    void GetGuildList(uint64 playerID, int32 page);

    /// 设置公会公告
    void SetGuildBulletin(uint64 guildID, const std::string& bulletin);

    /// 加载公会信息
    void LoadGuildInfoForMgr(uint64 guildID);

    /// 加载公会成员信息
    void LoadGuildMemberForMgr(uint64 guildID, GUILD_INFO& guildInfo);

    /// 角色加入公会前检查公会ID
    void CheckPlayerJoinGuild(uint64 applyPlayerID, uint64 invitePlayerID, uint64 guildID);

    /// 角色离开公会前检查公会ID
    void CheckPlayerLeaveGuild(uint64 playerID, int32 office, uint64 guildID, uint64 delMemberID);

    /// 将角色放入公会中
    void PlayerJoinGuild(uint64 playerID, uint64 guildID, GUILD_MEMBER& guildMember);

    /// 角色离开公会
    void PlayerLeaveGuild(uint64 delMemberID, uint64 guildID, uint64 playerID, int32 office);

    /// 添加加入公会申请
    void AddGuildApply(uint64 guildID, uint64 playerID, const std::string& playerName, int32 level, int32 career);

    /// 删除加入公会申请
    void DelGuildApply(uint64 guildID, uint64 playerID);

    /// 加入公会检查自动加入标记
    void JoinGuildCheckFlag(uint64 guildID, uint64 playerID);

    /// 设置公会成员职位
    void SetGuildMemberOffice(uint64 guildID, uint64 playerID, int32 office);

    /// 设置公会成员职位无锁
    void SetGuildMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office);

    /// 贡献点加入玩家总贡献里
    void AddGuildPointToContributeValue(uint64 guildID, uint64 playerID, int32 guildPoint);
private:
    GuildJob( const GuildJob& );
    GuildJob& operator=( const GuildJob& );
    GuildJob* operator&();
    const GuildJob* operator&() const;

    void CBCheckGuildExist(DBRecorder& res, uint64 playerID, std::string guild_name);               ///< 加载个人邮件数据

    void CBSetGuildFlag(DBRecorder& res, uint64 guildID, uint32 guildFlag);                         ///< 设置公会标记

    //void CBSetGuildLeveAndExp(DBRecorder& res, uint64 guildID, int32 guildLevel, int32 guildExp);   ///< 设置公会等级和经验

    void CBSetMemberCountAndBattlePower(DBRecorder& res, uint64 guildID, int32 battlePower);        ///< 设置公会战斗力

    void CBGetGuildList(DBRecorder& res, uint64 playerID, int32 page, int32 count);                 ///< 查询公会列表回调

    void CBGetGuildApplyList(DBRecorder& res, uint64 playerID);                                     ///< 获得加入公会的请求列表

    void CBGuildInfoCount(DBRecorder& res, uint64 playerID, int32 page);                            ///< 获得公会列表总数回调

    void CBSetGuildBulletin(DBRecorder& res, uint64 guildID);                                       ///< 设置公会公告

    void CBLoadGuildInfoForMgr(DBRecorder& res);                                                    ///< 加载公会信息

    void CBLoadGuildMemberForMgr(DBRecorder& res, GUILD_INFO guildInfo);                            ///< 加载公会成员信息

    void CBCheckPlayerJoinGuild(DBRecorder& res, uint64 applyPlayerID, uint64 invitePlayerID, uint64 guildID);          ///< 加入公会

    void CBCheckPlayerLeaveGuild(DBRecorder& res, uint64 playerID, int32 office, uint64 guildID, uint64 delMemberID);   ///< 离开公会

    void CBAddGuildApply(DBRecorder& res, uint64 guildID);                                          ///< 添加公会申请

    void CBJoinGuildCheckFlag(DBRecorder& res, uint64 guildID, uint64 playerID);                    ///< 加入公会检查标记

    void CBDelGuildApply(DBRecorder& res, uint64 guildID);                         ///< 删除公会申请

    void CBSetGuildMemberOffice(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office);    ///< 设置公会职位

    void CBSetGuildMemberOfficeNoLock(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office);      ///< 设置公会职位无锁

    void CBAddGuildPointToContributeValue(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office);    ///< 贡献点加入玩家总贡献里

    void ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo);

    DBExecutor*         m_dbExecutor;                                                               ///< SQL执行器
};

#endif
