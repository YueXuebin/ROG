#include "gameholder_pch.h"
#include "item_equip.h"
#include "item_config_manager.h"
#include "game_util.h"
#include "property_set_calculater.h"
#include "item_factory.h"
#include "gem_config_manager.h"

CnImplementRTTI(ItemEquip, ItemBase);

ItemEquip::ItemEquip( uint32 id ) : ItemBase(id, ITEM_TYPE_EQUIP),
m_PartType(0),
m_SubType(0),
m_Grade(0),
m_EqpLevel(0),
m_Quality(QUALITY_WHITE),
m_Star(0),
m_PropSetDirty(true)
{
    m_QualityAttributeList.clear();
    m_WashUpAttributeList.clear();
    m_WashUpLockList.clear();
    m_EnchantAttributeList.clear();
    m_GemHoleList.clear();
    m_GemStoneList.clear();
    m_PropSet.Clear();
    m_NextStarPropSet.Clear();
}

ItemEquip::~ItemEquip()
{
}

bool ItemEquip::Init()
{
    // 根据装备ID,获取基础属性
    const EQUIP_ROOT_TEMPLATE_STRUCT* pTplt = ItemConfigManager::Instance()->FindEquipTpltById(GetId());
    if (!pTplt)
    {
        CnWarn("ItemEquip::Init not found template by id=%u!\n", GetId());
        return false;
    }

    m_MaxStackNum = 1;

    m_Grade = pTplt->grade;
    m_PartType = pTplt->part_type;
    m_SubType = pTplt->sub_type;
    m_EqpLevel = pTplt->eqp_level;

    m_PropSetDirty = true;

    return true;
}

void ItemEquip::LoadItem( const void* pInfo )
{
    CnAssert(pInfo);
    const ITEM_EQUIP* pEquip = static_cast<const ITEM_EQUIP*>(pInfo);
    CnAssert(pEquip);

    LoadBase(pEquip->base);

    SetQuality(pEquip->base.quality);
    SetStar(pEquip->star);
    
    SetQualityAttributeList(pEquip->quality_attribute_list);
    SetWashUpAttributeList(pEquip->wash_up_attribute_list);
    SetWashUpLockList(pEquip->wash_up_lock_list);
    SetEnchantAttributeList(pEquip->enchant_attribute_list);

    SetGemHoleList(pEquip->gem_hole_list);
    SetGemStoneList(pEquip->gem_stone_list);

    m_PropSetDirty = true;
    CalcEquipProp();
}

void ItemEquip::SaveItem( void* pInfo ) const
{
    CalcEquipProp();

    CnAssert(pInfo);
    ITEM_EQUIP* pEquip = static_cast<ITEM_EQUIP*>(pInfo);
    CnAssert(pEquip);

    SaveBase(pEquip->base);

    pEquip->base.quality = GetQuality();
    pEquip->star = GetStar();

    pEquip->quality_attribute_list = GetQualityAttributeList();
    pEquip->wash_up_attribute_list = GetWashUpAttributeList();
    pEquip->wash_up_lock_list = GetWashUpLockList();
    pEquip->enchant_attribute_list = GetEnchantAttributeList();

    pEquip->gem_hole_list = GetGemHoleList();
    pEquip->gem_stone_list = GetGemStoneList();

    pEquip->all_attribute_list.clear();
    for(PropertyList::const_iterator iter = m_PropSet.m_list.begin(); iter != m_PropSet.m_list.end(); iter++)
    {
        const Property& prop = *iter;
        KEY_VALUE kv;
        kv.key = prop.type;
        kv.value = prop.value;
        pEquip->all_attribute_list.push_back(kv);
    }

    const EQUIP_ROOT_TEMPLATE_STRUCT* pTplt = ItemConfigManager::Instance()->FindEquipTpltById(GetId());
    pEquip->next_star_attribute_list.clear();
    for(PropertyList::const_iterator iter = m_NextStarPropSet.m_list.begin(); iter != m_NextStarPropSet.m_list.end(); iter++)
    {
        const Property& prop = *iter;
        
        if (pTplt->prop_key_1 == prop.type ||
            pTplt->prop_key_2 == prop.type) 
        {
            KEY_VALUE kv;
            kv.key = prop.type;
            kv.value = prop.value;
            pEquip->next_star_attribute_list.push_back(kv);
        }
    }

}

void ItemEquip::SetQuality( uint32 val) 
{ 
    m_Quality = val; 

    m_PropSetDirty = true;
}

void ItemEquip::SetStar( uint32 val ) 
{ 
    m_Star = val; 

    m_PropSetDirty = true;
}

void ItemEquip::SetQualityAttributeList(const std::vector<uint32>& attribute)
{
    m_QualityAttributeList = attribute;

    m_PropSetDirty = true;
}

void ItemEquip::SetQualityAttribute(const uint32 index, const uint32 attributeId)
{
    if (index >= m_QualityAttributeList.size())
    {
        CnAssert(false);
        return;
    }

    m_QualityAttributeList[index] = attributeId;

    m_PropSetDirty = true;
}

