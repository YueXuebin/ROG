#include "gameholder_pch.h"
#include "cell_visitor.h"
#include "player.h"
#include "aoi_object.h"


void CellVisitor::AddSeePlayer( Player* player )
{
#ifdef DEBUG
    // 添加的player，一定不存在
    for(std::list<Player*>::iterator iter = m_SeePlayers.begin(); iter != m_SeePlayers.end(); iter++)
    {
        if(*iter == player)
        {
            CnAssert(false);
            break;
        }
    }
#endif

    m_SeePlayers.push_back(player);
}

void CellVisitor::RemoveSeePlayer( Player* player )
{
#ifdef DEBUG
    // 要移除的player,一定存在
    CnAssert(HasSeePlayer(player));
#endif

    m_SeePlayers.remove(player);
}

bool CellVisitor::HasSeePlayer( Player* player )
{
    bool bFind = false;
    for(std::list<Player*>::iterator iter = m_SeePlayers.begin(); iter != m_SeePlayers.end(); iter++)
    {
        if(*iter == player)
        {
            bFind = true;
            break;
        }
    }
    return bFind;
}

void CellVisitor::AddAOIObject( AOIObject* obj )
{
#ifdef DEBUG
    // 添加的obj，一定不存在
    for(std::list<AOIObject*>::iterator iter = m_AOIObject.begin(); iter != m_AOIObject.end(); iter++)
    {
        if(*iter == obj)
        {
            CnAssert(false);
            break;
        }
    }
#endif

    m_AOIObject.push_back(obj);
}

void CellVisitor::RemoveAOIObject( AOIObject* obj )
{
#ifdef DEBUG
    // 要移除的obj,一定存在
    CnAssert(HasAOIObject(obj));
#endif

    m_AOIObject.remove(obj);
}

bool CellVisitor::HasAOIObject( AOIObject* obj )
{
    bool bFind = false;
    for(std::list<AOIObject*>::iterator iter = m_AOIObject.begin(); iter != m_AOIObject.end(); iter++)
    {
        if(*iter == obj)
        {
            bFind = true;
            break;
        }
    }
    return bFind;
}
