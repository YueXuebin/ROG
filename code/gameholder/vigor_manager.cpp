#include "gameholder_pch.h"
#include "vigor_manager.h"
#include "vigor_config.h"
#include "player.h"
#include "center_handler.h"
#include "scene.h"
#include "trade_manager.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"
#include "system_config_manager.h"

VigorManager::VigorManager(Player* player) :
m_player(player)
{
    Clear();
}

VigorManager::~VigorManager()
{

}

void VigorManager::Update(uint32 frame_count)
{
    int32 vigorMax = PLAYER_VIGOR_MAX;
    // ����ﵽ���ޣ�����Ҫ�ָ��������ʱ���
    if (m_player->GetVigorValue() >= vigorMax)
    {
        m_player->SetVigorReftime(0);
        return;
    }

    // ������������ʱ�����׼���ָ�
    if (m_player->GetVigorReftime() <= 0)
    {
        m_player->SetVigorReftime((uint32)Crown::SDNow().GetTimeValue());
        return;
    }

    // ����ʱ���������ָ�
    int64 diffTime = Crown::SDNow().GetTimeValue() - m_player->GetVigorReftime();

    int64 unitTime = 30 * 60;
    // ��λʱ��ָ�����, ����
    if (diffTime < unitTime)
    {
        return;
    }

    // ����ʱ����ڵĻָ�����
    int64 rate = diffTime / unitTime; 
    int32 addVigor = 0;
    addVigor += (uint32)rate * 1;

    // ����Ƿ񳬹��ָ�����
    if (m_player->GetVigorValue() + addVigor >= vigorMax)
    {
       addVigor = vigorMax - m_player->GetVigorValue();
    }

    // �ﵽ��λʱ�䣬���лָ��������ûָ�ʱ���
    int64 lastRefTime = Crown::SDNow().GetTimeValue()-(diffTime%unitTime);
    m_player->SetVigorReftime((uint32)lastRefTime);

    m_player->AddVigorValue(addVigor,0);
}

void VigorManager::Clear()
{
    m_meal_vigor_info.Clear();
}

void VigorManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
    {
    case MSG_GET_MEAL_VIGOR_REQ:
        RecvGetMealVigor(data);
        break;
    default:
        break;
    }
}

void VigorManager::LoadInfo(const VIGOR_INFO& info)
{
    m_meal_vigor_info = info;
}

void VigorManager::FillInfo(VIGOR_INFO& info)
{
    info = m_meal_vigor_info;
}

void VigorManager::RecvGetMealVigor(void* data)
{
    GET_MEAL_VIGOR_ACK sendMsg;

    // ��ҵȼ�δ�ﵽ
    if (m_player->GetLevel() < VigorConfig::Instance()->GetMealVigorStartLevel())
    {
        return;
    }

    if (GetMealVigorTemplate() !=  NULL)
    {
        m_player->AddBuyVigor(GetMealVigorTemplate()->add_vigor);
        m_meal_vigor_info.last_time = (uint32)Crown::SDNow().GetTimeValue();
        sendMsg.result = GET_MEAL_VIGOR_SUCCESS;
        sendMsg.last_time = m_meal_vigor_info.last_time;
    }
    else
    {
        bool is_meal_time = false;
        bool is_got = false;

        Crown::CSDDateTime today_time = Crown::SDNow();//�����賿ʱ���
        today_time.SetTime(0, 0, 0);
        Crown::CSDDateTime now_time = Crown::SDNow();
        Crown::CSDDateTime last_time(m_meal_vigor_info.last_time);

        uint64 now = now_time.GetTimeValue();
        uint64 last = last_time.GetTimeValue();

        const std::map<uint32, MealTimeTemplate>& meal_time = VigorConfig::Instance()->GetMealTime();
        for (std::map<uint32, MealTimeTemplate>::const_iterator it = meal_time.begin(); it != meal_time.end(); ++it)
        {
            uint64 start = Crown::TimeToUint(it->second.start_time) + today_time.GetTimeValue();
            uint64 end = Crown::TimeToUint(it->second.end_time) + today_time.GetTimeValue();

            //����Ƿ��ڷ���ʱ��
            if (now >= start && now <= end)
            {
                is_meal_time = true;
                //���ö�ʱ����û���������
                if (last >= start && last <= end)
                {
                    is_got = true;
                }
                break;
            }
        }

        if (!is_meal_time)
        {
            sendMsg.result = NOT_MEAL_TIME;
        }
        else if (is_got)
        {
            sendMsg.result = GOT_MEAL_VIGOR;
        }
    }

    m_player->SendToGate(MSG_GET_MEAL_VIGOR_ACK, &sendMsg);
}

const MealTimeTemplate* VigorManager::GetMealVigorTemplate()
{
    Crown::CSDDateTime today_time = Crown::SDNow();//�����賿ʱ���
    today_time.SetTime(0, 0, 0);

    Crown::CSDDateTime now_time = Crown::SDNow();
    Crown::CSDDateTime last_time(m_meal_vigor_info.last_time);

    uint64 now = now_time.GetTimeValue();
    uint64 last = last_time.GetTimeValue();

    const std::map<uint32, MealTimeTemplate>& meal_time = VigorConfig::Instance()->GetMealTime();
    for (std::map<uint32, MealTimeTemplate>::const_iterator it = meal_time.begin(); it != meal_time.end(); ++it)
    {
        uint64 start = Crown::TimeToUint(std::string(it->second.start_time)) + today_time.GetTimeValue();
        uint64 end = Crown::TimeToUint(std::string(it->second.end_time)) + today_time.GetTimeValue();

        //����Ƿ��ڷ���ʱ��
        if (now >= start && now <= end)
        {
            //���ö�ʱ����û���������
            if (last < start || last > end)
            {
                return &(it->second);
            }
        }
    }

    return NULL;
}

