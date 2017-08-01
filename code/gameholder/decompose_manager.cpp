#include "gameholder_pch.h"
#include "decompose_manager.h"
#include "player.h"
#include "system_config_manager.h"
#include "item_factory.h"
#include "item_manager.h"

ItemManager* DecomposeManager::GetItemMgr()
{
    return m_player->GetItemManager();
}

DecomposeManager::DecomposeManager(Player* player) :
m_player(player)
{
}

DecomposeManager::~DecomposeManager(void)
{
}

void DecomposeManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_DECOMPOSE_ITEM_REQ:
        {
            OnDecomposeReq(data);
        }
        break;
    default:
        break;
    }
}

void DecomposeManager::MergeMaterial(std::vector<ITEM_MATERIAL>& itemList, const ITEM_MATERIAL& item)
{
    for (size_t i=0; i<itemList.size(); ++i)
    {
        if (itemList[i].base.id == item.base.id && itemList[i].base.quality == item.base.quality)
        {
            itemList[i].base.num += item.base.num;
            return;
        }
    }

    itemList.push_back(item);
}

void DecomposeManager::OnDecomposeReq(void* data)
{
    if (!data)
    {
        CnAssert(false);
        return;
    }

    DECOMPOSE_INFO_REQ* pRecvMsg = (DECOMPOSE_INFO_REQ*)data;

    ItemBase* pItem(NULL);
    ItemStone* pStone(NULL);
    ItemEquip* pEquip(NULL);
    ItemMaterial* pMaterial(NULL);
    const DECOMPOSE_ROOT_FORMULA_STRUCT* pConfig(NULL);
    uint32 quality(0);
    std::set<ItemBase*> items;
    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    for (size_t i=0; i<pRecvMsg->items.size(); ++i)
    {
        ITEM_BASE& item = pRecvMsg->items[i];
        pItem = GetItemMgr()->GetPlaceItem(item.place, item.pos);
        if (!pItem)
        {
            CnWarn("Item place=%d pos=%d unfound item!\n", item.place, item.pos);
            continue;
        }
        switch(pItem->GetItemType())
        {
        case ITEM_TYPE_STONE:
            {
                pStone = CnDynamicCast(ItemStone, pItem);
                if (!pStone)
                {
                    CnAssert(false);
                    continue;
                }

                quality = pStone->GetQuality();
            }
            break;
        case ITEM_TYPE_EQUIP:
            {
                pEquip = CnDynamicCast(ItemEquip, pItem);
                if (!pEquip)
                {
                    CnAssert(false);
                    continue;
                }

                quality = pEquip->GetQuality();
            }
            break;
        default:
            CnWarn("Item id=%d type=%d error item type!\n", pItem->GetId(), pItem->GetItemType());
            continue;
        }

        if (items.find(pItem) != items.end())
        {
            CnWarn("Item id=%d type=%d error same item!\n", pItem->GetId(), pItem->GetItemType());
            continue;
        }

        items.insert(pItem);

        pConfig = SystemConfigManager::Instance()->GetDecomposeFormula(pItem->GetItemType(), quality);
        if (!pConfig)
        {
            CnWarn("Item id=%d type=%d quality=%d unfound decompose formula!\n", pItem->GetId(), pItem->GetItemType(), quality);
            continue;
        }
        
        if (pConfig->item1_id > 0 && pConfig->item1_num > 0)
        {
            rewardItem.Clear();
            rewardItem.id = pConfig->item1_id;
            rewardItem.num = pConfig->item1_num;
            rewardTable.items.push_back(rewardItem);
        }
        
        if (pConfig->item2_id > 0 && pConfig->item2_num > 0)
        {
            rewardItem.Clear();
            rewardItem.id = pConfig->item2_id;
            rewardItem.num = pConfig->item2_num;
            rewardTable.items.push_back(rewardItem);
        }

        if (pConfig->item3_id > 0 && pConfig->item3_num > 0)
        {
            rewardItem.Clear();
            rewardItem.id = pConfig->item3_id;
            rewardItem.num = pConfig->item3_num;
            rewardTable.items.push_back(rewardItem);
        }

        if (pConfig->item4_id > 0 && pConfig->item4_num > 0)
        {
            rewardItem.Clear();
            rewardItem.id = pConfig->item4_id;
            rewardItem.num = pConfig->item4_num;
            rewardTable.items.push_back(rewardItem);
        }
    }

    if (GetItemMgr()->CanAddRewardTable(&rewardTable))
    {
        OnDecomposeAck(ERR_SUCCEED, items, rewardTable);
    }
    else
    {
        OnDecomposeAck(ERR_FAILED, items, rewardTable);
    }
}

void DecomposeManager::OnDecomposeAck(uint16 errCode, std::set<ItemBase*>& items, REWARD_TABLE& rewardTable)
{
    DECOMPOSE_INFO_ACK sendMsg;
    ITEM_MATERIAL material;

    if (ERR_SUCCEED != errCode)
    {
        sendMsg.errcode = ERR_FAILED;
        goto _END;
    }

    if (items.empty())
    {
        sendMsg.errcode = ERR_FAILED;
        goto _END;
    }

    for (std::set<ItemBase*>::iterator mIter=items.begin(); mIter!=items.end(); ++mIter)
    {
        GetItemMgr()->RemoveItemNum(REASON_ITEM_SUB_DEMPOSE, (*mIter)->GetNum(), (*mIter)->GetPlace(), (*mIter)->GetPos());
    }

    GetItemMgr()->AddRewardTable(&rewardTable);

    for (std::vector<REWARD_ITEM>::const_iterator cIter = rewardTable.items.begin();
        cIter != rewardTable.items.end(); ++cIter)
    {
        material.Clear();
        material.base.id = cIter->id;
        material.base.num = cIter->num;
        MergeMaterial(sendMsg.materials, material);
    }

    sendMsg.errcode = ERR_SUCCEED;

_END:
    m_player->SendToGate(MSG_DECOMPOSE_ITEM_ACK, &sendMsg);
}
