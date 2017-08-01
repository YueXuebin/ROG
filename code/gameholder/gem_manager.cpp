#include "gameholder_pch.h"
#include "gem_manager.h"
#include "player.h"
#include "item_manager.h"
#include "gem_config_manager.h"
#include "item_factory.h"
#include "item_config_manager.h"
#include "bag_manager.h"
#include "pet_manager.h"
#include "soldier.h"
#include "equip_manager.h"
#include "game_define.h"
#include "shop_manager.h"
#include "shop_service_manager.h"

ItemManager* GemManager::GetItemMgr()
{ 
    return m_Player->GetItemManager(); 
}

GemManager::GemManager(Player* player) :
m_Player(player)
{
};

GemManager::~GemManager()
{
}

void GemManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GEM_SOCKET_REQ:
        SocketGem(data);
        break;
    case MSG_GEM_UNSOCKET_REQ:
        UnsocketGem(data);
        break;
    case MSG_GEM_FUSE_REQ:
        FuseGem(data);
        break;
    case MSG_GEM_EVOLUTE_REQ:
        EvoluteGem(data);
        break;
    case MSG_GEM_ADD_HOLE_REQ:
        AddHole(data);
        break;
    }
}

void GemManager::SocketGem(void* data)
{
    CnAssert(data);
    GEM_STRENGTHEN_ACK sendMsg;
    GEM_SOCKET_REQ* recvMsg = (GEM_SOCKET_REQ*)data;

    ItemBase* pItem = GetItemMgr()->GetPlaceItem(recvMsg->equip.place, recvMsg->equip.pos);
    ItemEquip* equip = CnDynamicCast(ItemEquip, pItem);
    if (!equip)
    {
        CnWarn("Equip not exist\n"); // 装备不存在
        return;
    }

    if (equip->GetGemHoleList().size() == 0)
    {
        CnWarn("Equip has no holes\n"); // 没有孔
        return;
    }

    if (recvMsg->gems.size() != equip->GetGemHoleList().size())
    {
        CnWarn("Gems not match holes\n"); // 孔数不匹配
        return;
    }

    // 检查宝石是否重复
    for (uint32 i = 0; i < recvMsg->gems.size(); i++)
    {
        for (uint32 j = 0; j < recvMsg->gems.size(); j++)
        {
            if (i != j)
            {
                if (recvMsg->gems[i].id > 0 &&
                    recvMsg->gems[i].place == recvMsg->gems[j].place &&
                    recvMsg->gems[i].pos == recvMsg->gems[j].pos)
                {
                    CnWarn("Duplicated gems in SocketGem\n"); // 不允许重复
                    return;
                }
            }
        }
    }

    int32 price = 0;
    int32 socketGemCount = 0;
    const ITEM_BASE* itemBase = NULL;
    size_t pos = 0;

    for (std::vector<ITEM_BASE>::const_iterator iter = recvMsg->gems.begin(); iter != recvMsg->gems.end(); iter++, pos++)
    {
        itemBase = &(*iter);

        if (equip->HasGemStone(pos)) // 孔已镶嵌了宝石
            continue;

        if (itemBase->id <= 0) // 未镶嵌
            continue;

        if (itemBase->place != PLACE_BAG)
        {
            CnAssert(false); // 必须是背包中的宝石
            return;
        }

        pItem = GetItemMgr()->GetPlaceItem(itemBase->place, itemBase->pos);
        ItemStone* gem = CnDynamicCast(ItemStone, pItem);
        if (!gem)
        {
            CnWarn("Gem not exist\n"); // 背包中宝石不存在
            return;
        }

        const GEM_ROOT_GEM_STRUCT* gemStruct = GemConfigManager::Instance()->GetGemConfigData(gem->GetId());
        const GEM_ROOT_GEM_ITEM_STRUCT* gemItemStruct = GemConfigManager::Instance()->GetGemItemConfigData(gem->GetId(), gem->GetLevel());
        if (!gemStruct || !gemItemStruct)
        {
            CnAssert(false); // 找不到配置
            return;
        }

        if (gemStruct->hole_type != equip->GetGemHoleType(pos))
        {
            CnWarn("Gem hole not match\n"); // 宝石与孔不匹配
            return;
        }

        price += gemItemStruct->socketing_price;
        socketGemCount++;
    }

    if (socketGemCount == 0)
    {
        CnAssert(false); // 没有要镶嵌的宝石
        return;
    }

    if (price > m_Player->GetGamePoint())
    {
        sendMsg.errcode = ERR_NO_ENOUGH_GAMEPOINT; // 金币不够
        m_Player->SendToGate(MSG_GEM_SOCKET_ACK, &sendMsg);
        return;
    }

    // 使用金币
    m_Player->SubGamePoint(price, 0); 

    // 更新装备上的宝石
    std::vector<ITEM_STONE> gemStoneList;
    pos = 0;
    for (std::vector<ITEM_BASE>::const_iterator iter = recvMsg->gems.begin(); iter != recvMsg->gems.end(); iter++, pos++)
    {
        itemBase = &(*iter);

        if (equip->HasGemStone(pos))
        {
            ITEM_STONE* equipGem = equip->GetGemStone(pos);
            gemStoneList.push_back(*equipGem); // 该孔已镶嵌了宝石
        }
        else
        {
            if (itemBase->id <= 0)
                continue;

            ITEM_STONE itemStone;

            pItem = GetItemMgr()->GetPlaceItem(itemBase->place, itemBase->pos);
            ItemStone* gem = CnDynamicCast(ItemStone, pItem);
            gem->SaveItem(&itemStone);

            itemStone.base.place = PLACE_EQUIP_GEM;
            itemStone.base.pos = pos;
            itemStone.base.num = 1;

            gemStoneList.push_back(itemStone); // 新镶嵌

            GetItemMgr()->RemoveItemNum(REASON_ITEM_SUB_GEM_SOCKET, 1, itemBase->place, itemBase->pos);
        }
    }
    equip->ClearGemStoneList();
    equip->SetGemStoneList(gemStoneList);

    ITEM_EQUIP itemEquip;
    equip->SaveItem(&itemEquip);
    sendMsg.item_info.equip_list.push_back(itemEquip);

    GetItemMgr()->NtfOptItemUpdate(equip, 1);

    uint32 place = equip->GetPlace();
    CalcProperty(place);

    m_Player->SendToGate(MSG_GEM_SOCKET_ACK, &sendMsg);
}

