#include "gameholder_pch.h"
#include "rog_regular_event_combo_kill.h"
#include "scene.h"
#include "player.h"
#include "battle_recorder.h"
#include "regular_dungeon.h"

CnImplementRTTI(ROGRegularEvent_ComboKill, ROGRegularEvent);

ROGRegularEvent_ComboKill::ROGRegularEvent_ComboKill(RegularDungeon* pRegular) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_COMBO_KILL)
{

}

ROGRegularEvent_ComboKill::~ROGRegularEvent_ComboKill()
{

}

void ROGRegularEvent_ComboKill::OneSecondUpdate(int32 deltatime)
{
    ROGRegularEvent::OneSecondUpdate(deltatime);

    // 遍历所有未达成此条件的玩家
    Scene* pScene = GetScene();
    const Scene::PlayerMap& players = pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* player = playerIt->second;
        if(GetActiveForPlayer(player))
            continue;

        int32 comboKillMax = player->GetBattleRecorder()->GetComboKillMax();
        if(comboKillMax >= GetComboKill())
        {
            SetActiveForPlayer(player);
            GetRegular()->SetConditionChanged();
        }
    }
}

void ROGRegularEvent_ComboKill::SetActiveForPlayer(Player* player)
{
    m_ActivePlayerIDs.push_back(player->GetPlayerID());
}

bool ROGRegularEvent_ComboKill::GetActiveForPlayer(Player* player)
{
    uint64 playerID = player->GetPlayerID();
    for(uint32 i=0; i<m_ActivePlayerIDs.size(); i++)
    {
        if(m_ActivePlayerIDs[i] == playerID)
            return true;
    }
    return false;
}
