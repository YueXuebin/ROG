#include "gameholder_pch.h"
#include "world_boss_channel_manager.h"
#include "dungeon_set.h"
#include "dungeon_data.h"
#include "dungeon.h"
#include "regular_world_boss.h"
#include "regular_manager.h"
#include "activity_manager.h"
#include "game_setting_manager.h"


WorldBossChannelManager::WorldBossChannelManager( DungeonSet* pDungeonSet ) :
ChannelManager(pDungeonSet)
{
    m_WorldBossState = WORLD_BOSS_CLOSE;
    m_StateTime = 0;
    m_CreatureID = 0;
}

WorldBossChannelManager::~WorldBossChannelManager()
{

}

uint32 WorldBossChannelManager::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID )
{
    if(m_CurrentChannels.size() == 0)
    {   // ����BOSSδ���������⿪��һ��Ƶ���������
        return EnterInvalidChannel();
    }

    // ����BOSS��Ƶ���������
    uint32 smallPlayerChannelID = 1;
    uint32 smallPlayerNum = 9999999;

    for(uint32 ci=0; ci<m_CurrentChannels.size(); ci++)
    {
        uint32 channelID = m_CurrentChannels[ci];
        Dungeon* pDungeon = m_pDungeonSet->GetDungeon(channelID);
        CnAssert(pDungeon);
        uint32 playerNum = pDungeon->GetPlayerNum();

        if((int32)playerNum < GameSettingManager::Instance()->GetWorldBossMaxPlayer())
            return UNION_ID(pDungeon->GetChannelID(), pDungeon->GetDungeonID(), 1);

        if(smallPlayerNum > playerNum)
        {
            smallPlayerNum = playerNum;
            smallPlayerChannelID = pDungeon->GetChannelID();
        }
    }

    return UNION_ID(smallPlayerChannelID, m_pDungeonSet->GetDungeonID(), 1);
}

void WorldBossChannelManager::Update()
{
    if(m_WorldBossState == WORLD_BOSS_CLOSE)
        return;

    m_StateTime += g_ServerUpdateTime;

    if(m_WorldBossState == WORLD_BOSS_OPEN)
    {
        // ��ʼ��10�룬����Boss����
        if(m_StateTime >= 20000)
        {
            m_WorldBossState = WORLD_BOSS_START;
            m_StateTime = 0;
            CreateWorldBoss(m_CreatureID);
        }
    }
    else if(m_WorldBossState == WORLD_BOSS_START)
    {
        
    }
    else if(m_WorldBossState == WORLD_BOSS_END)
    {
        
    }
}

void WorldBossChannelManager::OpenDungeon()
{
    // �¿���N������BOSSƵ��
    uint32 maxChannelNumber = m_pDungeonSet->GetChannelNumber();

    for(uint32 ci=0; ci<maxChannelNumber; ci++)
    {
        uint32 newChannelID = findValidChannelID();
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(newChannelID);
        pDungeon->SetCanDestory(false);     // ����BOSS�ĸ������Զ�����

        m_CurrentChannels.push_back(newChannelID);

        RegularWorldBoss* pRegular = (RegularWorldBoss*)pDungeon->GetDefaultScene()->GetRegular();
        pRegular->Start();

        RegularDoc* doc = RegularManager::Instance()->GetRegularDoc(WORLD_BOSS_MAP, 1);
        if(doc)
        {
            ACTIVITY_INFO* activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_WORLD_BOSS);
            if(activityInfo)
            {
                int32 runTime = activityInfo->end_time - activityInfo->start_time - doc->m_ReadyTime;
                CnAssert(runTime >= 0);
                if(runTime < 0) runTime = 0;
                pDungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, 0);
                CnDbg("World boss readtime: %u, runtime:%u\n", doc->m_ReadyTime, runTime);
            }
        }
    }

    m_WorldBossState = WORLD_BOSS_OPEN;
    m_StateTime = 0;
}

void WorldBossChannelManager::CreateWorldBoss(uint32 creatureID)
{
    if(m_CurrentChannels.size() == 0)
    {
        CnAssert(false);        // ����Bossʱ������һ������
        return;
    }

    for(uint32 ci=0; ci<m_CurrentChannels.size(); ci++)
    {
        uint32 ChannelID = m_CurrentChannels[ci];
        Dungeon* pDungeon = m_pDungeonSet->GetDungeon(ChannelID);
        RegularWorldBoss* pRegular = (RegularWorldBoss*)pDungeon->GetDefaultScene()->GetRegular();
        pRegular->CreateWorldBoss(creatureID, pDungeon->GetDungeonID());
    }
}

void WorldBossChannelManager::CloseDungeon()
{
    // �ر�N������BOSSƵ��
    for(uint32 ci=0; ci<m_CurrentChannels.size(); ci++)
    {
        uint32 ChannelID = m_CurrentChannels[ci];
        Dungeon* pDungeon = m_pDungeonSet->GetDungeon(ChannelID);
        pDungeon->SetCanDestory(true);      // ����BOSS����������������
        pDungeon->SetVirgin(false);         // ���Ϊ���˽������ʹ֮��������
        RegularWorldBoss* pRegular = (RegularWorldBoss*)pDungeon->GetDefaultScene()->GetRegular();
        pRegular->End();
    }
    m_CurrentChannels.clear();

    m_WorldBossState = WORLD_BOSS_CLOSE;
    m_StateTime = 0;
}


