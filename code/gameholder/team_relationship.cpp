#include "gameholder_pch.h"
#include "team_relationship.h"
#include "team_manager.h"
#include "player_manager.h"
#include "team_relationinfo.h"
#include "notifier_utils.h"


IMPLEMENT_SINGLETON(TeamRelationShip)

TeamRelationShip::TeamRelationShip()
{

}

TeamRelationShip::~TeamRelationShip()
{
    for(TeamRelationShipMap::iterator tIter = m_RelationShip.begin(); tIter != m_RelationShip.end(); tIter++)
    {
        CnDelete tIter->second;
    }
    m_RelationShip.clear();
}

void TeamRelationShip::OnCreateTeam(const TEAM_INFO& team_info)
{
    uint64 teamID = U64ID(team_info.team_id_h, team_info.team_id_l);
    CnAssert(GetTeamRelationInfo(teamID) == NULL);
    m_RelationShip[teamID] = CnNew TeamRelationInfo;

    UpdateTeamMember(team_info);
}

void TeamRelationShip::OnDestoryTeam(const TEAM_INFO& team_info)
{
    uint64 teamID = U64ID(team_info.team_id_h, team_info.team_id_l);
    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(teamID);
    CnAssert(pRelationInfo);

    for(std::vector<Player*>::const_iterator pIter = pRelationInfo->GetPlayers().begin(); pIter != pRelationInfo->GetPlayers().end(); pIter++)
    {
        OnDestoryRelation(pRelationInfo, *pIter);       // 删除关系
    }

    TeamRelationShipMap::iterator tIter = m_RelationShip.find(teamID);
    if(tIter != m_RelationShip.end())
        m_RelationShip.erase(tIter);

    CnDelete pRelationInfo;
}

void TeamRelationShip::OnAddMember(const TEAM_INFO& team_info)
{
    UpdateTeamMember(team_info);
}

void TeamRelationShip::OnDelMember(const TEAM_INFO& team_info)
{
    UpdateTeamMember(team_info);
}

void TeamRelationShip::OnUpdateInfo(const TEAM_INFO& team_info)
{
    // 更新信息，不改变关系
}

void TeamRelationShip::OnDelPlayer(Player* player)
{
    UpdatePlayerTeamMember(player);
}

void TeamRelationShip::OnPlayerChangeDungeon(Player* player)
{
    UpdatePlayerTeamMember(player);
}

void TeamRelationShip::UpdatePlayerTeamMember(Player* player)
{
    uint64 teamID = player->GetTeamID();
    if(teamID == 0)
        return;

    TEAM_INFO* pTeamInfo = TeamManager::Instance()->GetTeamInfo(teamID);
    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(teamID);
    if(!pRelationInfo)
    {
        CnAssert(!pTeamInfo);
        return;
    }

    CnAssert(pTeamInfo);
    UpdateTeamMember(*pTeamInfo);
}

void TeamRelationShip::UpdateTeamMember(const TEAM_INFO& team_info)
{
    uint64 teamID = U64ID(team_info.team_id_h, team_info.team_id_l);
    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(teamID);
    if(!pRelationInfo)
    {
        CnAssert(false);
        return;
    }

    for(std::vector<TEAM_MEMBER>::const_iterator tIter = team_info.member_list.begin(); tIter != team_info.member_list.end(); tIter++)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(tIter->player_id_h, tIter->player_id_l));
        if(!player)
            continue;

        CnAssert(player->GetTeamID() == teamID);

        if(!pRelationInfo->HasPlayer(player))
        {
            pRelationInfo->AddPlayer(player);       // 添加关系
            OnCreateRelation(pRelationInfo, player);
        }
    }

    uint32 index = 0;
    while(index < pRelationInfo->GetPlayers().size())
    {
        Player* player = pRelationInfo->GetPlayers()[index];
        uint64 playerID = player->GetPlayerID();

        bool bExist = false;
        for(std::vector<TEAM_MEMBER>::const_iterator tIter2 = team_info.member_list.begin(); tIter2 != team_info.member_list.end(); tIter2++)
        {
            if(playerID == U64ID(tIter2->player_id_h, tIter2->player_id_l) 
                && PlayerManager::Instance()->GetPlayerByPlayerID(playerID) )
            {
                bExist = true;
                break;
            }
        }

        if(!bExist)
        {
            OnDestoryRelation(pRelationInfo, player);
            pRelationInfo->RemovePlayer(player);    // 移除关系
        }
        else
            index++;
    }

}

TeamRelationInfo* TeamRelationShip::GetTeamRelationInfo(uint64 teamID)
{
    TeamRelationShipMap::iterator tIter = m_RelationShip.find(teamID);
    if(tIter == m_RelationShip.end())
        return NULL;

    return tIter->second;
}

void TeamRelationShip::SendMessageToTeamMemberInSameScene(Player* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    if(!sender) return;

    uint64 teamID = sender->GetTeamID();
    if(!teamID) 
        return;

    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(teamID);
    if(!pRelationInfo)
    {
        CnAssert(false);
        return;
    }

    for(std::vector<Player*>::const_iterator pIter = pRelationInfo->GetPlayers().begin(); pIter != pRelationInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(exclude_sender && sender == teamMember)
            continue;

        if(sender->GetScene() != teamMember->GetScene())
            continue;

        CnAssert(teamMember->GetTeamID() == teamID);
        teamMember->SendToGate(msgId, msg);
    }
}

