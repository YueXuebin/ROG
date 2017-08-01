#ifndef GUILD_MANAGER_H
#define GUILD_MANAGER_H

class Player;
class Boss;

// ==================================================
/// 公会管理系统
// ==================================================
class GuildManager
{
public:
    GuildManager();
    ~GuildManager();

    DECLARE_SINGLETON(GuildManager)

    void Clear();

    void OnRecv(Player* player, uint32 msgID, void* data);

    // 根据工会ID获得工会信息
    GUILD_INFO* GetGuildInfo(uint64 guild);

    // 根据工会名称获得工会信息
    GUILD_INFO* GetGuildInfo(const std::string& guildName);

    // 获得工会名
    std::string GetGuildName(uint64 guildID);

    // 公会升级捐献请求
    void OnGuildDonateReq(Player* player, void* data);

    // 回调魔石捐献
    void CBGuildDonateTrade(Player* player, uint32 needMoneyPoint);

    // 更新公会的BOSS创建时间
    void UpdateGuildBossCreateTime(uint64 guildID);

    // 更新工会的BOSS死亡时间
    void UpdateGuildBossDeadTime(uint64 guildID);

    // 判断角色是否能进入魔物讨伐战
    uint16 CanEnterCrusadeMap(Player* player);

    // 判断是否在工会战活动时间
    uint16 CanEnterCrusadeBoss(GUILD_INFO* guildInfo);

    // 判断角色是否能进入暮光之战地图
    uint16 CanEnterTwilightWarMap(Player* player);

    // 判断是否需要更新工会BOSS时间
    bool NeedUpdateCrusadeBossTime(GUILD_INFO* guildInfo);

    // 判断进入暮光之战的时间
    //uint16 CanEnterTwilightWar();

    // 获得最近一次进入暮光之城的ID
    //uint32 GetEnterTwilightWarID();

    // 发送公会BOSS时间
    void SendRefreshBossTime(uint64 guildID, int32 flag);

    // 根据公会信息，强化公会boss
    void StrongGuildBoss(Boss* boss, int32 strongLevel);

    // 广播暮光之战ID
    //void BroadcastTwilightOpenID();

    // 请求个人的公会福利信息
    void OnGuildWelfareInfoReq(Player* player, void* data);

    // 公会继续会长请求
    void OnGuildInheritAteventReq(Player* player, void* data);

    // 公会随机龙盘的结果
    void OnRandomTurntableResult(Player* player, void* data);

    // 公会个人随机龙盘的结果
    void OnMemberTurntableResult(Player* player, void* data);

    void ExitGame(Player* player);

    // 公会转盘的返回
    void OnGuilddMemberTurntableAck(Player* player, int32 id);

    //刷新每天的公会购买次数
    void OnFreshDayGuildBuyTimes(Player* player);

    // 清空已经关闭的中心服务器的数据
    void OnCloseCenterServer(uint32 centerID);

    // 得到公会加成信息
    GUILD_ADDITION* GetGuildAdditionItem(uint64 guildID, int32 additionID);
private:
    // 公会信息请求
    void OnGuildInfoReq(Player* player);

    // 公会信息通知
    void OnGuildInfoNtf(void* data);

    // 公会申请列表请求
    void OnGuildApplyListReq(Player* player, void* data);

    // 公会列表查询请求
    void OnGuildListReq(Player* player, void* data);

    // 创建公会请求
    void OnCreateGuildReq(Player* player, void* data);

    // 创建公会后扣费
    void OnCreateGuildAck(Player* player, void* data);

    // 销毁队伍请求
    void OnDestroyGuildReq(Player* player);

    // 公会信息请求
    void OnViewGuildInfoReq(Player* player, void* data);

    // 队伍进入副本请求
    void OnTeamEnterDungeonReq(Player* player);

    // 雇佣离线玩家
    void onTeamAddOfflineMemberAddReq(Player* player, void* data);

