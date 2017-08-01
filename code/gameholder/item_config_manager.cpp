#include "gameholder_pch.h"
#include "item_config_manager.h"
#include "item_equip.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(ItemConfigManager)

ItemConfigManager::ItemConfigManager() : 
m_EquipStarUpMax(0)
{
    EQUIP_ENTRY::CreateInstance();
    CONSUME_ENTRY::CreateInstance();
    MATERIAL_ENTRY::CreateInstance();
    STONE_ENTRY::CreateInstance();
    EQUIP_ATTRIBUTE_ENTRY::CreateInstance();
    EQUIP_QUALITY_ATTRIBUTE_ENTRY::CreateInstance();
    EQUIP_QUALITY_UP_ENTRY::CreateInstance();
    EQUIP_WASH_UP_ENTRY::CreateInstance();
    EQUIP_STAR_UP_ENTRY::CreateInstance();
    EQUIP_ENCHANTMENT_ENTRY::CreateInstance();
}

ItemConfigManager::~ItemConfigManager()
{
    EQUIP_ENTRY::DestroyInstance();
    CONSUME_ENTRY::DestroyInstance();
    MATERIAL_ENTRY::DestroyInstance();
    STONE_ENTRY::DestroyInstance();
    EQUIP_ATTRIBUTE_ENTRY::DestroyInstance();
    EQUIP_QUALITY_ATTRIBUTE_ENTRY::DestroyInstance();
    EQUIP_QUALITY_UP_ENTRY::DestroyInstance();
    EQUIP_WASH_UP_ENTRY::DestroyInstance();
    EQUIP_STAR_UP_ENTRY::DestroyInstance();
    EQUIP_ENCHANTMENT_ENTRY::DestroyInstance();
}

