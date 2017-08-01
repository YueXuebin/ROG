#include "gameholder_pch.h"
#include "task_define.h"

int32 ItemRandom(const ItemProList& qualityList )
{
    uint32 total = 0;
    for(ItemProList::const_iterator totalIt = qualityList.begin(); totalIt != qualityList.end(); ++totalIt)
    {
        total += totalIt->second.m_probability;
    }
    uint32 ran = Crown::SDRandom(total);

    uint32 ad = 0;
    for (ItemProList::const_iterator it = qualityList.begin(); it != qualityList.end(); ++it)
    {
        ad += it->second.m_probability;
        if (ad >= ran)
            return it->first;
    }
    return -1;
}

uint8 QueryTaskTypeById(uint32 taskID)
{
    if(taskID >= TASK_TYPE_MAIN*10000 && taskID <TASK_TYPE_BRANCH*10000)
        return TASK_TYPE_MAIN;
    else if(taskID >= TASK_TYPE_BRANCH*10000 && taskID <TASK_TYPE_DAILY*10000)
        return TASK_TYPE_BRANCH;
    else if(taskID >= TASK_TYPE_DAILY*10000 && taskID <TASK_TYPE_CRUSADE*10000)
        return TASK_TYPE_DAILY;
    else if(taskID >= TASK_TYPE_CRUSADE*10000 && taskID <TASK_TYPE_GUARD*10000)
        return TASK_TYPE_CRUSADE;
    else if(taskID >= TASK_TYPE_GUARD*10000 && taskID <TASK_TYPE_GUILD*10000)
        return TASK_TYPE_GUARD;
    else if(taskID >= TASK_TYPE_GUILD*10000 && taskID <TASK_TYPE_ACHIVE*10000)
        return TASK_TYPE_GUILD;
    else if(taskID >= TASK_TYPE_ACHIVE*10000 && taskID < TASK_FESTIVE_BASE_ID)
        return TASK_TYPE_ACHIVE;
    if(taskID >= TASK_FESTIVE_BASE_ID)
        return TASK_TYPE_FESTIVAL;
    else
        CnAssert(false);
    return 0;
}