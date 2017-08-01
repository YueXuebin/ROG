#include "gameholder_pch.h"
#include "vip_manager.h"
#include "player.h"
#include "vip_config.h"
#include "vip_reward_config.h"
#include "event_define.h"
#include "game_util.h"
#include "item_manager.h"
#include "vip_card_config.h"
#include "trade_manager.h"
#include "vip_gift_config.h"
#include "vip_statistics_manager.h"
#include "experience_vip.h"
#include "config_manager.h"


VIPManager::VIPManager( Player* player ) :
m_player(player)
{
    m_experienceVIP = CnNew ExperienceVIP(m_player);
}

VIPManager::~VIPManager()
{
    CnDelete m_experienceVIP;
}

void VIPManager::LoadInfo( const VIP_INFO& info )
{
    m_vip_info = info;
    m_experienceVIP->OnLoadVIPInfo(m_vip_info);
}

void VIPManager::FillInfo( VIP_INFO& info )
{
    InitVipGift();
    // VIP时效信息
    CheckVipTimeValid();
    m_experienceVIP->OnFillVIPInfo(m_vip_info);
    info = m_vip_info;
}

// 接收网络消息
void VIPManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VIP_REWARD_REQ:        // 领取VIP礼包
        {
            RecvVipRewardReq(data);
        }
        break;
    case MSG_VIP_BUY_REQ:
        {
            // tw版才会购买vip月卡
            if(ConfigManager::Instance()->IsTW())
                RecvVipCardBuyReq(data);
        }
        break;
        case MSG_VIP_REFRESH_REQ:
        {
            CheckVipTimeValid();
            // ack
            m_player->SendToGate(MSG_VIP_REFRESH_ACK, &m_vip_info);
        }
        break;
        //case MSG_VIP_INIT_REQ:
        //     SendVipInitAck();
        //    break;
    case MSG_VIP_INIT_REQ:
        SendVipInitAck();
        break;
    case MSG_VIP_GIFT_REQ:      // 领取VIP福利
        {
            RecVipGifReq(data);
        }
        break;
    case MSG_EXPERIENCE_VIP_RECEIVE_REQ:
        {
            OnExperienceVIPReceiveReq(data);
        }
        break;
    case MSG_EXPERIENCE_VIP_EXPIRE_REQ:
        {
            OnExperienceVIPExpireReq(data);
        }
        break;
    case MSG_EXPERIENCE_VIP_END_REQ:
        {
            OnExperienceVIPEndReq(data);
        }
        break;
    case MSG_EXPERIENCE_VIP_STATE_REQ:
        {
            OnexperienceVIPStateReq(data);
        }
        break;
    default:
        //CnAssert(false);
        break;
    }
}

void VIPManager::SendVipInitAck()
{
    m_vip_info.recharge_count = m_player->GetRechargeCount();

    // 填充体验VIP信息
    m_experienceVIP->FillExperienceVIPInfo(m_vip_info);

    m_player->SendToGate(MSG_VIP_INIT_ACK, &m_vip_info);
}

bool VIPManager::IsExperienceVIP()
{
    return m_experienceVIP->IsExperienceVIP(m_vip_info);
}

void VIPManager::ActiveExperienceVIP()
{
    m_experienceVIP->ActiveExperienceVIP(m_vip_info);
}

void VIPManager::SetVipGifFlag(uint32 level, int32 flag)
{
    if(flag == VIP_GIFT_GET_REWARD)
        m_vip_info.vip_gif_flag |= (1<< level); //设置某一位为1
    else 
        m_vip_info.vip_gif_flag &= (~(1<<level));//设置某一位为0
}

void VIPManager::InitVipGift()
{
    if(m_vip_info.vip_gif_flag == 0)
    {
        for(uint32 i=0; i<= VIP_GIFT_FlAG_LEN; ++i)
        {
            //将expr的第n（n从0开始）位设置为0:    expr &= (~(1<<n));
            m_vip_info.vip_gif_flag  &= (~(1<<i));
        }
    }
}

bool VIPManager::GetVipGitFlag(uint32 level)
{
    //判断expr的第n（n从0开始）位是否为1：bool b = expr &(1<<n);
    bool num =  m_vip_info.vip_gif_flag &(1 << level);
    return num;
}