bool ItemConfigManager::LoadConfig( const char* path )
{
    if (EQUIP_ENTRY::Instance()->LoadConfig(path))
    {
        m_EquipMap.clear();
        const EQUIP_ROOT_STRUCT& RootStruct = EQUIP_ENTRY::Instance()->GetStruct();
        std::vector<EQUIP_ROOT_TEMPLATE_STRUCT>::const_iterator cIter = RootStruct.template_list.begin();
        for (; cIter != RootStruct.template_list.end(); ++cIter)
        {
            if (m_EquipMap.end() != m_EquipMap.find(cIter->id))
            {
                CnWarn("EQUIP duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_EquipMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if (CONSUME_ENTRY::Instance()->LoadConfig(path))
    {
        m_ConsumeMap.clear();
        const CONSUME_ROOT_STRUCT& RootStruct = CONSUME_ENTRY::Instance()->GetStruct();
        std::vector<CONSUME_ROOT_TEMPLATE_STRUCT>::const_iterator cIter = RootStruct.template_list.begin();
        for (; cIter != RootStruct.template_list.end(); ++cIter)
        {
            if (m_ConsumeMap.end() != m_ConsumeMap.find(cIter->id))
            {
                CnWarn("CONSUME duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_ConsumeMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if (MATERIAL_ENTRY::Instance()->LoadConfig(path))
    {
        m_MaterialMap.clear();
        const MATERIAL_ROOT_STRUCT& RootStruct = MATERIAL_ENTRY::Instance()->GetStruct();
        std::vector<MATERIAL_ROOT_TEMPLATE_STRUCT>::const_iterator cIter = RootStruct.template_list.begin();
        for (; cIter != RootStruct.template_list.end(); ++cIter)
        {
            if (m_MaterialMap.end() != m_MaterialMap.find(cIter->id))
            {
                CnWarn("MATERIAL duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_MaterialMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if (STONE_ENTRY::Instance()->LoadConfig(path))
    {
        m_StoneMap.clear();
        const STONE_ROOT_STRUCT& RootStruct = STONE_ENTRY::Instance()->GetStruct();
        std::vector<STONE_ROOT_TEMPLATE_STRUCT>::const_iterator cIter = RootStruct.template_list.begin();
        for (; cIter != RootStruct.template_list.end(); ++cIter)
        {
            if (m_StoneMap.end() != m_StoneMap.find(cIter->id))
            {
                CnWarn("STONE duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_StoneMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }
    else
    {
        return false;
    }

    if (EQUIP_ATTRIBUTE_ENTRY::Instance()->LoadConfig(path))
    {
        m_EquipAttributeMap.clear();
        const EQUIP_ATTRIBUTE_ROOT_STRUCT& RootStruct = EQUIP_ATTRIBUTE_ENTRY::Instance()->GetStruct();
        std::vector<EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT>::const_iterator cIter = RootStruct.modifier_list.begin();
        for (; cIter != RootStruct.modifier_list.end(); ++cIter)
        {
            if (m_EquipAttributeMap.end() != m_EquipAttributeMap.find(cIter->id))
            {
                CnWarn("EQUIP_ATTRIBUTE duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_EquipAttributeMap.insert(std::make_pair(cIter->id, &(*cIter)));
            InsertToGradeMap(&(*cIter));
            InsertToPropMap(&(*cIter));
        }
    }
    else
    {
        return false;
    }

    if(EQUIP_QUALITY_ATTRIBUTE_ENTRY::Instance()->LoadConfig(path))
    {
        const EQUIP_QUALITY_ATTRIBUTE_ROOT_STRUCT& RootStruct = EQUIP_QUALITY_ATTRIBUTE_ENTRY::Instance()->GetStruct();

        std::vector<uint32> empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_1] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_2] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_3] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_4] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_5] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_6] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_7] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_8] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_9] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_10] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_11] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_12] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_13] = empty;
        m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_14] = empty;


        std::vector<EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT>::const_iterator cIter = RootStruct.prop_list.begin();
        for(; cIter != RootStruct.prop_list.end(); ++cIter)
        {
            uint32 prop_id = cIter->id;

            std::vector<uint32> equipClassIDs;
            const EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT& PropStruct = *cIter;
            if(PropStruct.class_1)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_1);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_1].push_back(prop_id);
            }
            if(PropStruct.class_2)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_2);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_2].push_back(prop_id);
            }
            if(PropStruct.class_3)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_3);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_3].push_back(prop_id);
            }
            if(PropStruct.class_4) 
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_4);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_4].push_back(prop_id);
            }
            if(PropStruct.class_5)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_5);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_5].push_back(prop_id);
            }
            if(PropStruct.class_6)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_6);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_6].push_back(prop_id);
            }
            if(PropStruct.class_7)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_7);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_7].push_back(prop_id);
            }
            if(PropStruct.class_8)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_8);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_8].push_back(prop_id);
            }
            if(PropStruct.class_9)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_9);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_9].push_back(prop_id);
            }
            if(PropStruct.class_10)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_10);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_10].push_back(prop_id);
            }
            if(PropStruct.class_11)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_11);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_11].push_back(prop_id);
            }
            if(PropStruct.class_12)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_12);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_12].push_back(prop_id);
            }
            if(PropStruct.class_13)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_13);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_13].push_back(prop_id);
            }
            if(PropStruct.class_14)
            {
                equipClassIDs.push_back(EQUIP_SUB_TYPE_14);
                m_EquipQualityAttributeClassMap[EQUIP_SUB_TYPE_14].push_back(prop_id);
            }

            m_EquipQualityAttributePropMap[prop_id] = equipClassIDs;
        }
    }

    if (EQUIP_QUALITY_UP_ENTRY::Instance()->LoadConfig(path))
    {
        m_EquipQualityUpMap.clear();

        const EQUIP_QUALITY_UP_ROOT_STRUCT& RootStruct = EQUIP_QUALITY_UP_ENTRY::Instance()->GetStruct();

        std::vector<EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT>::const_iterator cIter = RootStruct.head_list.begin();

        for (; cIter != RootStruct.head_list.end(); ++cIter)
        {
            std::string key = Crown::ToString(cIter->quality);

            if (m_EquipQualityUpMap.end() != m_EquipQualityUpMap.find(key))
            {
                CnWarn("EQUIP_ATTRIBUTE duplicate id = %d!\n", cIter->id);
                continue;
            }
            
            m_EquipQualityUpMap[key] = &(*cIter); 
        }
    }
    else
    {
        return false;
    }

    if (EQUIP_WASH_UP_ENTRY::Instance()->LoadConfig(path))
    {
        /*
        m_EquipWashUpMap.clear();

        const EQUIP_WASH_UP_ROOT_STRUCT& RootStruct = EQUIP_WASH_UP_ENTRY::Instance()->GetStruct();
      
        m_EquipWashUpMap[0] = &RootStruct.head; */

        m_EquipWashUpMap.clear();

        const EQUIP_WASH_UP_ROOT_STRUCT& RootStruct = EQUIP_WASH_UP_ENTRY::Instance()->GetStruct();

        std::vector<EQUIP_WASH_UP_ROOT_HEAD_STRUCT>::const_iterator cIter = RootStruct.head_list.begin();
        
        uint32 key = 0;
        for (; cIter != RootStruct.head_list.end(); ++cIter)
        {
            if (m_EquipWashUpMap.end() != m_EquipWashUpMap.find(cIter->id))
            {
                CnWarn("EQUIP_ATTRIBUTE duplicate id = %d!\n", cIter->id);
                continue;
            }

            m_EquipWashUpMap[key] = &(*cIter); 
            key++ ;
        }
    }
    else
    {
        return false;
    }

    if (EQUIP_STAR_UP_ENTRY::Instance()->LoadConfig(path))
    {
        m_EquipStarUpMap.clear();
        m_EquipStarUpMax = 0;

        const EQUIP_STAR_UP_ROOT_STRUCT& RootStruct = EQUIP_STAR_UP_ENTRY::Instance()->GetStruct();

        std::vector<EQUIP_STAR_UP_ROOT_HEAD_STRUCT>::const_iterator cIter = RootStruct.head_list.begin();

        for (; cIter != RootStruct.head_list.end(); ++cIter)
        {
            uint32 key = cIter->star;

            if (m_EquipStarUpMap.end() != m_EquipStarUpMap.find(key))
            {
                CnWarn("EQUIP_STAR_UP duplicate star = %d!\n", key);
                continue;
            }

            uint32 star = cIter->star;
            if (m_EquipStarUpMax < star)
            {
                m_EquipStarUpMax = star;
            }
            m_EquipStarUpMap[key] = &(*cIter); 
        }
    }
    else
    {
        return false;
    }

    if (EQUIP_ENCHANTMENT_ENTRY::Instance()->LoadConfig(path))
    {
        m_EquipEnchantMap.clear();

        const EQUIP_ENCHANTMENT_ROOT_STRUCT& RootStruct = EQUIP_ENCHANTMENT_ENTRY::Instance()->GetStruct();

        std::vector<EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT>::const_iterator cIter = RootStruct.formula_list.begin();

        for (; cIter != RootStruct.formula_list.end(); ++cIter)
        {
            uint32 key = cIter->attribute_id;

            if (m_EquipEnchantMap.end() != m_EquipEnchantMap.find(key))
            {
                CnWarn("EQUIP_ENCHANTMENT duplicate attribute_id = %d!\n", key);
                continue;
            }

            m_EquipEnchantMap[key] = &(*cIter); 
        }
    }
    else
    {
        return false;
    }


    return true;
}

