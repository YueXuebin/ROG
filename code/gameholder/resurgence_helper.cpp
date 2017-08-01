#include "gameholder_pch.h"
#include "resurgence_helper.h"
#include "battle_unit.h"
#include "trigger.h"
#include "scene.h"
#include "action_controller.h"
#include "state_controller.h"
#include "regular.h"
#include "game_define.h"
#include "prop_notifier.h"
#include "soldier.h"

ResurgenceHelper::ResurgenceHelper(BattleUnit* owner) :
m_Owner(owner)
{
    m_resurgence = false;
    m_resurgenceTime = 0;
    m_resurgenceTriggerID = 0;
    m_ResurgenceFullHp = false;
}

ResurgenceHelper::~ResurgenceHelper()
{

}

void ResurgenceHelper::EventResurgence(int32 triggerID, int32 delayTime)
{
    if(m_Owner->IsDead())
    {
        m_resurgence = true;
        m_resurgenceTime = delayTime;
        m_resurgenceTriggerID = triggerID;
    }
}

void ResurgenceHelper::Update(uint32 curr_tick)
{
    if(m_resurgence)
    {
        // 如果复活时间小于0 则复活
        m_resurgenceTime -= g_ServerUpdateTime;
        if(m_resurgenceTime <= 0)
        {
            m_resurgenceTime = 0;
            m_resurgence = false;

            if(m_Owner->IsDead())
            {
                Resurgence();

                // 复活后回复生命
                ResurgenceHp();
               
                // 复活后位置
                Trigger* trigger = m_Owner->GetScene()->GetTriggerByID(m_resurgenceTriggerID);
                if(trigger) // 有这个trigger点在这个trigger点范围内复活
                {
                    // trigger范围内查找一个有用的点
                    CoVec2 resurgencePositon = m_Owner->GetScene()->FindValidPositionInCircle(trigger->GetPosition(), trigger->GetRadius(), m_Owner->GetRadius());
                    m_Owner->GetActionController()->JumpTo((int32)resurgencePositon.x, (int32)resurgencePositon.y);
                }
            }
        }
    }
}

void ResurgenceHelper::Resurgence()
{
    m_Owner->GetStateController()->Resurgence();

    m_Owner->GetScene()->GetRegular()->OnResurgence(m_Owner);

    m_Owner->OnResurgence();

//     Soldier* pSoldier = CnDynamicCast(Soldier, m_Owner);
//     if (pSoldier)
//     {
//         pSoldier->OnResurgenceNtf();
//     }
}

void ResurgenceHelper::OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID)
{
//    CnAssert(m_Owner->GetObjectType() == OBJ_PLAYER);

    if(m_Owner->IsDead())
    {
        Resurgence();
        ResurgenceHp();
    }
}

void ResurgenceHelper::SetResurgenceFullHp()
{
    m_ResurgenceFullHp = true;
}

void ResurgenceHelper::ResurgenceHp()
{
    if(m_ResurgenceFullHp)
    {
        m_Owner->SetHP(m_Owner->GetMaxHp());
        m_Owner->SetMP(m_Owner->GetMaxMp());
    }
    else
    {
        int32 hp = Crown::SDMin(g_ResurgenceHp, m_Owner->GetMaxHp());
        m_Owner->SetHP(hp);
    }
}