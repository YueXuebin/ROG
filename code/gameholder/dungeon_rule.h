#ifndef dungeon_rule_h__
#define dungeon_rule_h__


class Dungeon;
class Player;

/*
 *	副本规则逻辑
 */
class DungeonRule
{
public:
    typedef struct _PlayerRecordData
    {
        uint64 playerID;
        std::string playerName;

        _PlayerRecordData() {playerID = 0;}
    } PlayerRecordData;

    DungeonRule(Dungeon* pDungeon);
    ~DungeonRule();

    void OneSecondUpdate();

    void DungeonClear(bool success, std::vector<std::string>* list);        // 副本完成结算
    
    bool IsClear() {return m_IsClear;}

    uint64  GetMasterPlayerID() {return m_FirstEnterPlayer.playerID;}

    void    OnPlayerEnter(Player* player);
    void    OnPlayerLeave(Player* player);
private:
    Dungeon*    m_pDungeon;

    bool        m_IsClear;      // 是否结算过

    PlayerRecordData    m_FirstEnterPlayer;     // 第一个进入副本的玩家信息
    std::vector<PlayerRecordData>     m_EnterPlayerList;
    std::vector<PlayerRecordData>     m_LeavePlayerList;

};



#endif // dungeon_rule_h__
