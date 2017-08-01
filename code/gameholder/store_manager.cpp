#include "gameholder_pch.h"
#include "store_manager.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "game_util.h"
#include "player.h"

ItemManager* StoreManager::GetItemMgr()
{
    return m_player->GetItemManager();
}

StoreManager::StoreManager(Player* player):
m_player(player)
{
    Clear();

    for (uint32 i = 0; i < ITEM_STORE_MAX_GRID_NUM; ++i)
    {
        m_ItemGrid.push_back(NULL);
    }
}

StoreManager::~StoreManager(void)
{
    Clear();
}

void StoreManager::Clear()
{
    m_ItemGrid.clear();
}

// ==================================================
// 加载仓库信息
// ==================================================
void StoreManager::LoadstoreInfo(const ITEM_INFO& info)
{
    ItemBase* pItem(NULL);

    // 加载装备
    for(std::vector<ITEM_EQUIP>::const_iterator cEquipIter = info.equip_list.begin(); cEquipIter != info.equip_list.end(); ++cEquipIter)
    {
        const ITEM_EQUIP& item = *cEquipIter;
        if (!IsEmptyGrid(item.base.pos))
        {
            CnAssert(false);
            continue;
        }

        pItem = ItemFactory::CreateItem(item.base.id, ITEM_TYPE_EQUIP);
        if (!pItem)
        {
            continue;
        }

        pItem->LoadItem(&item);
        pItem->SetPlace(PLACE_BAG);

        AddToGrid(pItem);
        GetItemMgr()->AddToItemList(pItem);
    }
    // 加载消耗品
    std::vector<ITEM_CONSUME>::const_iterator cConsumeIter = info.consume_list.begin();
    for (; cConsumeIter != info.consume_list.end(); ++cConsumeIter)
    {
        const ITEM_CONSUME& consume = *cConsumeIter;
        if (!IsEmptyGrid(consume.base.pos))
        {
            CnAssert(false);
            continue;
        }

        pItem = ItemFactory::CreateConsume(consume.base.id);
        if (!pItem)
        {
            continue;
        }

        pItem->LoadItem(&consume);
        pItem->SetPlace(PLACE_STORE);

        AddToGrid(pItem);
        GetItemMgr()->AddToItemList(pItem);
    }
    // 加载材料
    std::vector<ITEM_MATERIAL>::const_iterator cMaterialIter = info.material_list.begin();
    for (; cMaterialIter != info.material_list.end(); ++cMaterialIter)
    {
        const ITEM_MATERIAL & material = *cMaterialIter;
        if (!IsEmptyGrid(material.base.pos))
        {
            CnAssert(false);
            continue;
        }

        pItem = ItemFactory::CreateMaterial(material.base.id);
        if (!pItem)
        {
            continue;
        }

        pItem->LoadItem(&material);
        pItem->SetPlace(PLACE_STORE);

        AddToGrid(pItem);
        GetItemMgr()->AddToItemList(pItem);
    }

    // 加载石头
    std::vector<ITEM_STONE>::const_iterator cStoneIter = info.stone_list.begin();
    for(; cStoneIter != info.stone_list.end(); ++cStoneIter)
    {
        const ITEM_STONE& stone = *cStoneIter;
        if(!IsEmptyGrid(stone.base.pos))
        {
            CnAssert(false);
            continue;
        }

        pItem = ItemFactory::CreateStone(stone.base.id);
        if(!pItem)
        {
            continue;
        }

        pItem->LoadItem(&stone);
        pItem->SetPlace(PLACE_STORE);

        AddToGrid(pItem);
        GetItemMgr()->AddToItemList(pItem);
    }
}

// ==================================================
// 填充返回的物品信息
// ==================================================
void StoreManager::FillStoreInfo(ITEM_INFO& info)
{
    info.Clear();

    ITEM_EQUIP equip;
    ITEM_CONSUME consume;
    ITEM_MATERIAL material;
    ITEM_STONE stone;

    const ItemBase* pItem = NULL;

    for (ItemGrid::const_iterator cIter = GetItemGrid().begin(); cIter != GetItemGrid().end(); ++cIter)
    {
        pItem = *cIter;
        if (!pItem)
        {
            continue;
        }

        switch(pItem->GetItemType())
        {
        case ITEM_TYPE_EQUIP:
            {
                equip.Clear();
                pItem->SaveItem(&equip);
                info.equip_list.push_back(equip);
            }
            break;
        case ITEM_TYPE_CONSUME:
            {
                consume.Clear();
                pItem->SaveItem(&consume);
                info.consume_list.push_back(consume);
            }
            break;
        case ITEM_TYPE_MATERIAL:
            {
                material.Clear();
                pItem->SaveItem(&material);
                info.material_list.push_back(material);
            }
            break;
        case ITEM_TYPE_STONE:
            {
                stone.Clear();
                pItem->SaveItem(&stone);
                info.stone_list.push_back(stone);
            }
            break;
        default:
            CnAssert(false);
            continue;
        }
    }
}

void StoreManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_STORE_INFO_REQ:
        {
            OnItemInfoReq(data);
        }
        break;
    case MSG_STORE_SORT_REQ:
        {
            OnItemSortReq(data);
        }
        break;
    default:
        break;
    }
}


uint16 StoreManager::AddItem(uint32 reason, ItemBase* pItem, int32 pos)
{
    if (!pItem)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (pItem->GetNum() < 1)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (NULL_GRID == pos)
    {
        pos = FindEmptyGrid();
        if (NULL_GRID == pos)
        {
            return ERR_FAILED;
        }

        if (pItem->GetMaxStackNum() > 1)
        {
            for (size_t i=0; i < ITEM_STORE_MAX_GRID_NUM; ++i)
            {
                ItemBase* pFind = m_ItemGrid[i];
                if (!pFind)
                {
                    continue;
                }

                if (!IsCanStack(pItem, pFind))
                {
                    continue;
                }

                StackItem(pItem, pFind);

                if (pItem->GetNum() <= 0)
                {
                    GetItemMgr()->DestoryItem(pItem);
                    return ERR_SUCCEED;
                }
            }
        }
    }


    ItemBase* pDst = GetItemByPos(pos);
    if (pDst)
    {
        // 加进来的不是仓库内的东西，终点就不应该有东西
        if (pItem->GetPlace() != PLACE_STORE && pItem->GetPlace() != PLACE_BAG)
        {
            CnAssert(false);
            return ERR_FAILED;
        }

        // 进行叠加判断
        if (IsCanStack(pItem, pDst))
        {
            StackItem(pItem, pDst);
            GetItemMgr()->NtfOptItemUpdate(pDst, pDst->GetNum());
            return ERR_SUCCEED;
        }

        // 进行交换
        int16 errCode = SwapItem(pItem->GetPlace(), pItem->GetPos(), pDst->GetPlace(), pDst->GetPos());
        if (ERR_SUCCEED == errCode)
        {
            return ERR_SUCCEED_SWAP;
        }

        CnAssert(false);
        return ERR_FAILED;
    }

    CnAssert(pItem->GetNum() > 0);

    const uint32 nPlace = pItem->GetPlace();

    pItem->SetPos(pos);
    AddToGrid(pItem);
    if (PLACE_BAG != nPlace)
    {
        GetItemMgr()->NtfOptItemAdd(pItem);
    }

    return ERR_SUCCEED;
}

// ==================================================
// 删除物品
// ==================================================
void StoreManager::SubItem(uint32 reason, int32 pos, int32 num)
{
    // 不存在的格子
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return;
    }

    ItemBase* pItem = m_ItemGrid[pos];

    // 空格子
    if(!pItem) 
    {
        return;
    }

    if (num < 1)
    {
        return;
    }

    pItem->SubNum(num);
    GetItemMgr()->NtfOptItemSub(pItem, num);

    if (pItem->GetNum() > 0)
    {
        return;
    }

    m_ItemGrid[pos] = NULL;
    GetItemMgr()->DestoryItem(pItem);
}

int32 StoreManager::GetEmptyGridNum() const
{
    int32 num(0);

    for (uint32 i = 0; i < GetItemGrid().size(); ++i)
    {
        if (!GetItemGrid()[i])
        {
            ++num;
        }
    }

    return num;
}

int32 StoreManager::FindEmptyGrid() const
{
    for (uint32 i = 0; i < GetItemGrid().size(); ++i)
    {
        if (!GetItemGrid()[i])
        {
            return i;
        }
    }

    return NULL_GRID;
}

bool StoreManager::IsEmptyGrid( int32 pos ) const
{
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return false;
    }

    return (NULL == GetItemGrid()[pos]);
}

bool StoreManager::IsValidGrid(int32 pos) const
{
    return (pos >= 0 && pos < ITEM_STORE_MAX_GRID_NUM);
}

bool StoreManager::IsCanStack(ItemBase * pSrc, ItemBase * pDst) const
{
    if (!pSrc || !pDst)
    {
        CnAssert(false);
        return false;
    }

    if (pSrc->GetItemType() != pDst->GetItemType())
    {
        return false;
    }
    if (pSrc->GetId() != pDst->GetId())
    {
        return false;
    }

    if (pDst->GetCanStackNum() <= 0)
    {
        return false;
    }

    return true;
}

ItemBase* StoreManager::GetItemByPos( int32 pos )
{
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return NULL;
    }

    return m_ItemGrid[pos];
}

