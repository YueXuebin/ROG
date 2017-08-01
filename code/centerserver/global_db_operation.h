#ifndef global_db_operation_h__
#define global_db_operation_h__

typedef struct Chat_Info
{
    uint32 server_id;           // 服务器id
    uint64 player_id;           // 玩家id
    std::string player_name;    // 玩家名
    int32  chat_type;           // 聊天类型
    std::string context;        // 聊天内容
    std::string plaform_name;   // 平台名字

    void Clear()
    {
        server_id =0;
        player_id = 0;
        player_name ="";
        chat_type = 0;
        context = "";
        plaform_name = "";
    }
}Chat_Info;

class Player;
class DBExecutor;
class DBRecorder;
class GlobalDbOperation
{
public:
    GlobalDbOperation(void){m_dbGlobalExecutor= NULL;};
    ~GlobalDbOperation(void);

    DECLARE_SINGLETON(GlobalDbOperation)

    void SetDBExecutor(DBExecutor* gval) {m_dbGlobalExecutor = gval;}

    void SaveChatInfo(const Chat_Info& chatinfo);           // 保存聊天信息

    void SaveRankingAlliance(Player* pPlayer, int32 winPro);                   // 保存跨服联盟信息

    void GetRankingAlliances(uint64 playerID, std::vector<int32>& rankList);                             // 获得跨服排行信息
    void CBLoadPlayerListByAlliancesRank(DBRecorder& res, uint64 playerID);

    int32 GetSeasonRankByPlayerID(uint64 playerID);

    bool GetOldRankListInfo(std::string platform_name, int32 region_id, std::map<int32, std::string>& rankMap);

    void SendMultiSrvInfoAboutRank(uint64 playerID);
    void CBLoadMultiSrvInfoAboutRank(DBRecorder& res, uint64 playerID);

    bool ClearSeasonIntegral();

    bool SaveSeasonInfo(const SEASON_INFO& info);
    bool LoadSeasonInfo(SEASON_INFO& info);



    bool UpdateRankingAllianceRank();           //更新rank排名

    void Update();

    void RefreshRank();

private:
    DBExecutor*         m_dbGlobalExecutor;

    uint32  m_minute;                                        // 每分钟检测一次

};

#endif
