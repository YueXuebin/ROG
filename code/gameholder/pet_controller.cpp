#include "gameholder_pch.h"
#include "pet_controller.h"
#include "pet.h"
#include "battle_unit.h"
#include "scene.h"
#include "event_define.h"
#include "player.h"
#include "soldier.h"
#include "dungeon.h"
#include "state_define.h"
#include "resurgence_helper.h"

PetController::PetController(BattleUnit* owner) :
m_Owner(owner)
{
}

PetController::~PetController()
{
    CnAssert(m_PetList.size() == 0);
}

void PetController::AddPet(Pet* pPet)
{
    if (!pPet)
    {
        CnAssert(false);
        return;
    }

    CoVec2 pos = m_Owner->GetScene()->FindValidPositionInRing(m_Owner->GetPosition(), 300, 50, 10);

    pPet->SetPosition(pos);
    pPet->SetAngle(m_Owner->GetAngle());
    pPet->SetTeamID(m_Owner->GetTeamID());

    m_PetList.push_back(pPet);
    m_Owner->GetScene()->AddPet(pPet);

//     //// 宠物出现
//     BATTLE_PET_NTF ntf;
//     ntf.pet_id = pNewPet->GetId();
//     ntf.master_id = m_Owner->GetId();
//     ntf.is_destory = 0;
// 
//     m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BATTLE_PET_NTF, &ntf);
}

void PetController::RemovePet(uint32 id)
{
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); mIter++)
    {
        Pet* pPet = *mIter;
        if (pPet->GetId() == id)
        {
            if (pPet->GetScene())
            {
                pPet->GetScene()->DelPet(pPet->GetId());
            }

            m_PetList.erase(mIter);
            return;
        }
    }
}

void PetController::onEnterScene( Scene* pScene )
{
    // 注意：Update逻辑中，不可以 调用AddCreature 和 DelCreature
    CnAssert(m_PetList.empty());
}

void PetController::onLeaveScene( Scene* pScene )
{
    CnAssert(pScene);

    // 主人离开场景
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); mIter++)
    {
        Pet* pPet = *mIter;

        if (pPet->IsKindOf(&Soldier::ms_RTTI))    // 佣兵
        {
            CnAssert(pPet->GetScene());

            pScene->DelPet(pPet->GetId());
        }
    }

    m_PetList.clear();
}

void PetController::OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID)
{
    // 让宠物进入副本
    PetList* pList = m_Owner->GetPetList();
    if (!pList)
        return;

    for(PetList::iterator mIter = pList->begin(); mIter != pList->end(); ++mIter)
    {
        Pet* pPet = *mIter;

        if (!pPet->IsEnable())      // 不可用
        {
            continue;
        }

        if (pPet->IsKindOf(&Soldier::ms_RTTI))    // 佣兵
        {
            CnAssert(!pPet->GetScene());

            AddPet(pPet);

            pPet->OnEnterDungeon(dungeon, dungeonID);
        }
        else
        {
            CnAssert(false);            // 暂时无其他类型宠物
        }
    }
}

void PetController::KillAllPet()
{
    for(PetList::iterator iter = m_PetList.begin(); iter != m_PetList.end(); iter++)
    {
        Pet* pPet = (*iter);
        if (pPet->IsDead())
        {
            pPet->GetResurgenceHelper()->StopResurgence();
            continue;
        }

        pPet->Kill(NULL);
    }
}

void PetController::OnDead(BattleUnit* killer)
{
    // 随主人死亡
    for(PetList::iterator iter = m_PetList.begin(); iter != m_PetList.end(); iter++)
    {
        Pet* pPet = (*iter);
        if (pPet->IsDead())
        {
            pPet->GetResurgenceHelper()->StopResurgence();
            continue;
        }

        pPet->Dead(killer, NULL);
    }
}

void PetController::OnDestory()
{
    for(PetList::iterator iter = m_PetList.begin(); iter != m_PetList.end(); iter++)
    {
        Pet* pPet = (*iter);
        pPet->OnDestory();
    }

    m_PetList.clear();
}

void PetController::ReceiveEvent( uint32 eventId, uint32 arg1, uint32 arg2 )
{
    switch(eventId)
    {
    case BATTLE_EVENT_ONHIT:
        {
            for ( std::vector<Pet*>::iterator it = m_PetList.begin(); it != m_PetList.end(); it++)
            {
                (*it)->MasterOnhit(arg1);
            }
        }
        break;
    case BATTLE_EVENT_ATTACK:
        {
            for (std::vector<Pet*>::iterator it = m_PetList.begin(); it != m_PetList.end(); it++)
            {
                (*it)->MasterAttack(arg1);
            }
        }
        break;
    }

}

void PetController::SetTeamID(uint64 id)
{
    for(PetList::iterator iter = m_PetList.begin(); iter != m_PetList.end(); iter++)
    {
        Pet* pPet = (*iter);
        pPet->SetTeamID(id);
    }
}