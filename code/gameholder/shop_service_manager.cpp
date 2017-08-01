#include "gameholder_pch.h"
#include "shop_manager.h"
#include "player.h"
#include "game_define.h"
#include "shop_service_manager.h"

ShopServiceManager::ShopServiceManager(Player* player) :
m_player(player)
{
}

ShopServiceManager::~ShopServiceManager()
{

}

uint32 ShopServiceManager::GetEquipWashUpLockServiceId(uint32 lockCount)
{
    uint32 serviceId = 0;

    switch (lockCount)
    {
    case 1: 
        serviceId = MALL_SERVICE_EQUIP_WASH_UP_LOCK_1; 
        break;
    case 2: 
        serviceId = MALL_SERVICE_EQUIP_WASH_UP_LOCK_2; 
        break;
    case 3: 
        serviceId = MALL_SERVICE_EQUIP_WASH_UP_LOCK_3; 
        break;
    }

    return serviceId;
}

uint32 ShopServiceManager::GetEquipGemHoleChangeColorLockServiceId(uint32 lockCount)
{
    uint32 serviceId = 0;

    switch (lockCount)
    {
    case 1: 
        serviceId = MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_1; 
        break;
    case 2: 
        serviceId = MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_2; 
        break;
    case 3: 
        serviceId = MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_3; 
        break;
    }

    return serviceId;
}

uint32 ShopServiceManager::GetFusionAutoFillSkillStoneServiceId(uint32 quality, uint32 buyNum)
{
    uint32 serviceId = 0;

    switch (quality)
    {
    case QUALITY_WHITE: 
        serviceId = MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_1; 
        break;
    case QUALITY_GREEN: 
        serviceId = MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_2; 
        break;
    case QUALITY_BLUE: 
        serviceId = MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_3; 
        break;
    case QUALITY_PURPLE: 
        serviceId = MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_4; 
        break;
    }

    return serviceId;
}
