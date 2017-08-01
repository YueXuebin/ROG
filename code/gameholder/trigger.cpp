#include "gameholder_pch.h"
#include "trigger.h"
#include "game_define.h"
#include "game_util.h"
#include "scene.h"
#include "player.h"
#include "event_define.h"
#include "performance_analysis.h"

CnImplementRTTI(Trigger, SceneObject);

Trigger::Trigger() : SceneObject(OBJ_TRIGGER)
{
    m_CheckActiveTimer = 0;
    m_FuncID = 0;
    m_ShowID = 0;
    m_IsTouch = false;
    m_radius = 0;
    m_IsPlayerIn = false;
    m_IsEnable = true;
    m_IsTriggered = false;
}

Trigger::~Trigger()
{
}

void Trigger::onEnterScene(Scene * pScene)
{
    SceneObject::onEnterScene(pScene);
}

void Trigger::onLeaveScene(Scene * pScene)
{
    SceneObject::onLeaveScene(pScene);
}

void Trigger::Update(uint32 curr_time)
{
    PERFORM_HELPER;
    SceneObject::Update(curr_time);

    // 非触摸型检查范围内是否有玩家
    if(!m_IsTouch)
    {
        m_CheckActiveTimer += g_ServerUpdateTime;
        if(m_CheckActiveTimer >= 500)
        {
            m_CheckActiveTimer -= 500;

            CheckBattleUnitInOut();

            m_IsPlayerIn = (m_PlayerEnterList.size() > 0);
        }
    }
}

// 是否进入或离开Trigger
void Trigger::CheckBattleUnitInOut()
{
    Scene::BattleUnitMap battleUnits;
    battleUnits.clear();
    GetScene()->FindBattleUnitInCircle(GetPosition(), GetRadius(), battleUnits, GetId());

    // 遍历Enter列表中对象
    for (std::vector<uint32>::iterator battleUnitID = m_PlayerEnterList.begin(); battleUnitID != m_PlayerEnterList.end();)
    {
        Scene::BattleUnitMap::iterator battleLeave = battleUnits.find(*battleUnitID);

        // 如果对象不在trigger范围内 -> OnBattleUnitLeave
        if (battleLeave == battleUnits.end())
        {
            OnBattleUnitLeave(*battleUnitID);
            battleUnitID = m_PlayerEnterList.erase(battleUnitID);
        }
        else
        {
            battleUnitID++;
        }
    }

    for (Scene::BattleUnitMap::iterator battleUnitIt = battleUnits.begin(); battleUnitIt != battleUnits.end(); battleUnitIt++)
    {
        BattleUnit* pEnterBattleUnit = battleUnitIt->second;
        if(!pEnterBattleUnit->IsKindOf(&Player::ms_RTTI))
            continue;       // 只判断玩家

        std::vector<uint32>::iterator result = find(m_PlayerEnterList.begin(), m_PlayerEnterList.end(), battleUnitIt->second->GetId());

        // 如果不在,加入列表中 -> OnBattleUnitEnter;
        if (result == m_PlayerEnterList.end())
        {
            m_PlayerEnterList.push_back(battleUnitIt->second->GetId());
            OnBattleUnitEnter(battleUnitIt->second);
        }
    }
}

// 有单位进入Trigger
void Trigger::OnBattleUnitEnter(BattleUnit* pBattleUnit)
{
    // 单位进入事件
    GetScene()->SendEvent(0, SCENE_EVENT_TRIGGER_PLAYER_ENTER, m_triggerID, pBattleUnit->GetId(), 0);
}

// 有单位离开Trigger
void Trigger::OnBattleUnitLeave(uint32 pBattleUnitID)
{
    // 单位离开事件
    GetScene()->SendEvent(0, SCENE_EVENT_TRIGGER_PLAYER_LEAVE, m_triggerID, pBattleUnitID, 0);
}

void Trigger::SetEnable(bool enable)
{
    SetNetState(enable, m_IsTriggered);
}

void Trigger::SetTriggered(bool triggered)
{
    SetNetState(m_IsEnable, triggered);
}

void Trigger::SetNetState(bool IsEnable, bool IsTriggered)
{
    if(m_IsTriggered == IsTriggered && m_IsEnable == IsEnable)
        return;

    m_IsTriggered = IsTriggered;
    m_IsEnable = IsEnable;
    SendNetState();
}

void Trigger::SendNetState()
{
    TRIGGER_STATE_NTF sendMsg;
    sendMsg.object_id = GetId();
    sendMsg.trigger_id = GetTriggerId();
    sendMsg.enable = m_IsEnable;
    sendMsg.triggered = m_IsTriggered;
    GetScene()->NotifyMessageToAll(MSG_TRIGGER_STATE_NTF, &sendMsg);
}
