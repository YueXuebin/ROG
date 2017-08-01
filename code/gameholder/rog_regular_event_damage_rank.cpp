#include "gameholder_pch.h"
#include "rog_regular_event_damage_rank.h"
#include "scene.h"
#include "player.h"
#include "battle_recorder.h"
#include "regular_dungeon.h"


CnImplementRTTI(ROGRegularEvent_DamageRank, ROGRegularEvent);

ROGRegularEvent_DamageRank::ROGRegularEvent_DamageRank(RegularDungeon* pRegular) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_DAMAGE_RANK)
{
    m_FirstPlayerID = 0;
}

ROGRegularEvent_DamageRank::~ROGRegularEvent_DamageRank()
{

}

void ROGRegularEvent_DamageRank::OneSecondUpdate(int32 deltatime)
{
    ROGRegularEvent::OneSecondUpdate(deltatime);

    m_DamageRank.clear();

    // 计算所有玩家的伤害排行(冒泡)
    Scene* pScene = GetScene();
    const Scene::PlayerMap& players = pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* player = playerIt->second;
        uint32 damage = player->GetBattleRecorder()->GetAllAttackDamage();

        PLAYER_DAMAGE thisOne;
        thisOne.damage = damage;
        thisOne.player_id = player->GetPlayerID();
        thisOne.player_name = player->GetName();

        uint32 i=0;
        while(i < m_DamageRank.size())
        {
            if(thisOne.damage > m_DamageRank[i].damage)
            {
                m_DamageRank.insert(m_DamageRank.begin()+i, thisOne);
                break;
            }
            i++;
        }
        if(i == m_DamageRank.size())
        {
            m_DamageRank.push_back(thisOne);
        }
    }

    uint64 firstPlayerID = 0;
    if(m_DamageRank.size() > 0)
    {
        firstPlayerID = m_DamageRank[0].player_id;
    }

    if(m_FirstPlayerID != firstPlayerID)
    {
        GetRegular()->SetConditionChanged();
        m_FirstPlayerID = firstPlayerID;
    }

    // 发送伤害排行
    DAMAGE_RANK_NTF ntf;
    for(uint32 i=0; i<m_DamageRank.size(); i++)
    {
        PLAYER_DAMAGE_INFO info;
        info.name = m_DamageRank[i].player_name;
        info.damage = m_DamageRank[i].damage;
        ntf.damage_list.push_back(info);
    }

    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* player = playerIt->second;
        player->SendToGate(MSG_DAMAGE_RANK_NTF, &ntf);
    }
}

bool ROGRegularEvent_DamageRank::GetActiveForPlayer(Player* player)
{
    return player->GetPlayerID() == m_FirstPlayerID;
}