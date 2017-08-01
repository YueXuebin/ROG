#include "gameholder_pch.h"
#include "tower_defence_config.h"

IMPLEMENT_SINGLETON(TowerDefenceConfig)

TowerDefenceConfig::TowerDefenceConfig()
{

}

TowerDefenceConfig::~TowerDefenceConfig()
{

}

bool TowerDefenceConfig::LoadConfig(const char* path)
{
    return true;
}