const std::vector<uint32>* ItemConfigManager::FindCanPropIdListByEquipClass(uint32 equip_class) const
{
    TpltEquipQualityAttributeClassMap::const_iterator citer = m_EquipQualityAttributeClassMap.find(equip_class);
    if(citer == m_EquipQualityAttributeClassMap.end())
    {
        CnAssert(false);
        return NULL;
    }
    return &citer->second;
}

const EQUIP_ROOT_TEMPLATE_STRUCT* ItemConfigManager::FindEquipTpltById( uint32 id ) const
{
    TpltEquipMap::const_iterator cIter = m_EquipMap.find(id);
    if (m_EquipMap.end() == cIter)
    {
        return NULL;
    }
    return cIter->second;
}

const CONSUME_ROOT_TEMPLATE_STRUCT* ItemConfigManager::FindConsumeTpltById( uint32 id ) const
{
    TpltConsumeMap::const_iterator cIter = m_ConsumeMap.find(id);
    if (m_ConsumeMap.end() == cIter)
    {
        return NULL;
    }
    return cIter->second;
}

const MATERIAL_ROOT_TEMPLATE_STRUCT* ItemConfigManager::FindMaterialTpltById( uint32 id ) const
{
    TpltMaterialMap::const_iterator cIter = m_MaterialMap.find(id);
    if (m_MaterialMap.end() == cIter)
    {
        return NULL;
    }
    return cIter->second;
}