ItemBase* StoreManager::GetItemById(int32 id)
{
    ItemBase * pTemp(NULL);
    for (size_t i=0; i<ITEM_STORE_MAX_GRID_NUM; ++i)
    {
        pTemp = m_ItemGrid[i];
        if (!pTemp)
        {
            continue;
        }

        pTemp = m_ItemGrid[i];

        if (id == pTemp->GetId())
        {
            return pTemp;
        }
       
    }

    return NULL;

    /*
    for (auto Item : m_ItemGrid)
    {
       
    }*/

    /*
    for(ItemList::iterator mIter = m_AllItemList.begin(); mIter != m_AllItemList.end(); ++mIter)
    {
        ItemBase* pItem = *mIter;
    }
    */
}

void StoreManager::AddToGrid( ItemBase * pItem, bool bSwap )
{
    CnAssert(pItem);
    CnAssert(IsValidGrid(pItem->GetPos()));
    CnAssert(pItem->GetNum() > 0);
    if (!bSwap)
    {
        CnAssert(!GetItemGrid()[pItem->GetPos()]);
    }

    pItem->SetPlace(PLACE_STORE);
    m_ItemGrid[pItem->GetPos()] = pItem;
}

void StoreManager::StackItem(ItemBase * pSrc, ItemBase * pDst)
{
    if (!pSrc || !pDst)
    {
        CnAssert(false);
        return;
    }

    int32 nMove = Crown::SDMin(pSrc->GetNum(), pDst->GetCanStackNum());
    if (nMove <= 0 || nMove >= pDst->GetMaxStackNum())
    {
        CnAssert(false);
        return;
    }

    pDst->AddNum(nMove);
    pSrc->SubNum(nMove);

    if (pSrc->GetPlace() != PLACE_BAG)
    {
        GetItemMgr()->NtfOptItemAdd(pDst, nMove);
    }
}

uint16 StoreManager::SwapItem(uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos)
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

    return ERR_SUCCEED;
}

uint16 StoreManager::SellItem(int32 pos)
{
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemBase* pItem = GetItemByPos(pos);
    if (!pItem)
    {
        return ERR_FAILED;
    }

    SubItem(0, pos, pItem->GetNum());

    return ERR_SUCCEED;
}

bool StoreManager::SortItem(ItemBase* pItemA, ItemBase* pItemB)
{
    if (!pItemA && !pItemB)
    {
        return false;
    }

    if (!pItemA)
    {
        return true;
    }

    if (!pItemB)
    {
        return false;
    }

    if (pItemA->GetId() > pItemB->GetId())
    {
        return true;
    }
    if (pItemA->GetId() < pItemB->GetId())
    {
        return false;
    }

    switch (pItemA->GetItemType())
    {
    case ITEM_TYPE_EQUIP:
        {
            ItemEquip* pEquipA = CnDynamicCast(ItemEquip, pItemA);
            ItemEquip* pEquipB = CnDynamicCast(ItemEquip, pItemB);

            if (pEquipA->GetQuality() < pEquipB->GetQuality())
            {
                return true;
            }
            if (pEquipA->GetQuality() > pEquipB->GetQuality())
            {
                return false;
            }
        }
        break;
    case ITEM_TYPE_CONSUME:
    case ITEM_TYPE_MATERIAL:
        {
        }
        break;
    case ITEM_TYPE_STONE:
        {
            ItemStone* pStoneA = CnDynamicCast(ItemStone, pItemA);
            ItemStone* pStoneB = CnDynamicCast(ItemStone, pItemB);

            if (pStoneA->GetQuality() < pStoneB->GetQuality())
            {
                return true;
            }
            if (pStoneA->GetQuality() > pStoneB->GetQuality())
            {
                return false;
            }

            if (pStoneA->GetLevel() < pStoneB->GetLevel())
            {
                return true;
            }
            if (pStoneA->GetLevel() > pStoneB->GetLevel())
            {
                return false;
            }

            if (pStoneA->GetExp() < pStoneB->GetExp())
            {
                return true;
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    return false;
}

void StoreManager::OnItemInfoReq(void* data)
{
    SendItemInfo();
}

void StoreManager::OnItemSortReq(void* data)
{
    ItemBase * pTemp(NULL);
    for (size_t i=0; i<ITEM_STORE_MAX_GRID_NUM; ++i)
    {
        for (size_t j=i+1; j<ITEM_STORE_MAX_GRID_NUM; ++j)
        {
            if (SortItem(m_ItemGrid[i], m_ItemGrid[j]))
            {
                pTemp = m_ItemGrid[i];

                m_ItemGrid[i] = m_ItemGrid[j];
                if (m_ItemGrid[i])
                {
                    m_ItemGrid[i]->SetPos(i);
                }

                m_ItemGrid[j] =pTemp;
                if (m_ItemGrid[j])
                {
                    m_ItemGrid[j]->SetPos(j);
                }
            }
        }
    }

    SendItemInfo();
}

void StoreManager::SendItemInfo()
{
    ITEM_INFO itemInfo;
    FillStoreInfo(itemInfo);
    m_player->SendToGate(MSG_STORE_INFO_ACK, &itemInfo);
}