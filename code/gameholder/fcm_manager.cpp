#include "gameholder_pch.h"
#include "fcm_manager.h"
#include "game_util.h"
#include "player.h"


FCMManager::FCMManager( Player* player ) :
m_player(player)
{

}

FCMManager::~FCMManager()
{

}

void FCMManager::LoadInfo( const FCM_INFO& info )
{
    m_fcm_info = info;
    CalFcmGainRate();
}

void FCMManager::FillInfo( FCM_INFO& info )
{
    info = m_fcm_info;
}

void FCMManager::OnRecv( uint32 msgID, void* data )
{

}

void FCMManager::OnFcmInfoReq(void* data)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if(!m_fcm_info.fcm_flag)
    {
        goto _END;
    }

    if(now > m_fcm_info.ingame_time)
    {
        m_fcm_info.total_ingame_time += now - m_fcm_info.ingame_time;
        m_fcm_info.ingame_time = now;
    }

    CalFcmGainRate();

_END:
    m_player->SendToGate(MSG_FCM_INFO_ACK, &m_fcm_info);
}

void FCMManager::CalFcmGainRate()
{
    // 根据防沉迷状态设置奖励率
    if(!m_fcm_info.fcm_flag)
    {
        m_fcm_info.fcm_gain_rate = BASE_RATE_NUM;
    }
    else
    {
        // 如果防沉迷的下线超过 5 小时，则清空下线时间
        if(m_fcm_info.total_outgame_time >= FCM_OFFLINE_5_HOUR)
        {
            m_fcm_info.total_ingame_time = 0;
            m_fcm_info.total_outgame_time = 0;
            //CnDbg("FCM: FCM_OFFLINE_5_HOUR\n");
        }

        if(m_fcm_info.total_ingame_time >= FCM_ONLINE_5_HOUR)
        {
            m_fcm_info.fcm_gain_rate = FCM_ZERO_GAIN_RATE;
            //CnDbg("FCM: FCM_ZERO_GAIN_RATE\n");
        }
        else if(m_fcm_info.total_ingame_time >= FCM_ONLINE_3_HOUR)
        {
            m_fcm_info.fcm_gain_rate = FCM_HALF_GAIN_RATE;
            //CnDbg("FCM: FCM_HALF_GAIN_RATE\n");
        }
        else
        {
            if(m_fcm_info.fcm_gain_rate != BASE_RATE_NUM)
            {
                m_fcm_info.fcm_gain_rate = BASE_RATE_NUM;
                //CnDbg("FCM: BASE_RATE_NUM\n");
            }
        }
    }
}

int32 FCMManager::GetFcmGainValue(int32 val)
{
    int64 num = 0;

    if(!m_fcm_info.fcm_flag)
    {
        return val;
    }

    val = int32(val * ToPercent(m_fcm_info.fcm_gain_rate));
    return val;
}

// ==================================================
// 获得防沉迷后的奖励列表
// ==================================================
void FCMManager::GetFcmRewardList(const std::vector<REWARD_ITEM>& inList, 
                              std::vector<REWARD_ITEM>& outList)
{
    REWARD_ITEM rewardItem;
    int32 num = 0;

    num = (int32)inList.size();

    num = GetFcmGainValue(num);

    for(std::vector<REWARD_ITEM>::const_iterator it = inList.begin(); it != inList.end(); ++it)
    {
        rewardItem = *it;

        switch(rewardItem.id)
        {
        case PROP_GAMEPOINT:
        case PROP_SOULPOINT:
        case PROP_HONORPOINT:
        case PROP_EXP:
            {
                rewardItem.num = GetFcmGainValue(rewardItem.num);
            }
            break;
        case PROP_ITEM:
            {
                rewardItem.num = GetFcmGainValue(rewardItem.num);
                if(rewardItem.num < 1) rewardItem.num = 1;
            }
            break;
        default:
            break;
        }

        outList.push_back(rewardItem);
    }

}