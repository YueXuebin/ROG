#include "gameholder_pch.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "player.h"
#include "game_util.h"
#include "bag_manager.h"
#include "store_manager.h"
#include "equip_manager.h"
#include "skill_info_manager.h"
#include "pet_manager.h"
#include "soldier.h"
#include "soldier_talent_manager.h"
#include "game_define.h"
#include "random_package_manager.h"
#include "equip_manager.h"
#include "item_controller.h"

BagManager* ItemManager::GetBagMgr()
{
    return m_player->GetBagMgr();
}

StoreManager* ItemManager::GetStoreMgr()
{
    return m_player->GetStoreMgr();
}

EquipManager* ItemManager::GetEquipMgr()
{
    return m_player->GetEquipMgr();
}

SkillInfoManager* ItemManager::GetSkillInfoMgr()
{
    return m_player->GetSkillInfoMgr();
}

EquipManager* ItemManager::GetSoldierEquipMgr()
{
    return m_player->GetPetMgr()->GetSoldier()->GetEquipMgr();
}

SkillInfoManager* ItemManager::GetSoldierSkillInfoMgr()
{
    return m_player->GetPetMgr()->GetSoldier()->GetSkillInfoMgr();
}

SoldierTalentManager* ItemManager::GetSoldierTalentMgr()
{
    return m_player->GetPetMgr()->GetSoldier()->GetSoldierTalentMgr();
}

ItemController* ItemManager::GetItemController()
{
    return m_player->GetItemController();
}

ItemManager::ItemManager(Player* player):
m_player(player)
{
    m_AllItemList.clear();
}

ItemManager::~ItemManager(void)
{
    Clear();
}

void ItemManager::Clear()
{
    for(ItemList::iterator mIter = m_AllItemList.begin(); mIter != m_AllItemList.end(); ++mIter)
    {
        ItemBase* pItem = *mIter;
        ItemFactory::DestoryItem(pItem);
    }
    m_AllItemList.clear();
}

void ItemManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_OPT_ITEM_REQ:
        {
            OnOptItemReq(data);
        }
        break;
    default:
        break;
    }
}

void ItemManager::Update()
{
    ForceUpdate();
}

void ItemManager::ForceUpdate()
{
    if (!IsDirty())
    {
        return;
    }

    SetDirty(false);

    Notify(m_OptItemInfoNtf);

    m_OptItemInfoNtf.Clear();
}

