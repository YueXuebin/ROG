/*
 *	AI工厂
 */
#ifndef ai_factory_h__
#define ai_factory_h__

class AIController;
class AIState;
class AIConditionBase;
class BattleUnit;
class AIContainer;

class AIFactory
{
public:
    AIFactory();
    ~AIFactory();

    void CreateDefaultAI(AIController* controller);

    bool LoadAIStates(const std::string& aiName, AIContainer* container);

private:
    void ParseState(AIState* pAIState, TiXmlElement* stateXml);
    void ParsetCondition(AIConditionBase* pCondition, TiXmlElement* conditionXml);


};

#endif // ai_factory_h__
