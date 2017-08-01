#include "gameholder_pch.h"
#include "potion_manager.h"
#include "player.h"
#include "bag_manager.h"
#include "item_config_manager.h"
#include "buff_controller.h"
#include "buff_define.h"
#include "item_manager.h"

PotionManager::PotionManager(Player* player) :
m_player(player)
{

}

PotionManager::~PotionManager()
{

}

void PotionManager::LoadPotionInfo(const POTION_INFO& info)
{
    m_PotionInfo = info;
}

void PotionManager::FillPotionInfo(POTION_INFO& info) const
{
    info = m_PotionInfo;
}

void PotionManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_POTION_SET_REQ:        // 药剂设置
        OnSetPotion(data);
        break;
    }
}

void PotionManager::OnSetPotion(void* data)
{
    POTION_INFO* recvMsg = (POTION_INFO*)data;

    m_PotionInfo = *recvMsg;

    m_player->SendToGate(MSG_POTION_SET_ACK, &m_PotionInfo);

}

uint32 PotionManager::GetPotionIDBySlotID(uint32 slotID)
{
    switch(slotID)
    {
    case 1:
        return m_PotionInfo.potion1_id;
    case 2:
        return m_PotionInfo.potion2_id;
    case 3:
        return m_PotionInfo.potion3_id;
    default:
        CnAssert(false);
        return 0;
    }
}