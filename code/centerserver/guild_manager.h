#ifndef GUILD_MANAGER_H
#define GUILD_MANAGER_H

class Player;

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

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    // 加载公会信息
    void LoadGuildInfo(GUILD_INFO* guildInfo);

    // 设置公会成员在线状态
    void SetGuildMemberOnline(uint64 guildID, uint64 playerID);

    // 公会成员离线状态
    void SetGuildMemberOffline(uint64 guildID, uint64 playerID);

    // 获得公会信息
    GUILD_INFO* GetGuildInfo(uint64 guild);

    // 通知角色的公会ID
    void SendGuildID(Player* player);

    // 设置职位ID
    void CBSetMemberOffice(uint64 guildID, uint64 playerID, int32 office);

    // 设置职位ID无锁
    void CBSetMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office);

    // 添加公会总贡献点
    void AddGuildPointToContributeValue(uint64 guildID, uint64 playerID, int32 guildPoint);

    // 删除公会成员
    void DelGuildMember(uint64 guildID, uint64 playerID);

    // 添加公会成员
    void AddGuildMember(uint64 guildID, GUILD_MEMBER& guildMember);

    // 通知公会信息
    void SendGuildInfoToGate(Player* player);

    // 创建公会信息
    void CreateGuildInfo(uint64 guildID, const std::string& guildName, Player* player);

    // 发送创建公会成功的消息
    void SendCreateGuildAck(Player* player);

    // 删除公会信息
    void DelGuildInfo(uint64 guildID);

    // 检查是否为公会长
    bool IsMaster(Player* player);

    // 检查是否已存在此职位
    bool HasOffice(GUILD_INFO* guildInfo, int32 office);

    // 是否存在此会员
    bool HasGuildMember(uint64 guildID, uint64 playerID);

    // 获得公会成员信息
    GUILD_MEMBER* GetGuildMember(uint64 guildID, uint64 playerID);

    // 由职位获得公会成员
    std::vector<GUILD_MEMBER*> GetGuildMemberByOffice(uint64 guildID, uint32 office);

    // 发送添加申请加入公会通知
    void SendAddApplyNtf(uint64 guildID);

    // 设置公会标记
    void SetGuildFlag(uint64 guildID, uint32 flag);

    // 生成公会ID
    uint64 GenGuildID();

    // 得到公会名字
    std::string GetGuildName(uint64 guildID);

    // 发送给所有公会成员
    void SendToAllGuildMember(GUILD_INFO* guildInfo, uint32 msgID, void* data, uint64 exceptID = 0);

    // 发送订单
    void DeliverTradeOrder(TRADE_ORDER_INFO& tradeOrder);

    // 通知游戏服务器所有的公会信息
    void SendAllGuildInfoToGameSvr(uint32 sessionID);

    void SetMemberProp(uint64 playerID );

    // 设置公会成员职位
    void SetGuildMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office);

    // 得到公会科技加成信息
    std::vector<GUILD_ADDITION>* GetScienceAdditionList(uint64 guildID);    // 得到公会加成信息

    // 修改公会公告
    bool OnGuildBulletinUpdate(uint64 guildId, const std::string& bulletinstr);

    // 修改公会职位
    bool OnGuildOfficeUpdate(uint64 gid, uint64 pid, uint32 officeid);

    // 公会事件日志(公会id, 事件id, 相关角色id, 参数)
    void GuildLog(uint64 guildId, uint32 eventId, uint64 playerId, int32 arg1=0);

private:
    // 请求公会申请列表
    void OnGuildApplyListReq(uint64 playerID, void* data);

    // 公会列表信息请求
    void OnGuildListReq(uint64 playerID, void* data);

    // 创建公会请求
    void OnCreateGuildReq(uint64 playerID, void* data);

    // 销毁公会请求
    void OnDestroyGuildReq(uint64 playerID);

    // 销毁公会
    void OnDestoryGuildInfo(GUILD_INFO* guildInfo);

    // 邀请加入公会
    void OnInviteJoinGuildReq(uint64 playerID, void* data);

    // 邀请加入公会应答
    void OnInviteJoinGuildAns(uint64 playerID, void* data);

    // 申请加入公会
    void OnApplyJoinGuildReq(uint64 playerID, void* data);

    // 申请加入公会应答
    void OnApplyJoinGuildAns(uint64 playerID, void* data);

    // 删除公会成员请求
    void OnGuildMemberDelReq(uint64 playerID, void* data);

    // 设置公会职位请求
    void OnGuildSetOfficeReq(uint64 playerID, void* data);

    // 设置公会标记请求
    void OnGuildSetFlagReq(uint64 playerID, void* data);

    // 继承会长
    void OnGuildInheritAteventReq(uint64 playerID, void* data);

    // 公会捐献请求
    void OnGuildDonateReq(uint64 playerID, void* data);

    // 公会贡献点增加累积到总贡献点中
    void OnGuildAddGuildPointNtf(uint64 playerID, void* data);

    // 设置公会公告
    void OnGuildBulletinReq(uint64 playerID, void* data);

    // 公会BOSS升级请求
    void OnGuildBossLevelupReq(uint64 playerID, void* data);

    // 公会BOSS设置时间请求
    void OnGuildBossSetTimeReq(uint64 playerID, void* data);

    // 保存公会BOSS时间
    void OnGuildCrusadeBossTimeSave(uint64 playerID, void* data);

    // 填充公会更新信息
    void FillGuildUpdateInfo(GUILD_INFO* guildInfo, GUILD_INFO& sendMsg, uint8 opt_flag = OPT_UPDATE_FLAG);

    // 通知怪物讨伐战开启时间
    void NotifyGuildBossTime(GUILD_INFO* guildInfo);

    // 添加工会怪物经验
    void AddGuildBossExp(GUILD_INFO* guildInfo, int32 addExp);

    // 发送公会BOSS倒计时时间
    void SendGuildBossCountdown(GUILD_INFO* guildInfo);

    // 计算公会战斗力
    void CalGuildBattlePower(GUILD_INFO* guildInfo);

    // ================================================================================

    // 公会数据
    typedef std::map<uint64, GUILD_INFO>  GuildMap;    ///< [公会id, 队伍信息]
    GuildMap m_guildMap;
};

#endif
