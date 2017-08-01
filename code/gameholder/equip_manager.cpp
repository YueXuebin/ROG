#include "gameholder_pch.h"
#include "equip_manager.h"
#include "player.h"
#include "item_manager.h"
#include "item_factory.h"
#include "item_config_manager.h"
#include "property_set_calculater.h"
#include "game_util.h"
#include "bag_manager.h"
#include "pet.h"

ItemManager* EquipManager::GetItemMgr()
{
    if (m_pCreature->IsKindOf(&Player::ms_RTTI))
    {
        Player* pPlayer = CnDynamicCast(Player, m_pCreature);
        return pPlayer->GetItemManager();
    }
    else if(m_pCreature->IsKindOf(&Pet::ms_RTTI))
    {
        Pet* pPet = CnDynamicCast(Pet, m_pCreature);
        if (pPet->GetMaster() && pPet->GetMaster()->IsKindOf(&Player::ms_RTTI))
        {
            Player* pPlayer = CnDynamicCast(Player, pPet->GetMaster());
            return pPlayer->GetItemManager();
        }
    }

    return NULL;
}

// ==================================================
// 构造函数
// ==================================================
EquipManager::EquipManager(Creature* pCreature, uint8 place) :
m_Place(place),
m_pCreature(pCreature),
m_PropertyDirty(true)
{
    Clear();

    for (uint32 i = 0; i < EQUIP_PART_WHOLE_BODY; ++i)
    {
        m_EquipGrid.push_back(NULL);
    }
}

// ==================================================
// 析构函数
// ==================================================
EquipManager::~EquipManager()
{
    Clear();
}

// ==================================================
// 清除
// ==================================================
void EquipManager::Clear()
{
    m_EquipGrid.clear();

    m_PropertyDirty = true;
}

// ==================================================
// 接收消息
// ==================================================
void EquipManager::OnRecv(uint32 msgID, void* data)
{

}

// ==================================================
// 加载装备信息
// ==================================================
void EquipManager::LoadEquipInfo(const EQUIP_INFO& info)
{
    ItemEquip * pEquip(NULL);

    std::vector<ITEM_EQUIP>::const_iterator cEquipIter = info.equip_list.begin();
    for (; cEquipIter != info.equip_list.end(); ++cEquipIter)
    {
        const ITEM_EQUIP & equip = *cEquipIter;
        if (!IsEmptyGrid(equip.base.pos))
        {
            continue;
        }
        
        pEquip = ItemFactory::CreateEquip(equip.base.id);
        if (!pEquip)
        {
            continue;
        }

        pEquip->LoadItem(&equip);

        if (!ItemConfigManager::CheckEquipTypeWithPos(pEquip->GetPartType(), pEquip->GetPos()))
        {
            ItemFactory::DestoryItem(pEquip);
            continue;
        }

        AddToGrid(pEquip);
        GetItemMgr()->AddToItemList(pEquip);
    }
}

// ==================================================
// 填充装备信息
// ==================================================
void EquipManager::FillEquipInfo(EQUIP_INFO& info) const
{
    info.Clear();

    info.equip_animation = ItemConfigManager::GetEquipAnimation(m_EquipGrid);

    ITEM_EQUIP equip;
    const ItemEquip * pEquip(NULL);

    EquipGrid::const_iterator cIter = GetEquipGrid().begin();
    for (; cIter != GetEquipGrid().end(); ++cIter)
    {
        pEquip = *cIter;
        if (!pEquip)
        {
            continue;
        }

        pEquip->SaveItem(&equip);
        info.equip_list.push_back(equip);
    }
}

bool EquipManager::IsEmptyGrid( int32 pos )
{
    if (pos < 0 || pos >= EQUIP_PART_WHOLE_BODY)
    {
        return false;
    }

    return (NULL == GetEquipGrid()[pos]);
}

int32 EquipManager::GetEquipAnimation() const
{
    return ItemConfigManager::GetEquipAnimation(m_EquipGrid);
}

