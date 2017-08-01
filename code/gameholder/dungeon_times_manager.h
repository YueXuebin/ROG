#ifndef dungeon_times_manager_h__
#define dungeon_times_manager_h__

// 副本进入次数

class Player;

class DungeonTimesRecorder
{
public:
    DungeonTimesRecorder(Player* player);
    ~DungeonTimesRecorder();

    void LoadDungeonTimesInfo(const DUNGEON_TIMES_LIST& dungeonInfo);
    void FillDungeonTimesInfo(DUNGEON_TIMES_LIST& dungeonInfo);

    void CalDailyRefresh();                     // 每天刷新

    bool HasEnterCount(uint32 dungeonID);       // 是否有进入次数

    void AddEnterCount(uint32 dungeonID);       // 添加副本使用次数

    bool ReduceEnterCount(uint32 dungeonID, int32 num);       // 减少副本使用次数

private:
    bool    IsRecordTimes(int32 dungeonID);     // 是否记录进入副本次数
    DUNGEON_TIMES_INFO* GetDungeonTimesInfo(uint32 dungeonId);

    void    SaveYesterData();
    void    SaveOneYesterdayData(uint32 opensysID);
    uint32  GetOpenSysIDByDungeonID(uint32 dungeonID);
    uint32  GetDungeonIDByOpenSysID(uint32 OpensysID);

    void    SendTimes(uint32 dungeonID, int32 enterTimes, int32 maxTimes);        // 通知客户端次数变更

    Player* m_player;

    DUNGEON_TIMES_LIST m_dungeonTimesList;
};

#endif // dungeon_times_manager_h__