const STONE_ROOT_TEMPLATE_STRUCT* ItemConfigManager::FindStoneTpltById(uint32 id) const
{
    TpltStoneMap::const_iterator cIter = m_StoneMap.find(id);
    if (m_StoneMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT* ItemConfigManager::FindQualityUpData(uint32 quality) const
{
    const EQUIP_QUALITY_UP_ROOT_STRUCT& RootStruct = EQUIP_QUALITY_UP_ENTRY::Instance()->GetStruct();
    for (std::vector<EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT>::const_iterator cIter = RootStruct.head_list.begin(); cIter != RootStruct.head_list.end(); ++cIter)
    {
        if(cIter->quality == quality)
        {
            return &(*cIter);
        }
    }
    return NULL;
}

const EQUIP_WASH_UP_ROOT_HEAD_STRUCT* ItemConfigManager::FindWashStone() const
{
    TpltEquipWashUpMap::const_iterator cIter = m_EquipWashUpMap.find(0); // 现在只有一个洗练石

    if (m_EquipWashUpMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const EQUIP_STAR_UP_ROOT_HEAD_STRUCT* ItemConfigManager::FindStarUpData(uint32 star) const
{
    TpltEquipStarUpMap::const_iterator cIter = m_EquipStarUpMap.find(star); // 现在只有一个洗练石

    if (m_EquipStarUpMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT* ItemConfigManager::FindEnchantData(uint32 attribute_id) const
{
    TpltEquipEnchantMap::const_iterator cIter = m_EquipEnchantMap.find(attribute_id);

    if (m_EquipEnchantMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second; 
}

const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* ItemConfigManager::FindEquipAttributeTpltById( uint32 id ) const
{
    TpltEquipAttributeMap::const_iterator cIter = m_EquipAttributeMap.find(id);
    if (m_EquipAttributeMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}

const TpltEquipAttributeMap* ItemConfigManager::FindEquipAttributeMapByGrade( uint32 grade ) const
{
    TpltEquipAttributeGradeMap::const_iterator cIter = m_EquipAttributeGradeMap.find(grade);
    if (m_EquipAttributeGradeMap.end() == cIter)
    {
        return NULL;
    }
    return &(cIter->second);
}

const TpltEquipAttributeMap* ItemConfigManager::FindEquipAttributeMapByProp( uint32 prop ) const
{
    TpltEquipAttributePropMap::const_iterator cIter = m_EquipAttributePropMap.find(prop);
    if (m_EquipAttributePropMap.end() == cIter)
    {
        return NULL;
    }
    return &(cIter->second);
}

bool ItemConfigManager::CheckEquipTypeWithPos( int32 type, int32 part )
{
    switch(type)
    {
    case EQUIP_TYPE_HEAD:
        return (EQUIP_PART_HEAD == part);
    case EQUIP_TYPE_BODY:
        return (EQUIP_PART_BODY == part);
    case EQUIP_TYPE_SHOULDER:
        return (EQUIP_PART_SHOULDER == part);
    case EQUIP_TYPE_HAND:
        return (EQUIP_PART_HAND == part);
    case EQUIP_TYPE_FOOT:
        return (EQUIP_PART_FOOT == part);
    case EQUIP_TYPE_NECK:
        return (EQUIP_PART_NECK == part);
    case EQUIP_TYPE_FINGER:
        return (EQUIP_PART_FINGER_L == part || EQUIP_PART_FINGER_R == part);
    case EQUIP_TYPE_TWO_HAND:
        return (EQUIP_PART_HAND_R == part);
    case EQUIP_TYPE_ONE_HAND:
        return (EQUIP_PART_HAND_L == part || EQUIP_PART_HAND_R == part);
    case EQUIP_TYPE_DEPUTY_HAND:
        return (EQUIP_PART_HAND_L == part);
    default:
        break;
    }

    return false;
}

bool ItemConfigManager::CheckEquipSwapType( int32 type )
{
    switch (type)
    {
    case EQUIP_TYPE_FINGER:
        return true;
    case EQUIP_TYPE_ONE_HAND:
        return true;
    default:
        break;
    }

    return false;
}

bool ItemConfigManager::CheckEquipSwapPart( int32 part1, int32 part2 )
{
    switch (part1)
    {
    case EQUIP_PART_FINGER_L:
        return (EQUIP_PART_FINGER_R == part2);
    case EQUIP_PART_FINGER_R:
        return (EQUIP_PART_FINGER_L == part2);
    case EQUIP_PART_HAND_L:
        return (EQUIP_PART_HAND_R == part2);
    case EQUIP_PART_HAND_R:
        return (EQUIP_PART_HAND_L == part2);
    default:
        break;
    }

    return false;
}

int32 ItemConfigManager::FindEquipPartByType(uint32 type, const EquipGrid& grid)
{
    if (grid.size() != EQUIP_PART_WHOLE_BODY)
    {
        CnAssert(false);
        return NULL_GRID;
    }

    switch(type)
    {
    case EQUIP_TYPE_HEAD:
        return EQUIP_PART_HEAD;
    case EQUIP_TYPE_BODY:
        return EQUIP_PART_BODY;
    case EQUIP_TYPE_SHOULDER:
        return EQUIP_PART_SHOULDER;
    case EQUIP_TYPE_HAND:
        return EQUIP_PART_HAND;
    case EQUIP_TYPE_FOOT:
        return EQUIP_PART_FOOT;
    case EQUIP_TYPE_NECK:
        return EQUIP_PART_NECK;
    case EQUIP_TYPE_FINGER:
        {
            if (NULL == grid[EQUIP_PART_FINGER_R] || NULL != grid[EQUIP_PART_FINGER_L])
            {
                return EQUIP_PART_FINGER_R;
            }

            return EQUIP_PART_FINGER_L;
        }
        break;
    case EQUIP_TYPE_TWO_HAND:
        return EQUIP_PART_HAND_R;
    case EQUIP_TYPE_ONE_HAND:
        {
            if (NULL == grid[EQUIP_PART_HAND_R] || NULL != grid[EQUIP_PART_HAND_L])
            {
                return EQUIP_PART_HAND_R;
            }

            if (EQUIP_TYPE_TWO_HAND == grid[EQUIP_PART_HAND_R]->GetPartType())
            {
                return EQUIP_PART_HAND_R;
            }

            return EQUIP_PART_HAND_L;
        }
        break;
    case EQUIP_TYPE_DEPUTY_HAND:
        return EQUIP_PART_HAND_L;
    default:
        break;
    }

    return -1;
}

uint8 ItemConfigManager::GetEquipAnimation(const EquipGrid& grid)
{
    if (grid.size() != EQUIP_PART_WHOLE_BODY)
    {
        CnAssert(false);
        return EQUIP_ANIMATION_JD;
    }

    // 右手是空的--剑盾
    if (!grid[EQUIP_PART_HAND_R])
    {
        return EQUIP_ANIMATION_JD;
    }

    // 右手是单手武器
    if (EQUIP_TYPE_ONE_HAND == grid[EQUIP_PART_HAND_R]->GetPartType())
    {
        // 左手是空的--剑盾
        if (!grid[EQUIP_PART_HAND_L])
        {
            return EQUIP_ANIMATION_JD;
        }
        // 左手是单手武器--双持
        if (EQUIP_TYPE_ONE_HAND == grid[EQUIP_PART_HAND_L]->GetPartType())
        {
            return EQUIP_ANIMATION_SC;
        }
        // 其他情况--剑盾
        return EQUIP_ANIMATION_JD;
    }

    // 右手是双手武器

    // 双手剑
    if (EQUIP_SUB_TYPE_11 == grid[EQUIP_PART_HAND_R]->GetSubType())
    {
        return EQUIP_ANIMATION_SJ;
    }

    // 双手杖
    if (EQUIP_SUB_TYPE_12 == grid[EQUIP_PART_HAND_R]->GetSubType())
    {
        return EQUIP_ANIMATION_SZ;
    }

    // 双手弓
    if (EQUIP_SUB_TYPE_13 == grid[EQUIP_PART_HAND_R]->GetSubType())
    {
        return EQUIP_ANIMATION_GJ;
    }

    // 其他情况--剑盾
    return EQUIP_ANIMATION_JD;
}

void ItemConfigManager::FindEquipAttributeProp(int32 type, std::vector<const TpltEquipAttributeMap*>& Vector)
{
    Vector.clear();
    for (TpltEquipAttributePropMap::const_iterator cIter = m_EquipAttributePropMap.begin();
        cIter != m_EquipAttributePropMap.end(); ++cIter)
    {
        Vector.push_back(&(cIter->second));
    }
}

void ItemConfigManager::InsertToGradeMap( const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct )
{
    CnAssert(pStruct);

    TpltEquipAttributeGradeMap::iterator mIter = m_EquipAttributeGradeMap.find(pStruct->grade);
    if (m_EquipAttributeGradeMap.end() == mIter)
    {
        TpltEquipAttributeMap tpltMap;
        tpltMap.insert(std::make_pair(pStruct->id, pStruct));
        m_EquipAttributeGradeMap.insert(std::make_pair(pStruct->grade, tpltMap));
        return;
    }

    TpltEquipAttributeMap & tpltMap = mIter->second;
    if (tpltMap.end() != tpltMap.find(pStruct->id))
    {
        CnWarn("EQUIP_ATTRIBUTE grade duplicate id = %d!\n", pStruct->id);
        return;
    }

    tpltMap.insert(std::make_pair(pStruct->id, pStruct));
}

void ItemConfigManager::InsertToPropMap( const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct )
{
    CnAssert(pStruct);

    TpltEquipAttributeGradeMap::iterator mIter = m_EquipAttributePropMap.find(pStruct->prop_id);
    if (m_EquipAttributePropMap.end() == mIter)
    {
        TpltEquipAttributeMap tpltMap;
        tpltMap.insert(std::make_pair(pStruct->id, pStruct));
        m_EquipAttributePropMap.insert(std::make_pair(pStruct->prop_id, tpltMap));
        return;
    }

    TpltEquipAttributeMap & tpltMap = mIter->second;
    if (tpltMap.end() != tpltMap.find(pStruct->id))
    {
        CnWarn("EQUIP_ATTRIBUTE grade duplicate id = %d!\n", pStruct->id);
        return;
    }

    tpltMap.insert(std::make_pair(pStruct->id, pStruct));
}

bool ItemConfigManager::IsWeapon(uint32 equipSubType)
{
    return equipSubType == EQUIP_SUB_TYPE_8 
        || equipSubType == EQUIP_SUB_TYPE_9
        || equipSubType == EQUIP_SUB_TYPE_10
        || equipSubType == EQUIP_SUB_TYPE_11
        || equipSubType == EQUIP_SUB_TYPE_12
        || equipSubType == EQUIP_SUB_TYPE_13;
    
}

bool ItemConfigManager::IsArmor(uint32 equipSubType)
{
    return equipSubType == EQUIP_SUB_TYPE_1 
        || equipSubType == EQUIP_SUB_TYPE_2
        || equipSubType == EQUIP_SUB_TYPE_3
        || equipSubType == EQUIP_SUB_TYPE_4
        || equipSubType == EQUIP_SUB_TYPE_5
        || equipSubType == EQUIP_SUB_TYPE_14;

}

bool ItemConfigManager::IsJewelry(uint32 equipSubType)
{
    return equipSubType == EQUIP_SUB_TYPE_6
        || equipSubType == EQUIP_SUB_TYPE_7;
}

uint32 ItemConfigManager::GetItemTypeById(uint32 id) const
{
    if(FindEquipTpltById(id))
        return ITEM_TYPE_EQUIP;
    else if(FindConsumeTpltById(id))
        return ITEM_TYPE_CONSUME;
    else if(FindMaterialTpltById(id))
        return ITEM_TYPE_MATERIAL;
    else if(FindStoneTpltById(id))
        return ITEM_TYPE_STONE;
    else
    {
        CnAssert(false);            // 找不到对应的物品
        return 0;
    }
}

uint32 ItemConfigManager::IsItemId(uint32 id) const
{
    if (FindEquipTpltById(id) ||
        FindConsumeTpltById(id) ||
        FindMaterialTpltById(id) ||
        FindStoneTpltById(id))
    {
        return true;
    }
    return false;
}