void ItemEquip::AddQualityAttribute(const uint32 attributeId)
{
    m_QualityAttributeList.push_back(attributeId);

    m_PropSetDirty = true;
}

void ItemEquip::SetEnchantAttributeList(const std::vector<uint32>& attribute)
{
    m_EnchantAttributeList = attribute;

    m_PropSetDirty = true;
}

void ItemEquip::ClearEnchantAttributeList()
{
    m_EnchantAttributeList.clear();

    m_PropSetDirty = true;
}

void ItemEquip::AddEnchantAttribute(const uint32 attributeId)
{
    m_EnchantAttributeList.push_back(attributeId);

    m_PropSetDirty = true;
}

void ItemEquip::SetGemHoleList(const std::vector<uint32>& holes)
{
    m_GemHoleList = holes;
}

void ItemEquip::AddGemHole(uint32 holeType)
{
    m_GemHoleList.push_back(holeType);
}

void ItemEquip::SetGemHole(uint32 pos, uint32 holeType)
{
    if (pos >= 0 && m_GemHoleList.size() > pos)
    {
        m_GemHoleList[pos] = holeType;
    }
}

void ItemEquip::SetGemStoneList(const std::vector<ITEM_STONE>& gems)
{
    m_GemStoneList = gems;

    m_PropSetDirty = true;
}

ITEM_STONE* ItemEquip::GetGemStone(uint32 pos)
{
    for(std::vector<ITEM_STONE>::iterator iter = m_GemStoneList.begin(); iter != m_GemStoneList.end(); iter++)
    {
        ITEM_STONE& itemStone = (*iter);
        if(itemStone.base.pos == pos)
            return &itemStone;
    }
    return NULL;
}

bool ItemEquip::HasGemStone(uint32 pos)
{
    for(std::vector<ITEM_STONE>::iterator iter = m_GemStoneList.begin(); iter != m_GemStoneList.end(); iter++)
    {
        ITEM_STONE& itemStone = (*iter);
        if(itemStone.base.pos == pos)
            return true;
    }
    return false;
}

void ItemEquip::ClearGemStoneList() 
{ 
    m_GemStoneList.clear(); 

    m_PropSetDirty = true;
}

const PropertySet& ItemEquip::GetPropertySet() const
{
    CalcEquipProp();
    return m_PropSet; 
}

void ItemEquip::CalcEquipProp() const
{
    if(!m_PropSetDirty)
        return;
    m_PropSetDirty = false;

    uint32 curStar = GetStar();
    uint32 maxStar = ItemConfigManager::Instance()->GetEquipStarUpMax();
    if (GetStar() < maxStar)
    {
        uint32 nextStar = curStar + 1;
        CalcEquipPropImpl(m_NextStarPropSet, nextStar);
    }

    CalcEquipPropImpl(m_PropSet, curStar);
}