void GemManager::UnsocketGem(void* data)
{
    GEM_STRENGTHEN_ACK sendMsg;

    CnAssert(data);
    GEM_UNSOCKET_REQ* recvMsg = (GEM_UNSOCKET_REQ*)data;

    ItemBase* pItem = GetItemMgr()->GetPlaceItem(recvMsg->equip.place, recvMsg->equip.pos);
    ItemEquip* equip = CnDynamicCast(ItemEquip, pItem);
    if (!equip)
    {
        CnWarn("Equip not exist\n");
        return;
    }

    if (equip->GetGemStoneList().size() == 0)
    {
        CnWarn("No gems to unsocket\n");
        return;
    }

    int32 place = equip->GetPlace();

    int32 price = 0;
    std::vector<ITEM_STONE> stones = equip->GetGemStoneList();
    for (std::vector<ITEM_STONE>::const_iterator iter = stones.begin(); iter != stones.end(); ++iter)
    {
        const ITEM_STONE& itemStone = *iter;
        const GEM_ROOT_GEM_ITEM_STRUCT* gemItemStruct = GemConfigManager::Instance()->GetGemItemConfigData(itemStone.base.id, itemStone.level);
        if (!gemItemStruct)
        {
            CnAssert(false);
            return;
        }
        price += gemItemStruct->remove_price;
    }

    int32 emptyGridNum = GetItemMgr()->GetBagMgr()->GetEmptyGridNum();
    if (emptyGridNum < (int32)stones.size())
    {
        sendMsg.errcode = ERR_NO_ENOUGH_BAGPOS; // 背包位不够
        m_Player->SendToGate(MSG_GEM_UNSOCKET_ACK, &sendMsg);
        return;
    }

    if (price > m_Player->GetGamePoint())
    {
        sendMsg.errcode = ERR_NO_ENOUGH_GAMEPOINT; // 金币不够
        m_Player->SendToGate(MSG_GEM_UNSOCKET_ACK, &sendMsg);
        return;
    }

    // 使用金币
    m_Player->SubGamePoint(price, 0); 

    // 删除装备上的宝石并放到背包
    for (std::vector<ITEM_STONE>::const_iterator iter = stones.begin(); iter != stones.end(); iter++)
    {
        const ITEM_STONE itemStone = *iter;
        ItemStone* gem = ItemFactory::Instance()->CreateStone(itemStone.base.id);
        gem->LoadItem(&itemStone);
        GetItemMgr()->AddToBag(REASON_ITEM_ADD_GM, gem);
    }
    equip->ClearGemStoneList();

    ITEM_EQUIP itemEquip;
    equip->SaveItem(&itemEquip);
    sendMsg.item_info.equip_list.push_back(itemEquip);

    GetItemMgr()->NtfOptItemUpdate(equip, 1);

    CalcProperty(place);

    m_Player->SendToGate(MSG_GEM_UNSOCKET_ACK, &sendMsg);
}

