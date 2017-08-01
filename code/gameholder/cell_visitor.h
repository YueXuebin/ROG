#ifndef cell_visitor_h__
#define cell_visitor_h__


class AOIObject;
class Player;
/*
 *	剖分格的可视数据对象
 */
class CellVisitor
{
public:
    // Player
    void AddSeePlayer(Player* player);
    void RemoveSeePlayer(Player* player);

    bool HasSeePlayer(Player* player);
    
    std::list<Player*>& GetSeePlayers() {return m_SeePlayers;}

    // AOIObject
    void AddAOIObject(AOIObject* obj);
    void RemoveAOIObject(AOIObject* obj);
    bool HasAOIObject(AOIObject* obj);

    std::list<AOIObject*>& GetAOIObjects() {return m_AOIObject;}

private:
    std::list<Player*>      m_SeePlayers;       // 能够看到该Cell的玩家
    std::list<AOIObject*>   m_AOIObject;        // 在该Cell中的对象
};

#endif // cell_visitor_h__
