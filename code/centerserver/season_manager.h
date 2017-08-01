#ifndef season_manager_h__
#define season_manager_h__

class Player;

/************************************************************************/
/*                           赛季管理                                   */
/************************************************************************/
class SeasonManager
{
public:
    SeasonManager();
    ~SeasonManager();

    DECLARE_SINGLETON(SeasonManager)

    void Init();
    void SaveDB();

    void Update();

    void SendInfo(uint32 sessionID);

    void RefreshRank();
private:
    void SendNewSeasonNtf();
    void SendReward();

private:
    SEASON_INFO m_season_info;

    uint32 m_minute;                                        // 每分钟检测一次
};

#endif // season_manager_h__