void GemManager::FuseGem(void* data)
{
    GEM_STRENGTHEN_ACK sendMsg;

    CnAssert(data);
    GEM_FUSE_REQ* recvMsg = (GEM_FUSE_REQ*)data;

    ITEM_STONE* mainGem;
    ItemEquip* mainEquip;

    if (recvMsg->parentPlace > 0)
    {
        // 装备上的宝石
        ItemBase* pParentItem = GetItemMgr()->GetPlaceItem(recvMsg->parentPlace, recvMsg->parentPos);
        mainEquip = CnDynamicCast(ItemEquip, pParentItem);
        if (!mainEquip)
        {
            CnAssert(false);
            return;
        }
        if (false == mainEquip->HasGemStone(recvMsg->gem.pos))
        {
            CnAssert(false);
            return;
        }

        mainGem = mainEquip->GetGemStone(recvMsg->gem.pos);

        if (!mainGem)
        {
            CnWarn("Gem not exist\n");
            return;
        }
    }
    else
    {
        // 未被镶嵌的宝石
        ItemBase* pItem = GetItemMgr()->GetPlaceItem(recvMsg->gem.place, recvMsg->gem.pos);
        ItemStone* pItemStone = CnDynamicCast(ItemStone, pItem);
        if (!pItemStone)
        {
            CnWarn("Gem not exist\n");
            return;
        }

        ITEM_STONE stone;
        pItemStone->SaveItem(&stone);
        mainGem = &stone;
    }

    int32 maxLevel = GemConfigManager::Instance()->GetGemMaxLevel(mainGem->base.id);
    if (mainGem->level >= maxLevel)
    {
        CnWarn("Already max level\n");
        return;
    }

    // 检查宝石是否重复
    for (uint32 i = 0; i < recvMsg->gems.size(); i++)
    {
        if (recvMsg->gems[i].place == recvMsg->gem.place && 
            recvMsg->gems[i].pos == recvMsg->gem.pos)
        {
            CnWarn("Gems duplicated with main gem in FuseGem\n"); // 子石头不允许和主石头重复
            return;
        }

        for (uint32 j = 0; j < recvMsg->gems.size(); j++)
        {
            if (i != j)
            {
                if (recvMsg->gems[i].place == recvMsg->gems[j].place &&
                    recvMsg->gems[i].pos == recvMsg->gems[j].pos)
                {
                    CnWarn("Gems duplicated in FuseGem\n"); // 子石头不允许重复
                    return;
                }
            }
        }
    }

    int32 price = 0;
    int32 exp = 0;
    const ITEM_BASE* itemBase = NULL;

    for (std::vector<ITEM_BASE>::const_iterator iter = recvMsg->gems.begin(); iter != recvMsg->gems.end(); iter++)
    {
        itemBase = &(*iter);

        if (iter->place != PLACE_BAG)
        {
            CnAssert(false); // 要牺牲的宝石只能是背包中的宝石
            return;
        }

        ItemBase* pItem = GetItemMgr()->GetPlaceItem(iter->place, iter->pos);
        ItemStone* gem = CnDynamicCast(ItemStone, pItem);
        if (!gem)
        {
            CnWarn("Gem not exist\n");
            return;
        }

        const GEM_ROOT_GEM_ITEM_STRUCT* gemItemStruct = GemConfigManager::Instance()->GetGemItemConfigData(gem->GetId(), gem->GetLevel());
        if (!gemItemStruct)
        {
            CnAssert(false);
            return;
        }

        price += gemItemStruct->fusion_price;
        exp += gemItemStruct->fusion_exp;
    }

    if (price > m_Player->GetGamePoint())
    {
        sendMsg.errcode = ERR_NO_ENOUGH_GAMEPOINT; // 金币不够
        m_Player->SendToGate(MSG_GEM_FUSE_ACK, &sendMsg);
        return;
    }

    // 使用金币
    m_Player->SubGamePoint(price, 0); 

    // 融合宝石
    for (std::vector<ITEM_BASE>::const_iterator iter = recvMsg->gems.begin(); iter != recvMsg->gems.end(); iter++)
    {
        itemBase = &(*iter);
        GetItemMgr()->RemoveItemNum(REASON_ITEM_SUB_GEM_FUSION, 1, itemBase->place, itemBase->pos);
    }

    // 加经验
    ItemStone::AddExpToItem(mainGem, exp);

    if (recvMsg->parentPlace > 0)
    {
        // 装备上的宝石
        mainEquip->SetGemStoneList(mainEquip->GetGemStoneList()); // 发生变更，装备dirty

        ITEM_EQUIP itemEquip;
        mainEquip->SaveItem(&itemEquip);
        sendMsg.item_info.equip_list.push_back(itemEquip);

        GetItemMgr()->NtfOptItemUpdate(mainEquip, 1);

        CalcProperty(recvMsg->parentPlace);
    }
    else
    {
        // 未被镶嵌的宝石
        ItemBase* pItem = GetItemMgr()->GetPlaceItem(mainGem->base.place, mainGem->base.pos);
        ItemStone* pItemStone = CnDynamicCast(ItemStone, pItem);
        if (!pItemStone)
        {
            CnAssert(false);
            return;
        }
        pItemStone->LoadItem(mainGem);

        sendMsg.item_info.stone_list.push_back(*mainGem);

        GetItemMgr()->NtfOptItemUpdate(pItemStone, 1);
    }

    m_Player->SendToGate(MSG_GEM_FUSE_ACK, &sendMsg);
}