// ==================================================
// 计算装备属性
// ==================================================
void EquipManager::CalcAllEquipProp()
{
    if (!m_PropertyDirty)
        return;
    m_PropertyDirty = false;

    // 汇总所有装备属性
    PropertySet& propSet = m_pCreature->GetPropertySetCalculater()->GetEquipProp();
    propSet.Clear();

    const ItemEquip* pLeftWeapon = NULL;
    const ItemEquip* pRightWeapon = NULL;

    for (uint32 gridIndex = 0; gridIndex < m_EquipGrid.size(); gridIndex++)
    {
        const ItemEquip* pEquip = m_EquipGrid[gridIndex];
        if (!pEquip)
            continue;

        if(gridIndex == EQUIP_PART_HAND_R)
        {
            pRightWeapon = pEquip;
            continue;
        }
        else if(gridIndex == EQUIP_PART_HAND_L)
        {
            pLeftWeapon = pEquip;
            continue;
        }

        propSet.Add(pEquip->GetPropertySet());
    }

    // 双手武器的处理
    if(pLeftWeapon && pRightWeapon && ItemConfigManager::IsWeapon(pLeftWeapon->GetSubType()) && ItemConfigManager::IsWeapon(pRightWeapon->GetSubType()))
    {
        PropertySet DuelWeaponSum;
        DuelWeaponSum.Add(pLeftWeapon->GetPropertySet());
        DuelWeaponSum.Add(pRightWeapon->GetPropertySet());

        // 双手武器的伤害和攻速,取平均值
        int32 damage1 = pLeftWeapon->GetPropertySet().GetValue(PROP_PHYSICS_DAMAGE);
        int32 damage2 = pRightWeapon->GetPropertySet().GetValue(PROP_PHYSICS_DAMAGE);
        DuelWeaponSum.Set(PROP_PHYSICS_DAMAGE, (damage1 + damage2)/2);

        int32 AttackSpeed1 = pLeftWeapon->GetPropertySet().GetValue(PROP_ATTACK_PER_SEC);
        int32 AttackSpeed2 = pRightWeapon->GetPropertySet().GetValue(PROP_ATTACK_PER_SEC);
        DuelWeaponSum.Set(PROP_ATTACK_PER_SEC, (AttackSpeed1 + AttackSpeed2)/2);

        propSet.Add(DuelWeaponSum);
    }
    else
    {
        if(pLeftWeapon)
            propSet.Add(pLeftWeapon->GetPropertySet());
        if(pRightWeapon)
            propSet.Add(pRightWeapon->GetPropertySet());
    }

    // 武器持装方式的属性补偿
    int32 equipAnimation = ItemConfigManager::GetEquipAnimation(m_EquipGrid);
    if(equipAnimation == EQUIP_ANIMATION_SC)
    {   // 双持补偿,攻击速度10%,攻击格挡15%,增加20%物理伤害
        propSet.Add(PROP_ATTACK_PER_SEC, 1000);
        propSet.Add(PROP_BLOCK, 15);
        propSet.Add(PROP_PHYSICS_DAMAGE_MUL, 2000);
    }
    else if(equipAnimation == EQUIP_ANIMATION_SZ)
    {   // 双杖补偿,攻击格挡12%
        propSet.Add(PROP_BLOCK, 12);
    }
}

void EquipManager::SetEquipRandom(ItemEquip* pEquip)
{
    if (!pEquip)
    {
        CnAssert(false);
        return;
    }

    // 根据装备类型,计算可以随出的属性
    if (pEquip->GetQuality() > QUALITY_WHITE) // 绿装以上才有随机条目
    {
        if(pEquip->GetQualityAttributeList().size() > 0)
        {
            CnAssert(false);        // 装备已经有随机属性条目了
            return;
        }

        uint32 randomCount = pEquip->GetQuality() - QUALITY_WHITE;
        std::vector<uint32> random_id_list = EquipManager::RandomQualityUpAttributes(pEquip, randomCount, NULL);
        if (random_id_list.empty()) // 未随机到属性
        {
            CnAssert(false);
            return;
        }

        pEquip->SetQualityAttributeList(random_id_list);
    }

    // 随机宝石孔
    const EQUIP_ROOT_TEMPLATE_STRUCT* equipTplt = ItemConfigManager::Instance()->FindEquipTpltById(pEquip->GetId());
    uint32 holeCount = Crown::SDRandom(equipTplt->max_hole + 1); // 随机孔数量
    for (size_t i=0; i<holeCount; ++i)
    {
        int32 holeType = Crown::SDRandom(EQUIP_HOLE_TYPE_1, EQUIP_HOLE_TYPE_1 + EQUIP_HOLE_MAX);
        pEquip->AddGemHole(holeType);
    }

}

