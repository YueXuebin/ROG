#ifndef season_manager_h__
#define season_manager_h__

class Player;

/************************************************************************/
/*                           ��������                                   */
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

    uint32 m_minute;                                        // ÿ���Ӽ��һ��
};

#endif // season_manager_h__
