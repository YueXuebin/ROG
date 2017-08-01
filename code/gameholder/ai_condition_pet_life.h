#ifndef ai_condition_pet_life_h__
#define ai_condition_pet_life_h__

#include "ai_condition_base.h"

/*
 *	AI条件：宠物是否活着
 */
class AIConditionPetLife : public AIConditionBase
{
public:
    AIConditionPetLife();
    virtual ~AIConditionPetLife();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void onEnterState();
    virtual bool IsSatisfy();

    virtual void Update(int32 deltaTime);

private:
    bool m_petLife;
    bool m_alive;
};

#endif // ai_condition_pet_life_h__
