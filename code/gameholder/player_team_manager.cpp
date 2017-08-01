#include "gameholder_pch.h"
#include "player_team_manager.h"
#include "player.h"
#include "team_manager.h"
#include "player_manager.h"

PlayerTeamManager::PlayerTeamManager( Player* player ) :
m_player(player)
{

}

PlayerTeamManager::~PlayerTeamManager()
{

}

void PlayerTeamManager::OnAddTeamMember(Player* player)
{
    CnAssert(player != m_player);
    CnAssert(!IsTeamMate(player));
    m_TeamMates.push_back(player);
}

void PlayerTeamManager::OnRemoveTeamMember(Player* player)
{
    CnAssert(player != m_player);
    CnAssert(IsTeamMate(player));
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        if(player == *iter)
        {
            m_TeamMates.erase(iter);
            return;
        }
    }
}

void PlayerTeamManager::OnRemoveAll()
{
    while(m_TeamMates.size() > 0)
    {
        OnRemoveTeamMember( m_TeamMates[0] );
    }
}

bool PlayerTeamManager::IsTeamMate(Player* player)
{
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        if(player == *iter)
            return true;
    }
    return false;
}

void PlayerTeamManager::ShareExpToTeamInSameScene(int32 exp)
{
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        Player* other = *iter;
        if(other->GetUnionMapID() == m_player->GetUnionMapID())
            other->AddExp(exp);
    }
}

void PlayerTeamManager::ShareGamePointToTeamInSameScene(int32 game_point, int32 reason)
{
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        Player* other = *iter;
        if(other->GetUnionMapID() == m_player->GetUnionMapID())
            other->AddGamePoint(game_point, reason);
    }
}

void PlayerTeamManager::ShareSoulPointToTeamInSameScene(int32 soul_point, int32 reason)
{
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        Player* other = *iter;
        if(other->GetUnionMapID() == m_player->GetUnionMapID())
            other->AddSoulPoint(soul_point, reason);
    }
}

void PlayerTeamManager::SendTaskEventToTeamInSameScene(uint32 event_id, int32 arg1, int32 arg2, int32 arg3)
{
    for(std::vector<Player*>::iterator iter = m_TeamMates.begin(); iter != m_TeamMates.end(); iter++)
    {
        Player* other = *iter;
        if(other->GetUnionMapID() == m_player->GetUnionMapID())
        {
            other->SendTaskEvent(event_id, arg1, arg2, arg3);     // 通知任务
        }
    }
}


