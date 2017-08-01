#include "gameholder_pch.h"
#include "player_config_manager.h"

IMPLEMENT_SINGLETON(PlayerConfigManager)

PlayerConfigManager::PlayerConfigManager()
{
    LEVELUP_ENTRY::CreateInstance();
}

PlayerConfigManager::~PlayerConfigManager()
{
    LEVELUP_ENTRY::DestroyInstance();
}

bool PlayerConfigManager::LoadConfig( const char* path )
{
    if(LEVELUP_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<LEVELUP_ROOT_LEVEL_STRUCT>::const_iterator iter = LEVELUP_ENTRY::Instance()->GetStruct().level_list.begin();
            iter != LEVELUP_ENTRY::Instance()->GetStruct().level_list.end(); iter++)
        {
            m_LevelupConfigMap[iter->level] = &(*iter);
        }
    }

    return true;
}

const LEVELUP_ROOT_LEVEL_STRUCT* PlayerConfigManager::GetPropConfigByLevel(int32 level) const
{
    std::map<uint32, const LEVELUP_ROOT_LEVEL_STRUCT*>::const_iterator iter = m_LevelupConfigMap.find(level);
    if(iter != m_LevelupConfigMap.end())
        return iter->second;
    else
    {
        CnWarn("level %d config data not exist\n", level);
        return NULL;
    }
}

int32 PlayerConfigManager::GetPropByLevel(int32 level, int32 propertyId) const
{
    const LEVELUP_ROOT_LEVEL_STRUCT* levelConfig = GetPropConfigByLevel(level);
    if(!levelConfig)
        return 0;

    switch(propertyId)
    {
    case PROP_PHYSICS_DAMAGE:
        return levelConfig->physics_damage;
    case PROP_MAXHP:
        return levelConfig->hp;
    case PROP_HP_REC_RATE:
        return levelConfig->hp_recovery;
    case PROP_MAXMP:
        return levelConfig->mp;
    case PROP_MP_REC_RATE:
        return levelConfig->mp_recovery;
    case PROP_ARMOR:
        return levelConfig->armor;
    case PROP_DODGE:
        return levelConfig->dodge;
    case PROP_MAX_ENERGY_SHIELD:
        return levelConfig->energy_shield;
    case PROP_HIT_RATE:
        return levelConfig->hit;
    case PROP_ATTACK_PER_SEC:
        return levelConfig->attack_speed;
    case PROP_CAST_PER_SEC:
        return levelConfig->cast_speed;
    case PROP_MOVE_SPEED:
        return levelConfig->move_speed;
    case PROP_CRITICAL_ATTACK:
        return levelConfig->crit;
    case PROP_CRITICAL_DAMAGE_ADDITION:
        return levelConfig->crit_damage;
    case PROP_BLOCK:
        return levelConfig->attack_block;
    case PROP_MAGIC_BLOCK:
        return levelConfig->magic_block;
    case PROP_FIRE_DEFENCE:
        return levelConfig->fire_resistance;
    case PROP_ICE_DEFENCE:
        return levelConfig->frost_resistance;
    case PROP_THUNDER_DEFENCE:
        return levelConfig->lightning_resistance;
    default:
        CnAssert(false);
        return 0;
    }
}

int32 PlayerConfigManager::GetMaxExp(int32 level) const
{
    const LEVELUP_ROOT_LEVEL_STRUCT* levelData = GetPropConfigByLevel(level);
    if(!levelData)
        return 999999999;

    return levelData->max_exp;

}