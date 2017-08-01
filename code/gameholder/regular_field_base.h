#ifndef regular_field_base_h__
#define regular_field_base_h__

#include "regular_builder.h"

// 野外地图规则
class RegularFieldBase : public RegularBuilder
{
    CnDeclareRTTI
public:
    RegularFieldBase(RegularDoc* pDoc);
    virtual ~RegularFieldBase();

    virtual void OnCreatureEnter(BattleUnit* creature);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

private:
    void OnBattleEventPlayerKill(uint32 attackerID, uint32 defencerID);         // 接收角色死亡事件
    void OnPlayerResurgenceReq(int32 arg1); // 接受玩家复活事件

private:
    uint32 m_PlayerGroupCounter;

};

#endif // regular_field_base_h__
