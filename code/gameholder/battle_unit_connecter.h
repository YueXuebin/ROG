/*
 *	世界Boss管理器
 */
#ifndef battle_unit_connecter_h__
#define battle_unit_connecter_h__

class BattleUnitCommunicator;
class BattleUnit;
class DungeonSet;

class BattleUnitConnecter
{
public:
    BattleUnitConnecter(DungeonSet* dungeonSet);
    ~BattleUnitConnecter();

    void Update();

    void ConnectBattleUnit(uint32 dungeonID, const std::string& name, BattleUnit* pBattleUnit);
    void DisconnectBattleUnit(std::string name, BattleUnit* pBattleUnit);
    void EndConnecter(bool success, std::string* name = NULL);

private:
    typedef std::map<std::string, BattleUnitCommunicator*>  BattleUnitCommunicatorMap;
    BattleUnitCommunicatorMap   m_battleUnitDataMap;
    DungeonSet*                 m_dungeonSet;
};


#endif // battle_unit_connecter_h__
