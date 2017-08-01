#include "commonlib_pch.h"
#include "special_activity_config.h"
#include "activity_reward_config.h"


IMPLEMENT_SINGLETON(SpecialActivityConfig)

SpecialActivityConfig::SpecialActivityConfig()
{
    Clear();
}

SpecialActivityConfig::~SpecialActivityConfig()
{

}

void SpecialActivityConfig::Clear()
{
    m_specialActivityMap.clear();
}

bool SpecialActivityConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load special_activity file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if (!xmlRoot) 
        return false;

    SpecialActivity specialActivity;
    SpecialActivityInfo activityInfo;
    REWARD_ITEM rewardItem;
    std::string timeString;
    Crown::CSDDateTime dateTime;

    //服务器配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "activity") == 0)
        {
            specialActivity.Clear();
            specialActivity.id = SDAtoi(xmlEle->Attribute("id"));
            specialActivity.param = SDAtoi(xmlEle->Attribute("param"));

            timeString = xmlEle->Attribute("start_time");
            if (!timeString.empty())
            {
                dateTime.FromString(timeString);
                specialActivity.start_time = (uint32)dateTime.GetTimeValue();
            }

            timeString = xmlEle->Attribute("end_time");
            if (!timeString.empty())
            {
                dateTime.FromString(timeString);
                specialActivity.end_time = (uint32)dateTime.GetTimeValue();
            }

            for(TiXmlElement* xmlRow = xmlEle->FirstChildElement(); xmlRow; xmlRow = xmlRow->NextSiblingElement())
            {
                activityInfo.Clear();
                activityInfo.param1 = SDAtoi(xmlRow->Attribute("param1"));
                activityInfo.param2 = SDAtoi(xmlRow->Attribute("param2"));
                activityInfo.rewardId = SDAtoi(xmlRow->Attribute("reward_id"));

                specialActivity.infoList.push_back(activityInfo);
            }
            
            m_specialActivityMap.insert(std::make_pair(specialActivity.id, specialActivity));
        }
        else
        {
            CnAssert(false);
        }
    }

    return true;
}

const SpecialActivity* SpecialActivityConfig::GetActivity( int32 activityId )
{
    SpecialActivityMap::const_iterator iter = m_specialActivityMap.find(activityId);
    if (iter != m_specialActivityMap.end())
    {
        return &(iter->second);
    }

    return nullptr;
}

const SpecialActivityInfo* SpecialActivityConfig::GetActivityInfoByIdAndParam1( int32 activityId, int32 param1 )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
        return nullptr;


    SpecialActivityInfoList::const_iterator iter = activity->infoList.begin();
    for (; iter != activity->infoList.end(); iter++)
    {
        if (iter->param1 == param1)
        {
            return &(*iter);
        }
    }

    return nullptr;
}

const SpecialActivityInfo* SpecialActivityConfig::GetActivityInfoByIdAndParam2( int32 activityId, int32 param2 )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
        return nullptr;


    SpecialActivityInfoList::const_iterator iter = activity->infoList.begin();
    for (; iter != activity->infoList.end(); iter++)
    {
        if (iter->param2 == param2)
        {
            return &(*iter);
        }
    }

    return nullptr;
}

int32 SpecialActivityConfig::GetRewardByActivityIdAndIndex( int32 activityId, uint32 index, REWARD_TABLE& rewardTable )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
    {
        return ERR_FAILED;
    }

    if (index >= activity->infoList.size())
    {
        return ERR_FAILED;
    }

    const SpecialActivityInfo* info = &(activity->infoList.at(index));
    if (!info)
    {
        return ERR_FAILED;
    }

    const REWARD_TABLE* rewards = ActivityRewardConfig::Instance()->GetReward(info->rewardId);
    if (!rewards)
    {
        return ERR_FAILED;
    }

    for (std::vector<REWARD_ITEM>::const_iterator it = rewards->items.begin(); it != rewards->items.end(); ++it)
    {
        rewardTable.items.push_back(*it);
    }
    rewardTable.reason = rewards->reason;
    rewardTable.reason_arg = rewards->reason_arg;

    return ERR_SUCCEED;
}

int32 SpecialActivityConfig::GetRewardByActivityId( int32 activityId, REWARD_TABLE_LIST& rewardTableList )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
    {
        return ERR_FAILED;
    }

    REWARD_TABLE rewardTable;
    SpecialActivityInfoList::const_iterator iter = activity->infoList.begin();
    for (; iter != activity->infoList.end(); iter++)
    {
        const REWARD_TABLE* rewards = ActivityRewardConfig::Instance()->GetReward(iter->rewardId);
        if (!rewards)
        {
            return ERR_FAILED;
        }

        rewardTable.Clear();
        for (std::vector<REWARD_ITEM>::const_iterator it = rewards->items.begin(); it != rewards->items.end(); ++it)
        {
            rewardTable.items.push_back(*it);
        }
        rewardTable.reason = rewards->reason;
        rewardTable.reason_arg = rewards->reason_arg;
        rewardTableList.reward_tables.push_back(rewardTable);

        rewardTableList.reason = rewards->reason;
        rewardTableList.reason_arg = rewards->reason_arg;
    }

    return ERR_SUCCEED;
}

int32 SpecialActivityConfig::GetInfoByActivityId( int32 activityId, SPECIAL_ACTIVITY_INFO_LIST& infoList )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
    {
        return ERR_FAILED;
    }

    SPECIAL_ACTIVITY_INFO info;
    SpecialActivityInfoList::const_iterator iter = activity->infoList.begin();
    for (; iter != activity->infoList.end(); iter++)
    {
        info.Clear();
        info.param1 = iter->param1;
        info.param2 = iter->param2;
        infoList.infos.push_back(info);
    }

    return ERR_SUCCEED;
}

int32 SpecialActivityConfig::GetActivityTime( int32 activityId, uint32& start_time, uint32& end_time )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
    {
        return ERR_FAILED;
    }

    start_time = activity->start_time;
    end_time = activity->end_time;
    return ERR_SUCCEED;
}

int32 SpecialActivityConfig::CheckActivityTime( int32 activityId )
{
    const SpecialActivity* activity = GetActivity(activityId);
    if (!activity)
    {
        return ERR_FAILED;
    }

    uint32 curTime = (uint32)Crown::SDNow().GetTimeValue();
    
    // 活动开启时间检查
    if ((activity->start_time > 0) && (curTime < activity->start_time))
    {
        return ERR_NO_REACH_TIME;
    }

    // 活动结束时间检查
    if ((activity->end_time > 0) && (curTime > activity->end_time))
    {
        return ERR_FAILED;
    }

    return ERR_SUCCEED;
}

void SpecialActivityConfig::UpdateStatus( SPECIAL_ACTIVITY_INFO_LIST& infoList, int32 param, int32 status )
{
    for (uint32 i = 0; i < infoList.infos.size(); i++)
    {
        // 参数不够,直接跳出,因为param2递增
        if (param < infoList.infos[i].param2)
        {
            break;
        }

        // 位标记0:未领奖  1:已领奖
        infoList.infos[i].status = NO_REWARD;
        if (status & (1 << i))
        {
            infoList.infos[i].status = ACCEPTED_REWARD;
            continue;
        }

        // 未领奖且参数符合
        infoList.infos[i].status = CAN_ACCEPT_REWARD;
    }
}