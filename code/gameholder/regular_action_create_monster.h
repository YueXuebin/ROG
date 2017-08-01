/*
 *	创建怪物行为
 */
#ifndef regular_action_create_monster_h__
#define regular_action_create_monster_h__

#include "regular_action.h"
#include "regular_data.h"

class RegularAction_CreateMonster : public RegularAction
{
public:
    RegularAction_CreateMonster(Regular* pRegular);
    virtual ~RegularAction_CreateMonster();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    uint32  m_MonsterNum;
    MonsterData m_MonsterData;
    uint32  m_MonsterRadius;
};


#endif // regular_action_create_monster_h__
