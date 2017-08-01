#ifndef ai_manager_h__
#define ai_manager_h__

class Player;
class BattleUnit;

/*
 *	AI资源管理器
 */
class AIManager
{
public:
    AIManager();
    ~AIManager();

    DECLARE_SINGLETON(AIManager)

    bool Init();

    TiXmlDocument*  GetAIDoc(const std::string& aiName);

    void    AddDebugListener(Player* listener, BattleUnit* target);

    void    OnAIChangeState(BattleUnit* pBattle, const std::string& ai_name, uint32 oldStateId, uint32 newStateId);

private:
    typedef std::map<std::string, TiXmlDocument*>   AIDocMap;
    AIDocMap    m_AiDocMap;

    std::map<uint64, uint32>    m_DebuggerListenerMap;      // <player id, object id>

};


#endif // ai_manager_h__
