#include "gameholder_pch.h"
#include "alchemy_manager.h"
#include "item_config_manager.h"
#include "item_consume.h"
#include "item_material.h"
#include "item_stone.h"
#include "item_factory.h"
#include "player.h"
#include "bag_manager.h"
#include "item_manager.h"
#include "drop_ruler.h"
#include "shop_manager.h"
#include "shop_service_manager.h"
#include "equip_manager.h"

ItemManager* AlchemyManager::GetItemMgr()
{
    return m_player->GetItemManager();
}

BagManager* AlchemyManager::GetBagMgr()
{
    return m_player->GetBagMgr();
}

AlchemyManager::AlchemyManager(Player* player) :
m_player(player)
{
}

AlchemyManager::~AlchemyManager(void)
{
}

void AlchemyManager::OnRecv(uint32 msgID, void* data)
{

    switch(msgID)
    {
        case MSG_ALCHEMY_MAKE_INFO_REQ: // 制作物品
            OnMakeItem(data);
            break;

        default:
            break;
    }
}

void AlchemyManager::OnMakeItem(void* data)
{
    ALCHEMY_MAKE_ITEM* recvMsg = (ALCHEMY_MAKE_ITEM*)data;
    ALCHEMY_MAKE_INFO_ACK sendMsg;

    // 获取消耗的物品信息
    const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct = AlchemyConfigManager::Instance()->FindManorMakeDstItem(recvMsg->id);
    if (!alchemyStruct)
    {
        CnAssert(false);
        return;
    }

    // 生成物品的数量
    uint32 num = recvMsg->num;
    if (num <= 0)
    {
        CnAssert(false);
        return;
    }

    // 检查生成的物品需要的背包空间（其实应该先算消耗物品后的背包空间，不过较麻烦，现在先这样处理）
    std::map<uint32, uint32> targetItems;
    targetItems[alchemyStruct->dst_item_id] = num;
    if (false == m_player->GetItemManager()->CanAddToBag(&targetItems, NULL))
    {
        sendMsg.errcode = ERR_NO_ENOUGH_BAGPOS; // 背包空间不足
        m_player->SendToGate(MSG_ALCHEMY_MAKE_INFO_ACK, &sendMsg);
        return;
    }

    // 需要消耗的物品
    std::map<uint32, uint32> needItems;
    
    if (alchemyStruct->item1_id > 0 && alchemyStruct->item1_num > 0) 
        needItems[alchemyStruct->item1_id] = alchemyStruct->item1_num * num;
    
    if (alchemyStruct->item2_id > 0 && alchemyStruct->item2_num > 0) 
        needItems[alchemyStruct->item2_id] = alchemyStruct->item2_num * num;
    
    if (alchemyStruct->item3_id > 0 && alchemyStruct->item3_num > 0) 
        needItems[alchemyStruct->item3_id] = alchemyStruct->item3_num * num;
    
    if (alchemyStruct->item4_id > 0 && alchemyStruct->item4_num > 0) 
        needItems[alchemyStruct->item4_id] = alchemyStruct->item4_num * num;

    // 自动补全
    std::map<uint32, uint32> autoFillItems;
    for (std::map<uint32, uint32>::iterator iter = needItems.begin(); iter != needItems.end(); iter++)
    {
        int32 currentItemCount = m_player->GetItemManager()->GetItemNum(iter->first);
        int32 autoFillCount = iter->second - currentItemCount;
        if (autoFillCount > 0)
        {
            autoFillItems[iter->first] = autoFillCount;
        }
    }
    if (!autoFillItems.empty())
    {
        if (false == m_player->GetShopManager()->ListIsEnoughMoney(&autoFillItems))
        {
            sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT; // 钻石不足
            m_player->SendToGate(MSG_ALCHEMY_MAKE_INFO_ACK, &sendMsg);
            return; 
        }
    }

    // 自动补全花费钻石
    if (!autoFillItems.empty())
    {
        if (false == m_player->GetShopManager()->DoListMallBuy(&autoFillItems, BUY_REASON_NORMAL))
        {
            sendMsg.errcode = ERR_FAILED;   // 购买失败
            m_player->SendToGate(MSG_ALCHEMY_MAKE_INFO_ACK, &sendMsg);
            return; 
        }
    }

    // 消耗材料
    for (std::map<uint32, uint32>::iterator iter = needItems.begin(); iter != needItems.end(); iter++)
    {
        int32 itemNum = m_player->GetItemManager()->GetItemNum(iter->first);
        int32 needItemNum = iter->second;
        if (itemNum > 0 && needItemNum > 0)
        {
            int32 removeItemNum = Crown::SDMin(itemNum, needItemNum);
            m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_ALCHEMY, iter->first, removeItemNum);
        }
    }

    // 根据条件生成指定物品
    uint32 ItemId = alchemyStruct->dst_item_id;
    uint32 ItemType = ItemConfigManager::Instance()->GetItemTypeById(ItemId);
    bool success = false;
    switch(ItemType)
    {
        case ITEM_TYPE_EQUIP:    // 装备
            success = MakeEquip(alchemyStruct, num, &sendMsg);
            break;

        case ITEM_TYPE_CONSUME:  // 消耗品
            success = MakeConsume(alchemyStruct, num, &sendMsg);
            break;

        case ITEM_TYPE_MATERIAL: // 材料
            success = MakeMaterial(alchemyStruct, num, &sendMsg);
            break;

        case ITEM_TYPE_STONE:    // 石头
            success = MakeStone(alchemyStruct, num, &sendMsg);
            break;

        default:
            CnAssert(false);
            break;
    }

    if (!success && sendMsg.errcode == 0)
    {
        sendMsg.errcode = ERR_FAILED;
    }
    m_player->SendToGate(MSG_ALCHEMY_MAKE_INFO_ACK, &sendMsg);
}

