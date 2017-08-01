#include "gameholder_pch.h"
#include "battle_unit_communicator.h"
#include "prop_notifier.h"
#include "boss.h"
#include "player.h"
#include "game_define.h"
#include "scene.h"
#include "player_manager.h"
#include "battle_recorder.h"
#include "dungeon_set.h"
#include "event_define.h"
#include "world_boss_config.h"
#include "pet.h"
#include "dungeon.h"

BattleUnitCommunicator::BattleUnitCommunicator(int32 type, DungeonSet* dungeonSet) :
m_type(type),
m_dungeonSet(dungeonSet)
{
    m_hp = 0;
    m_maxhp = 0;
    m_startcreatebooss = 0;
}

BattleUnitCommunicator::~BattleUnitCommunicator()
{
    Disconnect();
}

void BattleUnitCommunicator::Connect(BattleUnit* pBattleUnit)
{
#ifdef DEBUG
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        CnAssert(m_BattleUnits[i] != pBattleUnit);
    }
#endif

    m_BattleUnits.push_back(pBattleUnit);

    OnConnect(pBattleUnit);
}

void BattleUnitCommunicator::Disconnect()
{
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        m_BattleUnits[i]->SetCommunicator(NULL);
        // 同步属性
        m_BattleUnits[i]->SetHP(m_hp);
    }

    m_BattleUnits.clear();
}

// 获得所有场景中的角色
void BattleUnitCommunicator::GetScenesPlayers(std::set<uint64>& scenesPlayers)
{
    DungeonSet* dungeonSet = GetDungeonSet();

    if(!dungeonSet) return;

    std::map<uint32, Dungeon*>& dungeons = dungeonSet->GetDungeons();
    for(std::map<uint32, Dungeon*>::iterator dungeonIt = dungeons.begin(); dungeonIt != dungeons.end(); ++dungeonIt)
    {
        std::map<uint32, Scene*>& scenes = dungeonIt->second->GetScenes();
        for(std::map<uint32, Scene*>::iterator sceneIt = scenes.begin(); sceneIt != scenes.end(); ++sceneIt)
        {
            std::map<uint32, Player*>& players = sceneIt->second->GetPlayers();
            for(std::map<uint32, Player*>::iterator playerIt = players.begin(); playerIt != players.end(); ++playerIt)
            {
                scenesPlayers.insert(playerIt->second->GetPlayerID());
            }
        }
    }
}

void BattleUnitCommunicator::OnDestroy()
{
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        if(!m_BattleUnits[i]->isDestory())
            m_BattleUnits[i]->SetDestory();
    }
}

uint32 BattleUnitCommunicator::GetDungeonID()
{
    if(m_dungeonSet)
    {
        return m_dungeonSet->GetDungeonID();
    }
    return 0;
}

bool BattleUnitCommunicator::HasBattleUnit(BattleUnit* battleUnit)
{
    std::vector<BattleUnit*>::iterator it;

    it = find(m_BattleUnits.begin(), m_BattleUnits.end(), battleUnit);

    if(it != m_BattleUnits.end())
    {
        return true;
    }

    return false;
}

uint32 BattleUnitCommunicator::GetBattleUnitNum()
{
    return (uint32)m_BattleUnits.size();
}

void BattleUnitCommunicator:: DelBattleUnit(BattleUnit* battleUnit)
{
    std::vector<BattleUnit*>::iterator it;

    it = find(m_BattleUnits.begin(), m_BattleUnits.end(), battleUnit);

    if(it == m_BattleUnits.end())
    {
        CnAssert(false);
        return;
    }

    m_BattleUnits.erase(it);
}

int32 BattleUnitCommunicator::GetHp()
{
    return m_hp;
}

void BattleUnitCommunicator::SetHp(int32 hp)
{
    if(m_hp == hp)
        return;

    if(hp > m_maxhp)
        hp = m_maxhp;

    m_hp = hp;

    SetProp(PROP_HP, hp);
}

int32 BattleUnitCommunicator::GetMaxHp()
{
    return m_maxhp;
}

void BattleUnitCommunicator::SetMaxHp(int32 maxhp)
{
    m_maxhp = maxhp;

    SetProp(PROP_MAXHP, maxhp);
}

void BattleUnitCommunicator::SetProp(uint32 propertyId, int32 val)
{
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        m_BattleUnits[i]->SetProp(propertyId, val);
    }
}

int32 BattleUnitCommunicator::GetProp(uint32 propertyId)
{
    int32 retVal = 0;
    for(uint32 i=0; i<m_BattleUnits.size(); i++)
    {
        retVal = m_BattleUnits[i]->GetProp(propertyId);
        break;
    }

    return retVal;
}