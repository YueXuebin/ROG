#include "gameholder_pch.h"
#include "pet_manager.h"
#include "scene.h"
#include "event_define.h"
#include "player.h"
#include "soldier.h"
#include "ai_controller.h"
#include "resurgence_helper.h"
#include "state_define.h"
#include "pet_controller.h"

PetManager::PetManager(Player* pPlayer) :
m_pPlayer(pPlayer),
m_pSoldier(NULL)
{
}

PetManager::~PetManager()
{
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); ++mIter)
    {
        Pet* pDelPet = *mIter;
        if(pDelPet->GetScene())
        {
            pDelPet->GetScene()->DelPet(pDelPet->GetId());
        }
        CnDelete pDelPet;
    }
    m_PetList.clear();

    m_pSoldier = NULL;
}

void PetManager::LoadPetInfo(const PET_INFO& info)
{
    // 创建佣兵对象
    m_pSoldier = CnNew Soldier(m_pPlayer);
    m_pSoldier->LoadInfo(info.soldier_info);

    m_pSoldier->SetNoCollisionObject(true);
    if (m_pPlayer->GetSex() == SEX_MALE)
    {
        m_pSoldier->SetSex(SEX_FEMALE);
        m_pSoldier->SetName("Amy");
    }
    else
    {
        m_pSoldier->SetSex(SEX_MALE);
        m_pSoldier->SetName("John");
    }

//     m_pSoldier->ChangeState(STATE_DIE);
//     m_pSoldier->GetResurgenceHelper()->SetResurgenceFullHp();

    m_PetList.push_back(m_pSoldier);
}

void PetManager::FillPetInfo(PET_INFO& info) const
{
    CnAssert(m_pSoldier);

    m_pSoldier->FillInfo(info.soldier_info);
    info.soldier_info.object_id = m_pSoldier->GetId();
}

void PetManager::OnEndSet()
{
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); ++mIter)
    {
        Pet* pPet = *mIter;
        pPet->SetHP(pPet->GetMaxHp());
        pPet->SetMP(pPet->GetMaxMp());
    }
}

void PetManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
    {
    case MSG_SOLDIER_ENABLE_REQ:
        {
            OnSoldierEnableReq(data);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

uint32 PetManager::GetSoldierObjectID()
{
    CnAssert(m_pSoldier);           // 必存在佣兵对象
    return m_pSoldier->GetId();
}

void PetManager::Update(uint32 frame_count)
{
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); ++mIter)
    {
        Pet* pPet = *mIter;
        if (pPet->GetScene())
        {
            continue;
        }

        pPet->Update(frame_count);
    }
}

void PetManager::OnSoldierEnableReq(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    SOLDIER_ENABLE_REQ* pRecvMsg = (SOLDIER_ENABLE_REQ*)data;
    SOLDIER_ENABLE_ACK sendMsg;

    if (!m_pSoldier)
    {
        sendMsg.errcode = ERR_FAILED;
        goto _END;
    }

    if (!m_pSoldier->IsEnable())
    {
        m_pSoldier->GetResurgenceHelper()->StartResurgence();
        m_pSoldier->SetEnable(true);

        m_pPlayer->GetPetController()->AddPet(m_pSoldier);
        m_pSoldier->OnEnterDungeon(m_pPlayer->GetScene()->GetDungeon(), m_pPlayer->GetScene()->GetDungeonID());

        m_pSoldier->OnResurgenceNtf();
    }
    else
    {
        m_pSoldier->GetResurgenceHelper()->StopResurgence();
        m_pSoldier->SetEnable(false);
        m_pPlayer->GetPetController()->RemovePet(m_pSoldier->GetId());
    }

    sendMsg.errcode = ERR_SUCCEED;
    sendMsg.is_enable = m_pSoldier->IsEnable() == true ? 1 : 0;

_END:
    m_pPlayer->SendToGate(MSG_SOLDIER_ENABLE_ACK, &sendMsg);
}

void PetManager::AddExp(int32 val)
{
    if (m_pSoldier && m_pSoldier->IsEnable())
    {
        m_pSoldier->AddExp(val);
    }
}

void PetManager::CalcPetProperty()
{
    for(PetList::iterator mIter = m_PetList.begin(); mIter != m_PetList.end(); ++mIter)
    {
        Pet* pPet = *mIter;
        pPet->CalcProperty();
    }
}