void ItemEquip::CalcEquipPropImpl(PropertySet& pPropSet, uint32 star) const
{
    pPropSet.Clear();

    const EQUIP_ROOT_TEMPLATE_STRUCT* pTplt = ItemConfigManager::Instance()->FindEquipTpltById(GetId());
    if (!pTplt)
    {
        CnWarn("ItemEquip::Init not found template by id=%u!\n", GetId());
        return;
    }

    // 星级加成比例
    float starPercent = 1.0f;
    if (star > 0)
    {
        const EQUIP_STAR_UP_ROOT_HEAD_STRUCT* pStarUpStruct = ItemConfigManager::Instance()->FindStarUpData(star);
        if (pStarUpStruct)
        {
            starPercent += pStarUpStruct->percent / 10000.0f;
        }
    }

    // 基础属性条目
    if(pTplt->prop_key_1)
        pPropSet.Add(pTplt->prop_key_1, (int32)(pTplt->prop_value_1 * starPercent));
    if(pTplt->prop_key_2)
        pPropSet.Add(pTplt->prop_key_2, (int32)(pTplt->prop_value_2 * starPercent));

    
    // 计算该装备的所有品质条目
    for (std::vector<uint32>::const_iterator qIter = m_QualityAttributeList.begin(); qIter != m_QualityAttributeList.end(); ++qIter)
    {
        uint32 modifierId = *qIter;
        const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct = ItemConfigManager::Instance()->FindEquipAttributeTpltById(modifierId);
        if(!pStruct)
        {
            CnAssert(false);
            continue;
        }
        pPropSet.Add(pStruct->prop_id, (int32)(pStruct->prop_value * starPercent));
    }

    // 计算该装备的所有附魔条目
    for (std::vector<uint32>::const_iterator qIter = m_EnchantAttributeList.begin(); qIter != m_EnchantAttributeList.end(); ++qIter)
    {
        uint32 modifierId = *qIter;
        const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pStruct = ItemConfigManager::Instance()->FindEquipAttributeTpltById(modifierId);
        if(!pStruct)
        {
            CnAssert(false);
            continue;
        }
        pPropSet.Add(pStruct->prop_id, pStruct->prop_value);
    }

    // 计算镶嵌的宝石的属性
    for (std::vector<ITEM_STONE>::const_iterator gIter = m_GemStoneList.begin(); gIter != m_GemStoneList.end(); ++gIter)
    {
        const ITEM_STONE& gem = *gIter;
        const GEM_ROOT_GEM_ITEM_STRUCT* gemStruct = GemConfigManager::Instance()->GetGemItemConfigData(gem.base.id, gem.level);
        if (!gemStruct)
        {
            CnAssert(false);
            continue;
        }
        pPropSet.Add(gemStruct->prop_id, gemStruct->prop_value);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // 装备属性汇总
    //////////////////////////////////////////////////////////////////////////////////////

    // 物理伤害
    int32 old_physics_damage = pPropSet.GetValue(PROP_PHYSICS_DAMAGE);
    if(old_physics_damage)
    {
        int32 physics_damage_add = pPropSet.GetValue(PROP_EQUIP_PHYSICS_DAMAGE);
        int32 physics_damage_mul = pPropSet.GetValue(PROP_EQUIP_PHYSICS_DAMAGE_MUL);

        int32 new_physics_damage = int32( (old_physics_damage + physics_damage_add) * (1.0f + ToPercent(physics_damage_mul)) );

        pPropSet.Set(PROP_PHYSICS_DAMAGE, new_physics_damage);
    }
    pPropSet.Remove(PROP_EQUIP_PHYSICS_DAMAGE);
    pPropSet.Remove(PROP_EQUIP_PHYSICS_DAMAGE_MUL);

    // 攻击速度
    int32 old_attack_speed = pPropSet.GetValue(PROP_ATTACK_PER_SEC);
    if(old_attack_speed)
    {
        int32 attack_speed_mul = pPropSet.GetValue(PROP_EQUIP_ATTACK_SPEED);
    
        int32 new_attack_speed = int32(old_attack_speed * (1.0f + ToPercent(attack_speed_mul)));

        pPropSet.Set(PROP_ATTACK_PER_SEC, new_attack_speed);
    }
    pPropSet.Remove(PROP_EQUIP_ATTACK_SPEED);

    // 护甲
    int32 old_armor = pPropSet.GetValue(PROP_ARMOR);
    if(old_armor)
    {
        int32 armor_add = pPropSet.GetValue(PROP_EQUIP_ARMOR);
        int32 armor_mul = pPropSet.GetValue(PROP_EQUIP_ARMOR_MUL);

        int32 new_armor = int32((old_armor + armor_add) * (1.0f + ToPercent(armor_mul)));

        pPropSet.Set(PROP_ARMOR, new_armor);
    }
    pPropSet.Remove(PROP_EQUIP_ARMOR);
    pPropSet.Remove(PROP_EQUIP_ARMOR_MUL);

    // 闪避
    int32 old_evsion = pPropSet.GetValue(PROP_DODGE);
    if(old_evsion)
    {
        int32 evsion_add = pPropSet.GetValue(PROP_EQUIP_EVASION);
        int32 evsion_mul = pPropSet.GetValue(PROP_EQUIP_EVASION_MUL);

        int32 new_evsion = int32( (old_evsion + evsion_add) * (1.0f + ToPercent(evsion_mul)) );

        pPropSet.Set(PROP_DODGE, new_evsion);
    }
    pPropSet.Remove(PROP_EQUIP_EVASION);
    pPropSet.Remove(PROP_EQUIP_EVASION_MUL);

    // 能量盾
    int32 old_es = pPropSet.GetValue(PROP_MAX_ENERGY_SHIELD);
    if(old_es)
    {
        int32 es_add = pPropSet.GetValue(PROP_EQUIP_ENERGY_SHIELD);
        int32 es_mul = pPropSet.GetValue(PROP_EQUIP_ENERGY_SHIELD_MUL);

        int32 new_es = int32((old_es + es_add) * (1.0f + ToPercent(es_mul)));

        pPropSet.Set(PROP_MAX_ENERGY_SHIELD, new_es);
    }
    pPropSet.Remove(PROP_EQUIP_ENERGY_SHIELD);
    pPropSet.Remove(PROP_EQUIP_ENERGY_SHIELD_MUL);
    
    // 攻击格挡
    int32 old_attack_block = pPropSet.GetValue(PROP_BLOCK);
    if(old_attack_block)
    {
        int32 attack_block_add = pPropSet.GetValue(PROP_EQUIP_ATTACK_BLOCK);

        int32 new_attack_block = old_attack_block + attack_block_add;

        pPropSet.Set(PROP_BLOCK, new_attack_block);
    }
    pPropSet.Remove(PROP_EQUIP_ATTACK_BLOCK);

    // 法术格挡
    int32 old_magic_block = pPropSet.GetValue(PROP_MAGIC_BLOCK);
    if(old_magic_block)
    {
        int32 magic_block_add = pPropSet.GetValue(PROP_EQUIP_MAGIC_BLOCK);

        int32 new_magic_block = old_magic_block + magic_block_add;

        pPropSet.Set(PROP_MAGIC_BLOCK, new_magic_block);
    }
    pPropSet.Remove(PROP_EQUIP_MAGIC_BLOCK);
}
