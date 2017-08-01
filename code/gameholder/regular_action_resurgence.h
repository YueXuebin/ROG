/*
 *	复活
 */
#ifndef regular_action_resurgence_h__
#define regular_action_resurgence_h__

#include "regular_action.h"
#include "regular_data.h"
class BattleUnit;

class RegularAction_Resurgence : public RegularAction
{
public:
    RegularAction_Resurgence(Regular* pRegular);
    virtual ~RegularAction_Resurgence();

    virtual void ParseActionXml(TiXmlElement* ActionElement);

    virtual void Do();

    void DirectResurgence(BattleUnit* battleUnit);
    void ItemResurgence(BattleUnit* battleUnit);

    void Resurgence(BattleUnit* battleUnit);
private:
    int32 m_resurgenceType;     // 复活类型 1.直接复活 2.花复活币复活
    int32 m_triggerID;          // 复活后所在trigger点
    int32 m_delayTime;          // 延迟复活时间    (毫秒)
    int32 m_resurgenceCount;    // 复活次数 (小于0 无限复活)
    bool  m_useGroup;           // 复活到队伍ID所在的triggerID上
};


#endif // regular_action_resurgence_h__
