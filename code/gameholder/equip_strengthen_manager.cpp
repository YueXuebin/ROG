#include "gameholder_pch.h"
#include "player.h"
#include "equip_strengthen_manager.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "pet_manager.h"
#include "soldier.h"
#include "equip_manager.h"
#include "game_define.h"
#include "shop_manager.h"
#include "shop_service_manager.h"
#include "equip_manager.h"

ItemManager* EquipStrengthenManager::GetItemMgr()
{
    return m_player->GetItemManager();
}

EquipStrengthenManager::EquipStrengthenManager(Player* player) :
m_player(player)
{
}

EquipStrengthenManager::~EquipStrengthenManager()
{

}


void EquipStrengthenManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
        case MSG_EQUIP_QUALITY_UP_REQ:  // 装备升品
            OnQualityUp(data);
            break;

        case MSG_EQUIP_WASH_UP_REQ:     // 装备洗练
            OnWashUp(data);
            break;
        case MSG_EQUIP_WASH_UP_USE_OLD_REQ:
            OnWashUpUseOld(data);
            break;
        case MSG_EQUIP_WASH_UP_USE_NEW_REQ:
            OnWashUpUseNew(data);
            break;

        case MSG_EQUIP_STAR_UP_REQ:     // 装备升星
            OnStarUp(data);
            break;

        case MSG_EQUIP_ENCHANT_REQ:     // 装备附魔
            OnEnchant(data);
            break;

        default:
            break;
    }
}
 
void EquipStrengthenManager::OnQualityUp(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    ITEM_EQUIP* recvMsg = (ITEM_EQUIP*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->base.place, recvMsg->base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
       CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }
   
    // 根据装备的品质,计算升级需要的品质石信息
    const EQUIP_QUALITY_UP_ROOT_HEAD_STRUCT* pQualityUpStruct = ItemConfigManager::Instance()->FindQualityUpData(pItemEquip->GetQuality() + 1);
    if (!pQualityUpStruct)
    {
        CnWarn("Equip quality up material not found"); // 无需升品
        return;
    }

    if (false == m_player->GetItemManager()->CheckItemNum(pQualityUpStruct->id, pQualityUpStruct->num))
    {
        CnWarn("Equip quality up material not enough"); // 升品石不足
        return;
    }

    std::vector<uint32> random_id_list = EquipManager::RandomQualityUpAttributes(pItemEquip, 1, &(pItemEquip->GetQualityAttributeList()));
    if (random_id_list.empty()) // 未随机到属性
    {
        sendMsg.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_EQUIP_QUALITY_UP_ACK, &sendMsg);
        return;
    }

    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_QUAULITY_UP, pQualityUpStruct->id, pQualityUpStruct->num); // 使用道具
    pItemEquip->SetQuality(pItemEquip->GetQuality()+1); // 品质+1
    pItemEquip->AddQualityAttribute(random_id_list[0]); // 增加一条属性

    SendStrengthenAck(MSG_EQUIP_QUALITY_UP_ACK, &sendMsg, pItemEquip, true);
}