int32 VIPManager::GetVipLevelByGrowthPoint()
{
    int32 vipLevel = VipConfig::Instance()->GetVipLevel(GetGrowthPointAll());
    if (vipLevel < 0)
    {
        vipLevel = 0;
        CnAssert(false);
        CnWarn("VIP level cannot be negative! \n");
    }

    return vipLevel;
}

// 充值发生改变，成长值发生改变
void VIPManager::CalGrowPoint()
{
    //if(!m_player)
    //    return;

    //// tw版本充值不再计算vip成长值
    //if(ConfigManager::Instance()->IsTW())
    //    return;

    //int32 howmoney = m_player->GetRechargeCount();

    //int32 moneypoint = GameParamConfig::Instance()->GetGrowthPointNeedConsumeMoneyPoint();
    //if(moneypoint <= 0)
    //    CnAssert(false);

    //int32 growpoint = howmoney/moneypoint;
    //if(growpoint < 0) 
    //    growpoint = 0;

    //int32 oldVipLevel = GetVipLevel();

    //m_vip_info.growth_point_recharge = growpoint;

    //UpdateVip(oldVipLevel);
    //SendVipInitAck();
}
// ==================================================
// 设置VIP成长点
// ==================================================
void VIPManager::SetGrowthPointNoRecharge(int32 val)
{
    if(val > MAX_GROWTH_POINT_NUM)
        val = MAX_GROWTH_POINT_NUM;

    int32 oldVipLevel = GetVipLevel();

    m_vip_info.growth_point_no_recharge = val;

    UpdateVip(oldVipLevel);
}

void VIPManager::UpdateVip(int32 oldVipLevel)
{
    m_player->SetPropDirty();

    m_player->SendTaskEvent(TASK_EVENT_VIP_CHANGE, GetVipLevel(), 0);
    m_player->SendMyselfEvent(EVENT_CHANGE_VIP, 0, 0, 0);
    m_player->SendMyselfEvent(EVENT_ACIIVATION_PET, GetVipLevel(), ACTIVATION_VIP, 0);

    VipStatisticsManager::Instance()->VipLevelChange(m_player);

    int32 newVipLevel = GetVipLevel();
    if (oldVipLevel < newVipLevel)
    {
        SERVER_MARQUEE sendMsg;
        sendMsg.id = MARQUEE_VIP;
        sendMsg.parame_list.push_back(MarQueePlayerName(GetVipLevel(), m_player->GetName()));
        sendMsg.parame_list.push_back(MarQueeVIP(GetVipLevel()));
        m_player->OnMarQuee(sendMsg);
    }
}

// ==================================================
// 添加VIP成长点
// ==================================================
void VIPManager::AddGrowthPointNoRecharge(int32 val)
{
    int32 growth_point = GetGrowthPointNoRecharge() + val;

    if(growth_point < 0)
    {
        growth_point = 0;
    }
    else if(growth_point > MAX_GROWTH_POINT_NUM)
    {
        growth_point = MAX_GROWTH_POINT_NUM;
    }

    SetGrowthPointNoRecharge(growth_point);

    //CheckVipTimeValid();

    m_player->SendMyselfEvent(EVENT_CHANGE_VIP, 0, 0, 0);

    m_player->SetPropDirty();
}

// ==================================================
// 删除VIP成长点
// ==================================================
void VIPManager::SubGrowthPointNoRecharge(int32 val)
{
    if(val < 0)
    {
        CnAssert(false);
        return;
    }

    if(GetGrowthPointNoRecharge() >= 0 && GetGrowthPointNoRecharge() >= val)
    {
        m_vip_info.growth_point_no_recharge -= val;
    }
    else
    {
        SetGrowthPointNoRecharge(0);
        //CnError("SubGrowthPoint overflow");
    }

    m_player->SetPropDirty();
}

// ==================================================
// 返回VIP有效的等级：如果VIP时效失效，返回VIP_0
// ==================================================
int32 VIPManager::GetVipLevel()
{
    int32 vipLevel = 0; 

    //CheckVipTimeValid();

    vipLevel = GetVipLevelByGrowthPoint();

    if (vipLevel < 0)
    {
        vipLevel = 0;
        CnAssert(false);
        CnWarn("VIP level cannot be negative! \n");
    }

    return vipLevel;
}

