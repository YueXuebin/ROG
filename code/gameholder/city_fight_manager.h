/*
    城市争夺战管理器
 */
#ifndef city_fight_manager_h__
#define city_fight_manager_h__

class Player;

// 公会伤害信息
typedef struct GuildScoreRecorder
{
    uint64      guildID;
    std::string guildName;
    uint32      score;

    void Clear()
    {
        guildID     = 0;
        guildName   = "";
        score       = 0;
    }
}GuildScoreRecorder;

// 城市的伤害信息
typedef struct CityScoreRecorder
{
    uint32 cityID;
    std::map<uint64, GuildScoreRecorder>    scoreMap;
    std::list<GuildScoreRecorder*>          scoreList;

    void Clear()
    {
        cityID = 0;
        scoreMap.clear();
        scoreList.clear();
    }
}CityScoreRecorder;

// 城市占领信息
typedef struct CityOccupyInfo
{
    uint32          cityID;     // 城市ID
    int32           state;      // 占领状态
    std::string     guildName;  // 公会名
    uint32          timeStamp;     // 城市ID

    void Clear()
    {
        cityID = 0;
        state = 0;
        guildName = "";
        timeStamp = 0;
    }
}CityOccupyInfo;

typedef std::map<uint32, CityScoreRecorder> CityScoreRecorderMap;
typedef std::map<uint32, CityOccupyInfo> CityOccupyInfoMap;

class CityFightManager
{
public:
    CityFightManager();
    ~CityFightManager();

    DECLARE_SINGLETON(CityFightManager)

    void Clear();

    void Update();

    void OnRecv(uint32 msgID, void* data, Player* player=0);

    void OnCSRecv(uint64 playerID, uint32 msgID, void* data);

    // 添加伤害
    void RecordGuildScore(uint32 cityID, uint64 guildID, uint32 score);

    // 获得城市保卫战排行信息
    void GetCityFightRankList(uint32 cityID, GUILD_DAMAGE_RANK_LIST& list);

    // 获得城市保卫战占领信息
    std::string GetCityOccupyGuild(uint32 cityID);

    // 获得城市保卫战占领时间
    uint32 GetCityOccupyTimeStamp(uint32 cityID);

    // 获得公会在该城市的得分
    uint32 GetGuildScore(uint32 cityID, uint64 guildID);

    // 毫秒
    uint32 GetCurrTime() {return m_cur_time;}

    // 服务器
    void OnServerReady();

    // 玩家进入城市
    void PlayerEnterCity(Player* player, uint32 cityId);

    // 玩家离开城市
    void PlayerLeaveCity(Player* player, uint32 cityId);

private:
    // 城市争夺战开启
    void OnCityFightStart(void* data);

    // 城市争夺战结束
    void OnCityFightEnd(void* data);

    // 城市信息请求
    void OnCityFightInfoReq(void* data, Player* player);

    // 城市争夺战进入请求
    void OnCityFightEnterReq(void* data, Player* player);
    // 发送城市争夺战进入回应
    void SendCityFightEnterAck(Player* player, uint16 errCode, uint32 cityId);

    // 城市争夺战奖励请求
    void OnCityFightRewardReq(void* data, Player* player);
    // 发送城市争夺战进入回应
    void SendCityFightRewardAck(Player* player, uint16 errCode, uint32 cityId);

    // 返回城市状态
    int32 GetCityState(uint32 cityId);

    // 设置城市状态
    void SetCityState(uint32 cityId, int32 state, std::string guildName, uint32 timeStamp);

    // 重置城市状态
    void ResetCityState();

    // 检查公会条件
    uint16 CheckGuildCondition(Player* player);

    // 检查公会奖励条件
    uint16 CheckRewardCondition(Player* player, uint32 cityId);

    // 关闭城市
    void CloseCity(uint32 cityId);

    // 发送城市争夺战信息广播
    void SendCityFightInfoNtf();
    // 城市信息广播
    void SendCityFightInfoNtfReq();
    // 接收城市争夺战信息广播
    void OnCityFightInfoNtfAck(void* data);

    // 是否是活动所在服务器
    bool IsActivityServer();

    // 分数比较函数
    static bool ScoreCompareRules(GuildScoreRecorder* left, GuildScoreRecorder* right) {return (left->score > right->score);}

    // 发送全服公告
    void SendMarquee(std::string guildName, uint32 cityId);

    // 发送公会邮件
    void SendMailToGuild(std::string guildName, uint32 cityId);

    // 增加时间分数
    void AddTimeScore();

    // 发送上飘字提示到城市
    void SendMessageNotifyToCity(uint32 cityId, MESSAGE_NTF& message);

    // 伤害记录
    CityScoreRecorderMap m_score_recorder;   // [城市ID, 城市中各公会的战斗信息]

    // 每个城市的状态
    CityOccupyInfoMap m_city_occupy_info;    // [城市ID, 城市状态]

    // 当前时间
    uint32 m_cur_time;

    // 城市信息变化广播时间戳
    uint32 m_ntf_time;

    // 时间分数时间戳
    uint32 m_time_score_time;

    // 城市信息变化标记
    bool m_city_info_modify;

    // 城市信息是否需要存档标记
    bool m_city_info_need_save;

    // 是否开启中
    bool m_open;

    // 是否活动所在服务器
    bool m_activity_server;
};


#endif
// city_fight_manager_h__
