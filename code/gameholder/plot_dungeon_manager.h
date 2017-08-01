#ifndef plot_dungeon_manager_h_
#define plot_dungeon_manager_h_

class Player;
struct DungeonGainTemplate;

/*
 *  冒险地图副本管理器
 */
class PlotDungeonManager
{
public:
    PlotDungeonManager(Player* player);
    ~PlotDungeonManager(void);

    void Refresh() {}

    void FillPlotDungeonInfo(PLOT_DUNGEON_INFO& info) const;
    void LoadPlotDungeonInfo(const PLOT_DUNGEON_INFO& info);

    void OnRecv(uint32 msgID, void * data);

    void   SetDungeonFlag(uint32 dungeonID, uint32 flag);   // 设置副本的标记
    bool   GetDungeonFlag(uint32 dungeonID);                // 获取副本的标记

    uint32 GetDungeonScoreFlag(uint32 dungeonID);               // 获取副本评价标记
    void   SetDungeonScoreFlag(uint32 dungeonID, uint32 score); // 设置副本评价标记


private:



private:
    Player*             m_player;
    PLOT_DUNGEON_INFO   m_plot_dungeon_info;
};
#endif
