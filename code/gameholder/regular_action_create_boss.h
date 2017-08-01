/*
 *	创建Boss行为
 */
#ifndef regular_action_create_boss_h__
#define regular_action_create_boss_h__

#include "regular_action.h"
#include "regular_data.h"

class RegularAction_CreateBoss : public RegularAction
{
public:
    RegularAction_CreateBoss(Regular* pRegular);
    virtual ~RegularAction_CreateBoss();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    uint32      m_BossCount;
    BossData    m_BossData;
    uint32      m_BossRadius;
};


#endif // regular_action_create_boss_h__
