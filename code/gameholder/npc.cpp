#include "gameholder_pch.h"
#include "npc.h"
#include "ai_controller.h"
#include "scene.h"

//CnImplementRTTI(Npc, Creature);
//
//Npc::Npc() :
//Creature(OBJ_NPC)
//{
//    m_IsSlowTime = false;
//    m_AccumulateTick = Crown::SDRandom(10);
//}
//
//Npc::~Npc()
//{
//}
//
//void Npc::Update(uint32 frame_count)
//{
//    m_IsSlowTime = !GetScene()->IsPlayerSee(this);
//    if(m_IsSlowTime)
//    {
//        m_AccumulateTick++;
//        if(m_AccumulateTick < AI_SLOW_UPDATE_TICK)       // 慢时钟
//            return;
//        m_AccumulateTick = 0;
//    }
//
//    Creature::Update(frame_count);
//}
//
//uint32 Npc::GetDefaultSkill()
//{
//    return MONSTER_DEFAULT_SKILLID_START;
//}