void GemManager::EvoluteGem(void* data)
{
    GEM_STRENGTHEN_ACK sendMsg;

    CnAssert(data);
    GEM_EVOLUTE_REQ* recvMsg = (GEM_EVOLUTE_REQ*)data;

    ItemBase* pItem = GetItemMgr()->GetPlaceItem(recvMsg->equip.place, recvMsg->equip.pos);
    ItemEquip* equip = CnDynamicCast(ItemEquip, pItem);
    if (!equip)
    {
        CnWarn("Equip not exist\n");
        return;
    }

    if (equip->GetGemHoleList().size() == 0)
    {
        CnWarn("Equip has no holes\n"); // 没有孔
        return;
    }

    if (false == GetItemMgr()->CheckItemNum(MATERIAL_ID_EQUIP_CHANGE_COLOR, 1))
    {
        CnWarn("No evolute material\n"); // 材料不足
        return;
    }

    // 获得加锁列表
    std::vector<uint32> lock_index;
    uint32 gemHoleCount = equip->GetGemHoleList().size();
    for (uint32 i = 0; i < gemHoleCount; i++)
    {
        if (equip->HasGemStone(i) ||    // 已镶嵌宝石的孔默认加锁
            recvMsg->lock_index.end() != std::find(recvMsg->lock_index.begin(), recvMsg->lock_index.end(), i)) // 手动加锁
        {
            lock_index.push_back(i); // 上锁了
        }
    }

    uint32 lockCount = lock_index.size();
    if (lockCount >= gemHoleCount)
    {
        CnWarn("Cannot lock all gem hole"); // 不能全部上锁
        return;
    }

    // 判断是否可购买锁
    uint32 serviceId;
    if (lockCount > 0)
    {
        serviceId = m_Player->GetShopServiceManager()->GetEquipGemHoleChangeColorLockServiceId(lockCount);
        if (false == m_Player->GetShopManager()->IsEnoughMoney(serviceId, 1))
        {
            sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT; // 钻石不足
            m_Player->SendToGate(MSG_GEM_EVOLUTE_ACK, &sendMsg);
            return;
        }
    }

    // 花费锁钱
    if (lockCount > 0)
    {
        if (false == m_Player->GetShopManager()->DoMallBuy(serviceId, 1, BUY_REASON_NORMAL))
        {
            sendMsg.errcode = ERR_FAILED;   // 购买失败
            m_Player->SendToGate(MSG_GEM_EVOLUTE_ACK, &sendMsg);
            return;
        }
    }

    // 使用幻色石
    GetItemMgr()->RemoveItemNumByItemId(REASON_ITEM_SUB_GEM_CHANGE_COLOR, MATERIAL_ID_EQUIP_CHANGE_COLOR, 1);

    // 随机宝石孔
    for (size_t i = 0; i < gemHoleCount; i++)
    {
        if (lock_index.end() == std::find(lock_index.begin(), lock_index.end(), i)) // 没加锁
        {
            uint32 holeType = Crown::SDRandom(EQUIP_HOLE_TYPE_1, EQUIP_HOLE_TYPE_1 + EQUIP_HOLE_MAX);
            equip->SetGemHole(i, holeType);
        }
    }

    ITEM_EQUIP itemEquip;
    equip->SaveItem(&itemEquip);
    sendMsg.item_info.equip_list.push_back(itemEquip);

    GetItemMgr()->NtfOptItemUpdate(equip, 1);

    m_Player->SendToGate(MSG_GEM_EVOLUTE_ACK, &sendMsg);
}

