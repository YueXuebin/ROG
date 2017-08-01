#include "gameholder_pch.h"
#include "experience_vip.h"
#include "player.h"
#include "game_setting_manager.h"
#include "vip_config.h"

ExperienceVIP::ExperienceVIP(Player *player) : m_player(player)
{
}

ExperienceVIP::~ExperienceVIP()
{
}

bool ExperienceVIP::IsExperienceVIP(VIP_INFO &info)
{
    return info.experience_vip_state == RECEIVING || info.experience_vip_state == RECEIVED;
}

void ExperienceVIP::FillExperienceVIPInfo(VIP_INFO &info)
{
    info.server_open_time = GameSettingManager::Instance()->GetServerOpenTime();
    info.experience_vip_time = 0;
    CalExperienceVIPState(info);
}

void ExperienceVIP::ActiveExperienceVIP(VIP_INFO &info)
{
    if (info.experience_vip_state < RECEIVING)
    {
        info.experience_vip_state = RECEIVING;
        SendStateAck(info);
    }
}

void ExperienceVIP::OnLoadVIPInfo(VIP_INFO &info)
{
    FillExperienceVIPInfo(info);
}

void ExperienceVIP::OnFillVIPInfo(VIP_INFO &info)
{
    FillExperienceVIPInfo(info);
}

void ExperienceVIP::OnReceiveReq(VIP_INFO &info)
{
    if (info.experience_vip_state < RECEIVED)
    {
        info.experience_vip_state = RECEIVED;
        SendStateAck(info);
    }
}

void ExperienceVIP::OnExpireReq(VIP_INFO &info)
{
    if (info.experience_vip_state < EXPIRED)
    {
        info.experience_vip_state = EXPIRED;
        SendStateAck(info);
    }
}

void ExperienceVIP::OnEndReq(VIP_INFO &info)
{
    if (info.experience_vip_state < END)
    {
        info.experience_vip_state = END;
        SendStateAck(info);
    }
}

void ExperienceVIP::OnStateReq(VIP_INFO &info)
{
    SendStateAck(info);
}

void ExperienceVIP::SendStateAck(VIP_INFO &info)
{
    FillExperienceVIPInfo(info);
    m_player->SendToGate(MSG_EXPERIENCE_VIP_STATE_ACK, &info);
}

void ExperienceVIP::CalExperienceVIPState(VIP_INFO &info)
{
    int32 state = info.experience_vip_state;

    if (state == END)
    {
        return;
    }

    int32 vipLevel = CalVIPLevel(); //忽略使用GM命令获得的VIP
    if (vipLevel > 0)
    {
        info.experience_vip_state = END;
        return;
    }

    if (state == EXPIRED)
    {
        return;
    }

    int32 playerLevel = m_player->GetLevel();
    //if (playerLevel >= GameParamConfig::Instance()->GetExperienceVIPLevel())
    //{
    //    if (info.experience_vip_state == UNCLAIMED)
    //    {
    //        info.experience_vip_state = END;
    //    }
    //    else
    //    {
    //        info.experience_vip_state = EXPIRED;
    //    }
    //    return;
    //}

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if (nowTime - info.server_open_time >= info.experience_vip_time)
    {
        if (info.experience_vip_state == UNCLAIMED)
        {
            info.experience_vip_state = END;
        }
        else
        {
            info.experience_vip_state = EXPIRED;
        }
        return;
    }
}

int32 ExperienceVIP::CalVIPLevel()
{
    //int32 howmoney = m_player->GetRechargeCount();
    //int32 moneypoint = GameParamConfig::Instance()->GetGrowthPointNeedConsumeMoneyPoint();
    //CnAssert(moneypoint > 0);

    //int32 growpoint = howmoney / moneypoint;
    //if (growpoint < 0)
    //{
    //    growpoint = 0;
    //}
    //else if (growpoint > MAX_GROWTH_POINT_NUM)
    //{
    //    growpoint = MAX_GROWTH_POINT_NUM;
    //}

    //int32 vipLevel = VipConfig::Instance()->GetVipLevel(growpoint);
    //if (vipLevel < 0)
    //{
    //    vipLevel = 0;
    //}

    //return vipLevel;
    return 0;
}