void EquipStrengthenManager::OnWashUp(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    EQUIP_WASH_UP_REQ* recvMsg = (EQUIP_WASH_UP_REQ*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->equip.base.place, recvMsg->equip.base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
        CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }

    const EQUIP_WASH_UP_ROOT_HEAD_STRUCT* pWashUpStruct = ItemConfigManager::Instance()->FindWashStone();
    if (!pWashUpStruct)
    {
        CnAssert(false); // 未找到相关配置,无需洗练
        return;
    }

    if (false == m_player->GetItemManager()->CheckItemNum(pWashUpStruct->id, pWashUpStruct->num))
    {
        CnWarn("Equip wash up material not enough"); // 洗练石不足
        return;
    }

    // 当前属性条目
    const std::vector<uint32> qualityAttributeList = pItemEquip->GetQualityAttributeList();

    // 验证锁的合法性
    if (recvMsg->lock_index.size() >= qualityAttributeList.size())
    {
        CnWarn("Cannot lock all quality attributes"); // 不能全部上锁
        return;
    }
    for (uint32 i = 0; i < recvMsg->lock_index.size(); i++)
    {
        if (recvMsg->lock_index[i] >= qualityAttributeList.size())
        {
            CnWarn("Equip wash up lock index is invalided"); // 索引无效
            return;
        }

        for (uint32 j = 0; j < recvMsg->lock_index.size(); j++)
        {
            if (i != j)
            {
                if (recvMsg->lock_index[i] == recvMsg->lock_index[j])
                {
                    CnWarn("Equip wash up lock index duplicated"); // 重复
                    return;
                }
            }
        }
    }

    // 锁
    std::vector<uint32> washUpLockList;
    for (uint32 i = 0; i < recvMsg->lock_index.size(); i++)
    {
        washUpLockList.push_back(recvMsg->lock_index[i]);
    }
  
    // 上锁的属性条目排除掉
    std::vector<uint32> excludeAttributes;
    for (uint32 i = 0; i < washUpLockList.size(); i++)
    {
        excludeAttributes.push_back(qualityAttributeList[washUpLockList[i]]);
    }

    // 随机属性条目
    uint32 randomCount = qualityAttributeList.size() - excludeAttributes.size();
    std::vector<uint32> result_id_list = EquipManager::RandomQualityUpAttributes(pItemEquip, randomCount, &excludeAttributes);
    if (result_id_list.empty())
    {
        sendMsg.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_EQUIP_WASH_UP_ACK, &sendMsg);
        return;
    }    

    // 洗练后的新属性条目
    std::vector<uint32> washUpAttributeList;
    for (uint32 i = 0, j = 0; i < qualityAttributeList.size(); i++)
    {
        if (washUpLockList.end() != std::find(washUpLockList.begin(), washUpLockList.end(), i))
        {
            washUpAttributeList.push_back(qualityAttributeList[i]); // 上锁了,则等于原来的属性条目
        }
        else
        {
            washUpAttributeList.push_back(result_id_list[j++]);   // 未上锁，则等于新的属性条目
        }
    }

    // 消耗洗练石,更新属性条目
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_WASH_UP, pWashUpStruct->id, pWashUpStruct->num);
    pItemEquip->SetWashUpAttributeList(washUpAttributeList);
    pItemEquip->SetWashUpLockList(washUpLockList);

    SendStrengthenAck(MSG_EQUIP_WASH_UP_ACK, &sendMsg, pItemEquip, false);
}

void EquipStrengthenManager::OnWashUpUseOld(void* data)    // 装备洗练保留原数值
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    ITEM_EQUIP* recvMsg = (ITEM_EQUIP*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->base.place, recvMsg->base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
        CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }

    // 清空
    pItemEquip->ClearWashUpAttributeList();
    pItemEquip->ClearWashUpLockList();

    SendStrengthenAck(MSG_EQUIP_WASH_UP_USE_OLD_ACK, &sendMsg, pItemEquip, false);
}

