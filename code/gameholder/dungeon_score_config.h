/*
*	副本结算配置
*/
#ifndef dungeon_score_config_h__
#define dungeon_score_config_h__
//
//typedef struct DungeonScoreData
//{
//    void Clear()
//    {
//        id = 0;
//        time = 0;
//        monsterNum = 0;
//    }
//
//    uint32 id;            // 副本id
//    int32 time;           // 通关限制时间
//    int32 monsterNum;     // 怪物数量 
//
//} DungeonScoreData;
//
//typedef struct ScoreData
//{
//    void Clear()
//    {
//        score = 0;
//        needPoint = 0;
//        extraRewardPercent = 0;
//    }
//
//    int32 score;               // 副本id
//    int32 needPoint;           // 通关限制时间
//    int32 extraRewardPercent;  // 额外奖励百分比
//
//}ScoreData;
//
//class DungeonScoreConfig
//{
//public:
//    DungeonScoreConfig();
//    ~DungeonScoreConfig();
//
//    DECLARE_SINGLETON(DungeonScoreConfig)
//
//    bool LoadConfig(const char* path);
//
//    DungeonScoreData* GetDungeonScoreData(uint32 dungeonId);
//    uint32 GetScoreByPoint(int32 point);
//    uint32 GetExtraRewardPercentByScore(int32 score);
//
//    //找到指定副本要评价的分数
//    int32 GetSpecifyDungeonScore(uint32 dungeonid);
//private:
//    bool LoadSpecifyScore(TiXmlElement* rootEle);
//private:
//    std::map<uint32, DungeonScoreData>  m_dungeon_list;
//    std::map<uint32, ScoreData>  m_score_list;
//
//    //指定副本的的分数
//    std::map<uint32,int32> m_dungeonsorelist;
//
//};

#endif // dungeon_score_config_h__
