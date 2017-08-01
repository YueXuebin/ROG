#include "gameholder_pch.h"
#include "player.h"
#include "skill_strengthen_manager.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "skill_info_manager.h"
#include "pet_manager.h"
#include "soldier.h"
#include "shop_manager.h"
#include "shop_service_manager.h"


ItemManager* SkillStrengthenManager::GetItemMgr()
{
    return m_player->GetItemManager();
}

SkillStrengthenManager::SkillStrengthenManager(Player* player) :
m_player(player)
{
}

SkillStrengthenManager::~SkillStrengthenManager()
{

}


void SkillStrengthenManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
        case MSG_SKILL_STRENGTHEN_INFO_REQ:     // 技能升品
            OnFuse(data);
            break;
        default:
            break;
    }
}
 
void SkillStrengthenManager::OnFuse(void* data)
{
    SKILL_STRENGTHEN_STONE* recvMsg = (SKILL_STRENGTHEN_STONE*)data;
    ERROR_CODE sendMsg;

    //  获取消耗的石头信息
    std::vector<ItemBase*> pItemStoneList;

    ItemBase* pItemBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->main.base.place, recvMsg->main.base.pos);
    ItemStone* pItemStone = CnDynamicCast(ItemStone, pItemBase);
    if(!pItemStone) 
    {
        sendMsg.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
        return;
    }

    uint32 maxHole = 0;
    switch (pItemStone->GetQuality())
    {
    case QUALITY_WHITE:
        maxHole = 3;
        break;
    case QUALITY_GREEN:
        maxHole = 4;
        break;
    case QUALITY_BLUE:
        maxHole = 5;
        break;
    case QUALITY_PURPLE:
        maxHole = 6;
        break;
    }

    // 最大孔数不可能为0，要牺牲的技能书数量也不可能大于最大孔数
    if (maxHole == 0 || recvMsg->stone_list.size() > maxHole)
    {
        sendMsg.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
        return;
    }
    
    // 检查石头是否重复
    for (uint32 i = 0; i < recvMsg->stone_list.size(); i++)
    {
        if (recvMsg->stone_list[i].base.place == recvMsg->main.base.place && 
            recvMsg->stone_list[i].base.pos == recvMsg->main.base.pos)
        {
            CnWarn("Stones duplicated with main stone in SkillStengthen\n"); // 子石头不允许和主石头重复
            return;
        }

        for (uint32 j = 0; j < recvMsg->stone_list.size(); j++)
        {
            if (i != j)
            {
                if (recvMsg->stone_list[i].base.place == recvMsg->stone_list[j].base.place &&
                    recvMsg->stone_list[i].base.pos == recvMsg->stone_list[j].base.pos)
                {
                    CnWarn("Stones duplicated in SkillStengthen\n"); // 子石头不允许重复
                    return;
                }
            }
        }
    }

    //获取消耗的石头信息
    for (uint32 i = 0; i < recvMsg->stone_list.size(); i++)
    {
        ItemBase* pTmpItemBase = m_player->GetItemManager()->GetPlaceItem(recvMsg->stone_list[i].base.place, recvMsg->stone_list[i].base.pos);
        ItemStone* pTmpItemStone = CnDynamicCast(ItemStone, pTmpItemBase);
        if(!pTmpItemStone) 
        {
            sendMsg.errcode = ERR_FAILED;
            m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
            return;
        }
        
        // 验证消耗石头的品质对不对
        if (pTmpItemStone->GetQuality() < pItemStone->GetQuality())
        {
            sendMsg.errcode = ERR_FAILED;
            m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
            return;
        }

        pItemStoneList.push_back(pTmpItemBase);
    }

    // 自动填满
    int32 autoFillCount = maxHole - recvMsg->stone_list.size(); // 需要自动填满的数量
    uint32 serviceId;
    if (autoFillCount > 0)
    {
        serviceId = m_player->GetShopServiceManager()->GetFusionAutoFillSkillStoneServiceId(pItemStone->GetQuality(), autoFillCount);
        if (false == m_player->GetShopManager()->IsEnoughMoney(serviceId, autoFillCount))
        {
            sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT; // 钻石不足
            m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
            return; 
        }
    }

    // 自动填满花费钻石
    if (autoFillCount > 0)
    {
        if (false == m_player->GetShopManager()->DoMallBuy(serviceId, autoFillCount, BUY_REASON_NORMAL))
        {
            sendMsg.errcode = ERR_FAILED;   // 购买失败
            m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);
            return; 
        }
    }

    // 消耗石头
    for (uint32 i = 0; i < pItemStoneList.size(); i++)
    {
        m_player->GetItemManager()->RemoveItemNum(REASON_ITEM_SUB_SKILL_FUSION, pItemStoneList[i]->GetNum(), pItemStoneList[i]->GetPlace(), pItemStoneList[i]->GetPos());
    }

    // 强化
    uint32 willQuality = pItemStone->GetQuality() + 1;
    pItemStone->SetQuality(willQuality); // 品质升一级
    pItemStone->SetLevel(1); // 等级重置1
    pItemStone->SetExp(0); // 经验重置0

    GetItemMgr()->NtfOptItemUpdate(pItemStone, 1);

    sendMsg.errcode = ERR_SUCCEED;
    m_player->SendToGate(MSG_SKILL_STRENGTHEN_INFO_ACK, &sendMsg);

    // 角色和佣兵身上的技能或符文,需要重新计算技能信息
    CalcSkillProperty(pItemStone->GetPlace(), pItemStone->GetPos());
}

// 重新计算技能属性
void SkillStrengthenManager::CalcSkillProperty(uint32 equipPlace, int32 skillPos)
{
    if (equipPlace == PLACE_ROLE_SKILL)
    {
        m_player->GetSkillInfoMgr()->ReFreshSkillInfo(skillPos);
    }
    else if (equipPlace == PLACE_SOLDIER_SKILL)
    {
        m_player->GetPetMgr()->GetSoldier()->GetSkillInfoMgr()->ReFreshSkillInfo(skillPos);
    }
}