void EquipStrengthenManager::OnWashUpUseNew(void* data)    // 装备洗练确认替换
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    ITEM_EQUIP* recvMsg = (ITEM_EQUIP*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->base.place, recvMsg->base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
        CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }

    // 洗练后的属性条目
    std::vector<uint32> result_id_list = pItemEquip->GetWashUpAttributeList();
    if (result_id_list.empty())
    {
        sendMsg.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_EQUIP_WASH_UP_ACK, &sendMsg);
        return;
    }

    // 加锁
    std::vector<uint32> lock_index_list = pItemEquip->GetWashUpLockList();
    uint32 lockCount = lock_index_list.size();
    uint32 serviceId;
    if (lockCount > 0)
    {
        serviceId = m_player->GetShopServiceManager()->GetEquipWashUpLockServiceId(lockCount);
        if (false == m_player->GetShopManager()->IsEnoughMoney(serviceId, 1))
        {
            sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT; // 钻石不足
            m_player->SendToGate(MSG_EQUIP_WASH_UP_ACK, &sendMsg);
            return;
        }
    }

    // 花费锁钱
    if (lockCount > 0)
    {
        if (false == m_player->GetShopManager()->DoMallBuy(serviceId, 1, BUY_REASON_NORMAL))
        {
            sendMsg.errcode = ERR_FAILED;   // 购买失败
            m_player->SendToGate(MSG_EQUIP_WASH_UP_ACK, &sendMsg);
            return;
        }
    }

    // 更新属性条目
    for (uint32 i = 0; i < result_id_list.size(); i++)
    {
        if (lock_index_list.end() != std::find(lock_index_list.begin(), lock_index_list.end(), i))
        {
            continue; // 上锁了
        }
        else
        {
            pItemEquip->SetQualityAttribute(i, result_id_list[i]);
        }
    }

    // 清空洗练属性
    pItemEquip->ClearWashUpAttributeList();
    pItemEquip->ClearWashUpLockList();

    SendStrengthenAck(MSG_EQUIP_WASH_UP_USE_NEW_ACK, &sendMsg, pItemEquip, true);
}

void EquipStrengthenManager::OnStarUp(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    EQUIP_STAR_UP_REQ* recvMsg = (EQUIP_STAR_UP_REQ*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->equip.base.place, recvMsg->equip.base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
        CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }

    const EQUIP_STAR_UP_ROOT_HEAD_STRUCT* pStarUpStruct = ItemConfigManager::Instance()->FindStarUpData(pItemEquip->GetStar() + 1);
    if (!pStarUpStruct)
    {
        CnAssert(false); // 未找到相关配置,无需升星
        return;
    }

    if (false == m_player->GetItemManager()->CheckItemNum(pStarUpStruct->id, pStarUpStruct->num))
    {
        CnWarn("Equip star up material not enough"); // 升星石不足
        return;
    }

    if (recvMsg->success_material_count > 0 &&
        false == m_player->GetItemManager()->CheckItemNum(MATERIAL_ID_EQUIP_STAR_SUCCESS, recvMsg->success_material_count))
    {
        CnWarn("Equip star up success material not enough"); // 升星辅石不足
        return;
    }

    // 升星是否成功
    uint32 successRate = pStarUpStruct->success_rate + recvMsg->success_material_count * 5;
    bool success = Crown::SDRandom(100) < successRate;

    // 消耗洗星石和辅石,开始升星
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_STAR_UP, pStarUpStruct->id, pStarUpStruct->num);
    if (recvMsg->success_material_count > 0)
    {
        m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_STAR_UP, MATERIAL_ID_EQUIP_STAR_SUCCESS, recvMsg->success_material_count);
    }

    if (false == success) // 升星未成功
    {
        sendMsg.errcode = ERR_EQUIP_STAR_UP_FAILED;
        m_player->SendToGate(MSG_EQUIP_STAR_UP_ACK, &sendMsg);
        return;
    }

    pItemEquip->SetStar(pItemEquip->GetStar() + 1); // +1星

    SendStrengthenAck(MSG_EQUIP_STAR_UP_ACK, &sendMsg, pItemEquip, true);
}

