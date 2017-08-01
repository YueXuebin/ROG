#ifndef team_relationinfo_h__
#define team_relationinfo_h__


class Player;

// 组队关系
class TeamRelationInfo
{
public:
    TeamRelationInfo();
    ~TeamRelationInfo();

    bool HasPlayer(Player* player);
    void AddPlayer(Player* player);
    void RemovePlayer(Player* player);

    const std::vector<Player*>& GetPlayers() {return m_players;}

private:
    std::vector<Player*>        m_players;
};



#endif // team_relationinfo_h__
