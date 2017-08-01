#ifndef daily_dungeon_manager_h__
#define daily_dungeon_manager_h__

class Player;

/*
 *	日常地图管理器
 */
class DailyDungeonManager
{
public:
    DailyDungeonManager(Player* player);
    ~DailyDungeonManager(void);

    void Refresh() {}

    void FillDailyDungeonInfo(DAILY_DUNGEON_INFO& info) const;
    void LoadDailyDungeonInfo(const DAILY_DUNGEON_INFO& info);

private:
    Player*             m_player;
    DAILY_DUNGEON_INFO  m_daily_dungeon_info;

};


#endif // daily_dungeon_manager_h__
