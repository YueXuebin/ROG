#include "gameholder_pch.h"
#include "drop_ruler.h"
#include "item_factory.h"
#include "item_config_manager.h"
#include "regular_builder.h"
#include "gem_config_manager.h"
#include "equip_manager.h"

DropRuler::DropRuler(RegularBuilder* pReguler) :
m_pReguler(pReguler)
{
    test_x = 0;
    test_q = 0;
}

DropRuler::~DropRuler()
{

}

ItemBase* DropRuler::DropItem(uint32 owner_id, const RandomDropData& data)
{
    switch(data.firstType)
    {
    case ITEM_TYPE_EQUIP:       // 装备
        return DropEquip(owner_id, data);
    case ITEM_TYPE_STONE:       // 石头
        return DropStone(owner_id, data);
    case ITEM_TYPE_MATERIAL:    // 材料
        return DropMaterial(owner_id, data);
    case ITEM_TYPE_CONSUME:     // 消耗品
        return DropConsume(owner_id, data);
    default:
        CnAssert(false);
        return NULL;
    }
}

ItemBase* DropRuler::DropItem(uint32 owner_id, const REWARD_ITEM& item)
{
    uint32 itemType = ItemConfigManager::Instance()->GetItemTypeById(item.id);

    switch(itemType)
    {
    case ITEM_TYPE_EQUIP:       // 装备
        {
            ItemEquip* pEquip = ItemFactory::CreateEquip(item.id);
            if (!pEquip)
            {
                return NULL;
            }

            // 随机装备品质
            int32 quality = item.flag;
            if(ItemConfigManager::IsJewelry(pEquip->GetSubType()))
            {
                quality = Crown::SDMax(QUALITY_GREEN, quality);         // 饰品至少是绿品
            }

            pEquip->SetQuality(Crown::SDRange((uint32)QUALITY_WHITE, (uint32)quality, (uint32)QUALITY_GOLD));

            EquipManager::SetEquipRandom(pEquip);

            return pEquip;
        }
    case ITEM_TYPE_CONSUME:     // 消耗品
    case ITEM_TYPE_MATERIAL:    // 材料
        {
            ItemBase* pItem = ItemFactory::CreateItem(item.id, itemType);
            if (!pItem)
            {
                return NULL;
            }

            pItem->SetNum(item.num);

            return pItem;
        }
    case ITEM_TYPE_STONE:       // 石头
        {
            ItemStone* pStone= ItemFactory::CreateStone(item.id);
            if(!pStone)
            {
                return NULL;
            }

            pStone->SetQuality(Crown::SDRange((uint32)QUALITY_WHITE, (uint32)item.flag, (uint32)QUALITY_GOLD));

            return pStone;
        }
    default:
        CnAssert(false);
        return NULL;
    }
}

// 掉落装备规则
ItemBase* DropRuler::DropEquip(uint32 owner_id, const RandomDropData& data)
{
    const int32 map_level = m_pReguler->GetMapLevel();
    
    // 随机装备
    const std::vector<EQUIP_ROOT_TEMPLATE_STRUCT>& equip_list = EQUIP_ENTRY::Instance()->GetStruct().template_list;

    std::vector<int32> id_list;
    for (size_t i=0; i<equip_list.size(); ++i)
    {
        if (equip_list[i].eqp_level > map_level)
        {
            continue;
        }
        if (data.secondType != equip_list[i].sub_type)
        {
            continue;
        }

        id_list.push_back(equip_list[i].id);
    }

    if(id_list.empty())
    {
        CnWarn("no equip can drop mapLevel=%d secondType=%d\n", map_level, data.secondType);
        return NULL;
    }

    uint32 random_index = Crown::SDRandom(id_list.size());
    uint32 random_id = id_list[random_index];

    ItemEquip* pDropItem = ItemFactory::CreateEquip(random_id);
    if (!pDropItem)
    {
        CnAssert(false);
        return NULL;
    }

    // 随机装备品质
    int32 quality = data.quality;
    if(ItemConfigManager::IsJewelry(pDropItem->GetSubType()))
    {
        quality = Crown::SDMax(QUALITY_GREEN, quality);         // 饰品至少是绿品
    }

    pDropItem->SetQuality(quality);

    EquipManager::SetEquipRandom(pDropItem);

    return pDropItem;
}

ItemBase* DropRuler::DropMaterial(uint32 owner_id, const RandomDropData& data)
{
    // 随机材料
    const std::vector<MATERIAL_ROOT_TEMPLATE_STRUCT>& material_list = MATERIAL_ENTRY::Instance()->GetStruct().template_list;

    std::vector<int32> id_list;
    for (size_t i=0; i<material_list.size(); ++i)
    {
        if (data.secondType != 0 && data.secondType != material_list[i].sub_type)
        {
            continue;
        }

        id_list.push_back(material_list[i].id);
    }

    if(id_list.empty())
    {
        CnWarn("no material can drop secondType=%d\n", data.secondType);
        return NULL;
    }

    uint32 random_index = Crown::SDRandom(id_list.size());
    uint32 random_id = id_list[random_index];

    ItemMaterial* pDropItem = ItemFactory::CreateMaterial(random_id);
    if (!pDropItem)
    {
        CnAssert(false);
        return NULL;
    }

    pDropItem->SetNum(1);

    return pDropItem;
}

ItemBase* DropRuler::DropConsume(uint32 owner_id, const RandomDropData& data)
{
    // 随机消耗品
    const std::vector<CONSUME_ROOT_TEMPLATE_STRUCT>& template_list = CONSUME_ENTRY::Instance()->GetStruct().template_list;

    std::vector<int32> id_list;
    for (size_t i=0; i<template_list.size(); ++i)
    {
        // 消耗品暂时还没有掉落规则可用

        //if (data.secondType != 0 && data.secondType != template_list[i].sub_type)
        //{
        //    continue;
        //}

        id_list.push_back(template_list[i].id);
    }

    if(id_list.empty())
    {
        CnWarn("no consume can drop\n");
        return NULL;
    }

    uint32 random_index = Crown::SDRandom(id_list.size());
    uint32 random_id = id_list[random_index];

    ItemConsume* pDropItem = ItemFactory::CreateConsume(random_id);
    if (!pDropItem)
    {
        CnAssert(false);
        return NULL;
    }

    pDropItem->SetNum(1);

    return pDropItem;
}

ItemBase* DropRuler::DropStone(uint32 owner_id, const RandomDropData& data)
{
    // 随机石头
    const std::vector<STONE_ROOT_TEMPLATE_STRUCT>& template_list = STONE_ENTRY::Instance()->GetStruct().template_list;

    std::vector<int32> id_list;
    for (size_t i=0; i<template_list.size(); ++i)
    {
        if (data.secondType != 0 && data.secondType != template_list[i].sub_type)
        {
            continue;
        }
        id_list.push_back(template_list[i].id);
    }

    if(id_list.empty())
    {
        CnWarn("no stone can drop secondType=%d\n", data.secondType);
        return NULL;
    }

    uint32 random_index = Crown::SDRandom(id_list.size());
    uint32 random_id = id_list[random_index];

    ItemStone* pDropItem= ItemFactory::CreateStone(random_id);
    if(!pDropItem)
    {
        CnAssert(false);
        return NULL;
    }

    return pDropItem;
}