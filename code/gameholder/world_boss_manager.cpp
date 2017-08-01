#include "gameholder_pch.h"
#include "world_boss_manager.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "event_define.h"
#include "center_handler.h"
#include "boss.h"
#include "formula_manager.h"
#include "battle_unit_communicator.h"
#include "world_boss_channel_manager.h"

IMPLEMENT_SINGLETON(WorldBossManager)

WorldBossManager::WorldBossManager()
{
    m_WorldBossInfo.clear();
}

WorldBossManager::~WorldBossManager()
{

}

void WorldBossManager::OnServerReady()
{
    // 加载世界BOSS信息
    WorldBossInfoLoadReq();

}

void WorldBossManager::WorldBossInfoLoadReq()
{
    NONE sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_WORLD_BOSS_INFO_LOAD_REQ, &sendMsg);        // call -> MSG_WORLD_BOSS_INFO_LOAD
}

void WorldBossManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_WORLD_BOSS_START_NTF:
        {   // 世界Boss开始
            WORLD_BOSS_START_NTF* ntf = (WORLD_BOSS_START_NTF*)data;
            StartWorldBossDungeon(ntf->dungeon_id, ntf->boss_id);
        }
        break;
    case MSG_WORLD_BOSS_END_NTF:
        {   // 世界Boss结束
            WORLD_BOSS_END_NTF* ntf = (WORLD_BOSS_END_NTF*)data;
            EndWorldBossDungeon(ntf->dungeon_id);
        }
        break;
    case MSG_WORLD_BOSS_INFO_LOAD:
        {   // 更新世界Boss信息
            WORLD_BOSS_INFO_LIST* recvMsg = (WORLD_BOSS_INFO_LIST*)data;
            LoadWorldBoss(*recvMsg);
        }
        break;
    default:
        break;
    }
}

void WorldBossManager::Update()
{

    
}

void WorldBossManager::LoadWorldBoss(WORLD_BOSS_INFO_LIST& info)
{
    m_WorldBossInfo.clear();

    for(std::vector<WORLD_BOSS_INFO>::iterator it = info.boss_list.begin(); it != info.boss_list.end(); ++it)
    {
        CnAssert(m_WorldBossInfo.find(it->boss_id) == m_WorldBossInfo.end());
        m_WorldBossInfo[it->boss_id] = *it;
    }
}

void WorldBossManager::FillWorldBoss(WORLD_BOSS_INFO_LIST& info)
{
    for(WorldBossInfoMap::iterator it = m_WorldBossInfo.begin(); it != m_WorldBossInfo.end(); ++it)
    {
        info.boss_list.push_back(it->second);
    }
}

void WorldBossManager::LevelUpBoss(uint32 bossID , uint32 bosstarttime, uint64 levelUpSeconds)
{
    WORLD_BOSS_INFO bossInfo;
    uint64 costTime = 0;
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();

    WorldBossInfoMap::iterator iter = m_WorldBossInfo.find(bossID);
    if(iter == m_WorldBossInfo.end())
    {
        bossInfo.save_id = WORLDBOSS_SAVE_TYPE_ID;
        bossInfo.boss_id = bossID;
        bossInfo.strong_level = 1;
    }
    else
    {
        bossInfo = iter->second;

        if(nowTime > bosstarttime)
        {
            if(nowTime > bosstarttime)
            {
                costTime = nowTime - bosstarttime;
            }
            if(costTime < levelUpSeconds)
            {
                bossInfo.strong_level++;
            }
        }

        if( bossInfo.strong_level > MAX_WORLD_BOSS_LEVEL)
            bossInfo.strong_level  = MAX_WORLD_BOSS_LEVEL;
    }

    m_WorldBossInfo[bossInfo.boss_id] = bossInfo;

    // 更新世界BOSS信息
    SendWorldBossInfoToCenter();
}

// 世界BOSS开启
void WorldBossManager::StartWorldBossDungeon(uint32 dungeonID, uint32 bossID)
{
    CnInfo("StartWorldBoss dungeon=%d boss=%d\n", dungeonID, bossID);

    // 世界Boss副本
    DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeonID);
    if(!pDungeonSet)
        return;

    ChannelManager* pChannelManager = pDungeonSet->GetChannelManager();
    CnAssert(pChannelManager);

    if(pChannelManager->GetType() != CHANNEL_WORLD_BOSS)
    {
        CnAssert(false);
        return;
    }

    WorldBossChannelManager* pWorldBossChannelManager = (WorldBossChannelManager*)pChannelManager;
    CnAssert(bossID);
    pWorldBossChannelManager->SetWorldBossID(bossID);
    pWorldBossChannelManager->OpenDungeon();
}

