#include "gameholder_pch.h"
#include "regular_world.h"
#include "sub_regular.h"
#include "player.h"


CnImplementRTTI(RegularWorld, RegularBuilder);

RegularWorld::RegularWorld( RegularDoc* pDoc ) :
RegularBuilder(pDoc)
{

}

RegularWorld::~RegularWorld()
{
    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        CnDelete pSubRegular;
    }
    m_SubRegulars.clear();
}

void RegularWorld::AddSubRegular( SubRegular* pSubRegular )
{
    CnAssert(!HasSubRegular(pSubRegular));
    m_SubRegulars.push_back(pSubRegular);
}

void RegularWorld::RemoveSubRegular( SubRegular* pSubRegular )
{
    CnAssert(HasSubRegular(pSubRegular));
    
    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        if(*iter == pSubRegular)
        {
            m_SubRegulars.erase(iter);
            break;
        }
    }
}

bool RegularWorld::HasSubRegular(SubRegular* pSubRegular)
{
    for(uint32 i=0; i<m_SubRegulars.size(); i++)
    {
        if(pSubRegular == m_SubRegulars[i])
            return true;
    }
    return false;
}

void RegularWorld::OneSecondUpdate( int32 deltatime )
{
    RegularBuilder::OneSecondUpdate(deltatime);

    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        pSubRegular->OneSecondUpdate(deltatime);
    }
}

void RegularWorld::Update( uint32 curr_tick )
{
    RegularBuilder::Update(curr_tick);

    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        pSubRegular->Update(curr_tick);
    }
}

void RegularWorld::OnCreatureEnter( BattleUnit* creature )
{
    RegularBuilder::OnCreatureEnter(creature);

    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        pSubRegular->OnCreatureEnter(creature);
    }

    if(OBJ_PLAYER == creature->GetObjectType())
    {
        Player* player = (Player*)creature;
        player->SetGameLock(false);             // 世界地图无锁
    }
}

void RegularWorld::OnCreatureLeave( BattleUnit* creatrue )
{
    RegularBuilder::OnCreatureLeave(creatrue);

    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        pSubRegular->OnCreatureLeave(creatrue);
    }
}

void RegularWorld::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    RegularBuilder::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    for(std::vector<SubRegular*>::iterator iter = m_SubRegulars.begin(); iter != m_SubRegulars.end(); iter++)
    {
        SubRegular* pSubRegular = *iter;
        pSubRegular->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }
}


