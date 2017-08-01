#include "gameholder_pch.h"
#include "sword_manager.h"
#include "player.h"
#include "item_manager.h"
#include "sdbit.h"
#include "item_config_manager.h"
#include "formula_manager.h"
#include "game_util.h"
#include "event_define.h"
#include "trade_manager.h"
#include "property_set_calculater.h"
#include "sword_config.h"

SwordManager::SwordManager(Player* player) :
m_player(player)
{
    Clear();

}

SwordManager::~SwordManager()
{

}

void SwordManager::Clear()
{
    m_sword_info.Clear();
}

void SwordManager::LoadInfo(const SWORD_NEW_INFO& info)
{
    m_sword_info = info;
}

void SwordManager::FillInfo(SWORD_NEW_INFO& info)
{
    info = m_sword_info;
}

void SwordManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
    {
    case MSG_SWORD_INFO_REQ:                            // 初始化信息
        {
            onSwordInfoReq();
        }
        break;
    case MSG_SWORD_UPGRADE_REQ:                         // 升星
        {
            onUpgradeReq(data);
        }
        break;
    case MSG_SWORD_ADVANCE_REQ:                         // 升阶
        {
            onAdvanceReq(data);
        }
        break;
    case MSG_SWORD_ACTIVATE_REQ:                        // 激活
        {
            onActivateReq(data);
        }
        break;
    case MSG_SWORD_LOCK_REQ:                            // 解锁
        {
            onUnlockReq(data);
        }
        break;
    case MSG_SWORD_EQUIP_REQ:                           // 装备
        {
            onEquipReq(data);
        }
        break;
    default:
        break;
    }
}

//属性提升战斗力
void SwordManager::CalSwordProp()
{
    PropertySet PropSet;

    for (std::vector<SWORD_ONE_INFO>::iterator swordIt = m_sword_info.sword_list.begin(); swordIt != m_sword_info.sword_list.end(); ++swordIt)
    {
        const SwordStarTmpl* starTmpl = SwordConfig::Instance()->GetStarTmpl(swordIt->id, swordIt->level, swordIt->star);
        if(!starTmpl)
        {
            CnAssert(false);
            return;
        }

        for(std::vector<int>::iterator lockIt = swordIt->lock.begin(); lockIt != swordIt->lock.end(); ++lockIt)
        {
            PropSet.Add(starTmpl->level_propety[(*lockIt - 1)]);
        }
    }

//     m_player->GetPropertySetCalculater()->CalcBattlePowerBySwordBase(PropSet);
// 
//     // 计算圣剑系统的战斗力
//     m_sword_info.battle = m_player->GetPropertySetCalculater()->GetBattlePowerBySwordBase();
}

void SwordManager::onUnlock(int32 swordID, int32 lockID)
{
    SWORD_ONE_INFO* pSwordInfo = GetSwordOneInfo(swordID);
    if(!pSwordInfo)
    {
        CnAssert(false);
        return;
    }

    const SwordEquipTmpl* equip = SwordConfig::Instance()->GetSwordTmpl(swordID);
    if(!equip)
    {
        CnAssert(false);
        return;
    }
    
    pSwordInfo->lock.push_back(lockID);

    SendSwordUnlock(ERR_SUCCEED, pSwordInfo);

    m_player->CalcProperty();
}

//初始化信息
void SwordManager::onSwordInfoReq()
{
    InitProList();

    m_player->SendToGate(MSG_SWORD_INFO_ACK, &m_sword_info);
}

SWORD_ONE_INFO* SwordManager::GetSwordOneInfo(uint32 swordID)
{
    for(std::vector<SWORD_ONE_INFO>::iterator swordIt = m_sword_info.sword_list.begin(); 
        swordIt != m_sword_info.sword_list.end(); ++swordIt)
    {
        if(swordIt->id == swordID)
            return &(*swordIt);
    }

    return NULL;
}

//初始化信息
void SwordManager::InitProList()
{
    if (m_sword_info.sword_list.size() == 0)
    {
        const std::map<uint32, SwordEquipTmpl>& swordTmplMap = SwordConfig::Instance()->GetSwordMap();
        CnAssert(swordTmplMap.size() > 0);

        std::map<uint32, SwordEquipTmpl>::const_iterator firstSword = swordTmplMap.begin();       // 第一个翅膀
        CnAssert(firstSword->second.level_list.size() > 0);

        CnAssert(ActivitySword(firstSword->first));     // 激活第一个翅膀

        m_player->CalcProperty();
    }
}