ItemBase* ItemManager::GetPlaceItem(uint8 place, int32 pos)
{
    switch(place)
    {
    case PLACE_BAG:
        {
            return GetBagMgr()->GetItemByPos(pos);
        }
        break;
    case PLACE_ROLE_EQUIP:
        {
            return GetEquipMgr()->GetEquipByPos(pos);
        }
        break;
    case PLACE_ROLE_SKILL:
        {
            return GetSkillInfoMgr()->GetStoneByPos(pos);
        }
        break;
    case PLACE_SOLDIER_EQUIP:
        {
            return GetSoldierEquipMgr()->GetEquipByPos(pos);
        }
        break;
    case PLACE_SOLDIER_SKILL:
        {
            return GetSoldierSkillInfoMgr()->GetStoneByPos(pos);
        }
        break;
    case PLACE_SOLDIER_TALENT:
        {
            return GetSoldierTalentMgr()->GetStoneByPos(pos);
        }
        break;
    case PLACE_STORE:
        {
            return GetStoreMgr()->GetItemByPos(pos);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    return NULL;
}

ItemBase* ItemManager::GetPlaceItemById(uint8 place, uint32 itemId)
{   
    switch(place)
    {
        case PLACE_BAG:
            {
                return GetBagMgr()->GetItemById(itemId);
            }
            break;
        default:
            CnAssert(false);
            break;
    }
    return NULL;
}

void ItemManager::GetPlaceItemListById(uint8 place, uint32 itemId, std::vector<ItemBase*>& outList)
{
    switch(place)
    {
    case PLACE_BAG:
        {
            GetBagMgr()->GetItemListById(itemId, outList);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

uint16 ItemManager::CheckDstPlace(ItemBase* pSrc, uint8 dstPlace, int32 dstPos, int32 num)
{
    if (!pSrc)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    // 只检查合法性

    switch(dstPlace)
    {
    case PLACE_BAG:
        {
            if (GetBagMgr()->IsValidGrid(dstPos))
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_ROLE_EQUIP:
        {
            if(!pSrc->IsKindOf(&ItemEquip::ms_RTTI))
            {
                return ERR_FAILED;
            }
            ItemEquip* pEquip = CnDynamicCast(ItemEquip, pSrc);

            if (!ItemConfigManager::CheckEquipTypeWithPos(pEquip->GetPartType(), dstPos))
            {
                return ERR_FAILED;
            }

            if (m_player->GetLevel() >= pEquip->GetEqpLevel())
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_SOLDIER_EQUIP:
        {
            if(!pSrc->IsKindOf(&ItemEquip::ms_RTTI))
            {
                return ERR_FAILED;
            }
            ItemEquip* pEquip = CnDynamicCast(ItemEquip, pSrc);

            if (!ItemConfigManager::CheckEquipTypeWithPos(pEquip->GetPartType(), dstPos))
            {
                return ERR_FAILED;
            }

            if (m_player->GetPetMgr()->GetSoldier()->GetLevel() >= pEquip->GetEqpLevel())
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_ROLE_SKILL:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                return ERR_FAILED;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (GetSkillInfoMgr()->CheckStonePosWithType(dstPos, pStone->GetSubType()))
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_SOLDIER_SKILL:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                return ERR_FAILED;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (GetSoldierSkillInfoMgr()->CheckStonePosWithType(dstPos, pStone->GetSubType()))
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_SOLDIER_TALENT:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                return ERR_FAILED;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (GetSoldierTalentMgr()->CheckStoneTypeAndPos(pStone->GetSubType(), dstPos))
            {
                return ERR_SUCCEED;
            }
        }
        break;
    case PLACE_STORE:
        {
            if (GetStoreMgr()->IsValidGrid(dstPos))
            {
                return ERR_SUCCEED;
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    return ERR_FAILED;
}

uint32 ItemManager::GetItemNum(int32 itemId)
{
    std::vector<ItemBase*> itemBaseList;

    GetPlaceItemListById(PLACE_BAG, itemId, itemBaseList);

    if (itemBaseList.empty())
    {
        return 0;
    }

    // 指定物品在背包内的数量之和
    uint32 bagItemNumSum = 0;
    for (uint32 j = 0; j < itemBaseList.size(); j++)
    {
        ItemBase* pItemBase = itemBaseList[j];
        bagItemNumSum += pItemBase->GetNum();
    }

    return bagItemNumSum;
}

bool ItemManager::CheckItemNum(int32 itemId, int32 allNum)
{
    if(itemId <= 0 || allNum == 0)
        return true;

    uint32 bagItemNumSum = GetItemNum(itemId);
    if((int32)bagItemNumSum < allNum)
    {
        return false; // 背包里面的物品小于需要消耗的物品数量
    }

    return true;
}

void ItemManager::FindDstPlace(ItemBase* pSrc, uint8 dstPlace, int32& dstPos)
{
    if (!pSrc)
    {
        CnAssert(false);
        goto _RETURN;
    }

    switch(dstPlace)
    {
    case PLACE_BAG:
        {
            dstPos = GetBagMgr()->FindEmptyGrid();
        }
        break;
    case PLACE_ROLE_EQUIP:
        {
            if(!pSrc->IsKindOf(&ItemEquip::ms_RTTI))
            {
                goto _RETURN;
            }
            ItemEquip* pEquip = CnDynamicCast(ItemEquip, pSrc);

            if (m_player->GetLevel() < pEquip->GetEqpLevel())
            {
                goto _RETURN;
            }

            dstPos = ItemConfigManager::FindEquipPartByType(pEquip->GetPartType(), GetEquipMgr()->GetEquipGrid());
        }
        break;
    case PLACE_ROLE_SKILL:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                goto _RETURN;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (!GetSkillInfoMgr()->TryToCorrectStonePosWithType(pStone->GetSubType(), &dstPos))
            {
                goto _RETURN;
            }
        }
        break;
    case PLACE_SOLDIER_EQUIP:
        {
            if(!pSrc->IsKindOf(&ItemEquip::ms_RTTI))
            {
                goto _RETURN;
            }
            ItemEquip* pEquip = CnDynamicCast(ItemEquip, pSrc);

            if (m_player->GetPetMgr()->GetSoldier()->GetLevel() < pEquip->GetEqpLevel())
            {
                goto _RETURN;
            }

            dstPos = ItemConfigManager::FindEquipPartByType(pEquip->GetPartType(), GetSoldierEquipMgr()->GetEquipGrid());
        }
        break;
    case PLACE_SOLDIER_SKILL:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                goto _RETURN;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (!GetSoldierSkillInfoMgr()->TryToCorrectStonePosWithType(pStone->GetSubType(), &dstPos))
            {
                goto _RETURN;
            }
        }
        break;
    case PLACE_SOLDIER_TALENT:
        {
            if(!pSrc->IsKindOf(&ItemStone::ms_RTTI))
            {
                goto _RETURN;
            }
            ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);

            if (!GetSoldierTalentMgr()->TryToCorrectStonePosWithType(pStone->GetSubType(), &dstPos))
            {
                goto _RETURN;
            }
        }
        break;
    case PLACE_STORE:
        {
            dstPos = GetStoreMgr()->FindEmptyGrid();
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    return;

_RETURN:
    dstPos = NULL_GRID;
}

void ItemManager::RemovePlaceItem(uint8 place, int32 pos, ItemBase* pItem)
{
    ItemBase* pSrc(NULL);

    switch(place)
    {
    case PLACE_BAG:
        {
            pSrc = GetBagMgr()->GetItemByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetBagMgr()->m_ItemGrid[pos] = NULL;
        }
        break;
    case PLACE_STORE:
        {
            pSrc = GetStoreMgr()->GetItemByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetStoreMgr()->m_ItemGrid[pos] = NULL;
        }
        break;
    case PLACE_ROLE_EQUIP:
        {
            pSrc = GetEquipMgr()->GetEquipByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetEquipMgr()->m_EquipGrid[pos] = NULL;
        }
        break;
    case PLACE_ROLE_SKILL:
        {
            pSrc = GetSkillInfoMgr()->GetStoneByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetSkillInfoMgr()->RemoveStone(pos);
        }
        break;
    case PLACE_SOLDIER_EQUIP:
        {
            pSrc = GetSoldierEquipMgr()->GetEquipByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetSoldierEquipMgr()->m_EquipGrid[pos] = NULL;
        }
        break;
    case PLACE_SOLDIER_SKILL:
        {
            pSrc = GetSoldierSkillInfoMgr()->GetStoneByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetSoldierSkillInfoMgr()->m_StoneGrid[pos] = NULL;
        }
        break;
    case PLACE_SOLDIER_TALENT:
        {
            pSrc = GetSoldierTalentMgr()->GetStoneByPos(pos);
            if (pSrc != pItem)
            {
                CnAssert(false);
                return;
            }

            GetSoldierTalentMgr()->m_StoneGrid[pos] = NULL;
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    if (pItem->GetNum() <= 0)
    {
        DestoryItem(pItem);
    }
}

void ItemManager::RemoveItemNum(uint32 reason, int32 num, uint8 place, uint32 pos)
{
    switch(place)
    {
    case PLACE_BAG:
        {
            GetBagMgr()->SubItem(reason, pos, num);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

bool ItemManager::RemoveItemNumByItemId(uint32 reason, int32 itemId, int32 allNum)
{
    if(itemId<=0)
        return true;

    std::vector<ItemBase*> itemBaseList;
    GetPlaceItemListById(PLACE_BAG, itemId, itemBaseList);

    if (itemBaseList.empty())
    {
        CnAssert(false);

        return false;
    }

    // 开始删除物品
    for (uint32 j = 0; j < itemBaseList.size(); j++)
    {
        // 删除的数量够了，就不再删除了
        if(allNum == 0)
            continue;

        ItemBase* pItemBase = itemBaseList[j];
        // 计算应该减多少
        uint32 tmpSubNum = (int32)allNum < pItemBase->GetNum() ? allNum : pItemBase->GetNum();

        RemoveItemNum(reason, tmpSubNum, pItemBase->GetPlace(), pItemBase->GetPos());
        allNum -= tmpSubNum;
    }

    return true;
}

uint16 ItemManager::SellPlaceItem(uint8 place, int32 pos)
{
    switch(place)
    {
    case PLACE_BAG:
        return GetBagMgr()->SellItem(pos);
    default:
        CnAssert(false);
        break;
    }

    return ERR_FAILED;
}

uint16 ItemManager::MoveItem(ItemBase* pSrc, uint8 dstPlace, int32 dstPos)
{
    if (!pSrc)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    switch(dstPlace)
    {
    case PLACE_BAG:
        return GetBagMgr()->AddItem(0, pSrc, dstPos);
    case PLACE_ROLE_EQUIP:
        return GetEquipMgr()->OptInstall(pSrc, dstPos);
    case PLACE_ROLE_SKILL:
        return GetSkillInfoMgr()->OptInstall(pSrc, dstPos);
    case PLACE_SOLDIER_EQUIP:
        return GetSoldierEquipMgr()->OptInstall(pSrc, dstPos);
    case PLACE_SOLDIER_SKILL:
        return GetSoldierSkillInfoMgr()->OptInstall(pSrc, dstPos);
    case PLACE_SOLDIER_TALENT:
        return GetSoldierTalentMgr()->OptInstall(pSrc, dstPos);
    case PLACE_STORE:
        return GetStoreMgr()->AddItem(0, pSrc, dstPos);
    default:
        CnAssert(false);
        break;
    }

    return ERR_FAILED;
}

uint16 ItemManager::SwapItem(ItemBase* pSrc, ItemBase* pDst)
{
    if (!pSrc || !pDst)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    uint16 errCode(ERR_FAILED);

    errCode = CheckDstPlace(pSrc, pDst->GetPlace(), pDst->GetPos());
    if (ERR_SUCCEED != errCode)
    {
        return errCode;
    }

    errCode = CheckDstPlace(pDst, pSrc->GetPlace(), pSrc->GetPos());
    if (ERR_SUCCEED != errCode)
    {
        return errCode;
    }

    static const int32 Num(2);

    uint32 arrPlace[Num] = {pSrc->GetPlace(), pDst->GetPlace()};
    uint32 arrPos[Num] = {pSrc->GetPos(), pDst->GetPos()};
    ItemBase* arrItem[Num] = {pSrc, pDst};

    for (int32 i=0; i<Num; ++i)
    {
        int32 nSrc = i;
        int32 nDst = (i + 1) % Num;

        arrItem[nSrc]->SetPos(arrPos[nDst]);
        switch(arrPlace[nDst])
        {
        case PLACE_BAG:
            {
                GetBagMgr()->AddToGrid(arrItem[nSrc], true);
            }
            break;
        case PLACE_STORE:
            {
                GetStoreMgr()->AddToGrid(arrItem[nSrc], true);
            }
            break;
        case PLACE_ROLE_EQUIP:
            {
                ItemEquip* pEquip = CnDynamicCast(ItemEquip, arrItem[nSrc]);
                GetEquipMgr()->AddToGrid(pEquip, true);
            }
            break;
        case PLACE_ROLE_SKILL:
            {
                ItemStone* pStone = CnDynamicCast(ItemStone, arrItem[nSrc]);
                GetSkillInfoMgr()->AddToGrid(pStone, true);
            }
            break;
        case PLACE_SOLDIER_EQUIP:
            {
                ItemEquip* pEquip = CnDynamicCast(ItemEquip, arrItem[nSrc]);
                GetSoldierEquipMgr()->AddToGrid(pEquip, true);
            }
            break;
        case PLACE_SOLDIER_SKILL:
            {
                ItemStone* pStone = CnDynamicCast(ItemStone, arrItem[nSrc]);
                GetSoldierSkillInfoMgr()->AddToGrid(pStone, true);
            }
            break;
        case PLACE_SOLDIER_TALENT:
            {
                ItemStone* pStone = CnDynamicCast(ItemStone, arrItem[nSrc]);
                GetSoldierTalentMgr()->AddToGrid(pStone, true);
            }
            break;
        default:
            CnAssert(false);
            break;
        }

    }

    return ERR_SUCCEED;
}

uint16 ItemManager::UninstallToBag(uint8 srcPlace, int32 srcPos, int32 dstPos)
{
    switch(srcPlace)
    {

    case PLACE_ROLE_EQUIP:
        return GetEquipMgr()->OptUninstall(srcPos, dstPos);
    case PLACE_ROLE_SKILL:
        return GetSkillInfoMgr()->OptUninstall(srcPos, dstPos);
    case PLACE_SOLDIER_EQUIP:
        return GetSoldierEquipMgr()->OptUninstall(srcPos, dstPos);
    case PLACE_SOLDIER_SKILL:
        return GetSoldierSkillInfoMgr()->OptUninstall(srcPos, dstPos);
    case PLACE_SOLDIER_TALENT:
        return GetSoldierTalentMgr()->OptUninstall(srcPos, dstPos);

    default:
        //CnAssert(false);
        break;
    }

    return ERR_FAILED;
}

void ItemManager::DestoryItem(ItemBase* pItem)
{
    if (!pItem)
    {
        CnAssert(false);
        return;   
    }

    for (ItemList::iterator mIter = m_AllItemList.begin(); mIter != m_AllItemList.end(); ++mIter)
    {
        ItemBase* p = *mIter;
        if (p == pItem)
        {
            m_AllItemList.erase(mIter);
            break;
        }
    }

    ItemFactory::DestoryItem(pItem);
}

uint16 ItemManager::AddToBag(uint32 reason, ItemBase* pItem)
{
    uint16 errCode = GetBagMgr()->AddItem(reason, pItem);

    if (ERR_SUCCEED == errCode && pItem->GetNum() > 0)
    {
        AddToItemList(pItem);
    }

    return errCode;
}

void ItemManager::OnOptItemReq(void* data)
{
    OPT_ITEM_REQ* pRecv =  static_cast<OPT_ITEM_REQ*>(data);
    if (!pRecv)
    {
        CnAssert(false);
        return;
    }

    OPT_ITEM_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;
    sendMsg.opt_list = pRecv->opt_list;

    for (size_t i=0; i<pRecv->opt_list.size(); ++i)
    {
        OPT_ITEM_INFO& info = pRecv->opt_list[i];

        switch(info.opt)
        {
        case ITEM_OPT_MOVE:
            {
                sendMsg.errcode = OnOptItemMove(info);
            }
            break;
        case ITEM_OPT_SELL:
            {
                sendMsg.errcode = OnOptItemSell(info);
            }
            break;
        case ITEM_OPT_USE:
            {
                sendMsg.errcode = OnOptItemUse(info);
            }
            break;
        }
    }

    m_player->SendToGate(MSG_OPT_ITEM_ACK, &sendMsg);
}

uint16 ItemManager::OnOptItemMove(OPT_ITEM_INFO& info)
{
    uint16 errCode(ERR_FAILED);

    ItemBase* pSrc = GetPlaceItem(info.src.place, info.src.pos);
    if (!pSrc)
    {
        return ERR_FAILED;
    }

    if (ERR_FAILED == CheckDstPlace(pSrc, info.dst.place, info.dst.pos))
    {
        FindDstPlace(pSrc, info.dst.place, info.dst.pos);
    }

    if (NULL_GRID == info.dst.pos)
    {
        return ERR_FAILED;
    }

    // 其他模块到背包的
    if (( info.src.place>=PLACE_ROLE_EQUIP && info.src.place<=PLACE_SOLDIER_TALENT) && PLACE_BAG == info.dst.place)
    {
        return UninstallToBag(info.src.place, info.src.pos, info.dst.pos);
    }

    errCode = MoveItem(pSrc, info.dst.place, info.dst.pos);
    if (ERR_SUCCEED == errCode)
    {
        NtfOptItem(info);
        RemovePlaceItem(info.src.place, info.src.pos, pSrc);
    }
    else if (ERR_SUCCEED_SWAP == errCode)
    {
        errCode = ERR_SUCCEED;
    }

    return errCode;
}

uint16 ItemManager::OnOptItemSell(OPT_ITEM_INFO& info)
{
    return SellPlaceItem(info.src.place, info.src.pos);
}

uint16 ItemManager::OnOptItemUse(OPT_ITEM_INFO& info)
{
    if (PLACE_BAG != info.src.place)
    {
        return ERR_FAILED;
    }

    ItemBase* pItem = GetPlaceItem(info.src.place, info.src.pos);
    if (!pItem)
    {
        return ERR_FAILED;
    }

    if (pItem->GetNum() < 1)
    {
        return ERR_NO_ENOUGH_ITEM;
    }

    ItemConsume* pConsume = CnDynamicCast(ItemConsume, pItem);
    if (!pConsume)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    return GetItemController()->UseConsume(0, pConsume);
}

void ItemManager::NtfOptItem(const OPT_ITEM_INFO& info)
{
    m_OptItemInfoNtf.opt_list.push_back(info);

    SetDirty(true);
}

void ItemManager::NtfOptItemAdd(ItemBase* pItem, int32 num /*= -1*/)
{
    if (!pItem)
    {
        CnAssert(false);
        return;
    }

    switch (pItem->GetItemType())
    {
    case ITEM_TYPE_EQUIP:
        {
            ITEM_EQUIP item;
            pItem->SaveItem(&item);
            m_OptItemInfoNtf.add_info.equip_list.push_back(item);
        }
        break;
    case ITEM_TYPE_CONSUME:
        {
            ITEM_CONSUME item;
            pItem->SaveItem(&item);
            if (num > 0)
            {
                item.base.num = num;
            }
            m_OptItemInfoNtf.add_info.consume_list.push_back(item);
        }
        break;
    case ITEM_TYPE_MATERIAL:
        {
            ITEM_MATERIAL item;
            pItem->SaveItem(&item);
            if (num > 0)
            {
                item.base.num = num;
            }
            m_OptItemInfoNtf.add_info.material_list.push_back(item);
        }
        break;
    case ITEM_TYPE_STONE:
        {
            ITEM_STONE item;
            pItem->SaveItem(&item);
            m_OptItemInfoNtf.add_info.stone_list.push_back(item);
        }
        break;
    default:
        CnAssert(false);
        CnError("ItemNotifier unknown item type=%u!\n", pItem->GetItemType());
        return;
    }

    SetDirty(true);
}

void ItemManager::NtfOptItemUpdate(ItemBase* pItem, int32 num /*= -1*/)
{
    if (!pItem)
    {
        CnAssert(false);
        return;
    }

    switch (pItem->GetItemType())
    {
    case ITEM_TYPE_EQUIP:
        {
            ITEM_EQUIP item;
            pItem->SaveItem(&item);
            m_OptItemInfoNtf.update_info.equip_list.push_back(item);
        }
        break;
    case ITEM_TYPE_CONSUME:
        {
            ITEM_CONSUME item;
            pItem->SaveItem(&item);
            if (num > 0)
            {
                item.base.num = num;
            }
            m_OptItemInfoNtf.update_info.consume_list.push_back(item);
        }
        break;
    case ITEM_TYPE_MATERIAL:
        {
            ITEM_MATERIAL item;
            pItem->SaveItem(&item);
            if (num > 0)
            {
                item.base.num = num;
            }
            m_OptItemInfoNtf.update_info.material_list.push_back(item);
        }
        break;
    case ITEM_TYPE_STONE:
        {
            ITEM_STONE item;
            pItem->SaveItem(&item);
            m_OptItemInfoNtf.update_info.stone_list.push_back(item);
        }
        break;
    default:
        CnAssert(false);
        CnError("ItemNotifier unknown item type=%u!\n", pItem->GetItemType());
        return;
    }

    SetDirty(true);
}

void ItemManager::NtfOptItemSub(ItemBase* pItem, int32 num /*= -1*/)
{
    ITEM_BASE item;
    item.pos = pItem->GetPos();
    item.id = pItem->GetId();
    item.num = num > 0 ? num : pItem->GetNum();

    m_OptItemInfoNtf.sub_list.push_back(item);

    SetDirty(true);
}

void ItemManager::Notify(OPT_ITEM_INFO_NTF& msg)
{
    if(m_OptItemInfoNtf.opt_list.empty()&&
       m_OptItemInfoNtf.add_info.equip_list.empty()&&
       m_OptItemInfoNtf.add_info.consume_list.empty()&&
       m_OptItemInfoNtf.add_info.material_list.empty()&&
       m_OptItemInfoNtf.add_info.stone_list.empty()&&
       m_OptItemInfoNtf.sub_list.empty() &&
       m_OptItemInfoNtf.update_info.equip_list.empty() &&
       m_OptItemInfoNtf.update_info.consume_list.empty() &&
       m_OptItemInfoNtf.update_info.material_list.empty() &&
       m_OptItemInfoNtf.update_info.stone_list.empty())
    {
        return;
    }

    m_player->SendToGate(MSG_OPT_ITEM_INFO_NTF, &m_OptItemInfoNtf);
}

void ItemManager::AddToItemList(ItemBase * pItem)
{
    for (ItemList::iterator mIter = m_AllItemList.begin(); mIter != m_AllItemList.end(); ++mIter)
    {
        ItemBase* p = *mIter;
        if (p == pItem)
        {
            return;
        }
    }

    m_AllItemList.push_back(pItem);
}

bool ItemManager::CanAddRewardTable(const REWARD_TABLE* table, int32* needBagNum)
{
    std::map<uint32, uint32> items;

    for(std::vector<REWARD_ITEM>::const_iterator checkGainIt = table->items.begin(); checkGainIt != table->items.end(); ++checkGainIt)
    {
        const REWARD_ITEM& rewardItem = *checkGainIt;

        CnAssert(&rewardItem);

        items[rewardItem.id] = rewardItem.num;
    }

    return CanAddToBag(&items, needBagNum);
}

bool ItemManager::CanAddToBag(const std::map<uint32, uint32>* items, int32* needBagNum)
{
    if (items->size() == 0)
        return true;

    int32 bagNum = 0;       // 需要背包格数
    for(std::map<uint32, uint32>::const_iterator iter = items->begin(); iter != items->end(); ++iter)
    {
        uint32 itemID = iter->first;
        uint32 itemNum = iter->second;

        if (false == ItemConfigManager::Instance()->IsItemId(itemID))  // 不是物品不占背包
            continue;

        uint32 itemType = ItemConfigManager::Instance()->GetItemTypeById(itemID);

        switch(itemType)
        {
        case ITEM_TYPE_EQUIP:       // 装备
            bagNum++;
            break;
        case ITEM_TYPE_CONSUME:     // 消耗品
            {
                const CONSUME_ROOT_TEMPLATE_STRUCT* consumeConfig = ItemConfigManager::Instance()->FindConsumeTpltById(itemID);
                CnAssert(consumeConfig);

                // 检查最大堆叠数
                int32 tempNum = itemNum;
                while(tempNum > 0)
                {
                    if(tempNum > consumeConfig->max_stack)
                        tempNum -= consumeConfig->max_stack;
                    else
                        tempNum = 0;
                    bagNum++;
                }
            }
            break;
        case ITEM_TYPE_MATERIAL:    // 材料
            {
                const MATERIAL_ROOT_TEMPLATE_STRUCT* materialConfig = ItemConfigManager::Instance()->FindMaterialTpltById(itemID);
                CnAssert(materialConfig);

                // 检查最大堆叠数
                int32 tempNum = itemNum;
                while(tempNum > 0)
                {
                    if(tempNum > materialConfig->max_stack)
                        tempNum -= materialConfig->max_stack;
                    else
                        tempNum = 0;
                    bagNum++;
                }
            }
            break;
        case ITEM_TYPE_STONE:       // 石头
            bagNum++;
            break;
        default:
            CnAssert(false);
            return ERR_FAILED;
            break;
        }
    }

    if(needBagNum)
        *needBagNum = bagNum;

    // 检查背包是否有足够的空格
    return (bagNum <= GetBagMgr()->GetEmptyGridNum());
}

ERROR_CODE ItemManager::AddRewardTable(const REWARD_TABLE* table)
{
    ERROR_CODE errCode;
    errCode.errcode = ERR_SUCCEED;

    bool canAdd = CanAddRewardTable(table);
    if(!canAdd)
    {
        errCode.errcode = ERR_NO_ENOUGH_BAGPOS;
        return errCode;
    }

    for(std::vector<REWARD_ITEM>::const_iterator checkGainIt = table->items.begin(); checkGainIt != table->items.end(); ++checkGainIt)
    {
        const REWARD_ITEM& rewardItem = *checkGainIt;
        uint32 itemID = rewardItem.id;

        CnAssert(&rewardItem);

        if(ItemConfigManager::Instance()->IsItemId(rewardItem.id))
        {   // 物品进背包
            AddRewardItemToBag(&rewardItem);
        }
        else
        {   // 非物品奖励
            AddRewardItemByType(rewardItem.id, rewardItem.num);
        }
    }

    return errCode;
}

void ItemManager::AddRewardItemByType(int32 type, int32 value)
{
    switch(type)
    {
    case PROP_GAMEPOINT:        // 金币
        {
            m_player->AddGamePoint(value, 0);
        }
        break;
    case PROP_SOULPOINT:        // 暂时不用,但不要删除
        break;
    case PROP_EXP:              // 经验
        {
            m_player->AddExp(value);
        }
        break;
    case PROP_MONEYPOINT:
        {
            m_player->AddMoneyPoint(value, 0);
        }
        break;
    default:
        CnAssert(false);        // 未知的奖励类型
        break;
    }

}

void ItemManager::AddRewardItemToBag(const REWARD_ITEM* rewardItem)
{
    uint32 itemType = ItemConfigManager::Instance()->GetItemTypeById(rewardItem->id);

    switch(itemType)
    {   
    case ITEM_TYPE_EQUIP:       // 装备
        {
            ItemEquip* pEquip = ItemFactory::CreateEquip(rewardItem->id);
            if(!pEquip)
            {
                CnAssert(false);
                break;
            }

            uint32 quality(rewardItem->flag);
            if(ItemConfigManager::IsJewelry(pEquip->GetSubType()))
            {
                quality = Crown::SDMax((uint32)QUALITY_GREEN, quality);         // 饰品至少是绿品
            }

            pEquip->SetQuality(quality);

            EquipManager::SetEquipRandom(pEquip);

            AddToBag(REASON_ITEM_ADD_REWARD, pEquip);
        }
        break;
    case ITEM_TYPE_CONSUME:     // 消耗品
        {
            int32 tempNum = rewardItem->num;
            while(tempNum>0)
            {
                ItemConsume* pConsume = ItemFactory::CreateConsume(rewardItem->id);
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
                AddToBag(REASON_ITEM_ADD_REWARD, pConsume);
            }
        }
        break;
    case ITEM_TYPE_MATERIAL:    // 材料
        {
            int32 tempNum = rewardItem->num;
            while(tempNum>0)
            {
                ItemMaterial* pMaterial = ItemFactory::CreateMaterial(rewardItem->id);
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
                AddToBag(REASON_ITEM_ADD_REWARD, pMaterial);
            }
        }
        break;
    case ITEM_TYPE_STONE:       // 石头
        {
            ItemStone* pStone = ItemFactory::CreateStone(rewardItem->id);
            if(!pStone)
            {
                CnAssert(false);
                break;
            }

            pStone->SetQuality(rewardItem->flag);

            AddToBag(REASON_ITEM_ADD_REWARD, pStone);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

}