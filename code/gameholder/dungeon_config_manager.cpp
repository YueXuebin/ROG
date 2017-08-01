#include "gameholder_pch.h"
#include "dungeon_config_manager.h"

IMPLEMENT_SINGLETON(DungeonConfigManager)

DungeonConfigManager::DungeonConfigManager()
{
    DUNGEON_DROP_ENTRY::CreateInstance();
    DUNGEON_CLEARANCE_BONUS_ENTRY::CreateInstance();
    DUNGEON_EXP_BONUS_ENTRY::CreateInstance();
}

DungeonConfigManager::~DungeonConfigManager()
{
    DUNGEON_DROP_ENTRY::DestroyInstance();
    DUNGEON_CLEARANCE_BONUS_ENTRY::DestroyInstance();
    DUNGEON_EXP_BONUS_ENTRY::DestroyInstance();
}

bool DungeonConfigManager::LoadConfig( const char* path )
{
    if(DUNGEON_DROP_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<DUNGEON_DROP_ROOT_DROP_STRUCT>::const_iterator iter = DUNGEON_DROP_ENTRY::Instance()->GetStruct().drop_list.begin();
            iter != DUNGEON_DROP_ENTRY::Instance()->GetStruct().drop_list.end(); ++iter)
        {
            m_DungeonDrop[iter->id] = &(*iter);
        }
    }
    else
    {
        return false;
    }

    if(DUNGEON_CLEARANCE_BONUS_ENTRY::Instance()->LoadConfig(path))
    {
        SDungeonClearanceBonusConfig Clearance;
        for (std::vector<DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT>::const_iterator cIter = DUNGEON_CLEARANCE_BONUS_ENTRY::Instance()->GetStruct().dungeon_list.begin();
            cIter != DUNGEON_CLEARANCE_BONUS_ENTRY::Instance()->GetStruct().dungeon_list.end(); ++cIter)
        {
            Clearance.Clear();
            Clearance.pStruct = &(*cIter);
            Clearance.ClearanceBonus.items = ConfigReader::parseRewardItemList(Clearance.pStruct->clearance_bonus);
            Clearance.TwoStarBonus.items = ConfigReader::parseRewardItemList(Clearance.pStruct->two_star_bonus);
            Clearance.ThreeStarBonus.items = ConfigReader::parseRewardItemList(Clearance.pStruct->three_star_bonus);

            m_DungeonClearanceBonusConfigMap[Clearance.pStruct->dungeon_id] = Clearance;
        }
    }
    else
    {
        return false;
    }

    if(DUNGEON_EXP_BONUS_ENTRY::Instance()->LoadConfig(path))
    {   
        SDungeonExpBonusConfig Exp;
        for (std::vector<DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT>::const_iterator cIter = DUNGEON_EXP_BONUS_ENTRY::Instance()->GetStruct().dungeon_list.begin();
            cIter != DUNGEON_EXP_BONUS_ENTRY::Instance()->GetStruct().dungeon_list.end(); ++cIter)
        {   
            Exp.Clear();
            Exp.pStruct = &(*cIter);
            Exp.exp_bonus_1.items = ConfigReader::parseRewardItemList(Exp.pStruct->exp_bonus_1);
            Exp.exp_bonus_2.items = ConfigReader::parseRewardItemList(Exp.pStruct->exp_bonus_2);
            Exp.exp_bonus_3.items = ConfigReader::parseRewardItemList(Exp.pStruct->exp_bonus_3);
            Exp.exp_bonus_4.items = ConfigReader::parseRewardItemList(Exp.pStruct->exp_bonus_4);
            Exp.exp_bonus_5.items = ConfigReader::parseRewardItemList(Exp.pStruct->exp_bonus_5);

            m_DungeonExpBonusConfigMap[Exp.pStruct->id] = Exp;
        }
    }   
    else
    {
        return false;
    }

    return true;
}

const DUNGEON_DROP_ROOT_DROP_STRUCT* DungeonConfigManager::FindDropByID(uint32 id)
{
    std::map<uint32, const DUNGEON_DROP_ROOT_DROP_STRUCT*>::const_iterator cIter = m_DungeonDrop.find(id);
    if (cIter == m_DungeonDrop.end())
    {
        return NULL;
    }

    return cIter->second;
}

const SDungeonClearanceBonusConfig* DungeonConfigManager::FindClearanceBonusByID(uint32 id)
{
    DungeonClearanceBonusConfigMap::const_iterator cIter = m_DungeonClearanceBonusConfigMap.find(id);
    if (cIter != m_DungeonClearanceBonusConfigMap.end())
    {
        return &(cIter->second);
    }

    return NULL;
}

const SDungeonExpBonusConfig* DungeonConfigManager::FindExpBonusByID(uint32 id)
{
    DungeonExpBonusConfigMap::const_iterator cIter = m_DungeonExpBonusConfigMap.find(id);
    if (cIter != m_DungeonExpBonusConfigMap.end())
    {
        return &(cIter->second);
    }

    return NULL;
}