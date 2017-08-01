/*
 *	创建Boss行为
 */
#ifndef regular_action_clear_all_monsters_h__
#define regular_action_clear_all_monsters_h__

#include "regular_action.h"
#include "regular_data.h"

class RegularAction_ClearAllMosters : public RegularAction
{
public:
    RegularAction_ClearAllMosters(Regular* pRegular);
    virtual ~RegularAction_ClearAllMosters();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    uint32  m_spawn;
};


#endif // regular_action_clear_all_monsters_h__
