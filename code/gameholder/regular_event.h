/*
 *	关卡事件基类
 */
#ifndef regular_event_h__
#define regular_event_h__

#include "rtti.h"

class Regular;
class Scene;
class RegularAction;
class RegularEvent;
class Player;

// 解析关卡事件
RegularEvent* ParseRegularEvent(TiXmlElement* EventElement, Regular* pRegular);

class RegularEvent
{
    CnDeclareRootRTTI(RegularEvent)

    friend RegularEvent* ParseRegularEvent(TiXmlElement* EventElement, Regular* pRegular);
public:
    RegularEvent(Regular* pRegular, int32 type);
    virtual ~RegularEvent();

    int32   GetType() { return m_type; }
    void    SetType(int32 val) { m_type = val; }

    bool    GetActive() {return m_active;}
    void    SetActive(bool active);

    virtual bool GetActiveForPlayer(Player* player) {return m_active;}       // 事件针对某玩家是否激活

    virtual void ParseEventXml(TiXmlElement* EventElement) {}
    virtual void OneSecondUpdate(int32 deltatime) {}
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3) {}

protected:
    Scene*      GetScene();

protected:
    Regular*    m_pRegular;

    typedef std::vector<RegularAction*>  RegularActionList;     // 关卡行为列表
    RegularActionList   m_ActionList;

    bool    m_active;       // 事件激活标记

private:
    int32   m_type;         // 事件类型
    
};

#endif // regular_event_h__
