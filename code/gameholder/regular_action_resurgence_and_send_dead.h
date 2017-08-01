#ifndef regular_action_resurgence_and_send_dead_h__
#define regular_action_resurgence_and_send_dead_h__

// 复活和发送死亡
#include "regular_action.h"
#include "regular_data.h"
#include "battle_unit.h"

class RegularActionResurgenceAndSendDead : public RegularAction
{
public:
    RegularActionResurgenceAndSendDead(Regular* pRegular);
    ~RegularActionResurgenceAndSendDead();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

    void DirectResurgence(BattleUnit* battleUnit);
    void Resurgence(BattleUnit* battleUnit);
private:
    int32 m_triggerID;              // 复活后所在trigger点
    int32 m_delayTime;              // 延迟复活时间
    int32 m_resurgenceCount;        // 负值无限复活
    bool  m_useGroup;               // 根据队伍ID复活到对应triggerID上
};

#endif // regular_action_resurgence_and_send_dead_h__