void GemManager::AddHole(void* data)
{
    GEM_STRENGTHEN_ACK sendMsg;

    CnAssert(data);
    GEM_ADD_HOLE_REQ* recvMsg = (GEM_ADD_HOLE_REQ*)data;

    ItemBase* pItem = GetItemMgr()->GetPlaceItem(recvMsg->equip.place, recvMsg->equip.pos);
    ItemEquip* equip = CnDynamicCast(ItemEquip, pItem);
    if (!equip)
    {
        CnWarn("Equip not exist\n");
        return;
    }

    const EQUIP_ROOT_TEMPLATE_STRUCT* equipTplt = ItemConfigManager::Instance()->FindEquipTpltById(equip->GetId());
    if (!equipTplt)
    {
        CnAssert(false);
        return;
    }

    if (equip->GetGemHoleList().size() >= (uint32)(equipTplt->max_hole))
    {
        CnWarn("Already max hole\n");
        return;
    }

    if (false == GetItemMgr()->CheckItemNum(MATERIAL_ID_EQUIP_ADD_HOLE, 1))
    {
        CnWarn("No add hole material\n");
        return;
    }

    // 使用打孔石
    GetItemMgr()->RemoveItemNumByItemId(REASON_ITEM_SUB_GEM_ADD_HOLE, MATERIAL_ID_EQUIP_ADD_HOLE, 1);

    // 随机宝石孔
    uint32 holeType = Crown::SDRandom(EQUIP_HOLE_TYPE_1, EQUIP_HOLE_TYPE_1 + EQUIP_HOLE_MAX);
    equip->AddGemHole(holeType);

    ITEM_EQUIP itemEquip;
    equip->SaveItem(&itemEquip);
    sendMsg.item_info.equip_list.push_back(itemEquip);

    GetItemMgr()->NtfOptItemUpdate(equip, 1);

    m_Player->SendToGate(MSG_GEM_ADD_HOLE_ACK, &sendMsg);
}

void GemManager::CalcProperty(uint32 equipPlace) // 重新计算人物属性
{
    if (equipPlace == PLACE_ROLE_EQUIP)
    {
        m_Player->GetEquipMgr()->SetPropertyDirty();        // 角色装备属性重新计算
        m_Player->CalcProperty();
    }
    else if (equipPlace == PLACE_SOLDIER_EQUIP)
    {
        m_Player->GetPetMgr()->GetSoldier()->GetEquipMgr()->SetPropertyDirty();       // 佣兵的装备属性重新计算
        m_Player->GetPetMgr()->GetSoldier()->CalcProperty();
    }
}