// 获取VIP奖励
void VIPManager::RecvVipRewardReq(void* data)
{
    int32 myVipLevel = GetVipLevel();

    if (myVipLevel > VIP_0)
    {
        if (m_vip_info.is_vip_reward_today == 0)
        {
            m_vip_info.is_vip_reward_today = 1;
            // 领取VIP礼包奖励
            const VipReward* rewardItems = VipRewardConfig::Instance()->GetVipReward(myVipLevel);
            if (rewardItems)
            {
                std::vector<REWARD_ITEM>    items;
                m_player->GetFcmRewardList(rewardItems->items, items);
                if (items.size() > 0)
                {
                    REWARD_TABLE rewardTable;

                    for (std::vector<REWARD_ITEM>::const_iterator gainIt = items.begin(); gainIt != items.end(); ++gainIt)
                    {
                        rewardTable.items.push_back(*gainIt);
                    }
                    //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);

                    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
                    m_player->SendPlaySystemLog(PLAYSYSTEM_VIP_GIFT_REWARD, m_player->GetLevel(), now);
                }
            }
        }
    }

    // 填充体验VIP信息
    m_experienceVIP->FillExperienceVIPInfo(m_vip_info);

    // ack
    m_player->SendToGate(MSG_VIP_REWARD_ACK, &m_vip_info);
}

// ==================================================
// 购买VIP月卡卡请求
// ==================================================
void VIPManager::RecvVipCardBuyReq(void* data)
{
    VIP_BUY_REQ* recvMsg = (VIP_BUY_REQ*)data;
    int32 vip_card_id = recvMsg->vip_card;
    int32 num = recvMsg->num;
    const std::string& offerName = recvMsg->offer_name;

    // 计算价格
    int32 needMoneyPoint = VipCardConfig::Instance()->GetVipCardMoneyPoint(vip_card_id) * num;

    // trade
    if (m_player->GetMoneyPoint() >= needMoneyPoint)
    {
        Json::FastWriter writer;
        Json::Value json_info;
        json_info["vip_card_id"] = vip_card_id;
        json_info["num"] = num;
        json_info["needMoneyPoint"] = needMoneyPoint;
        std::string jsonParam;
        jsonParam = writer.write(json_info);

        if (offerName == "")    // 买给自己
        {
            TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_VIP_BUY, PROP_VIP, vip_card_id, num, needMoneyPoint, jsonParam);
        }
        else // 买来送给别人
        {
            //TradeManager::Instance()->SendTradeOrderToReq(m_player->GetPlayerID(), m_player->GetAccount(), m_player->GetName(), offerName, REWARD_REASON_VIP_BUY, PROP_VIP_CARD_OFFER, vip_card_id, num, needMoneyPoint, needMoneyPoint);
        }
    }
}

//接受领取vip 的福利信息
void VIPManager::RecVipGifReq(void* data)
{
    VIP_GIFT_REQ* recvMsg = (VIP_GIFT_REQ*)data;
    if(!recvMsg)
        return;

    int32 numlist = VipGiftConfig::Instance()->GetRewardItemList(recvMsg->level);
    if(numlist <= 0)
    {
        VIP_GIFT_ACK  sendGifAck;
        sendGifAck.level = recvMsg->level;
        sendGifAck.errcode = ERR_FAILED;
        m_player->SendToGate(MSG_VIP_GIFT_ACK, &sendGifAck);
        return;
    }

    if(recvMsg->level > (uint32)GetVipLevel())
    {
        VIP_GIFT_ACK  sendGifAck;
        sendGifAck.errcode = ERR_FAILED;
        sendGifAck.level = recvMsg->level;
        m_player->SendToGate(MSG_VIP_GIFT_ACK, &sendGifAck);
        return;
    }

    bool b_addGif = false;
    uint32 pos = recvMsg->level;
    if(!GetVipGitFlag(pos))
    {
        b_addGif = VipGiftConfig::Instance()->AddVipGift(m_player, recvMsg->level);
        if(b_addGif)
        {
            SetVipGifFlag(pos, VIP_GIFT_GET_REWARD);
        }
    }

    VIP_GIFT_ACK  sendGifAck;
    sendGifAck.errcode = ERR_FAILED;
    sendGifAck.level = recvMsg->level;
    if(b_addGif)
    {
        sendGifAck.errcode = ERR_SUCCEED;
        m_player->SendMyselfEvent(EVENT_GET_VIP_REWARD_TITLE, recvMsg->level, 0, 0);
    }

    m_player->SendToGate(MSG_VIP_GIFT_ACK, &sendGifAck);
}