// 世界BOSS结束
void WorldBossManager::EndWorldBossDungeon(uint32 dungeonID)
{
    CnInfo("EndWorldBoss dungeon=%d\n", dungeonID);

    // 世界Boss副本
    DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeonID);
    if(!pDungeonSet)
        return;

    ChannelManager* pChannelManager = pDungeonSet->GetChannelManager();
    CnAssert(pChannelManager);

    if(pChannelManager->GetType() != CHANNEL_WORLD_BOSS)
    {
        CnAssert(false);
        return;
    }

    WorldBossChannelManager* pWorldBossChannelManager = (WorldBossChannelManager*)pChannelManager;
    pWorldBossChannelManager->CloseDungeon();
}

void WorldBossManager::SendWorldBossInfoToCenter()
{
    WORLD_BOSS_INFO_LIST sendMsg;
    FillWorldBoss(sendMsg);
    CenterHandler::Instance()->SendToMasterCenter(MSG_WORLD_BOSS_INFO_FILL, &sendMsg);
}

// ==================================================
// 根据 BOSS 类型获得 BOSS 死亡时间
// ==================================================
uint32 WorldBossManager::GetBossDeadTime(uint32 boss_id)
{
    WorldBossInfoMap::iterator it = m_WorldBossInfo.find(boss_id);
    if(it == m_WorldBossInfo.end())
    {
        return 0;
    }
    return it->second.dead_time;
}

// ==================================================
// 设置 BOSS
// ==================================================
void WorldBossManager::SetBoss(uint32 save_id, uint32 boss_id, uint32 dead_time ,uint32 uplevel)
{
    WORLD_BOSS_INFO bossInfo;
    WorldBossInfoMap::iterator it = m_WorldBossInfo.find(boss_id);
    if(it == m_WorldBossInfo.end())
    {
        bossInfo.save_id = save_id;
        bossInfo.boss_id = boss_id;
        bossInfo.dead_time = dead_time;
        bossInfo.strong_level = uplevel;
        m_WorldBossInfo[boss_id] = bossInfo;
    }
    else
    {
        it->second.boss_id = boss_id;
        it->second.dead_time = dead_time;
    }
}

// ==================================================
// 设置 BOSS 死亡时间
// ==================================================
void WorldBossManager::SetBossDeadTime(uint32 boss_id, uint32 dead_time)
{
    WorldBossInfoMap::iterator it = m_WorldBossInfo.find(boss_id);
    if(it != m_WorldBossInfo.end())
    {
        it->second.dead_time = dead_time;
    }
}
// ==================================================
// 根据强壮等级，使世界boss变强
// ==================================================
void WorldBossManager::StrongWorldBoss(Creature* battleUnit)
{
    WorldBossInfoMap::iterator worldBossInfoIt;
    int32 maxhp = 0;
    int32 attack = 0;
    int32 defence = 0;
    int32 upLevel = 0;

    if(!battleUnit)
        return;

    worldBossInfoIt = m_WorldBossInfo.find(battleUnit->GetCreatureId());
    if(worldBossInfoIt == m_WorldBossInfo.end())
        return;     // 无此世界boss数据

    const WORLD_BOSS_INFO& bossInfo = worldBossInfoIt->second;

    if (bossInfo.save_id == VOID_FIELD_BOSS_SAVE_TYPE_ID)
    {
        maxhp = FormulaManager::Instance()->CalcVoidFieldBossHp(battleUnit, bossInfo.strong_level);
        attack = FormulaManager::Instance()->CalcVoidFieldBossAttack(battleUnit, bossInfo.strong_level);
        defence =  FormulaManager::Instance()->CalcVoidFieldBossDefence(battleUnit, bossInfo.strong_level);
    }
    else if (bossInfo.save_id == VOID_FIELD_SOLO_BOSS_SAVE_TYPE_ID)
    {
        maxhp = FormulaManager::Instance()->CalcVoidFieldSoloBossHp(battleUnit, bossInfo.strong_level);
        attack = FormulaManager::Instance()->CalcVoidFieldSoloBossAttack(battleUnit, bossInfo.strong_level);
        defence =  FormulaManager::Instance()->CalcVoidFieldSoloBossDefence(battleUnit, bossInfo.strong_level);
    }
    else
    {
        maxhp = FormulaManager::Instance()->CalcWorldBossHp(battleUnit, bossInfo.strong_level);
        attack = FormulaManager::Instance()->CalcWorldBossAttack(battleUnit, bossInfo.strong_level);
        defence =  FormulaManager::Instance()->CalcWorldBossDefence(battleUnit, bossInfo.strong_level);
    }

    if(maxhp > MAX_HP_LEN)
        maxhp = MAX_HP_LEN;

    battleUnit->SetMaxHp(maxhp);
    battleUnit->SetAttack(attack);
    battleUnit->SetArmor(defence);
    battleUnit->SetHP(maxhp);

    // 等级加成 技能变强
    upLevel = battleUnit->GetLevel() + bossInfo.strong_level;

    if(upLevel > MAX_WORLD_BOSS_LEVEL)
        upLevel = MAX_WORLD_BOSS_LEVEL;

    if(upLevel < 1)
        upLevel = 1;

    battleUnit->SetLevel(upLevel);
}

