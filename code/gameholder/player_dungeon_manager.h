#ifndef player_dungeon_manager_h__
#define player_dungeon_manager_h__

class Player;
class PlotDungeonManager;
class SoulDungeonManager;
class DailyDungeonManager;

/*
 *  玩家副本信息管理
 */
class PlayerDungeonManager
{
public:
    PlayerDungeonManager(Player* player);
    ~PlayerDungeonManager();

    void OnRecv(uint32 msgID, void* data);                      // 接收消息

    void DungeonTimesRefresh();                                 // 刷新副本次数

    void FillDungeonConditionNtf(DUNGEON_CONDITION_NTF& ntf) const;     // 填充副本通关条件

    void FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& info) const;        // 填充角色副本信息
    void LoadPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info);        // 设置角色副本信息

    bool IsDungeonOpen(uint32 dungeonID);                       // 副本是否开启
    void OpenDungeon(uint32 dungeonID);                         // 开启副本
    void CloseDungeon(uint32 dungeonID);                        // 关闭副本
    void OpenNextDungeon(uint32 dungeonID);                     // 开启当前副本的下一个副本

    void OnDungeonClearRecord(uint32 dungeonID, int32 score);                // 副本通关

    SoulDungeonManager* GetSoulDungeonManager() { return m_soulDungeonMgr; } // 返回精魂副本管理器

    uint16 CanChangeToDungeon(Player* player, uint32 dungeonID, uint32 sceneID = 0);  // 是否能够进入副本

    uint32 GetPlotDungonScore();                                // 返回通过剧情副本的总评分


private:
    void OnMove(void* data);                                    // 处理移动请求
    void OnJump(void* data);                                    // 处理跳跃请求
    void OnJumpDungeonReq(void* data);                          // 处理跳地图请求
    void OnChangeDungeonReq(void* data);                        // 处理切换副本
    void OnDungeonScoreInfoReq(void* data);                     // 处理请求副本分数信息
    void OnChangeScene(void* data);                             // 处理切换场景
    void OnGameStart(void* data);                               // 处理游戏开始

    void OnSceneInfoReq(void* data);                            // 请求频道信息

    void ReadyChangeScene();                                    // 切换场景前处理

    void OnPickUpDropItem(void* data);                              // 拾取掉落物品

    void OnDungeonRunTimeInfoReq(void* data);                       // 处理请求副本运行时间信息

    void OnDungeonConditionReq(void* data);                     // 请求副本通关条件

    void CheckFieldDungeonOpen();
    void CheckSoulDungeonOpen();
    void CheckDailyDungeonOpen();

private:
    Player* m_player;

    OPEN_DUNGEON_INFO	    m_OpenDungeonInfo;                      // 开启的副本ID列表 
    DUNGEON_RECORD          m_DungeonRecord;                        // 副本通关评价

    PlotDungeonManager*     m_plotDungeonMgr;                       // 冒险地图副本管理器
    SoulDungeonManager*     m_soulDungeonMgr;                       // 异界地图副本管理器
    DailyDungeonManager*     m_dailyDungeonMgr;                     // 日常地图副本管理器
};
#endif // player_dungeon_manager_h__
