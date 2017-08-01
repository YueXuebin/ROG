/*
*	服务器角色对象，基类
*/
#ifndef creature_h__
#define creature_h__

#include "battle_unit.h"


class Creature : public BattleUnit
{
    CnDeclareRTTI
public:
    Creature(uint32 objType);
    virtual ~Creature();

    void CreatureClear()
    {
        BattleUnitClear();

        m_creatureId = 0;                           // 角色ID
    }

    uint32  GetCreatureId() const { return m_creatureId; }
    void    SetCreatureId(uint32 cid) { m_creatureId = cid; }

    virtual void    onEnterScene(Scene* pScene);
    virtual void    onLeaveScene(Scene* pScene);

    virtual void    Update(uint32 frame_count);

    void    SetLogicName(const std::string& name) { m_LogicName = name; }
    const   std::string& GetLogicName() {return m_LogicName;}

    void    Bounce(int32 height, int32 time, bool exclude_me = false);         // 弹跳

public:
    virtual void    NotifyEquipInfo(UPDATE_EQUIP_INFO_NTF& info) {}             // 通知装备信息
    virtual bool    SendToGate(uint32 msgID, void* data) {return false;}
    virtual int32   GetEquipAnimation() const {return EQUIP_ANIMATION_JD;}

protected:
    uint32 m_creatureId;                                    // 角色ID

    std::string m_LogicName;                                // 生物的逻辑名
};

#endif // creature_h__
