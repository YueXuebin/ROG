/*
 *  触发器
 */
#ifndef trigger_h__
#define trigger_h__

#include "sceneobject.h"
#include "battle_unit.h"

class TriggerChecker;

class Trigger : public SceneObject
{
    CnDeclareRTTI
public:
    Trigger();
    virtual ~Trigger();

    virtual void onEnterScene(Scene* pScene);
    virtual void onLeaveScene(Scene* pScene);

    virtual void Update(uint32 curr_time);
    virtual void OnDestory() {}

    uint32 GetTriggerId() { return m_triggerID; }
    void SetTriggerId(uint32 id) { m_triggerID = id; }

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string &name) { m_name = name; }

    const std::string& GetObsName() const {return m_ObsName;}
    void SetObsName(const std::string& obs_name) {m_ObsName = obs_name;}

    int32 GetRadius() { return m_radius; }
    void SetRadius(int32 radius) { m_radius = radius; }

    bool IsPlayerIn() { return m_IsPlayerIn; }

    bool IsEnable() { return m_IsEnable; }
    void SetEnable(bool enable);                        // trigger的状态改变，通知客户端

    bool IsTriggered() {return m_IsTriggered;}
    void SetTriggered(bool triggered);

    void SetFuncID(uint32 funcID) {m_FuncID = funcID;}
    uint32 GetFuncID() {return m_FuncID;}

    void SetFuncType(uint32 funcType) {m_FuncType = funcType;}
    uint32 GetFuncType() {return m_FuncType;}

    void SetShowID(uint32 showID) {m_ShowID = showID;}
    uint32 GetShowID() {return m_ShowID;}

    void SetTouch(bool isTouch) {m_IsTouch = isTouch;}
    bool GetTouch() {return m_IsTouch;}

    VECTOR3D_INT m_ObsPos;              // 物件集位置
    VECTOR3D_INT m_PickSize;            // 拾取范围 
    VECTOR3D_INT m_PickPos;             // 拾取范围位置

    void SetNetState(bool IsEnable, bool IsTriggered);

private:
    void CheckBattleUnitInOut();

    virtual void    OnBattleUnitEnter(BattleUnit* pBattleUnit);
    virtual void    OnBattleUnitLeave(uint32 pBattleUnitID);

    void SendNetState();

private:
    uint32 m_triggerID;
    int32 m_radius;                 // 半径
    uint32  m_FuncID;               // 功能ID
    uint32  m_FuncType;             // 功能类型(cc/cs)
    uint32  m_ShowID;               // 外形ID
    bool    m_IsTouch;              // 是否触摸触发

    std::string m_name;                 // trigger的名字
    std::string m_ObsName;              // 物件集名
 

    bool m_IsPlayerIn;                  // 是否有玩家在半径内(有玩家在半径内，就算激活)

    bool m_IsEnable;                    // 是否可用
    bool m_IsTriggered;                 // 是否已经触发



    int32   m_CheckActiveTimer;                 // Trigger进出检查计时器
    std::vector<uint32>  m_PlayerEnterList;     // 所有进入Trigger上的玩家ObjectID
    
};

#endif // trigger_h__