#include "gameholder_pch.h"
#include "wing_manager.h"
#include "wing_attr_config_manager.h"
#include "player.h"
#include "item_manager.h"
#include "sdbit.h"
#include "item_config_manager.h"
#include "formula_manager.h"
#include "game_util.h"
#include "event_define.h"
#include "trade_manager.h"
#include "property_set_calculater.h"
#include "vip_config.h"


WingManager::WingManager(Player* player) :
m_player(player)
{
    m_PropertyDirty = true;
    Clear();
}

WingManager::~WingManager()
{

}

void WingManager::Clear()
{
    m_wing_info.Clear();
}

void WingManager::LoadInfo(const WING_INFO& info)
{
    m_wing_info = info;
}

void WingManager::FillInfo(WING_INFO& info)
{
    info = m_wing_info;
}

void WingManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_WING_INFO_REQ:
        {
            onWingInfoReq();
        }
        break;      
    case MSG_WING_ADVANCE_REQ:            // 翅膀升阶请求
        {
            onAdvanceReq(data);
        }
        break;
    case MSG_WING_TRANSMOGRIFICATION_REQ: // 穿戴翅膀
        {
            onTransmogrificationReq(msgID, data);
        }
        break;
    case MSG_WING_ACTIVATE_REQ:           // 翅膀激活
        {
            onActivateReq(data);
        }
        break;
    default:
        break;
    }
}

void WingManager::onWingInfoReq()
{
     m_player->SendToGate(MSG_WING_INFO_ACK, &m_wing_info);
}

void WingManager::onAdvanceReq(void* data)
{
    WING_ADVANCE_REQ* recvMsg = (WING_ADVANCE_REQ*)data;

    uint32 error_code = ERR_FAILED;
    // 验证翅膀在不在
    WING_ONE_INFO* pWingOneInfo = GetWingOneInfo(recvMsg->id);
    if(!pWingOneInfo)
        goto _END;

    // 普通升阶
    if (recvMsg->type == ORD_ADVANCE)
    {
        // 即将升的级数
        // x阶y星换算到wing_attr.xml中的等级中的公式： (x-1)*5+1 
        // uint32 upToLevel = ((pWingOneInfo->level -1) * 5 + pWingOneInfo->star) + 1;

        uint32 upToLevel = pWingOneInfo->level + 1;
        const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingTemplate = WingAttrConfigManager::Instance()->FindWingItem(recvMsg->id, upToLevel);
        if(!wingTemplate)
            goto _END;
        bool checkItemNum = m_player->GetItemManager()->CheckItemNum(wingTemplate->item_id_1,wingTemplate->num_1);

        error_code = checkItemNum ? ERR_SUCCEED : ERR_NO_MATERIAL;
        if(error_code != ERR_SUCCEED)
        {
            goto _END;
        }

        if(!AdvanceOneWing(recvMsg->id, wingTemplate))
        {
            error_code = ERR_FAILED;
            goto _END;
        }
        
        m_PropertyDirty = true;
        // 删除背包物品
        m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_WING_ADVANCE, wingTemplate->item_id_1,wingTemplate->num_1);
        m_player->CalcProperty();
    }
    else if (recvMsg->type == ONCE_ADVANCE)
    {
        // 即将升到xx级
        uint32 upToLevel = 0;
        // 如果是5星的情况下,+1即可升阶
        if (pWingOneInfo->star ==5)
        {
            upToLevel = (pWingOneInfo->level * pWingOneInfo->star) + 1;
        }else{
            // 如果是小于五星的
            upToLevel = (pWingOneInfo->level * pWingOneInfo->star) + (5-pWingOneInfo->star) + 1;
        }

        const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingTemplate = WingAttrConfigManager::Instance()->FindWingItem(recvMsg->id, upToLevel);
        if(!wingTemplate)
            goto _END;

        // 查询背包里的物品是否充足
        bool checkItemNum = m_player->GetItemManager()->CheckItemNum(wingTemplate->item_id_1,wingTemplate->num_1);

        error_code = checkItemNum ? ERR_SUCCEED : ERR_FAILED;
        if(error_code != ERR_SUCCEED)
        {
            goto _END;
        }

        if(!AdvanceOneWing(recvMsg->id, wingTemplate))
        {
            error_code = ERR_FAILED;
            goto _END;
        }

        // 删除背包物品
        m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_WING_ADVANCE, wingTemplate->item_id_1,wingTemplate->num_1);
    }
    else if (recvMsg->type == AUTO_ADVANCE)
    {
        do 
        {
            // 即将升的级数
            uint32 upToLevel = (pWingOneInfo->level * pWingOneInfo->star) + 1;
            const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingTemplate = WingAttrConfigManager::Instance()->FindWingItem(recvMsg->id, upToLevel);
            if(!wingTemplate)
                goto _END;
            bool checkItemNum = m_player->GetItemManager()->CheckItemNum(wingTemplate->item_id_1,wingTemplate->num_1);

            error_code = checkItemNum ? ERR_SUCCEED : ERR_FAILED;
            if(error_code != ERR_SUCCEED)
            {
                goto _END;
            }

            if(!AdvanceOneWing(recvMsg->id, wingTemplate))
            {
                error_code = ERR_FAILED;
                goto _END;
            }

            // 删除背包物品
            m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_WING_ADVANCE, wingTemplate->item_id_1,wingTemplate->num_1);
        } while (true);
    }

    error_code = ERR_SUCCEED;
    //m_player->SendTaskEvent(TASK_EVENT_UP_WING, pWingOneInfo->level, pWingOneInfo->id, 0);   // 通知任务
    //m_player->SendTaskEvent(TASK_EVENT_NEW_UP_WING, pWingOneInfo->level, pWingOneInfo->id, 0);   // 通知任务
    //m_player->SendTaskEvent(TASK_EVENT_USE_WING, 0, 0);                     // 通知任务

