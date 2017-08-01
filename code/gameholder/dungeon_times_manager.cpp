#include "gameholder_pch.h"
#include "dungeon_times_manager.h"
#include "player.h"
#include "activity_manager.h"
#include "gameplaytimes_notifier.h"

DungeonTimesRecorder::DungeonTimesRecorder(Player* player) :
m_player(player)
{

}

DungeonTimesRecorder::~DungeonTimesRecorder()
{

}

void DungeonTimesRecorder::LoadDungeonTimesInfo(const DUNGEON_TIMES_LIST& dungeonInfo)
{
    m_dungeonTimesList = dungeonInfo;

    return ;
    // 现在只发送世界尽头信息
    //const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(JUMPING_OFF_MAP);
    //if(!activityRegular)
    //{
    //    //CnWarn("dungeon_regular dungeon id:%d is not exist!\n", JUMPING_OFF_MAP);
    //    return;
    //}
    //DUNGEON_TIMES_INFO* dungeonTimesInfo = GetDungeonTimesInfo(JUMPING_OFF_MAP);
    //if(!dungeonTimesInfo)
    //{
    //    SendTimes(JUMPING_OFF_MAP, 0, activityRegular->count);
    //    return;            // 列表中没有可以进入
    //}

    //SendTimes(JUMPING_OFF_MAP, dungeonTimesInfo->enter_count, activityRegular->count);
}

void DungeonTimesRecorder::FillDungeonTimesInfo(DUNGEON_TIMES_LIST& dungeonInfo)
{
    dungeonInfo = m_dungeonTimesList;
}

void DungeonTimesRecorder::CalDailyRefresh()
{
    SaveYesterData();

    // 刷新进入次数
    m_dungeonTimesList.list.clear();

    // 现在只发送世界尽头信息
    //const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(JUMPING_OFF_MAP);
    //if(!activityRegular)
    //{
    //    //CnWarn("dungeon_regular dungeon id:%d is not exist!\n", JUMPING_OFF_MAP);
    //    return;
    //}
    //SendTimes(JUMPING_OFF_MAP, 0, activityRegular->count);
}

bool DungeonTimesRecorder::HasEnterCount(uint32 dungeonID)
{
    Crown::CSDDateTime nowTime = Crown::SDNow();

    if(!IsRecordTimes(dungeonID))
        return true;

    const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(dungeonID);
    if(!activityRegular)
    {
        CnWarn("dungeon_regular dungeon id:%d is not exist!\n", dungeonID);
        return false;
    }

    DUNGEON_TIMES_INFO* dungeonTimesInfo = GetDungeonTimesInfo(dungeonID);
    if(!dungeonTimesInfo)
    {
        SendTimes(dungeonID, 0, activityRegular->count);
        return true;            // 列表中没有可以进入
    }

    SendTimes(dungeonID, dungeonTimesInfo->enter_count, activityRegular->count);

    if(dungeonTimesInfo->enter_count >= activityRegular->count)
    {
        return false;
    }

    return true;
}

void DungeonTimesRecorder::AddEnterCount(uint32 dungeonID)
{
    // 过滤要记录的
    if(!IsRecordTimes(dungeonID))
        return;

    DUNGEON_TIMES_INFO* dungeonTimesInfo = GetDungeonTimesInfo(dungeonID);
    if(!dungeonTimesInfo)
    {
        DUNGEON_TIMES_INFO dungeon;
        dungeon.dungeon_id = dungeonID;
        m_dungeonTimesList.list.push_back(dungeon);
        dungeonTimesInfo = GetDungeonTimesInfo(dungeonID);
    }

    dungeonTimesInfo->enter_count++;

    // 发送副本剩余次数
    const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(dungeonID);
    if(!activityRegular)
    {
        CnWarn("dungeon_regular dungeon id:%d is not exist!\n", dungeonID);
        return;
    }
    SendTimes(dungeonID, dungeonTimesInfo->enter_count, activityRegular->count);

    //Crown::CSDDateTime tomorrow = Crown::SDNow();
    //tomorrow.IncDay(1);
    //tomorrow.SetTime(0,0,0);
    //dungeonTimesInfo->refresh_time = (uint32)tomorrow.GetTimeValue();
}

DUNGEON_TIMES_INFO* DungeonTimesRecorder::GetDungeonTimesInfo(uint32 dungeonId)
{
    Crown::CSDDateTime nowTime = Crown::SDNow();

    std::vector<DUNGEON_TIMES_INFO>::iterator dungeonIt = m_dungeonTimesList.list.begin();
    for(; dungeonIt != m_dungeonTimesList.list.end(); ++dungeonIt)
    {
        if(dungeonIt->dungeon_id == dungeonId)
        {
            break;
        }
    }
    if(dungeonIt == m_dungeonTimesList.list.end())
    {
        return NULL;
    }

    return &(*dungeonIt);
}

