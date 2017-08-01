#ifndef open_system_manager_h__
#define open_system_manager_h__

class Player;

/*
 *	功能开启
 */
class OpenSystemManager
{
public:
    OpenSystemManager(Player* player);
    ~OpenSystemManager();

    void LoadOpenSystemInfo(const OPEN_SYSTEM_INFO& info);
    void FillOpenSystemInfo(OPEN_SYSTEM_INFO& info) const;

    void SetOpenSystemInfo(const std::vector<uint32>& info);

private:
    Player*     m_player;

    OPEN_SYSTEM_INFO  m_open_system_info;
};

#endif // open_system_manager_h__
