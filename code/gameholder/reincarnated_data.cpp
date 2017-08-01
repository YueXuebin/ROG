#include "gameholder_pch.h"
#include "reincarnated_data.h"


void ReincarnatedList::Clear()
{
    for(int32 i=0; i<MAX_REINCARNATED_COUNT; i++)
    {
        list[i].Clear();
    }
    m_CurrentMax = 0;
}

void ReincarnatedList::Add(const ReincarnatedData& data)
{
    if(m_CurrentMax >= (MAX_REINCARNATED_COUNT-1))
    {
        CnAssert(false);
        return;
    }

    CnAssert(list[m_CurrentMax].IsClear());
    list[m_CurrentMax] = data;
    m_CurrentMax++;
}

ReincarnatedData ReincarnatedList::Pop()
{
    ReincarnatedData popData;
    if(m_CurrentMax==0)
        return popData;

    popData = list[0];
    CnAssert(!popData.IsClear());
    CnAssert(!list[m_CurrentMax-1].IsClear());
    CnAssert(list[m_CurrentMax].IsClear());

    for(int32 i=0; i<m_CurrentMax; i++)
    {
        if(i < (MAX_REINCARNATED_COUNT-1))
            list[i] = list[i+1];
    }
    
    list[m_CurrentMax].Clear();
    m_CurrentMax--;

    return popData;
}