void TeamRelationShip::SendMessageToTeamMember(Player* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    if(!sender) return;

    uint64 teamID = sender->GetTeamID();
    if(!teamID) 
        return;

    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(teamID);
    if(!pRelationInfo)
    {
        CnAssert(false);
        return;
    }

    for(std::vector<Player*>::const_iterator pIter = pRelationInfo->GetPlayers().begin(); pIter != pRelationInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(exclude_sender && sender == teamMember)
            continue;

        CnAssert(teamMember->GetTeamID() == teamID);
        teamMember->SendToGate(msgId, msg);
    }
}

void TeamRelationShip::OnCreateRelation(TeamRelationInfo* pInfo, Player* player)
{
    BATTLEUNIT_APPEAR_LIST sendMsg1;
    uint32 count1 = 0;
    for(std::vector<Player*>::const_iterator pIter = pInfo->GetPlayers().begin(); pIter != pInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(teamMember == player)
            continue;

        if(teamMember->GetScene() != player->GetScene())
            continue;

        // 建立联系时,显示同场景中我看不见的队友.
        if(!player->IsPlayerCanSee(teamMember))
        {
            FILL_MSG_APPEAR_LIST(sendMsg1, teamMember, player);
            count1++;
        }

        // 建立联系时,将我显示给同场景中看不见我的玩家
        if(!teamMember->IsPlayerCanSee(player))
        {
            BATTLEUNIT_APPEAR_LIST sendMsg2;
            FILL_MSG_APPEAR_LIST(sendMsg2, player, teamMember);
            teamMember->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg2);
        }
    }
    if(count1>0)
        player->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);

}

void TeamRelationShip::OnDestoryRelation(TeamRelationInfo* pInfo, Player* player)
{
    for(std::vector<Player*>::const_iterator pIter = pInfo->GetPlayers().begin(); pIter != pInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(teamMember == player)
            continue;

        if(teamMember->GetScene() != player->GetScene())
            continue;

        // 断开联系时,隐藏同场景中我看不见的队友.
        if(!player->IsPlayerCanSee(teamMember))
        {
            BATTLEUNIT_DISAPPEAR sendMsg1;
            sendMsg1.object_id = teamMember->GetId();
            player->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
        }

        // 断开联系时,将我对同场景中看不见我的玩家隐藏
        if(!teamMember->IsPlayerCanSee(player))
        {
            BATTLEUNIT_DISAPPEAR sendMsg1;
            sendMsg1.object_id = player->GetId();
            teamMember->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
        }
    }
}
// 进入场景通知
void TeamRelationShip::SendAppearToTeamMemberInSameScene(SceneObject* obj)
{
    if(!obj || obj->GetObjectType() != OBJ_PLAYER)
        return;

    Player* player = (Player*)obj;
    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(player->GetTeamID());
    if(!pRelationInfo)
        return;

    // 显示给同场景中的队友
    BATTLEUNIT_APPEAR_LIST sendMsg1;
    uint32 count1 = 0;
    for(std::vector<Player*>::const_iterator pIter = pRelationInfo->GetPlayers().begin(); pIter != pRelationInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(teamMember == player)
            continue;

        if(teamMember->GetScene() != player->GetScene())
            continue;

        // 进入场景时,显示同场景中我的队友.
        FILL_MSG_APPEAR_LIST(sendMsg1, teamMember, player);
        count1++;

        // 进入场景时,将我显示给同场景中的队友
        BATTLEUNIT_APPEAR_LIST sendMsg2;
        FILL_MSG_APPEAR_LIST(sendMsg2, player, teamMember);
        teamMember->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg2);
    }
    if(count1>0)
        player->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);

}
// 离开场景通知
void TeamRelationShip::SendDisappearToTeamMemberInSameScene(SceneObject* obj)
{
    if(!obj || obj->GetObjectType() != OBJ_PLAYER)
        return;

    Player* player = (Player*)obj;
    TeamRelationInfo* pRelationInfo = GetTeamRelationInfo(player->GetTeamID());
    if(!pRelationInfo)
        return;

    // 显示给同场景中的队友
    BATTLEUNIT_APPEAR_LIST sendMsg1;
    uint32 count1 = 0;
    for(std::vector<Player*>::const_iterator pIter = pRelationInfo->GetPlayers().begin(); pIter != pRelationInfo->GetPlayers().end(); pIter++)
    {
        Player* teamMember = *pIter;
        if(teamMember == player)
            continue;

        if(teamMember->GetScene() != player->GetScene())
            continue;

        // 离开场景时,对我隐藏同场景中的队友
        BATTLEUNIT_DISAPPEAR sendMsg1;
        sendMsg1.object_id = teamMember->GetId();
        player->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);

        // 离开场景时,将我对同场景中的队友隐藏
        BATTLEUNIT_DISAPPEAR sendMsg2;
        sendMsg2.object_id = player->GetId();
        teamMember->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg2);
    }
}

bool TeamRelationShip::HasRelation(SceneObject* obj1, SceneObject* obj2)
{
    if(obj1 == obj2)
        return false;

    if(obj1->GetObjectType() == OBJ_PLAYER && obj2->GetObjectType() == OBJ_PLAYER)
        return IsSameTeam((Player*)obj1, (Player*)obj2);

    return false;
}

bool TeamRelationShip::IsSameTeam(Player* player1, Player* player2)
{
    if(!player1 || !player2)
        return false;

    if(player1 == player2)      // 排除自己
        return false;

    uint64 teamID1 = player1->GetTeamID();
    uint64 teamID2 = player2->GetTeamID();
    if(!teamID1 || !teamID2 || teamID1 != teamID2)
        return false;

    return true;
}
