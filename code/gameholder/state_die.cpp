#include "gameholder_pch.h"
#include "state_die.h"
#include "state_define.h"
#include "battle_unit.h"
#include "player.h"
#include "scene.h"
#include "soldier.h"


StateDie::StateDie(BattleUnit* owner) :
State(STATE_DIE, owner)
{
    m_DeadTime = 1500;      // 死亡时间
    m_isEnd = false;
}

StateDie::~StateDie()
{

}

bool StateDie::CanChangeTo( uint32 state )
{
    return false;
}

void StateDie::onEnterState()
{
    State::onEnterState();

    m_isEnd = false;
}

void StateDie::onLeaveState()
{
    State::onLeaveState();

    // 复活通知
    RELIVE_NTF ntf;
    ntf.object_id = m_Owner->GetId();
    m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_RELIVE_NTF, &ntf);
}

void StateDie::Update()
{
    State::Update();

    if(m_UpdateTimer >= m_DeadTime)
    {
        if(m_Owner->GetObjectType() != OBJ_PLAYER)
        {
            if (!m_Owner->IsKindOf(&Soldier::ms_RTTI))
            {
                m_Owner->SetDestory();  // 非玩家对象销毁
            }
        }
    }
}

bool StateDie::isEnd()
{
    return m_isEnd;
}
