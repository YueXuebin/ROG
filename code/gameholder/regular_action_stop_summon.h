/*
 *	创建停止刷怪行为
 */
#ifndef regular_action_stop_summon_h__
#define regular_action_stop_summon_h__

#include "regular_action.h"
#include "regular_data.h"

class RegularAction_StopSummon : public RegularAction
{
public:
    RegularAction_StopSummon(Regular* pRegular);
    virtual ~RegularAction_StopSummon();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    uint32  m_spawn;
    bool   m_stop;
};


#endif // regular_action_stop_summon_h__