    // 邀请加入公会请求
    void OnInviteJoinGuildReq(Player* player, void* data);

    // 邀请加入公会应答
    void OnInviteJoinGuildAns(Player* player, void* data);

    // 申请加入公会请求
    void OnApplyJoinGuildReq(Player* player, void* data);

    // 申请加入公会应答
    void OnApplyJoinGuildAns(Player* player, void* data);

    // 请求删除队伍成员
    void OnGuildMemberDelReq(Player* player, void* data);

    // 设置公会职位请求
    void OnGuildSetOfficeReq(Player* player, void* data);

    // 设置公会标记请求
    void OnGuildSetFlagReq(Player* player, void* data);

    // 设置队伍ID
    void OnGuildIDNtf(Player* player, void* data);

    // 公会升级捐献应答
    void OnGuildDonateAck(Player* player, void* data);

    // 设置公会公告板
    void OnGuildSetBulletinReq(Player* player, void* data);

    // 公会购买请求
    void OnGuildBuyReq(Player* player, void* data);

    // 公会购买贡献点
    //void OnGuildBuyGuildPointReq(Player* player, void* data);

    // 公会购买个人轮盘重置
    void OnGuildBuyMemberTurnTableReq(Player* player, void* data);

    // 公会开始转随机龙盘
    void OnGuildRandTurntable(Player* player, void* data);

    // 公会个人龙盘开始转
    void OnGuildMemberTurntable(Player* player, void* data);

    // 公会奖励金币
    void OnGuildRewardPersonalWelfare(Player* player, void* data);

    // 公会BOSS升级请求
    void OnGuildBossLevelupReq(Player* player, void* data);

    // 公会BOSS倒计时请求
    void OnGuildBossCountdownReq(Player* player, void* data);

    // 公会BOSS设置时间请求
    void OnGuildBossSetTimeReq(Player* player, void* data);

    // 请求公会暮光之城排行信息
    void OnGuildCrusadeBossRankReq(Player* player, void* data);

    // 请求暮光之战开启ID
    //void OnGuildTwilightWarOpenIDReq(Player* player, void* data);

    // 请求公会暮光之城排行信息
    void OnGuildTwilightWarRankReq(Player* player, void* data);

    // 重置队员状态
    //void OnTeamMemberResetState(Player* player, void* data);

    // 更新队伍信息
    void OnRefreshGuildInfo(GUILD_INFO* guildInfo);

    // 销毁公会
    void OnDestoryGuildInfo(GUILD_INFO* guildInfo);

    // 添加公会信息
    void OnAddGuildInfo(GUILD_INFO* guildInfo);

    // 删除公会信息
    void OnDelGuildInfo(GUILD_INFO* guildInfo);

    // 更新公会信息
    void OnUpdateGuildInfo(GUILD_INFO* guildInfo);

    // 填充公会更新信息
    void FillGuildUpdateInfo(GUILD_INFO* guildInfo, GUILD_INFO& sendMsg, uint8 opt_flag = OPT_UPDATE_FLAG);

    // 更新公会属性信息
    //void OnUpdateGuildProp(GUILD_INFO* guildInfo);

    int32 GetGuildTrunTablePos() {return m_guild_tabletrun_pos;}
    void SetGuildTrunTablePos(int32 val) { m_guild_tabletrun_pos = val; }                       ///< 设置公会转盘转盘的位置;

    int32 GetGuildMemberTrunTablePos() {return m_guild_member_trun_pos;}
    void SetGuildMemberTrunTablePos(int32 val) { m_guild_member_trun_pos = val; }               ///< 设置个人转盘转盘的位置;

    // 公会数据
    typedef std::map<uint64, GUILD_INFO> GuildMap;  // <队伍id, 队伍信息>
    GuildMap m_guildMap;

    int32 m_guild_member_trun_pos;
    int32 m_guild_tabletrun_pos;
};

#endif
