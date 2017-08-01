#include "commonlib_pch.h"
#include "common_util.h"
#include <fstream>

std::string MakeCmdName( int argc, char* argv[] )
{
#ifdef WIN32
    char dir_split = '\\';
#else
    char dir_split = '/';
#endif

    std::string name;
    for(int32 i=0; i<argc; i++)
    {
        if(i == 0)
        {
            std::string process_name = Crown::SDGetCurrentProcessFullName();
            // 先去除最前面的：号
            int32 p = process_name.find_first_of(':');
            if(p >= 0)
                process_name = process_name.substr(p+1);

            // 再做名字修改
            std::string rep_process_name = "";
            std::vector<std::string> dirList = Crown::SDSplitStrings(process_name, dir_split);
            for(uint32 strI=0; strI < dirList.size(); strI++)
            {
                if(strI!=0)
                    rep_process_name += "_";
                rep_process_name += dirList[strI];
            }
            
            name += rep_process_name;
        }
        else
            name += argv[i];

        if(i != (argc-1))
            name += "_";
    }
    return name;
}

Crown::CSDNamedSemaphoreObject CheckNamedSemaphore(int argc, char* argv[])
{
    std::string SemName = MakeCmdName(argc, argv);
    CnDbg("sema name=%s\n", SemName.c_str());
    Crown::CSDNamedSemaphoreObject semObj(SemName);
    
    if(semObj.Open())
    {
        CnInfo("semaphore %s already exist!\n", SemName.c_str());
        semObj.Release();
        return semObj;
    }

    if(!semObj.Create())
    {
        CnError("create semaphore %s failed!\n", SemName.c_str());
        return semObj;
    }
    CnDbg("create semaphore %s\n", SemName.c_str());

    return semObj;
}

bool IsSameDay(uint32 recordlasttime)
{
    //时间戳转本地时间
    Crown::CSDDateTime lasttime;
    lasttime.SetTimeValue(recordlasttime);

    Crown::CSDDateTime now = Crown::SDNow();
    if(now.GetDay()!= lasttime.GetDay() || now.GetMonth() != lasttime.GetMonth()
        || now.GetYear()!= lasttime.GetYear())
    {
        return false;
    }
    return true;
}

uint32 CalFestivalTaskID(uint32 festivalID, uint32 taskID)
{
    return (festivalID * 100 + TASK_FESTIVE_BASE_ID) + taskID;
}

uint32 CalOriginFestivalID(uint32 taskID)
{
    return (taskID - TASK_FESTIVE_BASE_ID) / 100;
}
// 
uint32 CalOriginFestivalTaskID(uint32 taskID)
{
    uint32 id = 0;
    id = taskID % 100;
    return id;
}

uint32 CalRegionID(uint32 centerID, uint32 gateID)
{
    uint32 id = 0;
    id = (centerID << 16) | gateID;
    return id;
}

uint32 CalGateIDByRegionID(uint32 regionID)
{
    uint32 id = regionID & 0xffff;
    return id;
}

uint32 CalGameIDByRegionID(uint32 regionID)
{
    uint32 id = regionID & 0xffff;
    return id;
}

uint32 CalCenterIDByRegionID(uint32 regionID)
{
    uint32 id = (regionID >> 16) & 0xffff;
    return id;
}

int32 ServerNameToRegionID( const std::string& serverName )
{
    std::string region_str = serverName;
    if(region_str.size() > 0)
    {
        if(!isdigit(region_str[0]))
            region_str = region_str.substr(1);
        return SDAtoi(region_str.c_str());
    }
    else
    {
        CnAssert(false);
        return 0;
    }
}

std::string MakeUpServerName(const std::string& serverName )
{
    std::string retServerName = serverName;
    if(retServerName.size() > 0)
    {
        if(isdigit(retServerName[0]))
            retServerName = "S"+retServerName;
    }
    else
        CnAssert(false);
    return retServerName;
}

std::string RegionIDToServerName( int32 regionID )
{
    std::string server_name = "S"+regionID;
    return server_name;
}

uint32 GetMaxTeamMenber(uint32 teamType)
{
    if(teamType == TEAM_TYPE_DUNGEON)
        return MAX_DUNGEON_TEAM_MEMBER;
    else if(teamType == TEAM_TYPE_PK)
        return MAX_PK_TEAM_MEMBER;
    else if(teamType == TEAM_TYPE_WORLD)
        return MAX_WORLD_TEAM_MEMBER;
    CnAssert(false);
    return 0;
}

void MultiRewardTable(REWARD_TABLE* rewardTable, real multiple)
{
    MultiRewardItems(&rewardTable->items, multiple);
}

void MultiRewardItems(std::vector<REWARD_ITEM>* items, real multiple)
{
    for(std::vector<REWARD_ITEM>::iterator iter = items->begin(); iter != items->end(); iter++)
    {
        REWARD_ITEM& item = *iter;
        item.num = Crown::SDMax(int32(multiple * item.num), 1);
    }
}

bool IsSame64(const BIGINT_ID& a, const BIGINT_ID& b)
{
    return (a.id_h == b.id_h) && (a.id_l == b.id_l);
}