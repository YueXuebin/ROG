#include "gameholder_pch.h"
#include "chaos_faction_channel_manager.h"
#include "dungeon_set.h"
#include "channel_manager.h"
#include "scene.h"
#include "dungeon.h"
#include "capture_flag_manager.h"
#include "regular_data.h"
#include "regular_manager.h"
#include "activity_manager.h"
#include "multiserver_battle_manager.h"
#include "player.h"
#include "chaos_faction_config.h"

ChaosFactionChannelManager::ChaosFactionChannelManager(DungeonSet* pDungeonSet) : 
ChannelManager(pDungeonSet)
{

}

ChaosFactionChannelManager::~ChaosFactionChannelManager()
{

}

void ChaosFactionChannelManager::Update()
{

}

uint32 ChaosFactionChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    uint32 playerChannelID = MultiserverBattleManager::Instance()->GetChannelIDByThreeApplyList(player->GetPlayerID());
    if(playerChannelID == 0)
    {
        //if(DUNGEON_ID(player->GetFieldMapID()) == CHAOS_MAP)
        //    player->SetFieldMapID(0);   // 不让他再重连
        return EnterInvalidChannel();
    }

    Dungeon* dungeon = m_pDungeonSet->GetDungeon(playerChannelID);
    if(!dungeon)
    {
        MultiserverBattleManager::Instance()->ThreeApplyListErase(player->GetPlayerID());
        //if(DUNGEON_ID(player->GetFieldMapID()) == CHAOS_MAP)
        //    player->SetFieldMapID(0);   // 不让他再重连
        return EnterInvalidChannel();
    }

    if(dungeon->IsClose() || dungeon->GetPlayerNum() >= ChaosFactionConfig::Instance()->GetPlayerMax())
    {
        MultiserverBattleManager::Instance()->ThreeApplyListErase(player->GetPlayerID());
        //if(DUNGEON_ID(player->GetFieldMapID()) == CHAOS_MAP)
        //    player->SetFieldMapID(0);   // 不让他再重连
        return EnterInvalidChannel();
    }

    uint32 union_id = UNION_ID(playerChannelID, m_pDungeonSet->GetDungeonID(), 1);

    //player->SetFieldMapID(union_id);        // 记录地图ID，便于断线重连

    return union_id;
}

uint32 ChaosFactionChannelManager::GetCreateChannelID()
{
    uint32 channelID = findValidChannelID();
    Dungeon* add_dungeon  = m_pDungeonSet->CreateDungeon(channelID);
    CnAssert(add_dungeon);

    return channelID;
}

