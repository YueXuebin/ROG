/*
 *	设置战斗单位能否被攻击
 */
#ifndef regular_action_attackable_h__
#define regular_action_attackable_h__

#include "regular_action.h"
#include "regular_data.h"

class RegularAction_Attackable : public RegularAction
{
public:
    RegularAction_Attackable(Regular* pRegular);
    virtual ~RegularAction_Attackable();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    bool    m_attackable;
    uint32  m_sid;
    int32   m_creatureGroup;
};


#endif // regular_action_attackable_h__
