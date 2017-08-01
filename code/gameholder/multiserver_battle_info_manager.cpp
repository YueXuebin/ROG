#include "gameholder_pch.h"
#include "multiserver_battle_info_manager.h"
#include "multiserver_battle_config.h"
#include "multiserver_battle_manager.h"
#include "server_config.h"
#include "game_define.h"
#include "item_manager.h"
#include "item_config_manager.h"
#include "game_setting_manager.h"
#include "player.h"


MultiserverBattleInfoManager::MultiserverBattleInfoManager(Player* pPlayer):
m_player(pPlayer)
{
}

MultiserverBattleInfoManager::~MultiserverBattleInfoManager(void)
{
}

void MultiserverBattleInfoManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_MULTISERVER_BATTLE_INFO_REQ:
        {
            OnBattleInfoReq(data);          // 战斗信息请求
        }
        break;
    case MSG_MULTISERVER_BATTLE_APPLY_REQ:
        {
            OnMultiserverBattlePlayerReq(msgID, data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_QUIT_REQ:
        {
            OnMultiserverBattlePlayerReq(msgID, data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_WAIT_REQ:
        {
            OnWaitAck(data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_EXPLOIT_REQ:
        {
            OnGetLevelExploitReq(data);          // 战斗信息请求
        }
        break;
    case MSG_MULTISERVER_BATTLE_ALLIANCE_REQ:
        {
            OnGetLevelAllianceReq(data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_EXPLOIT_BUY_REQ:
        {
            OnExploitBuyReq(data);              // 功勋值购买请求
        }
        break;
    case MSG_MULTISERVER_BATTLE_BUY_REQ_ALLIANCE:
        {
            OnAllianceBuyReq(data);             // 
        }
        break;
    case MSG_MULTISERVER_BATTLE_BUY_REQ_SEASON:
        {
            OnSeasonBuyReq(data);               // 
        }
        break;

    default:
        break;
    }
}

// ==================================================
// 跨服战斗信息请求
// ==================================================
void MultiserverBattleInfoManager::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    switch(eventId)
    {
    case OFF_EVENT_CHAOS_REWARD:
        //onOffEventChaosReward(arg3);
        break;
    }
}

// ==================================================
// 跨服战斗信息请求
// ==================================================
void MultiserverBattleInfoManager::OnMultiserverBattlePlayerReq(uint32 msgID, void* data)
{
    if(!data)
        return;

    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;

    const MultiserverBatttleItem* bItem  = MultiserverBattleConfig::Instance()->GetBattleInfo(recvMsg->battle_type);
    if(!bItem)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    if(m_player->GetLevel() < (int32)bItem->need_level)
    {
        //等级不够
        m_player->NotifyErrorCode(ERR_MULTISERVER_NEED_LEVEL);
        return;
    }

    MULTISERVER_BATTLE_ITEM_INFO* info = GetCurrentBattleInfo(recvMsg->battle_type);
    if(!info)
    {
        CnAssert(false);
        return;
    }

    if(GameSettingManager::Instance()->GetMultiserverEnterNum() != 0 && info->today_count >= (int)GameSettingManager::Instance()->GetMultiserverEnterNum())
    {
        // 次数用完
        m_player->NotifyErrorCode(ERR_MULTISERVER_NEED_COUNT);
        return;
    }

    bool openState = MultiserverBattleConfig::Instance()->IsOpenTimer(recvMsg->battle_type);
    if(!openState)
    {
        // 未开启
        m_player->NotifyErrorCode(ERR_MULTISERVER_NEED_TIME);
        return;
    }

    recvMsg->channel_id = 1;

    // 对角色的 gateID 和 playerID 赋值
    recvMsg->gate_id = m_player->GetGateSvrID();
    recvMsg->player_id.id_h = U64ID_HIGH(m_player->GetPlayerID());
    recvMsg->player_id.id_l = U64ID_LOW(m_player->GetPlayerID());
    recvMsg->game_id = ServerConfig::Instance()->GetMyID();
    recvMsg->apply_time = (int32)Crown::SDNow().GetTimeValue();
    recvMsg->wait_time = bItem->wait;
    recvMsg->battle_power = m_player->GetBattlePower();

    MultiserverBattleManager::Instance()->OnRecvFromPlayer(msgID, data);
}

void MultiserverBattleInfoManager::LoadMultiserverBattleInfo( const MULTISERVER_BATTLE_BAG& info )
{
    m_info = info;
    m_info.enter_num = GameSettingManager::Instance()->GetMultiserverEnterNum();
    m_info.yesterday_num = GameSettingManager::Instance()->GetMultiserverYesterdayNum();
    m_info.yesterday_3v3_num = GameSettingManager::Instance()->GetMultiserverYesterday3V3Num();
    CheckNewSeason();
}

void MultiserverBattleInfoManager::FillMultiserverBattleInfo( MULTISERVER_BATTLE_BAG& info )
{
    info = m_info;
}

void MultiserverBattleInfoManager::OnWin(uint32 battleType, int32 currentIntegral)
{
    const MultiserverBatttleItem* fashionItem  = MultiserverBattleConfig::Instance()->GetBattleInfo(battleType);
    if(fashionItem)
    {
        AddIntegralPoint(currentIntegral);
        AddExploitPoint(fashionItem->win_exploit);

        ++m_info.win_count;
    }

    MULTISERVER_BATTLE_ITEM_INFO* info = GetCurrentBattleInfo(battleType);
    if(info)
    {
        info->today_count += 1;
        info->today_exploit += fashionItem->win_exploit;
        info->last_time = (int32)Crown::SDNow().GetTimeValue();
    }
}

void MultiserverBattleInfoManager::onFail(uint32 battleType, int32 currentIntegral)
{
    const MultiserverBatttleItem* fashionItem  = MultiserverBattleConfig::Instance()->GetBattleInfo(battleType);
    if(fashionItem)
    {
        SubIntegralPoint(currentIntegral);
        AddExploitPoint(fashionItem->fail_exploit);

        ++m_info.lose_count;
    }

    MULTISERVER_BATTLE_ITEM_INFO* info = GetCurrentBattleInfo(battleType);
    if(info)
    {
        info->today_count += 1;
        info->today_exploit += fashionItem->fail_exploit;
        info->last_time = (int32)Crown::SDNow().GetTimeValue();
    }
}

void MultiserverBattleInfoManager::OnBattleInfoReq( void* data )
{
    NONE* recvMsg = (NONE*)data;
    if(!recvMsg)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    OnClearDayInfo();

    revcBattleInfoAck(ERR_SUCCEED);

    // 到center上去查找排名等数据
    m_player->SendToCenter(MSG_MULTISERVER_BATTLE_INFO_REQ, data);
}

MULTISERVER_BATTLE_ITEM_INFO* MultiserverBattleInfoManager::GetCurrentBattleInfo(uint32 type)
{
    for (std::vector<MULTISERVER_BATTLE_ITEM_INFO>::iterator it = m_info.list.begin(); it != m_info.list.end(); it++)
    {
        if(it->type == type)
        {
            return &(*it);
        }
    }

    MULTISERVER_BATTLE_ITEM_INFO newInfo;
    newInfo.type = type;
    newInfo.today_count = 0;
    newInfo.today_exploit = 0;
    newInfo.last_time = (int32)Crown::SDNow().GetTimeValue();
    m_info.list.push_back(newInfo);

    for (std::vector<MULTISERVER_BATTLE_ITEM_INFO>::iterator it = m_info.list.begin(); it != m_info.list.end(); it++)
    {
        if(it->type == type)
        {
            return &(*it);
        }
    }

    return NULL;
}

void MultiserverBattleInfoManager::OnClearDayInfo()
{
    Crown::CSDDateTime today_time = Crown::SDNow(); // 今日凌晨时间点
    today_time.SetTime(0, 0, 0);
    int32 tcount = 0;
    int32 t3v3count = 0;
    bool tflag = FALSE;

    for (std::vector<MULTISERVER_BATTLE_ITEM_INFO>::iterator it = m_info.list.begin(); it != m_info.list.end(); it++)
    {
        if((int32)it->last_time < (int32)today_time.GetTimeValue())
        {
            tflag = TRUE;
            if((int32)it->last_time > ((int32)today_time.GetTimeValue() - 86400))
            {
                if (it->type == MULTISERVER_BATTLE_APPLY_ONE)
                {
                    tcount += it->today_count;
                }
                else if (it->type == MULTISERVER_BATTLE_APPLY_THREE)
                {
                    t3v3count += it->today_count;
                }
            }
            it->today_count = 0;
            it->today_exploit = 0;
            it->last_time = (int32)Crown::SDNow ().GetTimeValue();
        }
    }
    if(tflag)
    {
        m_info.yesterday_integral = GetIntegralPoint();
        m_info.yesterday_count = tcount;
        m_info.yesterday_3v3_count = t3v3count;
        ResetSaleNumber();
    }
}

void MultiserverBattleInfoManager::OnWaitAck( void* data )
{
    MULTISERVER_BATTLE_WAIT_ACK* recvMsg = (MULTISERVER_BATTLE_WAIT_ACK*)data;

    if(!recvMsg)
        return;

    const MultiserverBatttleItem* fashionItem  = MultiserverBattleConfig::Instance()->GetBattleInfo(recvMsg->battle_type);
    if(!fashionItem)
        return;

    if(m_player->GetLevel() < (int32)fashionItem->need_level)
    {
        //等级不够
        return;
    }

    MULTISERVER_BATTLE_ITEM_INFO* info = GetCurrentBattleInfo(recvMsg->battle_type);
    if(GameSettingManager::Instance()->GetMultiserverEnterNum() != 0 && info->today_count >= (int32)GameSettingManager::Instance()->GetMultiserverEnterNum())
    {
        //次数用完
        return;
    }

    bool openState = MultiserverBattleConfig::Instance()->IsOpenTimer(recvMsg->battle_type);
    if(!openState)
    {
        //未开启
        return;
    }

    if(fashionItem)
    {
        //m_info.integral += fashionItem->win_integral;
        //m_info.exploit_point += fashionItem->win_exploit;
    }

    if(info)
    { 
        info->today_count += 1;
        //info->today_exploit += fashionItem->win_exploit;
        info->last_time = (int32)Crown::SDNow().GetTimeValue();
    }

    ERROR_CODE sendMsg;
    sendMsg.errcode = ERR_SUCCEED;
    m_player->SendToGate(MSG_MULTISERVER_BATTLE_WAIT_ACK, &sendMsg);

    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::OnGetLevelExploitReq( void* data )
{
    ERROR_CODE sendMsg;

    Crown::CSDDateTime today_time = Crown::SDNow(); // 今日凌晨时间点
    today_time.SetTime(0, 0, 0);
    if((int32)m_info.exploit_time < (int32)today_time.GetTimeValue())
    {
        if((int32)m_info.yesterday_count < GameSettingManager::Instance()->GetMultiserverYesterdayNum())
        {
            //昨天次数未完成
            sendMsg.errcode = ERR_MULTISERVER_YESTERDAY_COUNT;
        }
        else
        {
            const MultiserverLevelItem* lItem  = MultiserverBattleConfig::Instance()->GetLevelInfo(m_info.yesterday_integral);
            if(lItem)
            {
                AddExploitPoint(lItem->exploit);
            }

            m_info.exploit_time = (int32)Crown::SDNow().GetTimeValue();
            sendMsg.errcode = ERR_SUCCEED;
            revcBattleInfoAck(ERR_SUCCEED);
        }
    }
    else
    {
        //今天已领取
        sendMsg.errcode = ERR_MULTISERVER_YESTERDAY_GET;
    }

    m_player->SendToGate(MSG_MULTISERVER_BATTLE_EXPLOIT_ACK, &sendMsg);
}

void MultiserverBattleInfoManager::OnGetLevelAllianceReq(void *data)
{
    ERROR_CODE sendMsg;

    Crown::CSDDateTime today_time = Crown::SDNow(); // 今日凌晨时间点
    today_time.SetTime(0, 0, 0);
    if ((int32)m_info.alliance_time < (int32)today_time.GetTimeValue())
    {
        if ((int32)m_info.yesterday_3v3_count < GameSettingManager::Instance()->GetMultiserverYesterday3V3Num())
        {
            //昨天次数未完成
            sendMsg.errcode = ERR_MULTISERVER_YESTERDAY_COUNT_3V3;
        }
        else
        {
            const MultiserverLevelItem* lItem  = MultiserverBattleConfig::Instance()->GetLevelInfo(m_info.yesterday_integral);
            if (lItem)
            {
                AddAlliancePoint(lItem->alliance);
            }
            m_info.alliance_time = (int32)Crown::SDNow().GetTimeValue();
            sendMsg.errcode = ERR_SUCCEED;
            revcBattleInfoAck(ERR_SUCCEED);
        }
    }
    else
    {
        //今天已领取
        sendMsg.errcode = ERR_MULTISERVER_YESTERDAY_GET_3V3;
    }

    m_player->SendToGate(MSG_MULTISERVER_BATTLE_ALLIANCE_ACK, &sendMsg);
}

void MultiserverBattleInfoManager::OnExploitBuyReq( void* data )
{
    INCARNATION_BUY_REQ* recvMsg = (INCARNATION_BUY_REQ*)data;

    //const ItemTemplate* itemTmpl = NULL;
    const MultiserverSaleInfo* saleInfo = NULL;
    int32 honor_point = 0;

    if(!recvMsg) 
        return;

    saleInfo = MultiserverBattleConfig::Instance()->GetSaleItem(recvMsg->item_id);
    if(!saleInfo)
    {
        m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
        return;
    }

    //itemTmpl = ItemConfig::Instance()->GetItemTmpl(saleInfo->id);
    //if(!itemTmpl)
    //{
    //    m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
    //    return;
    //}


    // 超过购买限制的直接设置为购买最大值
    recvMsg->item_num = (recvMsg->item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->item_num);
    if((saleInfo->exploit <= 0) || (recvMsg->item_num < 1) || (recvMsg->item_num > g_MaxBuyNum))
    {
        return;
    }

    // 检查包裹
    //if(!m_player->GetItemManager()->CanAddItem(saleInfo->id, recvMsg->item_num))
    //{
    //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
    //    return;
    //}

    // 段位限制
    const MultiserverLevelItem* lItem  = MultiserverBattleConfig::Instance()->GetLevelInfo(GetIntegralPoint());
    if(!lItem)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    if(lItem->lv < saleInfo->level)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    bool limitFlag = CheckSalelimit(saleInfo->product_id, recvMsg->item_num, saleInfo->limit);
    if(!limitFlag)
    {
        // 超出购买限制
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_NUM);
        return;
    }


    // 所需功勋
    honor_point = saleInfo->exploit * recvMsg->item_num;
    if((int32)GetExploitPoint() < honor_point)
    {
        SendExploitBuyAck(ERR_FAILED);
        return;
    }
    else
    {
        // 检查并扣除功勋
        SubExploitPoint(honor_point);

        AddSaleNumber(saleInfo->product_id, recvMsg->item_num);
    }

    // 添加物品
    //m_player->GetItemManager()->AddItemByNum(saleInfo->id, recvMsg->item_num, REWARD_REASON_MULTISERVER_ITEM_BUY);

    SendExploitBuyAck(ERR_SUCCEED);
    revcBattleInfoAck(ERR_SUCCEED);

}

void MultiserverBattleInfoManager::SendExploitBuyAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_MULTISERVER_BATTLE_EXPLOIT_BUY_ACK, &sendMsg);
}

void MultiserverBattleInfoManager::revcBattleInfoAck( uint16 error )
{
    MULTISERVER_BATTLE_INFO_ACK sendMsg;
    sendMsg.errcode = error;
    sendMsg.info = m_info;
    m_player->SendToGate(MSG_MULTISERVER_BATTLE_INFO_ACK, &sendMsg);
}

void MultiserverBattleInfoManager::OnloadInfo( MULTISERVER_BATTLE_PK_END_INFO &sendMsg, uint32 battleType, int32 currentIntegral, int32 currentFailIntegral)
{
    sendMsg.name = m_player->GetName();
    sendMsg.integral = GetIntegralPoint();
    if(battleType == 0)
    {
        sendMsg.exploit = 0;
        return;
    }

    const MultiserverBatttleItem* bItem  = MultiserverBattleConfig::Instance()->GetBattleInfo(battleType);
    if(bItem)
    {
        if(sendMsg.success == TRUE)
        {
            //sendMsg.exploit = bItem->win_integral;
            sendMsg.exploit = currentIntegral;
        }
        else
        {
            //sendMsg.exploit = bItem->fail_integral;
            sendMsg.exploit = currentFailIntegral;
        }
    }

}

bool MultiserverBattleInfoManager::CheckSalelimit( uint32 id, uint32 num ,uint32 limit)
{
    bool limitFlag = false;
    bool limitNum = false;
    for (std::vector<LIMIT_BUY>::iterator it = m_info.item_list.begin(); it != m_info.item_list.end(); it++)
    {
        if(it->product_id == id)
        {
            limitFlag = true;
            //判断是否超出购买限制
            if(it->num + num > limit)
            {
                limitNum = false;
            }
            else
            {
                limitNum = true;
            }
        }
    }

    if(!limitFlag)
    {
        LIMIT_BUY limitItem ;
        limitItem.product_id = id;
        limitItem.num = 0;
        m_info.item_list.push_back(limitItem);

        if(num > limit)
        {
            limitNum = false;
        }
        else
        {
            limitNum = true;
        }
    }

    return limitNum;
}

bool MultiserverBattleInfoManager::AddSaleNumber( uint32 id, uint32 num )
{
    for (std::vector<LIMIT_BUY>::iterator it = m_info.item_list.begin(); it != m_info.item_list.end(); it++)
    {
        if(it->product_id == id)
        {
            it->num += num;
            return true;
        }
    }
    return false;
}

void MultiserverBattleInfoManager::ResetSaleNumber()
{
    for (std::vector<LIMIT_BUY>::iterator it = m_info.item_list.begin(); it != m_info.item_list.end(); it++)
    {
        it->num = 0; 
    }
}

void MultiserverBattleInfoManager::UpdateMultiserverBattleInfo()
{
    m_info.enter_num = GameSettingManager::Instance()->GetMultiserverEnterNum();
    m_info.yesterday_num = GameSettingManager::Instance()->GetMultiserverYesterdayNum();
    m_info.yesterday_3v3_num = GameSettingManager::Instance()->GetMultiserverYesterday3V3Num();
    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::AddIntegralPoint(int32 val)
{
    int64 integral_point = m_info.integral + val;

    if(integral_point < 0)
    {
        integral_point = 0;
    }
    else if(m_info.integral > (int32)MultiserverBattleConfig::Instance()->GetMaxIntegral())
    {
        integral_point = MultiserverBattleConfig::Instance()->GetMaxIntegral();
    }

    m_info.integral = (int32)integral_point;
    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SubIntegralPoint(int32 val)
{
    if(val < 0)
    {
        CnAssert(false);
        return;
    }

    if(m_info.integral >= 0 && m_info.integral >= val)
        m_info.integral -= val;
    else
        m_info.integral = 0;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::ChangeIntegralPoint(int32 val)
{
    if(val >= 0)
        AddIntegralPoint(val);
    else
        SubIntegralPoint(-val);
}

void MultiserverBattleInfoManager::SetIntegralPoint( int32 val )
{
    m_info.integral = val;
    if(m_info.integral > (int32)MultiserverBattleConfig::Instance()->GetMaxIntegral())
    {
        m_info.integral = MultiserverBattleConfig::Instance()->GetMaxIntegral();
    }

    m_player->SetPropDirty();
    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::AddExploitPoint(int32 val)
{
    int64 exploit_point = 0;

    exploit_point = m_info.exploit_point + val;

    if(exploit_point < 0)
    {
        exploit_point = 0;
    }
    else if(m_info.exploit_point > MAX_CURRENCY_NUM)
    {
        exploit_point = MAX_CURRENCY_NUM;
    }

    m_info.exploit_point = (int32)exploit_point;
    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SubExploitPoint(int32 val)
{
    if(val < 0)
    {
        CnAssert(false);
        return;
    }

    if(m_info.exploit_point >= 0 && m_info.exploit_point >= val)
        m_info.exploit_point -= val;
    else
        m_info.exploit_point = 0;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SetExploitPoint( int32 val )
{
    m_info.exploit_point = val;

    m_player->SetPropDirty();
    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::OnAllianceBuyReq(void* data)
{
    INCARNATION_BUY_REQ* recvMsg = (INCARNATION_BUY_REQ*)data;

    //const ItemTemplate* itemTmpl = NULL;
    const MultiserverSaleInfo* saleInfo = NULL;
    int32 honor_point = 0;

    if(!recvMsg) 
        return;

    saleInfo = MultiserverBattleConfig::Instance()->GetAllianceSaleItem(recvMsg->item_id);
    if(!saleInfo)
    {
        m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
        return;
    }

    //itemTmpl = ItemConfig::Instance()->GetItemTmpl(saleInfo->id);
    //if(!itemTmpl)
    //{
    //    m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
    //    return;
    //}


    // 超过购买限制的直接设置为购买最大值
    recvMsg->item_num = (recvMsg->item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->item_num);
    if((saleInfo->exploit <= 0) || (recvMsg->item_num < 1) || (recvMsg->item_num > g_MaxBuyNum))
    {
        return;
    }

    // 检查包裹
    //if(!m_player->GetItemManager()->CanAddItem(saleInfo->id, recvMsg->item_num))
    //{
    //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
    //    return;
    //}

    // 段位限制
    const MultiserverLevelItem* lItem  = MultiserverBattleConfig::Instance()->GetLevelInfo(GetIntegralPoint());
    if(!lItem)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    if(lItem->lv < saleInfo->level)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    bool limitFlag = CheckSalelimit(saleInfo->product_id, recvMsg->item_num, saleInfo->limit);
    if(!limitFlag)
    {
        // 超出购买限制
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_NUM);
        return;
    }

    // 所需
    honor_point = saleInfo->exploit * recvMsg->item_num;
    if((int32)GetAlliancePoint() < honor_point)
    {
        SendExploitBuyAck(ERR_FAILED);
        return;
    }
    else
    {
        // 检查并扣除
        SubAlliancePoint(honor_point);
        AddSaleNumber(saleInfo->product_id, recvMsg->item_num);
    }

    // 添加物品
    //m_player->GetItemManager()->AddItemByNum(saleInfo->id, recvMsg->item_num, REWARD_REASON_MULTISERVER_ITEM_BUY);

    SendAllianceBuyAck(ERR_SUCCEED);
    revcBattleInfoAck(ERR_SUCCEED);

}

void MultiserverBattleInfoManager::SendAllianceBuyAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_MULTISERVER_BATTLE_BUY_ACK_ALLIANCE, &sendMsg);
}

void MultiserverBattleInfoManager::OnSeasonBuyReq( void* data )
{
    INCARNATION_BUY_REQ* recvMsg = (INCARNATION_BUY_REQ*)data;

    //const ItemTemplate* itemTmpl = NULL;
    const MultiserverSaleInfo* saleInfo = NULL;
    int32 honor_point = 0;

    if(!recvMsg) 
        return;

    saleInfo = MultiserverBattleConfig::Instance()->GetSeasonSaleItem(recvMsg->item_id);
    if(!saleInfo)
    {
        m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
        return;
    }

    //itemTmpl = ItemConfig::Instance()->GetItemTmpl(saleInfo->id);
    //if(!itemTmpl)
    //{
    //    m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
    //    return;
    //}


    // 超过购买限制的直接设置为购买最大值
    recvMsg->item_num = (recvMsg->item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->item_num);
    if((saleInfo->exploit <= 0) || (recvMsg->item_num < 1) || (recvMsg->item_num > g_MaxBuyNum))
    {
        return;
    }

    // 检查包裹
    //if(!m_player->GetItemManager()->CanAddItem(saleInfo->id, recvMsg->item_num))
    //{
    //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
    //    return;
    //}

    // 段位限制
    const MultiserverLevelItem* lItem  = MultiserverBattleConfig::Instance()->GetLevelInfo(GetIntegralPoint());
    if(!lItem)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    if(lItem->lv < saleInfo->level)
    {
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_LEVEL);
        return;
    }

    bool limitFlag = CheckSalelimit(saleInfo->product_id, recvMsg->item_num, saleInfo->limit);
    if(!limitFlag)
    {
        // 超出购买限制
        m_player->NotifyErrorCode(ERR_MULTISERVER_ITEM_LIMIT_NUM);
        return;
    }

    //TODO:  扣除什么？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
    // 所需
    honor_point = saleInfo->exploit * recvMsg->item_num;
    if((int32)GetSeasonPoint() < honor_point)
    {
        SendExploitBuyAck(ERR_FAILED);
        return;
    }
    else
    {
        // 检查并扣除
        SubSeasonPoint(honor_point);
        AddSaleNumber(saleInfo->product_id, recvMsg->item_num);
    }

    // 添加物品
    //m_player->GetItemManager()->AddItemByNum(saleInfo->id, recvMsg->item_num, REWARD_REASON_MULTISERVER_ITEM_BUY);

    SendSeasonBuyAck(ERR_SUCCEED);
    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::SendSeasonBuyAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_MULTISERVER_BATTLE_BUY_ACK_SEASON, &sendMsg);
}

void MultiserverBattleInfoManager::AddAlliancePoint(int32 val)
{
    int64 alliance_point = 0;

    alliance_point = m_info.alliance_point + val;

    if(alliance_point < 0)
    {
        alliance_point = 0;
    }
    else if(m_info.alliance_point > MAX_CURRENCY_NUM)
    {
        alliance_point = MAX_CURRENCY_NUM;
    }

    m_info.alliance_point = (int32)alliance_point;
    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SubAlliancePoint(int32 val)
{
    if(val < 0)
    {
        CnAssert(false);
        return;
    }

    if(m_info.alliance_point >= 0 && m_info.alliance_point >= val)
        m_info.alliance_point -= val;
    else
        m_info.alliance_point = 0;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::ChangeAlliancePoint(int32 val)
{
    if(val >= 0)
        AddAlliancePoint(val);
    else
        SubAlliancePoint(-val);
}

void MultiserverBattleInfoManager::SetAlliancePoint( int32 val )
{
    m_info.alliance_point = val;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::AddSeasonPoint(int32 val)
{
    int64 season_point = 0;

    season_point = m_info.season_point + val;

    if(season_point < 0)
    {
        season_point = 0;
    }
    else if(m_info.season_point > MAX_CURRENCY_NUM)
    {
        season_point = MAX_CURRENCY_NUM;
    }

    m_info.season_point = (int32)season_point;
    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SubSeasonPoint(int32 val)
{
    if(val < 0)
    {
        CnAssert(false);
        return;
    }

    if(m_info.season_point >= 0 && m_info.season_point >= val)
        m_info.season_point -= val;
    else
        m_info.season_point = 0;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::SetSeasonPoint( int32 val )
{
    m_info.season_point = val;

    m_player->SetPropDirty();
}

void MultiserverBattleInfoManager::AddWinCount(int32 type, int32 val, uint32 time)
{
    if(val < 0)
        return;

    m_info.win_count += val;

    AddInvolvedCount(type, val, time);
}

void MultiserverBattleInfoManager::AddLoseCount(int32 type, int32 val,uint32 time)
{
    if(val < 0)
        return;

    m_info.lose_count += val;

    AddInvolvedCount(type, val, time);
}

void MultiserverBattleInfoManager::AddInvolvedCount(int32 type, int32 val, uint32 time)
{
    Crown::CSDDateTime date = Crown::SDNow();
    date.SetTime(0, 0, 0);
    uint32 todayMorningTime = (uint32)date.GetTimeValue();
    date.DecDay(1);
    uint32 yesterdayMorningTime = (uint32)date.GetTimeValue();

    if (time >= todayMorningTime)
    {
        MULTISERVER_BATTLE_ITEM_INFO* info = GetCurrentBattleInfo(type);
        info->today_count += val;
        info->last_time = Crown::SDMax(time, info->last_time);
    }
    else if (time >= yesterdayMorningTime)
    {
        if (type == MULTISERVER_BATTLE_APPLY_ONE)
        {
            m_info.yesterday_count += val;
        }
        else if (type == MULTISERVER_BATTLE_APPLY_THREE)
        {
            m_info.yesterday_3v3_count += val;
        }
    }
}

void MultiserverBattleInfoManager::CheckNewSeason()
{
    uint32 seasonTime = MultiserverBattleManager::Instance()->GetSeasonTime();

    if(seasonTime > (uint32)m_info.season_time)
    {
        SetIntegralPoint(0);
        m_info.win_count = 0;
        m_info.lose_count = 0;
        m_info.season_time = seasonTime;
    }
}

void MultiserverBattleInfoManager::CalDailyRefresh()
{
    OnClearDayInfo();
    revcBattleInfoAck(ERR_SUCCEED);
}

void MultiserverBattleInfoManager::onOffEventChaosReward(const std::string& infoStr)
{
    CHAOS_REWARD_OFFLINE_EVENT infoStruct;
    if(!JsonToStruct(infoStruct, infoStr))
    {
        CnError("chaos reward error! string to CHAOS_REWARD_OFFLINE_EVENT error!\n");
        return;
    }

    int32 nowTime = (int32)Crown::SDNow().GetTimeValue();
    if(nowTime > infoStruct.season_time)        // 过了一赛季 直接退出赛季
        return;

    AddAlliancePoint(infoStruct.alliance_point);

    ChangeIntegralPoint(infoStruct.integral_point);
    if(infoStruct.win)
    {
        AddWinCount(MULTISERVER_BATTLE_APPLY_THREE, 1, infoStruct.record_time);
    }
    else
    {
        AddLoseCount(MULTISERVER_BATTLE_APPLY_THREE, 1, infoStruct.record_time);
    }
}
