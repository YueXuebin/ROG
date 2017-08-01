/*
 *	通知玩家死亡
 */
#ifndef regular_action_sendDead_h__
#define regular_action_sendDead_h__



#include "regular_action.h"
#include "regular_data.h"

class RegularAction_SendDead : public RegularAction
{
public:
    RegularAction_SendDead(Regular* pRegular);
    virtual ~RegularAction_SendDead();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

private:
    int32   m_CountDown;
    int32   m_DeadAction;
    int32   m_resurgenceCount;      // 负值无限复活
};


#endif // regular_action_sendDead_h__