void EquipStrengthenManager::OnEnchant(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    EQUIP_ENCHANT_REQ* recvMsg = (EQUIP_ENCHANT_REQ*)data;
    ITEM_STRENGTHEN_ACK sendMsg;

    // 获取装备信息
    ItemBase* pEquipBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->equip.base.place, recvMsg->equip.base.pos);
    ItemEquip* pItemEquip = CnDynamicCast(ItemEquip, pEquipBase);
    if(!pItemEquip)
    {
        CnWarn("Equip not exist\n"); // 强化的装备不存在
        return;
    }

    const EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT* pEnchantStruct = ItemConfigManager::Instance()->FindEnchantData(recvMsg->attribute_id);
    if (!pEnchantStruct)
    {
        CnAssert(false); // 未找到相关配置
        return;
    }

    if ((pItemEquip->GetEnchantAttributeList().size() > 0) && 
        (pItemEquip->GetEnchantAttributeList()[0] == pEnchantStruct->attribute_id))
    {
        CnWarn("Same enchant attribute"); // 附魔属性相同
        return;
    }

    const EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT* modifierStruct = ItemConfigManager::Instance()->FindEquipAttributeTpltById(pEnchantStruct->attribute_id);
    if (modifierStruct->type != 2)
    {
        CnAssert(false); // 只有类型为2才是附魔属性条目
        return;
    }

    uint32 equipClass = pItemEquip->GetSubType(); // 装备类型
    const std::vector<uint32>* prop_list = ItemConfigManager::Instance()->FindCanPropIdListByEquipClass(equipClass);
    if (prop_list->end() == std::find(prop_list->begin(), prop_list->end(), modifierStruct->prop_id))
    {
        CnAssert(false); // 装备与属于条目不匹配
        return;
    }

    if (false == m_player->GetItemManager()->CheckItemNum(pEnchantStruct->item1_id, pEnchantStruct->item1_num) ||
        false == m_player->GetItemManager()->CheckItemNum(pEnchantStruct->item2_id, pEnchantStruct->item2_num) ||
        false == m_player->GetItemManager()->CheckItemNum(pEnchantStruct->item3_id, pEnchantStruct->item3_num) ||
        false == m_player->GetItemManager()->CheckItemNum(pEnchantStruct->item4_id, pEnchantStruct->item4_num))
    {
        CnWarn("Equip enchant material not enough"); // 材料不足
        return;
    }

    // 消耗材料
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_ENCHANT, pEnchantStruct->item1_id, pEnchantStruct->item1_num);
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_ENCHANT, pEnchantStruct->item2_id, pEnchantStruct->item2_num);
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_ENCHANT, pEnchantStruct->item3_id, pEnchantStruct->item3_num);
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_EQUIP_ENCHANT, pEnchantStruct->item4_id, pEnchantStruct->item4_num);

    // 设置一条且永远只有一条附魔属性
    pItemEquip->ClearEnchantAttributeList();
    pItemEquip->AddEnchantAttribute(pEnchantStruct->attribute_id);

    SendStrengthenAck(MSG_EQUIP_ENCHNAT_ACK, &sendMsg, pItemEquip, true);
}

void EquipStrengthenManager::SendStrengthenAck(uint32 msgID, ITEM_STRENGTHEN_ACK* sendMsg, ItemEquip* pItemEquip, bool needCalcProperty)
{
    ITEM_EQUIP itemEquip;
    pItemEquip->SaveItem(&itemEquip);
    sendMsg->item_info.equip_list.push_back(itemEquip); // 实时返回给前台

    GetItemMgr()->NtfOptItemUpdate(pItemEquip, 1);

    if (needCalcProperty)
    {
        CalcProperty(pItemEquip->GetPlace());
    }

    m_player->SendToGate(msgID, sendMsg);
}


void EquipStrengthenManager::CalcProperty(uint32 equipPlace) // 重新计算人物属性
{
    if (equipPlace == PLACE_ROLE_EQUIP)
    {
        m_player->GetEquipMgr()->SetPropertyDirty();        // 角色装备属性重新计算
        m_player->CalcProperty();
    }
    else if (equipPlace == PLACE_SOLDIER_EQUIP)
    {
        m_player->GetPetMgr()->GetSoldier()->GetEquipMgr()->SetPropertyDirty();       // 佣兵的装备属性重新计算
        m_player->GetPetMgr()->GetSoldier()->CalcProperty();
    }
}