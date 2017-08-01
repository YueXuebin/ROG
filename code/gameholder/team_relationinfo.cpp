#include "gameholder_pch.h"
#include "team_relationinfo.h"
#include "player.h"
#include "player_team_manager.h"
#include "event_define.h"

TeamRelationInfo::TeamRelationInfo()
{

}

TeamRelationInfo::~TeamRelationInfo()
{

}

bool TeamRelationInfo::HasPlayer(Player* player)
{
    for(std::vector<Player*>::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        if(player == *iter)
            return true;
    }
    return false;
}

void TeamRelationInfo::AddPlayer(Player* player)
{
    // 设置各玩家的队友管理器
    CnAssert(player->GetTeamManager()->GetTeamMateNum() == 0);
    for(std::vector<Player*>::iterator pIter = m_players.begin(); pIter != m_players.end(); pIter++)
    {
        Player* other = *pIter;
        other->GetTeamManager()->OnAddTeamMember(player);
        player->GetTeamManager()->OnAddTeamMember(other);
    }

    //
    if(HasPlayer(player))
    {
        CnAssert(false);
        return;
    }
    m_players.push_back(player);

    player->SendMyselfEvent(LIVENESS_EVENT_TARGET_PLAYER_PK, 1, 0, 0); // 活跃度组队一次
}

void TeamRelationInfo::RemovePlayer(Player* player)
{
    // 设置各玩家的队友管理器
    for(std::vector<Player*>::iterator pIter = m_players.begin(); pIter != m_players.end(); pIter++)
    {
        Player* other = *pIter;
        if(other == player)
            continue;
        other->GetTeamManager()->OnRemoveTeamMember(player);
    }
    player->GetTeamManager()->OnRemoveAll();
    
    //
    if(!HasPlayer(player))
    {
        CnAssert(false);
        return;
    }
    
    for(std::vector<Player*>::iterator iter = m_players.begin(); iter != m_players.end(); iter++)
    {
        if(player == *iter)
        {
            m_players.erase(iter);
            break;
        }
    }
}
