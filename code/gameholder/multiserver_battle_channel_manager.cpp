#include "gameholder_pch.h"
#include "multiserver_battle_channel_manager.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "game_define.h"
#include "player.h"
#include "regular.h"
#include "regular_dungeon.h"
#include "scene.h"
#include "game_setting_manager.h"


MultiserverBattleChannelManager::MultiserverBattleChannelManager(DungeonSet* pDungeonSet):
ChannelManager(pDungeonSet)
{
}

MultiserverBattleChannelManager::~MultiserverBattleChannelManager(void)
{
}

void MultiserverBattleChannelManager::Update()
{

}

uint32 MultiserverBattleChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    if(!HasChannelID(1))
    {
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(1);
        CnAssert(pDungeon);
    }
    return UNION_ID(1, m_pDungeonSet->GetDungeonID(), 1);



    if(channelID != 0 && HasChannelID(channelID))
    {   // 进入玩家期望的频道
        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
    }

    //机器人用
    channelID = GetCreateChannelID(MULTISERVER_BATTLE_APPLY_ONE);
    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
    
    CnAssert(false);      // 频道ID已经创建 没找到是错误

    // 在所有频道中寻找>n人的人最少的频道
    uint32 smallPlayerChannelID = 1;
    uint32 smallPlayerNum = 9999999;
    for(ChannelMap::iterator iter = m_ChannelIDs.begin(); iter != m_ChannelIDs.end(); iter++)
    {
        uint32 channelID = iter->first;
        Dungeon* pDungeon = m_pDungeonSet->GetDungeon(channelID);
        CnAssert(pDungeon);
        uint32 playerNum = pDungeon->GetPlayerNum();

        if(playerNum < GameSettingManager::Instance()->GetCityRoomNum())
            return UNION_ID(pDungeon->GetChannelID(), pDungeon->GetDungeonID(), 1);

        if(smallPlayerNum > playerNum)
        {   
            smallPlayerNum = playerNum;
            smallPlayerChannelID = pDungeon->GetChannelID();
        }
    }

    // 所有频道人数都达到上限，则动态增加频道，而且频道不释放
    if (smallPlayerNum >= GameSettingManager::Instance()->GetCityRoomNum())
    {
        uint32 add_channel_id = findValidChannelID();
        Dungeon* add_dungeon  = m_pDungeonSet->CreateDungeon(add_channel_id);

        m_pDungeonSet->SetChannelNumber(m_pDungeonSet->GetChannelNumber()+1);
        NotifyAddChannel();

        smallPlayerNum = 0;
        smallPlayerChannelID = add_channel_id;
    }

    CnAssert(m_pDungeonSet->GetDungeon(smallPlayerChannelID));      // 频道必然存在

    return UNION_ID(smallPlayerChannelID, m_pDungeonSet->GetDungeonID(), 1);
}

void MultiserverBattleChannelManager::NotifyAddChannel()
{
    //CHANNEL_INFO_ACK info;
    //info.channel_num = m_pDungeonSet->GetChannelNumber();

    //std::vector<Player*> players = m_pDungeonSet->GetPlayerMap();
    //std::vector<Player*>::const_iterator playerIter = players.begin();
    //for (; playerIter != players.end(); ++playerIter)
    //{
    //    (*playerIter)->SendToGate(MSG_CHANNEL_INFO_ACK, &info);
    //}
}

uint32 MultiserverBattleChannelManager::GetCreateChannelID(uint32 battle_type)
{
    uint32 smallPlayerChannelID = 1;
    uint32 smallPlayerNum = 9999999;

    uint32 add_channel_id = findValidChannelID();
    Dungeon* add_dungeon  = m_pDungeonSet->CreateDungeon(add_channel_id);

    m_pDungeonSet->SetChannelNumber(m_pDungeonSet->GetChannelNumber()+1);
    NotifyAddChannel();

    smallPlayerNum = 0;
    smallPlayerChannelID = add_channel_id;

    Dungeon* pDungeon = m_pDungeonSet->GetDungeon(add_channel_id);

    std::map<uint32, Scene*>& scenes = pDungeon->GetScenes();
    for(std::map<uint32, Scene*>::iterator sceneIt = scenes.begin(); sceneIt != scenes.end(); ++sceneIt)
    {
        sceneIt->second->GetRegular()->SetUserData(&battle_type);
    }

    CnAssert(m_pDungeonSet->GetDungeon(smallPlayerChannelID));      // 频道必然存在

    return  add_channel_id;
}

uint32 MultiserverBattleChannelManager::JudgeDungeonStart( uint32 channelID )
{
    Dungeon* pDungeon = m_pDungeonSet->GetDungeon(channelID);
    if(pDungeon)
    {
        std::map<uint32, Scene*>& scenes = pDungeon->GetScenes();
        for(std::map<uint32, Scene*>::iterator sceneIt = scenes.begin(); sceneIt != scenes.end(); ++sceneIt)
        {
            if(!sceneIt->second->GetRegular()->IsGameStart())
            {
                // 玩家退出时发现副本没有开始 玩家只有一个时 副本可以直接结束
                if(sceneIt->second->GetRegular()->GetScene()->GetPlayers().size() < MULTISERVER_BATTLE_APPLY_ONE_NUM)
                {
                    Regular* pRegular = sceneIt->second->GetRegular();
                    if(pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
                    {
                        RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, pRegular);
                        pRegularDungeon->DungeonClear(true, NULL);      // 副本结束掉
                    }
                }
            }
        }
    }
    return 0;
}
