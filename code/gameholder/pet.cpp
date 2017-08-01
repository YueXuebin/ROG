#include "gameholder_pch.h"
#include "pet.h"
#include "game_define.h"
#include "pet_controller.h"
#include "relife_helper.h"
#include "ai_controller.h"
#include "scene.h"
#include "resurgence_helper.h"

CnImplementRTTI(Pet, Creature);

Pet::Pet(BattleUnit* pMaster) :
Creature(OBJ_PET),
m_Master(pMaster),
m_IsAIRun(true),
m_IsEnable(false)
{
}

Pet::~Pet()
{

}

bool Pet::IsMasterDead()
{
    if (GetMaster())
    {
        return GetMaster()->IsDead();
    }
    else
    {
        CnAssert(false);
    }

    return true;
}

void Pet::MasterOnhit( uint32 arg1 )
{
    if(GetAIController())
    {
        GetAIController()->MasterOnHit(arg1);
    }
    else
    {
        CnAssert(false);
    }
}

void Pet::MasterAttack( uint32 arg1 )
{
    if(GetAIController())
    {
        GetAIController()->MasterAttack(arg1);
    }
    else
    {
        CnAssert(false);
    }
}

void Pet::OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID)
{
    if (IsDead())
    {
        m_ResurgenceHelper->StartResurgence();
    }
}

void Pet::SetEnable(bool b)
{
    m_IsEnable = b;
}
