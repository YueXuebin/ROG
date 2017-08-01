#ifndef rog_regular_event_h__
#define rog_regular_event_h__

#include "rtti.h"
#include "regular_event.h"

class Player;
class RegularDungeon;

class ROGRegularEvent : public RegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent(RegularDungeon* pRegular, int32 type);
    virtual ~ROGRegularEvent();

    RegularDungeon* GetRegular();

    int32 GetParam1() { return m_param1; }
    int32 GetParam2() { return m_param2; }
    int32 GetParam3() { return m_param3; }

    void SetParam1(int32 val) { m_param1 = val; }
    void SetParam2(int32 val) { m_param2 = val; }
    void SetParam3(int32 val) { m_param3 = val; }

    void SetParams(int32 param1, int32 param2, int32 param3)
    {
        m_param1 = param1;
        m_param2 = param2;
        m_param3 = param3;
    }

    DUNGEON_CONDITION GetCondition(Player* player);

protected:
    
    int32 m_param1;   // 通用参数
    int32 m_param2;   // 通用参数
    int32 m_param3;   // 通用参数
};

#endif // rog_regular_event_h__