//     m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_WING, m_player->GetPropertySetCalculater()->GetBattlePowerByWingAltas() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByWingBaseProp(), 1 , 0);

    // 重新计算角色属性
    //m_player->CalcProperty();

_END:
    // 发送给客户端
    SendWingAdvanck(error_code, recvMsg->id);

}

void WingManager::SendWingAdvanck(uint16 error_code, uint32 wingID)
{
    WING_ONE_INFO* pWingOneInfo = GetWingOneInfo(wingID);

    WING_ADVANCE_ACK sendMsg;
    sendMsg.err = error_code;
    if(error_code == ERR_SUCCEED)
    {
        if(pWingOneInfo)
            sendMsg.info = *pWingOneInfo;
    }
    m_player->SendToGate(MSG_WING_ADVANCE_ACK, &sendMsg);
}


void WingManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case EVENT_CHANGE_VIP:  //活动完成后
        //ChangeVipLock();
        break;
    }
}

WING_ONE_INFO* WingManager::GetWingOneInfo(uint32 id)
{
    for(std::vector<WING_ONE_INFO>::iterator it = m_wing_info.list.begin(); it != m_wing_info.list.end(); ++it)
    {
        if(it->id == id)
            return &(*it);
    }

    return NULL;
}

void WingManager::WingMarquee(int32 level)
{
    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_WING_LEVE_UP;
    marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    marquee.parame_list.push_back(MarQueeWing(level));
    m_player->OnMarQuee(marquee);
}

bool WingManager::AdvanceOneWing(uint32 id, const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingItemTpl)
{
    WING_ONE_INFO* pWingOneInfo = GetWingOneInfo(id);
    if(!pWingOneInfo)
        return false;

    pWingOneInfo->level = wingItemTpl->level;
    pWingOneInfo->step = wingItemTpl->step;
    pWingOneInfo->star = wingItemTpl->star;
    return true;


    //int32 star = pWingOneInfo->star;
    //int32 level = pWingOneInfo->level;

    //uint32 upToLevel = wingItemTpl->level;
    //
    //// 等级z换算到x阶y星的规则： z%5为0 => x=z/5,y=5; z%5!=0 => x为(z/5)+1,y为z%5
    //uint32 mod = upToLevel % 5;
    //uint32 div = upToLevel / 5;
    //if(mod == 0)
    //{
    //    // 取于等于0 ，表示取商阶数的最高星数，比如:5,是1阶5五星，30是6阶5星
    //    level = div;
    //    star = 5;
    //}else{
    //    // 取于不等于0 ，表示取商阶数+1的取余的星数，比如:12,是3阶2五星，31是7阶1星
    //    level = div+1;
    //    star = mod;
    //}
   
    //pWingOneInfo->star = star;
    //pWingOneInfo->level = level;

    //return true;
}


// 激活翅膀
void WingManager::onActivateReq(void* data)
{
    WING_ACTIVATE_REQ* recvMsg = (WING_ACTIVATE_REQ*)data;
    if(recvMsg->id == 0)
        return;

    // 是否存在这种翅膀
    const WING_ATTR_ROOT_WING_STRUCT* wingTemplate = WingAttrConfigManager::Instance()->FindWing(recvMsg->id);
    if(!wingTemplate)
    {
        m_player->NotifyErrorCode(ERR_NO_EXIST);
        CnAssert(false);
        return;
    }

    // 是否有翅膀卡
    bool checkItemNum = m_player->GetItemManager()->CheckItemNum(wingTemplate->item_id, 1);

    uint16 errCode = checkItemNum ? ERR_SUCCEED : ERR_NO_MATERIAL;
    if(errCode != ERR_SUCCEED)
    {
        m_player->NotifyErrorCode(errCode);
        return;
    }

    // 激活翅膀
    if(!ActivateWing(recvMsg->id))
    {
        m_player->NotifyErrorCode(ERR_WIMG_ACTIVATE_FAILED);
        return;
    }

    m_PropertyDirty = true;
    // 删除背包物品
    m_player->GetItemManager()->RemoveItemNumByItemId(REASON_ITEM_SUB_WING_ACTIVATE, wingTemplate->item_id, 1);

    // 计算加成属性
    m_player->CalcProperty();
    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_WING, 0, 1 , 0);


    //m_player->GetPropertySetCalculater()->GetWingPro();

    m_player->SendToGate(MSG_WING_ACTIVATE_ACK, &m_wing_info);
}