//升星请求
void SwordManager::onUpgradeReq(void* data)
{
    SWORD_UPGRADE_REQ* recvMsg = (SWORD_UPGRADE_REQ*)data;

    uint32 err_code = ERR_FAILED;
    int32 upStarNum = 0;

    SWORD_ONE_INFO* swordOneInfo = GetSwordOneInfo(recvMsg->id);
    if(!swordOneInfo)
    {
        CnAssert(false);
        return;
    }

    const SwordEquipTmpl* swordOneTmpl = SwordConfig::Instance()->GetSwordTmpl(recvMsg->id);
    if(!swordOneTmpl)
    {
        SendSwordUpgradeAck(ERR_FAILED, swordOneInfo);
        return;
    }

    if(recvMsg->type == SWORD_ORD_UPGRADE)
    {
        upStarNum = 1;
        const SwordStarTmpl* swordStarTmpl = SwordConfig::Instance()->GetStarTmpl(swordOneInfo->id, swordOneInfo->level, swordOneInfo->star);
        if(!swordStarTmpl)
        {
            CnAssert(false);
            return;
        }

        err_code = m_player->CheckItemTable(swordStarTmpl->need_items);
        if(err_code != ERR_SUCCEED)
        {
            SendSwordUpgradeAck(err_code, swordOneInfo);
            return;
        }

        if(!UpStar(swordOneInfo, upStarNum))
        {
            SendSwordUpgradeAck(ERR_FAILED, swordOneInfo);
            return;
        }

        m_player->SubItemTable(swordStarTmpl->need_items);
    }
    else if(recvMsg->type == SWORD_ONCE_UPGRADE)
    {
        upStarNum = SwordConfig::Instance()->GetMaxStarTmpl(swordOneInfo->id, swordOneInfo->level) 
            - swordOneInfo->star;
        CnAssert(upStarNum >= 0);

        if(upStarNum == 0)
        {
            SendSwordUpgradeAck(ERR_FAILED, swordOneInfo);
            return;
        }

        REWARD_TABLE subItems;
        for (int32 i = 0; i < upStarNum; ++i)
        {
            const SwordStarTmpl* curStarTmpl = 
                SwordConfig::Instance()->GetStarTmpl(swordOneInfo->id, swordOneInfo->level, swordOneInfo->star + i);
            if(!curStarTmpl)
            {
                CnAssert(false);
                return;
            }

            subItems.reason = REWARD_REASON_SWORD_UPGRADE;
            subItems.items.insert(subItems.items.end(), 
                curStarTmpl->need_items.items.begin(), curStarTmpl->need_items.items.end());
        }

        err_code = m_player->CheckItemTable(subItems);
        if(err_code != ERR_SUCCEED)
        {
            SendSwordUpgradeAck(err_code, swordOneInfo);
            return;
        }

        if(!UpStar(swordOneInfo, upStarNum))
        { 
            SendSwordUpgradeAck(ERR_FAILED, swordOneInfo);
            return;
        }

        m_player->SubItemTable(subItems);
    }
    else
    {
        CnAssert(false);
        return;
    }

    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_SWORD_UPGRADE_STAR, upStarNum, swordOneInfo->id, 0);
    m_player->SendTaskEvent(TASK_EVENT_LEVE_UP_SWORD, upStarNum, 0 ,0);
    m_player->SendTaskEvent(TASK_EVENT_NEW_LEVE_UP_SWORD, swordOneInfo->level, swordOneInfo->id ,0);

    // 重新计算角色属性
    m_player->CalcProperty();

    SendSwordUpgradeAck(ERR_SUCCEED, swordOneInfo);

}

// 升阶请求
void SwordManager::onAdvanceReq(void* data)
{
    SWORD_ADVANCE_REQ* recvMsg = (SWORD_ADVANCE_REQ*)data;

    uint32 err_code = ERR_FAILED;

    SWORD_ONE_INFO* swordOneInfo = GetSwordOneInfo(recvMsg->id);
    if(!swordOneInfo)
    {
        CnAssert(false);
        return;
    }

    // 超过最大阶
    int32 maxLevel = SwordConfig::Instance()->GetMaxLevel(swordOneInfo->id);
    CnAssert(swordOneInfo->level <= maxLevel);
    if(swordOneInfo->level == maxLevel)
    {
        SendSwordAdvanceAck(ERR_FAILED, swordOneInfo);
        return;
    }

    // 星数不满
    if(swordOneInfo->star < SwordConfig::Instance()->GetMaxStarTmpl(swordOneInfo->id, swordOneInfo->level))
    {
        SendSwordAdvanceAck(ERR_FAILED, swordOneInfo);
        return;
    }

    const SwordLevelTmpl* levelTmpl = SwordConfig::Instance()->GetLevelTmpl(swordOneInfo->id, swordOneInfo->level);
    if(!levelTmpl)
    {
        CnAssert(false);
        return;
    }

    // 物品数量不足
    err_code = m_player->CheckItemTable(levelTmpl->need_items);
    if(err_code != ERR_SUCCEED)
    {
        SendSwordAdvanceAck(ERR_FAILED, swordOneInfo);
        return;
    }

    swordOneInfo->level++;
    swordOneInfo->star = 0;

    m_player->SubItemTable(levelTmpl->need_items);

    // 重新计算角色属性
    m_player->CalcProperty();
    SendSwordAdvanceAck(ERR_SUCCEED, swordOneInfo);

}

