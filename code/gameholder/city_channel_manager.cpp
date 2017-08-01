#include "gameholder_pch.h"
#include "city_channel_manager.h"
#include "dungeon.h"
#include "dungeon_set.h"
#include "game_define.h"
#include "game_util.h"
#include "player.h"
#include "game_setting_manager.h"
#include "dungeon_manager.h"


CityChannelManager::CityChannelManager(DungeonSet* pDungeonSet) :
ChannelManager(pDungeonSet)
{

}

CityChannelManager::~CityChannelManager()
{

}

void CityChannelManager::Update()
{

}

uint32 CityChannelManager::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID )
{
    if(sceneID == 0)
        sceneID = 1;

    if(channelID != 0 && HasChannelID(channelID))
    {   // 进入玩家期望的频道
        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), sceneID);
    }

    // 频道上限人数
    uint32 MaxPlayerNum = 10;
    DungeonInfo* pDungeonInfo = DungeonManager::Instance()->GetDungeonInfo(m_pDungeonSet->GetDungeonID());
    if(pDungeonInfo)
        MaxPlayerNum = pDungeonInfo->m_max_player;
    if(DungeonManager::Instance()->IsCity(m_pDungeonSet->GetDungeonID()))
        MaxPlayerNum = GameSettingManager::Instance()->GetCityRoomNum();

    // 在所有频道中寻找>n人的人最少的频道
    uint32 smallPlayerChannelID = 1;
    uint32 smallPlayerNum = 9999999;
    for(ChannelMap::iterator iter = m_ChannelIDs.begin(); iter != m_ChannelIDs.end(); iter++)
    {
        uint32 channelID = iter->first;
        Dungeon* pDungeon = m_pDungeonSet->GetDungeon(channelID);
        CnAssert(pDungeon);
        uint32 playerNum = pDungeon->GetPlayerNum();
        
        if(playerNum < MaxPlayerNum)
            return UNION_ID(pDungeon->GetChannelID(), pDungeon->GetDungeonID(), sceneID);

        if(smallPlayerNum > playerNum)
        {   
            smallPlayerNum = playerNum;
            smallPlayerChannelID = pDungeon->GetChannelID();
        }
    }

    // 所有频道人数都达到上限，则动态增加频道，而且频道不释放
    if (smallPlayerNum >= MaxPlayerNum)
    {
        uint32 add_channel_id = findValidChannelID();
        Dungeon* add_dungeon  = m_pDungeonSet->CreateDungeon(add_channel_id);

        m_pDungeonSet->SetChannelNumber(m_pDungeonSet->GetChannelNumber()+1);
        NotifyAddChannel();

        smallPlayerNum = 0;
        smallPlayerChannelID = add_channel_id;
    }
    
    CnAssert(m_pDungeonSet->GetDungeon(smallPlayerChannelID));      // 频道必然存在

    return UNION_ID(smallPlayerChannelID, m_pDungeonSet->GetDungeonID(), sceneID);
}

void CityChannelManager::NotifyAddChannel()
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