std::vector<uint32> EquipManager::RandomQualityUpAttributes(ItemEquip* pEquip, uint32 propNum, const std::vector<uint32>* excludeAttributes )
{
    uint32 configType = 1; // 升品随机条目类型为1，附魔为2

    uint32 equipPart = pEquip->GetPartType();   // 装备部位
    uint32 equipGrade = pEquip->GetGrade();     // 装备等阶
    uint32 equipClass = pEquip->GetSubType();   // 装备类型
    uint32 attrNum = propNum;  // 随机条目数量


    // 根据装备类型,计算可以随出的属性
    std::vector<uint32> prop_id_list;       // 可以随机出现的属性条目
    std::vector<uint32> resurt_id_list;     // 随机结果
    std::vector<uint32> exclude_prop_id_list; // 排除的属性条目

    // 先算出要排除的属性条目
    if (excludeAttributes)
    {
        for (std::vector<uint32>::const_iterator curPropIt = excludeAttributes->begin();
            curPropIt != excludeAttributes->end(); curPropIt++)
        {
            uint32 modifier_id = *curPropIt;
            const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pModifierData = ItemConfigManager::Instance()->FindEquipAttributeTpltById(modifier_id);
            if(!pModifierData)
            {
                CnAssert(false);            // 不存在条目属性
                continue;
            }
            exclude_prop_id_list.push_back(pModifierData->prop_id);
        }
    }


    // 获取该装备上可以出的属性条目
    const std::vector<uint32>* prop_list = ItemConfigManager::Instance()->FindCanPropIdListByEquipClass(equipClass);
    for(std::vector<uint32>::const_iterator pIter = prop_list->begin(); pIter != prop_list->end(); pIter++)
    {
        uint32 prop_id = *pIter;

        if (std::find(exclude_prop_id_list.begin(), exclude_prop_id_list.end(), prop_id) != exclude_prop_id_list.end())
            continue; // 排除

        bool canUp = true;

        // 防具需要再根据基础属性进行判断
        if(ItemConfigManager::IsArmor(equipClass))
        {
            const EQUIP_ROOT_TEMPLATE_STRUCT* pTplt = ItemConfigManager::Instance()->FindEquipTpltById(pEquip->GetId());
            if(!pTplt)
            {
                CnAssert(false);
                continue;
            }

            if(prop_id == PROP_EQUIP_ARMOR || prop_id == PROP_EQUIP_ARMOR_MUL)
            {
                if(pTplt->prop_key_1 != PROP_ARMOR && pTplt->prop_key_2 != PROP_ARMOR)
                    canUp = false;          // 装备基础属性没有护甲
            }
            else if(prop_id == PROP_EQUIP_EVASION || prop_id == PROP_EQUIP_EVASION_MUL)
            {
                if(pTplt->prop_key_1 != PROP_DODGE && pTplt->prop_key_2 != PROP_DODGE)
                    canUp = false;          // 装备基础属性没有闪避
            }
            else if(prop_id == PROP_EQUIP_ENERGY_SHIELD || prop_id == PROP_EQUIP_ENERGY_SHIELD_MUL)
            {
                if(pTplt->prop_key_1 != PROP_MAX_ENERGY_SHIELD && pTplt->prop_key_2 != PROP_MAX_ENERGY_SHIELD)
                    canUp = false;          // 装备基础属性没有能量盾
            }
        }

        if(!canUp)
            continue;           // 该装备上不能出现此属性

        prop_id_list.push_back(prop_id);
    }

    for(uint32 ai=0; ai<attrNum; ai++)
    {
        if(prop_id_list.size() == 0)
            continue;

        uint32 attr_index = Crown::SDRandom(prop_id_list.size());
        uint32 prop_id = prop_id_list[attr_index];          // 随机的属性条目

        // 计算属性的等阶
        std::vector<uint32> modifier_id_list;               // 可以出现的条目列表

        const TpltEquipAttributeMap* pPropGradeList = ItemConfigManager::Instance()->FindEquipAttributeMapByProp(prop_id);  // 改属性的所有等阶条目

        for(TpltEquipAttributeMap::const_iterator gIter = pPropGradeList->begin(); gIter != pPropGradeList->end(); gIter++)
        {
            const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* modifier = gIter->second;

            if (modifier->type != configType)
                continue;               // 排除不是升品的属性条目

            if(modifier->grade > (int32)equipGrade)
                continue;               // 排除大于装备等阶的属性条目

            modifier_id_list.push_back(modifier->id);
        }

        if(modifier_id_list.size() <= 0)
        {
            CnAssert(false);            // 没有可选条目
            continue;
        }

        uint32 modifier_index = Crown::SDRandom(modifier_id_list.size());
        uint32 modifier_id = modifier_id_list[modifier_index];

        const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* pModifierData = ItemConfigManager::Instance()->FindEquipAttributeTpltById(modifier_id);
        if(!pModifierData)
        {
            CnAssert(false);            // 不存在条目属性
            continue;
        }

        // 去掉已经添加的属性(不能出现重复的属性)
        prop_id_list.erase(prop_id_list.begin() + attr_index);
        resurt_id_list.push_back(pModifierData->id);
    }

    return resurt_id_list;
}

