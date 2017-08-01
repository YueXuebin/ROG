#include "gameholder_pch.h"
#include "enemy_manager.h"
#include "player.h"
#include "scene.h"
#include "event_define.h"
#include "void_field_manager.h"
#include "hatred_manager.h"
#include "pet.h"
#include "creature.h"
#include "ai_controller.h"


HatredManager::HatredManager(Creature* creature) :
m_Creature(creature)
{
    m_HatredMap.clear();
    m_HatredList.clear();

    m_LastUpdateFrame = 0;
    m_HateDirty = true;
}

HatredManager::~HatredManager()
{
    
}

void HatredManager::OnRecv(uint32 msgID, void* data)
{

}

void HatredManager::OnDamage(const DamageResult& damage, BattleUnit* pAttacker)
{
    if(!pAttacker)
        return;

    Player* pAttackerPlayer = NULL;
    if(pAttacker->GetObjectType() == OBJ_PLAYER)
    {
        pAttackerPlayer = CnDynamicCast(Player, pAttacker);
    }

    int32 allDamage = damage.sum_damage+damage.poison_damage;
    // 玩家的仇恨记录
    if(pAttackerPlayer)
    {
        // 记录仇恨
        if(GetHatredInfo(pAttackerPlayer->GetId()))
        {
            AddHatredValue(pAttackerPlayer->GetId(), allDamage);
        }
        else
        {
            AddHatredByPlayer(pAttackerPlayer, allDamage);
        }
    }
    else 
    {
        if(pAttacker->IsKindOf(&Creature::ms_RTTI))
        {
            Creature* pAttackerCreature = CnDynamicCast(Creature, pAttacker);
            AddHatredByCreature(pAttackerCreature, allDamage);
        }
    }
}

void HatredManager::Update(uint32 frame_count)
{
    if(frame_count - m_LastUpdateFrame > 20)        // 每秒更新一次
    {   
        m_LastUpdateFrame += 20;
        // 间隔更新仇恨列表
        UpdateHatredList();

        // 仇恨衰减
        ReductionHatred();

        // 每次清除一个仇恨=0的记录
        if(m_HatredList.size() > 0)
        {
            HatredInfo* lastHatred = m_HatredList[m_HatredList.size() -1];
            if(lastHatred->damage <= 0)
            {
                ClearHatred(lastHatred->object_id);
            }
        }

        // 将目标改为仇恨最高的那个人
        if(m_HatredList.size() > 0)
        {
            HatredInfo* firstHatred = m_HatredList[0];
            uint32 nowTargetID = m_Creature->GetAIController()->GetTargetUnitId();
            if(nowTargetID && nowTargetID != firstHatred->object_id)
                m_Creature->GetAIController()->SetTargetUnitId(firstHatred->object_id);
        }
    }
}

void HatredManager::AddHatredByCreature(Creature* pCreature, int32 damage)
{
    HatredInfo info;
    info.object_id = pCreature->GetId();
    info.damage = damage;
    AddHatred(pCreature->GetId(), info);
}

void HatredManager::AddHatredByPlayer(Player* Attacter, int32 damage)
{
    HatredInfo info;
    info.object_id      = Attacter->GetId();
    info.center_id      = Attacter->GetCenterSvrID();
    info.player_id      = Attacter->GetPlayerID();
    info.player_name    = Attacter->GetName();
    info.guild_id       = Attacter->GetGuildID();
    info.damage         = damage;
    info.battle_power   = Attacter->GetBattlePower();
    info.level          = Attacter->GetLevel();
    AddHatred(Attacter->GetId(), info);
}

void HatredManager::AddHatred(uint32 objectID, const HatredInfo& info)
{
    std::map<uint32, HatredInfo>::iterator hatredIt = m_HatredMap.find(objectID);
    if(hatredIt == m_HatredMap.end())
    {
        m_HatredMap[objectID] = info;
        m_HatredList.push_back(&m_HatredMap[objectID]);
    }
    else
    {
        hatredIt->second.damage += info.damage;
    }

    m_HateDirty = true;
}

void HatredManager::AddHatredValue(uint32 objectID, int32 damage)
{
    std::map<uint32, HatredInfo>::iterator hatredIt = m_HatredMap.find(objectID);
    if(hatredIt == m_HatredMap.end())
        return;

    hatredIt->second.damage += damage;
    m_HateDirty = true;
}

void HatredManager::SubHatredValue(uint32 objectID, int32 damage)
{
    std::map<uint32, HatredInfo>::iterator hatredIt = m_HatredMap.find(objectID);
    if (hatredIt == m_HatredMap.end())
        return;

    hatredIt->second.damage = Crown::SDMax(hatredIt->second.damage - damage, 0);
    m_HateDirty = true;
}

void HatredManager::ClearHatred(uint32 objectID)
{
    HatredInfo* pDelInfo = NULL;
    // 从仇恨表中删除逃跑玩家的仇恨记录
    std::map<uint32, HatredInfo>::iterator hatredIt = m_HatredMap.find(objectID);
    if(hatredIt == m_HatredMap.end())
        return;

    pDelInfo = &hatredIt->second;
    
    // 从排序表中删除仇恨记录
    bool IsExist = false;
    for (std::vector<HatredInfo*>::iterator listIt = m_HatredList.begin(); listIt != m_HatredList.end(); listIt++)
    {
        HatredInfo* pHatredInfo = *listIt;
        if(pHatredInfo == pDelInfo)
        {
            listIt = m_HatredList.erase(listIt);
            IsExist = true;
            break;
        }
    }
    CnAssert(IsExist);

    m_HatredMap.erase(hatredIt);

    m_HateDirty = true;
}

void HatredManager::ClearHatredByPlayreID(uint64 playerID)
{
    uint32 ObjectID = 0;
    for (std::map<uint32, HatredInfo>::iterator mapIt = m_HatredMap.begin(); mapIt != m_HatredMap.end(); mapIt++)
    {
        const HatredInfo& hatredInfo = mapIt->second;
        if (hatredInfo.player_id == playerID)
        {
            std::map<uint32, HatredInfo>::iterator nextIter = mapIt;
            ObjectID = mapIt->first;
            break;
        }
    }
    if(ObjectID)
        ClearHatred(ObjectID);
}

HatredInfo* HatredManager::GetHatredInfo(uint32 objectID)
{
    std::map<uint32, HatredInfo>::iterator it = m_HatredMap.find(objectID);
    if(it != m_HatredMap.end())
    {
        return &(it->second);
    }
    return NULL;
}

std::vector<HatredInfo*>& HatredManager::GetSortHatred()
{
    UpdateHatredList();
    return m_HatredList;
}

void HatredManager::UpdateHatredList()
{
    if(!m_HateDirty)
        return;

    m_HateDirty = false;
    
    if(m_HatredList.size() > 0)
    {
        std::sort(m_HatredList.begin(), m_HatredList.end(), SortHatred);
    }
}

void HatredManager::ReductionHatred()
{
    for(std::map<uint32, HatredInfo>::iterator it = m_HatredMap.begin(); it != m_HatredMap.end(); it++)
    {
        it->second.damage = it->second.damage / 3;      // 每秒衰减
    }
}

bool HatredManager::SortHatred(HatredInfo* a, HatredInfo* b)
{
    return a->damage > b->damage;
}

void  HatredManager::ClearAllHatreList()
{
    m_HatredList.clear();
    m_HatredMap.clear();
    m_HateDirty = true;
}