bool DungeonTimesRecorder::IsRecordTimes(int32 dungeonID)
{
    //switch(dungeonID)
    //{
    ////case TD_MAP:
    ////case CAPTURE_FLAG_MAP:
    ////case FACTION_PK_MAP:
    ////case JUMPING_OFF_MAP:
    ////case WORLD_BOSS_MAP:
    //    break;
    //default:
    //    return false;
    //}

    return true;
}

void DungeonTimesRecorder::SaveYesterData()
{
    //SaveOneYesterdayData(OPEN_SYS_WORLD_BOSS);
    //SaveOneYesterdayData(OPEN_SYS_FACTION_PK);
    //SaveOneYesterdayData(OPEN_SYS_CAPTURE_FLAG);
    //SaveOneYesterdayData(OPEN_SYS_TD);
}

void DungeonTimesRecorder::SaveOneYesterdayData(uint32 opensysID)
{
    //// 是否保存最大次数
    //bool isSaveMaxCount = m_player->GetGainbackResourceManager()->IsSaveMaxCount();
    //int32 count = 0;

    //uint32 dungeonID = GetDungeonIDByOpenSysID(opensysID);
    //if(dungeonID == 0)
    //    return;

    //const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(dungeonID);
    //if(!activityRegular)
    //{
    //    CnWarn("dungeon_regular dungeon id:%d is not exist!\n", dungeonID);
    //    return;
    //}

    //DUNGEON_TIMES_INFO* timesInfo = GetDungeonTimesInfo(dungeonID);
    //if(!timesInfo)
    //{
    //    count = activityRegular->count; // 没有找到进入次数为0 剩余次数最大
    //}
    //else
    //{
    //    if(isSaveMaxCount)
    //        count = activityRegular->count;
    //    else
    //        count = activityRegular->count - timesInfo->enter_count;
    //}

    //if(count < 0)
    //{
    //    count = 0;
    //    CnError("dungeon id:%d is enter count < 0");
    //}

    //m_player->SaveYesterdayCount(opensysID, count);
}

uint32 DungeonTimesRecorder::GetOpenSysIDByDungeonID(uint32 dungeonID)
{
    //switch(dungeonID)
    //{
    //case WORLD_BOSS_MAP:
    //    return OPEN_SYS_WORLD_BOSS;
    //case FACTION_PK_MAP:
    //    return OPEN_SYS_FACTION_PK;
    //case CAPTURE_FLAG_MAP:
    //    return OPEN_SYS_CAPTURE_FLAG;
    //case TD_MAP:
    //    return OPEN_SYS_TD;
    //default:
    //    return 0;
    //}

    return 0;
}

uint32 DungeonTimesRecorder::GetDungeonIDByOpenSysID(uint32 OpensysID)
{
    //switch(OpensysID)
    //{
    //case OPEN_SYS_WORLD_BOSS:
    //    return WORLD_BOSS_MAP;
    //case OPEN_SYS_FACTION_PK:
    //    return FACTION_PK_MAP;
    //case OPEN_SYS_CAPTURE_FLAG:
    //    return CAPTURE_FLAG_MAP;
    //case OPEN_SYS_TD:
    //    return TD_MAP;
    //default:
    //    return 0;
    //}

    return 0;
}

void DungeonTimesRecorder::SendTimes(uint32 dungeonID, int32 enterTimes, int32 maxTimes)
{
    int32 curTimes = maxTimes - enterTimes;
    if(curTimes < 0)
    {
        curTimes = 0;
        CnFatal("dungeonTimes < 0");
    }

    //switch(dungeonID)
    //{
    //case JUMPING_OFF_MAP:
    //    {
    //        m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PRINCESS, curTimes, maxTimes);
    //    }
    //    break;
    //}
}

bool DungeonTimesRecorder::ReduceEnterCount( uint32 dungeonID, int32 num)
{
    // 过滤要记录的
    if(!IsRecordTimes(dungeonID))
        return false;

    DUNGEON_TIMES_INFO* dungeonTimesInfo = GetDungeonTimesInfo(dungeonID);
    if(!dungeonTimesInfo)
        return false;

    if(dungeonTimesInfo->enter_count - num < 0)
    {
        return false;
    }

    dungeonTimesInfo->enter_count -= num;

    // 发送副本剩余次数
    //const ActivityRegular* activityRegular = ActivityConfig::Instance()->GetActivityrRegularByDungeonId(dungeonID);
    //if(!activityRegular)
    //{
    //    CnWarn("dungeon_regular dungeon id:%d is not exist!\n", dungeonID);
    //    return false;
    //}
    //SendTimes(dungeonID, dungeonTimesInfo->enter_count, activityRegular->count);
    return true;
}