uint16 EquipManager::OptInstall( ItemBase* pSrc,  const int32 dstPos )
{
    if (!IsValidGrid(dstPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemEquip* pEquip = CnDynamicCast(ItemEquip, pSrc);
    if (!pEquip)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (!ItemConfigManager::CheckEquipTypeWithPos(pEquip->GetPartType(), dstPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    // 处理双手武器替换两个武器问题
    if (EQUIP_TYPE_TWO_HAND == pEquip->GetPartType() &&
        GetEquipByPos(EQUIP_PART_HAND_L))
    {
        // 先写下副(左)手装备
        int16 errCode = OptUninstall(EQUIP_PART_HAND_L);
        if (ERR_SUCCEED != errCode)
        {
            return errCode;
        }
    }
    // 处理副手替换 主手是双手武器问题
    else if (EQUIP_PART_HAND_L == dstPos &&
        GetEquipByPos(EQUIP_PART_HAND_R) &&
        EQUIP_TYPE_TWO_HAND == GetEquipByPos(EQUIP_PART_HAND_R)->GetPartType())
    {
        // 卸下主(右)手装备
        int16 errCode = OptUninstall(EQUIP_PART_HAND_R);
        if (ERR_SUCCEED != errCode)
        {
            return errCode;
        }
    }

    ItemEquip* pDst = GetEquipByPos(dstPos);
    // 查看安装位置是否有装备
    if (pDst)
    {
        // 进行交换逻辑
        int16 errCode = OptSwap(pEquip->GetPlace(), pEquip->GetPos(), pDst->GetPlace(), pDst->GetPos());
        if (ERR_SUCCEED == errCode) 
        {
            return ERR_SUCCEED_SWAP;
        }
        
        CnAssert(false);
        return ERR_FAILED;
    }

    // 安装装备
    pEquip->SetPos(dstPos);
    AddToGrid(pEquip);

    UpdateEquipInfoNtf();

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pCreature->CalcProperty();

    return ERR_SUCCEED;
}

uint16 EquipManager::OptUninstall( int32 srcPos, int32 dstPos /*= -1*/ )
{
    if (!IsValidGrid(srcPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemEquip* pEquip = GetEquipByPos(srcPos);

    if (!pEquip)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    // 通知卸下装备到背包
    OPT_ITEM_INFO info;
    info.opt = ITEM_OPT_MOVE;

    // 填充起点数据
    pEquip->SaveBase(info.src);

    // 填充终点数据
    info.dst.place = PLACE_BAG;
    info.dst.id = pEquip->GetId();
    info.dst.num = pEquip->GetNum();
    if(NULL_GRID == dstPos)
    {
        GetItemMgr()->FindDstPlace(pEquip, PLACE_BAG, info.dst.pos);
        if (NULL_GRID == info.dst.pos)
        {
            return ERR_FAILED;
        }
    }
    else
    {
        info.dst.pos = dstPos;
    }

    // 移动
    uint16 errCode = GetItemMgr()->MoveItem(pEquip, info.dst.place, info.dst.pos);
    if (ERR_SUCCEED != errCode)
    {
        CnAssert(false);
        return errCode;
    }

    GetItemMgr()->NtfOptItem(info);
    // 移除起点数据
    GetItemMgr()->RemovePlaceItem(m_Place, info.src.pos, pEquip);

    UpdateEquipInfoNtf();

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pCreature->CalcProperty();

    return ERR_SUCCEED;
}

uint16 EquipManager::OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos )
{
    ItemBase* pSrc = GetItemMgr()->GetPlaceItem(srcPlace, srcPos);
    ItemBase* pDst = GetItemMgr()->GetPlaceItem(dstPlace, dstPos);

    if (!pSrc || !pDst)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    OPT_ITEM_INFO info;
    info.opt = ITEM_OPT_SWAP;
    pSrc->SaveBase(info.src);
    pDst->SaveBase(info.dst);

    // 移动
    uint16 errCode = GetItemMgr()->SwapItem(pSrc, pDst);
    if (ERR_SUCCEED != errCode)
    {
        CnAssert(false);
        return errCode;
    }

    GetItemMgr()->NtfOptItem(info);

    UpdateEquipInfoNtf();

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pCreature->CalcProperty();

    return ERR_SUCCEED;
}

void EquipManager::AddToGrid(ItemEquip * pEquip, bool bSwap /*= false*/)
{
    CnAssert(pEquip);
    CnAssert(IsValidGrid(pEquip->GetPos()));
    CnAssert(pEquip->GetNum() > 0);
    if (!bSwap)
    {
        CnAssert(!m_EquipGrid[pEquip->GetPos()]);
    }

    pEquip->SetPlace(m_Place);
    m_EquipGrid[pEquip->GetPos()] = pEquip;
}


void EquipManager::UpdateEquipInfoNtf()
{
    UPDATE_EQUIP_INFO_NTF info;
    FillEquipInfo(info.equip_info);
    m_pCreature->NotifyEquipInfo(info);
}

ItemEquip* EquipManager::GetEquipByPos(int32 pos)
{
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return NULL;
    }

    return m_EquipGrid[pos];
}

bool EquipManager::IsValidGrid(int32 pos) const
{
    return (pos >= 0 && pos < EQUIP_PART_WHOLE_BODY);
}