// vip月卡到期检查
bool VIPManager::CheckVipTimeValid()
{
    bool bValid = true;
    // 台湾版才有月卡到期情况
    if(!ConfigManager::Instance()->IsTW())
        return bValid;

    time_t curTime = Crown::SDNow().GetTimeValue();

    if (m_vip_info.vip_ts < (uint32)curTime) // vip过期了
    {
        if(m_vip_info.vip_ts)
        {
            m_player->SendMyselfEvent(EVENT_VIP_OUT_OF_DATE, 0, 0, 0);

        }

        bValid = false;
        m_vip_info.vip_ts = 0;
        m_vip_info.is_vip = 0;

        SetGrowthPointNoRecharge(0);
    }
    else
    {
        m_vip_info.is_vip = 1;
    }

    return bValid;
}

// vip月卡购买调用
void VIPManager::CBVipOffer(int32 cardId, uint32 num)
{
    // 更新vip时效数据
    time_t curTime = Crown::SDNow().GetTimeValue();

    if (m_vip_info.vip_first_ts == 0) // first time
    {
        m_vip_info.vip_first_ts = (uint32)curTime;
    }

    // 更新时间戳
    time_t validts = curTime;
    if (m_vip_info.vip_ts > (uint32)curTime)
    {
        validts = m_vip_info.vip_ts;
    }

    uint32 inc = (uint32)validts;
    uint32 durationDay = VipCardConfig::Instance()->GetVipCardDuration(cardId) * num;     // 增加的时长月
    validts = Crown::CSDDateTime(validts).IncDay(durationDay).GetTimeValue();   // 有效期时间戳
    inc = (uint32)validts - inc;                                                    // 增加的有效时长值

    if (cardId == VIP_CARD_1)
    {
        m_vip_info.vip_card_1_num += num;
    }
    else if (cardId == VIP_CARD_2)
    {
        m_vip_info.vip_card_2_num += num;
    }
    else if (cardId == VIP_CARD_3)
    {
        m_vip_info.vip_card_3_num += num;
    }

    // 添加VIP时间
    AddVipTime(cardId, inc);

    // 购买VIP时效卡后VIP直通
    int32 directVIP = VipCardConfig::Instance()->GetVipCardDirectVIP(cardId);
    if (GetVipLevelByGrowthPoint() < directVIP)
    {
        int32 growthPointOffer = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_999, directVIP);
        AddGrowthPointNoRecharge(growthPointOffer);
    }
}

void VIPManager::AddVipTime(uint32 cardId, uint32 vipTime)
{
    if (vipTime <= 0)
        return;

    uint32 curTime = (uint32)Crown::SDNow().GetTimeValue();

    // 过期
    if(m_vip_info.vip_ts <= curTime)
    {
        m_vip_info.vip_ts = 0;
    }

    // 加上VIP时间
    if (m_vip_info.vip_ts == 0)
    {
        m_vip_info.vip_ts = curTime + vipTime;
    }
    else
    {
        m_vip_info.vip_ts += vipTime;
    }

    m_vip_info.is_vip = 1;

    // 填充体验VIP信息
    m_experienceVIP->FillExperienceVIPInfo(m_vip_info);

    // ack
    m_player->SendToGate(MSG_VIP_REFRESH_ACK, &m_vip_info); // vip时效信息刷新

    m_player->SendToGate(MSG_VIP_BUY_ACK, &m_vip_info);   //购买vip的消息返回
}

void VIPManager::CalDailyRefresh(int32 days)
{
    m_vip_info.is_vip_reward_today = 0;
}

void VIPManager::OnExperienceVIPReceiveReq(void *data)
{
    m_experienceVIP->OnReceiveReq(m_vip_info);
}

void VIPManager::OnExperienceVIPExpireReq(void *data)
{
    m_experienceVIP->OnExpireReq(m_vip_info);
}

void VIPManager::OnExperienceVIPEndReq(void *data)
{
    m_experienceVIP->OnEndReq(m_vip_info);
}

void VIPManager::OnexperienceVIPStateReq(void *data)
{
    m_experienceVIP->OnStateReq(m_vip_info);
}