bool AlchemyManager::MakeEquip(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg)
{
    for (uint32 i = 0; i < pNum; i++)
    {
        ItemEquip* pEquip = ItemFactory::CreateEquip(alchemyStruct->dst_item_id);
        if (!pEquip)
        {
            CnAssert(false);
            continue;
        }

        pEquip->SetNum(1);
        pEquip->SetQuality(alchemyStruct->dst_quality);
        EquipManager::SetEquipRandom(pEquip);

        m_player->GetItemManager()->AddToBag(REASON_ITEM_ADD_ALCHEMY, pEquip);

        ITEM_EQUIP itemEquip;
        pEquip->SaveItem(&itemEquip);
        sendMsg->item_info.equip_list.push_back(itemEquip); // 实时返回给前台
    }
    
    return true;
}

bool AlchemyManager::MakeMaterial(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg)
{
    int32 tempNum = pNum;
    while(tempNum>0)
    {
        ItemMaterial* pMaterial = ItemFactory::CreateMaterial(alchemyStruct->dst_item_id);
        if(!pMaterial)
        {
            CnAssert(false);
            continue;
        }
        if(tempNum > pMaterial->GetMaxStackNum())
        {
            tempNum -= pMaterial->GetMaxStackNum();
            pMaterial->SetNum(pMaterial->GetMaxStackNum());
        }
        else
        {
            pMaterial->SetNum(tempNum);
            tempNum = 0;
        }

        ITEM_MATERIAL itemMaterial;
        pMaterial->SaveItem(&itemMaterial);
        sendMsg->item_info.material_list.push_back(itemMaterial); // 实时返回给前台,只用来显示，因为跟背包中的位置和数量可能是不匹配的

        m_player->GetItemManager()->AddToBag(REASON_ITEM_ADD_ALCHEMY, pMaterial);
    }

    return true;
}

bool AlchemyManager::MakeConsume(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg)
{
    int32 tempNum = pNum;
    while(tempNum>0)
    {
        ItemConsume* pConsume = ItemFactory::CreateConsume(alchemyStruct->dst_item_id);
        if(!pConsume)
        {
            CnAssert(false);
            continue;
        }
        if(tempNum > pConsume->GetMaxStackNum())
        {
            tempNum -= pConsume->GetMaxStackNum();
            pConsume->SetNum(pConsume->GetMaxStackNum());
        }
        else
        {
            pConsume->SetNum(tempNum);
            tempNum = 0;
        }

        ITEM_CONSUME itemConsume;
        pConsume->SaveItem(&itemConsume);
        sendMsg->item_info.consume_list.push_back(itemConsume); // 实时返回给前台,只用来显示，因为跟背包中的位置和数量可能是不匹配的

        m_player->GetItemManager()->AddToBag(REASON_ITEM_ADD_ALCHEMY, pConsume);
    }

    return true;
}

bool AlchemyManager::MakeStone(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg)
{
    for (uint32 i = 0; i < pNum; i++)
    {
        ItemStone* pMakeItem = ItemFactory::CreateStone(alchemyStruct->dst_item_id);
        if (!pMakeItem)
        {
            CnAssert(false);
            continue;
        }

        pMakeItem->SetNum(1);
        pMakeItem->SetQuality(alchemyStruct->dst_quality);

        m_player->GetItemManager()->AddToBag(REASON_ITEM_ADD_ALCHEMY, pMakeItem);

        ITEM_STONE itemStone;
        pMakeItem->SaveItem(&itemStone);
        sendMsg->item_info.stone_list.push_back(itemStone); // 实时返回给前台
    }

    return true;
}