// 激活圣剑
void SwordManager::onActivateReq(void* data)
{
    SWORD_ACTIVATE_REQ* recvMsg = (SWORD_ACTIVATE_REQ*)data;

    SWORD_ONE_INFO* swordOneInfo = GetSwordOneInfo(recvMsg->id);
    if(swordOneInfo)        // 已经激活
    {
        SendSwordActivateAck(ERR_FAILED, recvMsg->id);
        return;
    }

    const SwordEquipTmpl* swordTmpl = SwordConfig::Instance()->GetSwordTmpl(recvMsg->id);
    if(!swordTmpl)
    {
        CnAssert(false);
        SendSwordActivateAck(ERR_FAILED, recvMsg->id);
        return;
    }

    // 物品数量不足
    if(m_player->CheckItemOne(PROP_ITEM, 1, swordTmpl->item_id) != ERR_SUCCEED)
    {
        SendSwordActivateAck(ERR_NO_ENOUGH_ITEM, recvMsg->id);
        return;
    }

    if (!ActivitySword(recvMsg->id))            // 激活翅膀
    {
        SendSwordActivateAck(ERR_FAILED, recvMsg->id);
        return;
    }

    m_player->SubItemOne(REWARD_REASON_SWORD_ACTIVATE, PROP_ITEM, 1, swordTmpl->item_id);

    SendSwordActivateAck(ERR_SUCCEED, recvMsg->id);

    m_player->CalcProperty();      // 重新计算角色属性
}

bool SwordManager::ActivitySword(uint32 swordID)
{
    SWORD_ONE_INFO* swordOneInfo = GetSwordOneInfo(swordID);
    if(swordOneInfo)
        return false;

    const SwordEquipTmpl* swordTmpl = SwordConfig::Instance()->GetSwordTmpl(swordID);
    if(!swordTmpl)
        return false;

    CnAssert(swordTmpl->level_list.size() > 0);
    std::map<uint32, SwordLevelTmpl>::const_iterator firstLevel = swordTmpl->level_list.begin();  // 第一阶

    CnAssert(firstLevel->second.star_list.size() > 0);
    std::map<uint32, SwordStarTmpl>::const_iterator firstStar = firstLevel->second.star_list.begin();     // 第一星

    SWORD_ONE_INFO newSwordOneInfo;
    newSwordOneInfo.id = swordTmpl->id;
    newSwordOneInfo.level = firstLevel->first;
    newSwordOneInfo.star = firstStar->first;

    for (std::map<uint32, int32>::const_iterator swordIt = swordTmpl->lock_list.begin(); swordIt != swordTmpl->lock_list.end(); ++swordIt)
    {
        if(swordIt->second == 0)
        {
            newSwordOneInfo.lock.push_back(swordIt->first);
        }
    }

    m_sword_info.sword_list.push_back(newSwordOneInfo);

    return true;
}

//发送升星回应
void SwordManager::SendSwordUpgradeAck(uint16 error_code, SWORD_ONE_INFO* swordOneInfo)
{
    SWORD_UPGRADE_ACK sendMsg;

    sendMsg.err = error_code;
    sendMsg.id = swordOneInfo->id;
    if(error_code == ERR_SUCCEED)
    {
        sendMsg.info = *swordOneInfo;
    }
    m_player->SendToGate(MSG_SWORD_UPGRADE_ACK, &sendMsg);
}

//发送升阶回应
void SwordManager::SendSwordAdvanceAck(uint16 error_code, SWORD_ONE_INFO* swordOneInfo)
{
    SWORD_ADVANCE_ACK sendMsg;
    sendMsg.Clear();
    sendMsg.err = error_code;
    sendMsg.id = swordOneInfo->id;
    if(error_code == ERR_SUCCEED)
    {
        sendMsg.info = *swordOneInfo;
    }
    m_player->SendToGate(MSG_SWORD_ADVANCE_ACK, &sendMsg);
}

