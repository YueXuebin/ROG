#include "centerserver_pch.h"
#include "world_boss_manager.h"
#include "dungeon_manager.h"
#include "game_handler.h"
#include "world_boss_job.h"

IMPLEMENT_SINGLETON(WorldBossManager)

WorldBossManager::WorldBossManager()
{
    m_nextBossList.Clear();
}

WorldBossManager::~WorldBossManager()
{

}

void WorldBossManager::OnRecv( uint64 player, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_WORLD_BOSS_START_NTF:
        RecvWorldBossStart(data);
        break;
    case MSG_WORLD_BOSS_END_NTF:
        RecvWorldBossEnd(data);
        break;
    case MSG_WORLD_BOSS_INFO_FILL:
        FillWorldBoss(data);
        break;
    case MSG_WORLD_BOSS_INFO_LOAD_REQ:
        SendWorldBossList(GameHandler::Instance()->GetRegionIDBySessionID(sessionID));
        break;
    default:
        CnAssert(false);
        break;
    }
}

void WorldBossManager::SaveWorldBossInfo()
{
    WorldBossJob::Instance()->SaveWorldBossInfo();
}

void WorldBossManager::Init()
{
    WorldBossJob::Instance()->LoadWorldBossInfo();
}

void WorldBossManager::RecvWorldBossStart( void* data )
{
    WORLD_BOSS_START_NTF* startNtf = (WORLD_BOSS_START_NTF*)data;

    // 通知拥有该DungeonID的GameServer开启世界Boss
    uint32 GameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(startNtf->dungeon_id);
    if(GameServerID == 0)
    {
        CnAssert(false);
        CnError("%s start world boss failed\n", __FUNCTION__);
        return;
    }

    CnDbg("Start world boss\n");

    GameHandler::Instance()->SendByID(GameServerID, MSG_WORLD_BOSS_START_NTF, startNtf);
}

void WorldBossManager::RecvWorldBossEnd( void* data )
{
    WORLD_BOSS_END_NTF* endNtf = (WORLD_BOSS_END_NTF*)data;
    
    // 通知拥有该DungeonID的GameServer开启世界Boss
    uint32 GameServerID = DungeonManager::Instance()->GetServerIdByDungeonId(endNtf->dungeon_id);
    if(GameServerID == 0)
    {
        CnAssert(false);
        return;
    }

    CnDbg("End world boss\n");

    GameHandler::Instance()->SendByID(GameServerID, MSG_WORLD_BOSS_END_NTF, endNtf);
}

void WorldBossManager::FillWorldBoss(void* data)
{
    WORLD_BOSS_INFO_LIST* recvMsg = (WORLD_BOSS_INFO_LIST*)data;

    // 当有BOSS等级大于列表中的等级时, 更新BOSS等级
    for(std::vector<WORLD_BOSS_INFO>::iterator bossIt = recvMsg->boss_list.begin(); bossIt != recvMsg->boss_list.end(); ++bossIt)
    {
        WORLD_BOSS_INFO& bossInfo = *bossIt;

        // 是否已经有这个BOSS信息
        std::vector<WORLD_BOSS_INFO>::iterator nextIt = m_nextBossList.boss_list.begin();
        for(; nextIt != m_nextBossList.boss_list.end(); ++nextIt)
        {
            if(bossIt->boss_id == nextIt->boss_id)
            {
                break;
            }
        }

        // 不存在这个BOSS信息
        if(nextIt == m_nextBossList.boss_list.end())
        {
            m_nextBossList.boss_list.push_back(*bossIt);
        }
        else
        {
            // 存在这个BOSS等级比新的小 更新
            nextIt->strong_level = bossIt->strong_level;
            nextIt->dead_time = bossIt->dead_time;
        }
    }

    // 保存数据库
    SaveWorldBossInfo();
    // 发送给客户端
    SendWorldBossList();
}

void WorldBossManager::SendWorldBossList(uint32 gameserverID)
{
    WORLD_BOSS_INFO_LIST sendMsg;
    sendMsg.Clear();
    sendMsg = m_nextBossList;

    if(gameserverID == 0)
    {
        GameHandler::Instance()->SendToAll(MSG_WORLD_BOSS_INFO_LOAD, &sendMsg);
    }
    else
    {
        GameHandler::Instance()->SendByID(gameserverID, MSG_WORLD_BOSS_INFO_LOAD, &sendMsg);
    }
}