void WingManager::onTransmogrificationReq(uint32 msgID, void* data)
{
    WING_TRANSMOGRIFICATION_REQ* recvMsg = (WING_TRANSMOGRIFICATION_REQ*)data;

    WING_TRANSMOGRIFICATION_ACK sendMsg;

    if(recvMsg->id == 0)        // 脱下翅膀
    {
        m_wing_info.transmogrification = 0;
        m_player->NotifyProperty(PROP_WING, m_wing_info.transmogrification);

        sendMsg.err = ERR_SUCCEED;
        sendMsg.info = m_wing_info;

        m_player->SendToGate(MSG_WING_TRANSMOGRIFICATION_ACK, &sendMsg);
        return;
    }

    // 如果翅膀没有激活
    WING_ONE_INFO* wingOneInfo = GetWingOneInfo(recvMsg->id);
    if(!wingOneInfo)
    {
        m_player->NotifyErrorCode(ERR_WIMG_NO_ACTIVATE);
        return;
    }

    // 是否存在这种翅膀
    if(!WingAttrConfigManager::Instance()->FindWing(recvMsg->id))
    {
        sendMsg.err = ERR_NO_EXIST;
        m_player->SendToGate(MSG_WING_TRANSMOGRIFICATION_ACK, &sendMsg);
        // 已经激活的翅膀不应该不存在
        CnAssert(false);
        return;
    }

    // 幻化
    m_wing_info.transmogrification = recvMsg->id;

    m_player->NotifyProperty(PROP_WING, m_wing_info.transmogrification);

    sendMsg.err = ERR_SUCCEED;
    sendMsg.info = m_wing_info;

    m_player->SendToGate(MSG_WING_TRANSMOGRIFICATION_ACK, &sendMsg);
}

void WingManager::sendPro()
{
    PROPERTY_LIST_NTF sendMsg;
    PropertySet& wingProSet = m_player->GetPropertySetCalculater()->GetWingPro();  // 翅膀属性

    for (PropertyList::iterator Iter = wingProSet.m_list.begin(); Iter != wingProSet.m_list.end();++Iter)
    {
        PROPERTY_NTF val;
        val.property_id = Iter->type;
        val.value = Iter->value;
        val.str = "";

        sendMsg.list.push_back(val);
    }
    sendMsg.object_id = m_player->GetId();

    m_player->SendToGate(MSG_WING_PRO_NTF, &sendMsg);
}

void WingManager::CalWingProp()
{
    if(m_wing_info.list.size() <= 0)
        return;

    if (m_PropertyDirty == false)
    {
        return;
    }

    m_PropertyDirty = false;

    PropertySet& wingProSet = m_player->GetPropertySetCalculater()->GetWingPro();  // 翅膀属性
    wingProSet.Clear();

    for (std::vector<WING_ONE_INFO>::iterator wingIt = m_wing_info.list.begin(); wingIt != m_wing_info.list.end(); ++wingIt)
    {
        const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingItemTpl = WingAttrConfigManager::Instance()->FindWingItem(wingIt->id, wingIt->level);
        if(!wingItemTpl){
            CnAssert(false);
            CnWarn("WingAttr::Init not found template by id=%u! and level=%u\n", wingIt->id,wingIt->level);
            continue;
        }

        wingProSet.Add(wingItemTpl->attr_type_1,wingItemTpl->attr_1);
        wingProSet.Add(wingItemTpl->attr_type_2,wingItemTpl->attr_2);
        wingProSet.Add(wingItemTpl->attr_type_3,wingItemTpl->attr_3);
        wingProSet.Add(wingItemTpl->attr_type_4,wingItemTpl->attr_4);
    }

    // 每计算一次翅膀属性都发送一次
    sendPro();
}

bool WingManager::ActivateWing(uint32 id)
{
    // 翅膀已经激活
    WING_ONE_INFO* wingIt = GetWingOneInfo(id);
    if(wingIt)
        return false;

    // 是否存在这种翅膀
    if(!WingAttrConfigManager::Instance()->FindWing(id))
    {
        return false;
    }

    // 激活翅膀
    WING_ONE_INFO wingOneInfo;
    wingOneInfo.id = id;
    wingOneInfo.level = 1;
    wingOneInfo.step = 1;
    wingOneInfo.star = 0;
    m_wing_info.list.push_back(wingOneInfo);
    m_wing_info.count = m_wing_info.list.size();

    m_player->SendPlaySystemLog(PLAYSYSTEM_WING_OPEN);          // 幻化激活

    // 重新计算人物属性
    m_player->CalcProperty();
    // 通知客户端更新翅膀信息
    m_player->SendToGate(MSG_WING_INFO_ACK, &m_wing_info);
    

    return true;
}

int32 WingManager::GetWingLevel()
{
    uint32 maxLevel = 0;
    for (std::vector<WING_ONE_INFO>::iterator wingIt = m_wing_info.list.begin(); wingIt != m_wing_info.list.end(); ++wingIt)
    {
        if(wingIt->level > maxLevel)
            maxLevel = wingIt->level;
    }

    return maxLevel;
}