void SwordManager::SendSwordActivateAck(uint16 error_code, uint32 swordID)
{
    SWORD_ACTIVATE_ACK sendMsg;
    sendMsg.id = swordID;
    sendMsg.info = m_sword_info;
    m_player->SendToGate(MSG_SWORD_ACTIVATE_ACK, &sendMsg);
}

void SwordManager::onUnlockReq(void* data)
{
    SWORD_LOCK_REQ* recvMsg = (SWORD_LOCK_REQ*)data;

    int32 err_code = ERR_FAILED;

    SWORD_ONE_INFO* pSwordInfo = GetSwordOneInfo(recvMsg->id);
    if(!pSwordInfo)
    {
        CnAssert(false);
        return;
    }

    const SwordEquipTmpl* swordTmpl =SwordConfig::Instance()->GetSwordTmpl(recvMsg->id);
    if(!swordTmpl)
    {
        CnAssert(false);
        return;
    }

    std::map<uint32, int32>::const_iterator lockTmplIt = swordTmpl->lock_list.find(recvMsg->lock_id);
    if(lockTmplIt == swordTmpl->lock_list.end())
    {
        CnAssert(false);
        return;
    }

    // 已经解锁
    for (std::vector<int>::iterator lockIt = pSwordInfo->lock.begin(); lockIt != pSwordInfo->lock.end(); ++lockIt)
    {
        if(*lockIt == recvMsg->lock_id)
        {
            SendSwordUnlock(ERR_FAILED, pSwordInfo);
            return;
        }
    }

    err_code = m_player->CheckItemOne(PROP_MONEYPOINT, lockTmplIt->second);
    if(err_code != ERR_SUCCEED)
    {
        SendSwordUnlock(err_code, pSwordInfo);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_SWORD_UNLOCK, PROP_SWORD_UNLOCK, recvMsg->id, recvMsg->lock_id, lockTmplIt->second);
}

void SwordManager::onEquipReq(void* data)
{
    SWORD_EQUIP_REQ* recvMsg = (SWORD_EQUIP_REQ*)data;
    SWORD_EQUIP_ACK sendMsg;

    if(recvMsg->id == 0)        // 脱下武器
    {
        m_sword_info.equip_id = 0;
        m_player->NotifyProperty(PROP_SWORD, m_sword_info.equip_id);

        sendMsg.err = ERR_SUCCEED;
        sendMsg.id = m_sword_info.equip_id;

        m_player->SendToGate(MSG_SWORD_EQUIP_ACK, &sendMsg);
        return;
    }

    SWORD_ONE_INFO* swordOneInfo = GetSwordOneInfo(recvMsg->id);
    if(!swordOneInfo)
        return;

    if(swordOneInfo->lock.size() < 3)   // 没有全部解锁
    {
        sendMsg.err = ERR_FAILED;
        sendMsg.id = recvMsg->id;

        m_player->SendToGate(MSG_SWORD_EQUIP_ACK, &sendMsg);
        return;
    }

    m_sword_info.equip_id = recvMsg->id;
    m_player->NotifyProperty(PROP_SWORD, m_sword_info.equip_id);

    sendMsg.err = ERR_SUCCEED;
    sendMsg.id = m_sword_info.equip_id;

    m_player->SendToGate(MSG_SWORD_EQUIP_ACK, &sendMsg);

}

void SwordManager::SendSwordUnlock(int32 errCode, SWORD_ONE_INFO* pSwordInfo)
{
    SWORD_LOCK_ACK sendMsg;
    sendMsg.id = pSwordInfo->id;
    sendMsg.info = *pSwordInfo;
    m_player->SendToGate(MSG_SWORD_LOCK_ACK, &sendMsg);
}

bool SwordManager::UpStar(SWORD_ONE_INFO* swordOneInfo, int starNum)
{
    const SwordLevelTmpl* levelTmpl = SwordConfig::Instance()->GetLevelTmpl(swordOneInfo->id, swordOneInfo->level);
    if(!levelTmpl)
        return false;

    int32 star = swordOneInfo->star;

    int32 maxLevel = SwordConfig::Instance()->GetMaxLevel(swordOneInfo->id);
    for (int32 i = 0; i < starNum; ++i)
    {
        if(swordOneInfo->level >= maxLevel)
        {
            if(swordOneInfo->star >= SwordConfig::Instance()->GetMaxStarTmpl(swordOneInfo->id, swordOneInfo->level))
                return false;

            star++;
        }
        else
        {
            star++;

            if(star > SwordConfig::Instance()->GetMaxStarTmpl(swordOneInfo->id, swordOneInfo->level))
                return false;
        }
    }

    swordOneInfo->star = star;

    return true;
}
