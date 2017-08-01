#include "gameholder_pch.h"
#include "regular_base.h"
#include "regular_event.h"
#include "monster.h"
#include "ai_controller.h"
#include "scene.h"
#include "boss.h"
#include "npc.h"
#include "trigger.h"
#include "checkpoint.h"
#include "spawn.h"
#include "battle_unit_connecter.h"
#include "offline_player.h"
#include "offline_player_manager.h"
#include "event_define.h"
#include "touch.h"
#include "drop_object.h"
#include "dungeon.h"
#include "state_controller.h"
#include "regular.h"
#include "ai_group_register.h"
#include "ai_group.h"
#include "property_set_calculater.h"
#include "dungeon_data.h"


CnImplementRootRTTI(RegularBase);

RegularBase::RegularBase(RegularDoc* pDoc)
{
    m_pRegularDoc = pDoc;
}

RegularBase::~RegularBase()
{
}

uint32 RegularBase::GetType()
{
    return m_pRegularDoc->m_type;
}

void RegularBase::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case SYSTEM_EVENT_LOAD_OFFLINE_PLAYER:
        {
            uint64 offlinePlayerID = U64ID(arg1, arg2);
            for(uint32 i=0; i < m_CacheCreateOfflinePlayerData.size(); )
            {
                const OfflinePlayerData& playerData = m_CacheCreateOfflinePlayerData[i];
                const OFFLINE_PLAYER_INFO* info = OfflinePlayerManager::Instance()->GetOfflinePlayerRecord(offlinePlayerID);
                if(info)
                {
                    if( (playerData.player_id != 0 && (playerData.player_id == U64ID(info->player_id_h, info->player_id_l)))
                        || (playerData.player_name.length() > 0 && playerData.player_name == info->name) )
                    {
                        OfflinePlayer* newOfflinePlayer = AddOfflinePlayer(*info, playerData.x, playerData.y, playerData.angle);
                        OnCreateOfflinePlayer(newOfflinePlayer);
                        m_CacheCreateOfflinePlayerData.erase(m_CacheCreateOfflinePlayerData.begin() + i);
                        continue;
                    }
                }
                else
                    CnWarn("load offline player info is null\n");

                i++;
            }
        }
        break;
    default:
        break;
    }

}

Monster* RegularBase::AddMonster(MonsterData& data)
{
    CnAssert(data.creatureType == 1);

    if (!data.valid())
    {
        return NULL;
    }

    data.LoadTmplData();        // 动态加载怪物基础数值


    Monster* pNewMonster = CnNew Monster();
    pNewMonster->SetCreatureId(data.creatureId);
    pNewMonster->InitMonster(data);
    pNewMonster->SetPosition(data.x, data.y);
    pNewMonster->SetAngle(data.angle);
    pNewMonster->SetAttackable(data.attackable);
    pNewMonster->SetCantMove(data.cantMove);
    pNewMonster->SetCantAngle(data.cantAngle);
    pNewMonster->SetCantBuff(data.cantBuff);
    pNewMonster->SetCantHeal(data.cantHeal);
    pNewMonster->SetCantTaunt(data.cantTaunt);
    pNewMonster->SetCantPush(data.cantPush);
    pNewMonster->SetAttackable(data.attackable);
    pNewMonster->SetLogicName(data.name);

    // group
    if (data.group != 0)
    {
        pNewMonster->SetGroup(data.group);
    }

    // 初始状态
    pNewMonster->GetStateController()->SetMovieAttackAble(data.movie_attackable);
    pNewMonster->ChangeState(data.state);
    if(data.state == STATE_MOVIE)
    {
        pNewMonster->GetStateController()->SetMovieTime(data.state_arg);
        pNewMonster->GetStateController()->SetMovieName(data.movie_name);
    }

    // AI
    AIController* pAIController = pNewMonster->GetAIController();
    pAIController->SetBasePosition(data.x, data.y);
    pAIController->SetPath(data.pathdata);
    pAIController->SetPathLoop(data.pathloop);

    if(data.ai.length() > 0)
    {
        pAIController->LoadAI(data.ai);
    }

    // 动态调整怪物属性
    ChangeStrong(pNewMonster);

    m_pScene->AddMonster(pNewMonster);
    m_monsterSum++;

    // AI组设置(必须在AddMonster之后)
    if(data.ai_group > 0)
    {
        pAIController->AddToAIGroup(data.ai_group);
    }

    return pNewMonster;
}

Monster* RegularBase::AddMonster(int32 creatureId, int32 x, int32 y, int32 angle, int32 level, int32 quality)
{
    MonsterData data;
    data.creatureType = 1;
    data.creatureId = creatureId;
    data.x = x;
    data.y = y;
    data.angle = angle;
    data.level = level;
    data.quality = quality;

    return AddMonster(data);
}

Boss* RegularBase::AddBoss(BossData& data, const std::string& name, bool connect)
{
    if (!data.valid())
    {
        return NULL;
    }

    if (data.autoLoadTmpl)
    {
        data.LoadTmplData();
    }

    Boss* pNewBoss = CnNew Boss();
    pNewBoss->SetCreatureId(data.creatureId);
    pNewBoss->InitBoss(data);
    pNewBoss->SetPosition(data.x, data.y);
    pNewBoss->SetAngle(data.angle);
    pNewBoss->SetBossUI(data.bossui);
    pNewBoss->SetBcorpse(data.b_corpse);
    pNewBoss->SetLifeTime(data.lifetime);
    pNewBoss->SetCreateTime();
    pNewBoss->SetCantMove(data.cantMove);
    pNewBoss->SetCantAngle(data.cantAngle);
    pNewBoss->SetCantBuff(data.cantBuff);
    pNewBoss->SetCantHeal(data.cantHeal);
    pNewBoss->SetCantTaunt(data.cantTaunt);
    pNewBoss->SetCantPush(data.cantPush);
    pNewBoss->SetAttackable(data.attackable);
    pNewBoss->SetLogicName(data.name);

    if (data.group != 0)
    {
        pNewBoss->SetGroup(data.group);
    }

    AIController* pAIController = pNewBoss->GetAIController();
    pAIController->SetBasePosition(data.x, data.y);
    pAIController->SetPath(data.pathdata);
    pAIController->SetPathLoop(data.pathloop);

    if (data.ai.length() > 0)
    {
        pAIController->LoadAI(data.ai);
    }

    pNewBoss->SetReincarnated(data.reincarnate_list);

    if (data.worldboss)
    {
        if (name == "")
        {
            pNewBoss->SetInstanceName(pNewBoss->GetLogicName());
        }
        else
        {
            pNewBoss->SetInstanceName(name);
        }

        if (connect)
        {
            ConnectBoss(pNewBoss);
        }
    }

    ChangeStrong(pNewBoss);      // 设置怪物难度

    m_pScene->AddBoss(pNewBoss);
    m_bossSum++;

    return pNewBoss;
}

Boss* RegularBase::AddBoss(int32 bossId, int32 x, int32 y, int32 angle, bool bossUI, bool bcorpse, uint32 lifetime)
{
    BossData data;
    data.creatureId = bossId;
    data.x = x;
    data.y = y;
    data.angle = angle;
    data.bossui = bossUI;
    data.b_corpse = bcorpse;
    data.lifetime = lifetime;

    return AddBoss(data);
}

DropObject* RegularBase::AddDropObject(const DropData& dropdata, ItemBase* pItemBase)
{
    CnAssert(pItemBase);

    DropObject* newDropObject = CnNew DropObject;
    newDropObject->Init(dropdata, pItemBase);
    m_pScene->AddDropItem(newDropObject);
    return newDropObject;
}

void RegularBase::KillMonster(int32 number, BattleUnit* pKiller)
{
    // 杀死离凶手最近的怪
    int32 kx = pKiller->GetX();
    int32 ky = pKiller->GetY();

    Scene::MonsterMap& monsterMap = m_pScene->GetMonsters();
    for(int32 ki=0; ki<number; ki++)
    {
        // 找到最近的怪
        int32 nearest = 9999999;
        Monster* pNearest = NULL;
        for(Scene::MonsterMap::iterator iter = monsterMap.begin(); iter != monsterMap.end(); iter++)
        {
            Monster* pkillMonster = iter->second;
            if(!pKiller->CanAttack(pkillMonster))
                continue;

            if(!pkillMonster->IsDead())
            {
                CoVec2 p = pkillMonster->GetPosition();
                p.x -= kx;
                p.y -= ky;

                if(nearest > p.Length())
                {
                    pNearest = pkillMonster;
                    nearest = (int32)p.Length();
                }
            }
        }

        if(pNearest)
        {
            pNearest->Kill(pKiller);
        }
    }

}

void RegularBase::KillBoss(int32 number, BattleUnit* pKiller)
{
    // 杀死离凶手最近的Boss
    int32 kx = pKiller->GetX();
    int32 ky = pKiller->GetY();

    Scene::BossMap& bossMap = m_pScene->GetBosses();
    for(int32 ki=0; ki<number; ki++)
    {
        // 找到最近的怪
        int32 nearest = 9999999;
        Boss* pNearest = NULL;
        for(Scene::BossMap::iterator iter = bossMap.begin(); iter != bossMap.end(); iter++)
        {
            Boss* pkillBoss = iter->second;
            if(!pKiller->CanAttack(pkillBoss))
                continue;

            if(!pkillBoss->IsDead())
            {
                CoVec2 p = pkillBoss->GetPosition();
                p.x -= kx;
                p.y -= ky;

                if(nearest > p.Length())
                {
                    pNearest = pkillBoss;
                    nearest = (int32)p.Length();
                }
            }
        }

        if(pNearest)
        {
            pNearest->Kill(pKiller);
        }
    }
}

void RegularBase::KillAll()
{
    Scene::MonsterMap& monsterMap = m_pScene->GetMonsters();
    for(Scene::MonsterMap::iterator iter = monsterMap.begin(); iter != monsterMap.end(); iter++)
    {
        Monster* pkillMonster = iter->second;
        if(!pkillMonster->IsDead())
        {
            pkillMonster->Kill(NULL);
        }
    }

    Scene::BossMap& bossMap = m_pScene->GetBosses();
    for(Scene::BossMap::iterator iter = bossMap.begin(); iter != bossMap.end(); iter++)
    {
        Boss* pkillBoss = iter->second;
        if(!pkillBoss->IsDead())
        {
            pkillBoss->Kill(NULL);
        }
    }
}

Trigger* RegularBase::CreateTrigger(const TriggerData& data)
{
    Trigger* pNewTrigger = CnNew Trigger;
    pNewTrigger->SetName(data.name);
    pNewTrigger->SetTriggerId(data.triggerId);
    pNewTrigger->SetShowID(data.showId);
    pNewTrigger->SetObsName(data.obs_name);
    pNewTrigger->SetPosition(data.x, data.y);
    pNewTrigger->SetAngle(data.angle);
    pNewTrigger->SetRadius(data.radius);
    pNewTrigger->SetTouch(data.isTouch);
    pNewTrigger->SetFuncID(data.funcId);
    pNewTrigger->SetFuncType(data.funcType);
    pNewTrigger->m_ObsPos = data.obs_pos;
    pNewTrigger->m_PickSize = data.pick_size;
    pNewTrigger->m_PickPos = data.pick_pos;
    return pNewTrigger;
}

Checkpoint* RegularBase::CreateCheckPoint(const CheckPointData& data)
{
    Checkpoint* pNewCheckpoint = CnNew Checkpoint;
    pNewCheckpoint->SetCheckpointId(data.pinID);
    pNewCheckpoint->SetPosition(data.x, data.y);
    pNewCheckpoint->SetAngle(data.angle);
    pNewCheckpoint->SetRadius(data.radius);
    return pNewCheckpoint;
}

Spawn* RegularBase::CreateSpawn(const SpawnData& data)
{
    Spawn* pNewSpawn = CnNew Spawn(data.spawnType);
    pNewSpawn->SetSpawnId(data.spawnId);
    pNewSpawn->SetPosition(data.x, data.y);
    pNewSpawn->SetAngle(data.angle);
    pNewSpawn->SetSpawnRadius(data.radius);
    pNewSpawn->SetLoop(data.loop);
    pNewSpawn->SetNoCollisionObject(data.no_collison_object);
    pNewSpawn->SetSpawning(!data.no_start);

    // 载入波数据
    for(uint32 i=0; i<data.WaveDatas.size(); i++)
    {
        const SpawnWaveData& waveData = data.WaveDatas[i];
        pNewSpawn->AddWave(waveData);
    }
    // 载入出怪控制器
    for(uint32 i=0; i<data.WaveDataControls.size(); i++)
    {
        const SpawnWaveControlData& waveControlData = data.WaveDataControls[i];
        pNewSpawn->AddWaveControl(waveControlData);
    }

    return pNewSpawn;
}

Touch* RegularBase::CreateTouch(const TouchData& data)
{
    Touch* pNewTouch = CnNew Touch;
    pNewTouch->SetTouchID(data.touchId);
    pNewTouch->SetShowID(data.showId);
    pNewTouch->SetObsName(data.obs_name);
    pNewTouch->m_ObsPos = data.obs_pos;
    pNewTouch->m_PickSize = data.pick_size;
    pNewTouch->m_PickPos = data.pick_pos;
    pNewTouch->SetPosition(data.x, data.y);
    pNewTouch->SetAngle(data.angle);
    pNewTouch->SetRadius(data.radius);
    pNewTouch->SetTime(data.time);
    return pNewTouch;
}

// 建立世界boss联系
void RegularBase::ConnectBoss(Boss* pBoss)
{
    BattleUnitConnecter* pBattleUnitConnecter = GetScene()->GetBattleUnitConnecter();
    if(pBattleUnitConnecter)
    {
        pBattleUnitConnecter->ConnectBattleUnit(m_pScene->GetDungeonID(), pBoss->GetInstanceName(), pBoss);
    }
}

void RegularBase::CreateAllBattleUnits()
{
    // 小怪
    for(std::vector<MonsterData>::iterator monsterIter = m_pRegularDoc->m_monsterList.begin(); 
        monsterIter != m_pRegularDoc->m_monsterList.end(); monsterIter++)
    {
        MonsterData& monsterData = *monsterIter;
        CnAssert(monsterData.creatureType == 1);
        AddMonster(monsterData);
    }
    // npc
    //for(std::vector<MonsterData>::iterator npcIter = m_pRegularDoc->m_npcList.begin(); 
    //    npcIter != m_pRegularDoc->m_npcList.end(); npcIter++)
    //{
    //    MonsterData& npcData = *npcIter;
    //    CnAssert(npcData.creatureType == 2);
    //    AddNpc(npcData);
    //}
    // Boss
    for(std::vector<BossData>::iterator bossIter = m_pRegularDoc->m_bossList.begin();
        bossIter != m_pRegularDoc->m_bossList.end(); bossIter++)
    {
        BossData& bossData = *bossIter;
        AddBoss(bossData);
    }
}

void RegularBase::CreateOfflinePlayer(uint64 offlinePlayerID, int32 x, int32 y, int32 angle)
{
    const OFFLINE_PLAYER_INFO* pRecord = OfflinePlayerManager::Instance()->GetOfflinePlayerRecord(offlinePlayerID);
    if(pRecord)
    {
        OfflinePlayer* pOfflinePlayer = AddOfflinePlayer(*pRecord, x, y, angle);
        OnCreateOfflinePlayer(pOfflinePlayer);
    }
    else
    {
        OfflinePlayerData data;
        data.player_id = offlinePlayerID;
        data.x = x;
        data.y = y;
        data.angle = angle;

        m_CacheCreateOfflinePlayerData.push_back(data);
        OfflinePlayerManager::Instance()->AskCSOfflinePlayerInfo(offlinePlayerID, OFF_PLAYER_REASON_DEFAULT);
    }
}

void RegularBase::CreateOfflinePlayerByName(const std::string& offlinePlayerName, int32 x, int32 y, int32 angle)
{
    const OFFLINE_PLAYER_INFO* pRecord = OfflinePlayerManager::Instance()->GetOfflinePlayerRecordByName(offlinePlayerName);
    if(pRecord)
    {
        OfflinePlayer* pOfflinePlayer = AddOfflinePlayer(*pRecord, x, y, angle);
        OnCreateOfflinePlayer(pOfflinePlayer);
    }
    else
    {
        OfflinePlayerData data;
        data.player_name = offlinePlayerName;
        data.x = x;
        data.y = y;
        data.angle = angle;

        m_CacheCreateOfflinePlayerData.push_back(data);
        OfflinePlayerManager::Instance()->AskCSOfflinePlayerInfoByName(offlinePlayerName, OFF_PLAYER_REASON_DEFAULT);
    }
}

OfflinePlayer* RegularBase::AddOfflinePlayer(const OFFLINE_PLAYER_INFO& OfflinePlayerInfo, int32 x, int32 y, int32 angle)
{
    OfflinePlayer* pOfflinePlayer = CnNew OfflinePlayer;
    pOfflinePlayer->SetPorperyByInfo(OfflinePlayerInfo);
    pOfflinePlayer->SetPosition(x, y);
    pOfflinePlayer->SetAngle(angle);
    pOfflinePlayer->GetAIController()->SetBasePosition(x, y);

    m_pScene->AddOfflinePlayer(pOfflinePlayer);

    return pOfflinePlayer;
}

void RegularBase::OnCreateOfflinePlayer(OfflinePlayer* pOfflinePlayer)
{
    pOfflinePlayer->SetGroup(FRIEND_GROUP);
    // 设置离线玩家的AI
    switch (pOfflinePlayer->GetPlayerInfo().career)
    {
    case CAREER_WARRIOR:
        pOfflinePlayer->GetAIController()->LoadAI("ai/team_offline_warrior.aix");
        break;
    case CAREER_MAGE:
        pOfflinePlayer->GetAIController()->LoadAI("ai/team_offline_mage.aix");
        break;
    case CAREER_ARCHER:
        pOfflinePlayer->GetAIController()->LoadAI("ai/team_offline_preist.aix");
        break;
    default:
        CnAssert("carrer is error\n");
        break;
    }
}

void RegularBase::CreateObjects()
{
    // 触发器
    RegularDoc::TriggerDataMap::iterator triggerIte, triggerIteEnd = m_pRegularDoc->m_triggerMap.end();
    for (triggerIte = m_pRegularDoc->m_triggerMap.begin(); triggerIte != triggerIteEnd; ++triggerIte)
    {
        m_pScene->AddTrigger(CreateTrigger(triggerIte->second));
    }
    // 路点
    RegularDoc::CheckPointDataMap::iterator pinIte, pinIteEnd = m_pRegularDoc->m_pinMap.end();
    for (pinIte = m_pRegularDoc->m_pinMap.begin(); pinIte != pinIteEnd; ++pinIte)
    {
        m_pScene->AddCheckpoint(CreateCheckPoint(pinIte->second));
    }
    // 出生点
    RegularDoc::SpawnDataMap::iterator spawnIte, spawnIteEnd = m_pRegularDoc->m_spawnMap.end();
    for (spawnIte = m_pRegularDoc->m_spawnMap.begin(); spawnIte != spawnIteEnd; ++spawnIte)
    {
        m_pScene->AddSpawn(CreateSpawn(spawnIte->second));
    }
    // 采集点
    RegularDoc::TouchDataMap::iterator touchIte, touchIteEnd = m_pRegularDoc->m_touchMap.end();
    for (touchIte = m_pRegularDoc->m_touchMap.begin(); touchIte != touchIteEnd; ++touchIte)
    {
        m_pScene->AddTouch(CreateTouch(touchIte->second));
    }
}

void RegularBase::CreateSceneObjects()
{
    CreateAllBattleUnits();
    CreateObjects();
}

void RegularBase::GetRaidsMonsterList(std::map<uint32, int32>& list)
{
    std::map<uint32, int32>::iterator monsterIt;

    if(!m_pRegularDoc) return;

    // 所有的小怪
    for(std::vector<MonsterData>::iterator msIter = m_pRegularDoc->m_monsterList.begin(); msIter != m_pRegularDoc->m_monsterList.end(); ++msIter)
    {
        monsterIt = list.find(msIter->creatureId);
        if(monsterIt != list.end())
            ++monsterIt->second;
        else
            list[msIter->creatureId] = 1;
    }
    // 所有的Boss
    for(std::vector<BossData>::iterator bsIter = m_pRegularDoc->m_bossList.begin(); bsIter != m_pRegularDoc->m_bossList.end(); ++bsIter)
    {
        monsterIt = list.find(bsIter->creatureId);
        if(monsterIt != list.end())
            ++monsterIt->second;
        else
            list[bsIter->creatureId] = 1;
    }
    // 怪物出生点中的怪
    RegularDoc::SpawnDataMap::iterator spawnIte, spawnIteEnd = m_pRegularDoc->m_spawnMap.end();
    for (spawnIte = m_pRegularDoc->m_spawnMap.begin(); spawnIte != spawnIteEnd; ++spawnIte)
    {
        const SpawnData::WaveDataList &waveDatas = spawnIte->second.WaveDatas;
        uint32 waveLen = waveDatas.size();

        for (uint32 wi = 0; wi < waveLen; ++wi)
        {
            const SpawnWaveData& waveData = waveDatas[wi];
            uint32 monsterLen = waveData.MonsterDatas.size();

            for (uint32 mi = 0; mi < monsterLen; ++mi)
            {
                uint32 monsterId = waveData.MonsterDatas[mi].creatureId;
                int32 monsterNum = waveData.MonsterDatas[mi].num;
                if (monsterId == 0 || monsterNum == 0)
                {
                    continue;
                }

                monsterIt = list.find(monsterId);
                if (monsterIt != list.end())
                {
                    monsterIt->second += monsterNum;
                }
                else
                {
                    list[monsterId] = monsterNum;
                }
            }
        }
    }
}

void RegularBase::ChangeStrong(BattleUnit* pBattleUnit)
{
    if(pBattleUnit->GetCommunicator())
        return;     // 世界Boss不做强度改变

    uint32 DifficultLevel = GetDifficultLevel();

    real hpPercent = (real)pBattleUnit->GetHP() / pBattleUnit->GetMaxHp();

    // 计算各难度的属性调整
    uint32 MaxHP_Percent = GetStrongVal(PROP_MAXHP, DifficultLevel);
    uint32 ResHp_Percent = GetStrongVal(PROP_HP_REC_RATE, DifficultLevel);

    PropertySet& monsterMapPropSet = pBattleUnit->GetPropertySetCalculater()->GetMapProp();
    monsterMapPropSet.Set(PROP_MAXHP_MUL, MaxHP_Percent);               // 生命值增强
    monsterMapPropSet.Set(PROP_HP_REC_RATE_MUL, MaxHP_Percent);         // 生命回复增强
    
    pBattleUnit->GetPropertySetCalculater()->CalcFinalProp();

    pBattleUnit->SetHP( int32(pBattleUnit->GetMaxHp() * hpPercent) );
}

uint32 RegularBase::GetStrongVal(uint32 type, uint32 DifficultLevel)
{
    // 根据不同类型关卡,提升幅度不同
    uint32 dungeonClass = GetDungeonClass(m_pScene->GetDungeonID());
    // 冒险地图
    if(dungeonClass == DUNGEON_CLASS_PLOT_ID)
    {
        switch(type)
        {
        case PROP_MAXHP:
            return uint32(BASE_RATE_NUM * (DifficultLevel-1) * 0.2f);
        case PROP_HP_REC_RATE:
            return BASE_RATE_NUM * (DifficultLevel-1);
        }
    }
    // 异界地图
    else if(dungeonClass == DUNGEON_CLASS_SOUL_ID)
    {
        switch(type)
        {
        case PROP_MAXHP:
            return uint32(BASE_RATE_NUM * (DifficultLevel-1) * 0.5f);
        case PROP_HP_REC_RATE:
            return BASE_RATE_NUM * (DifficultLevel-1);
        }
    }
    // 日常地图
    else if(dungeonClass == DUNGEON_CLASS_DAILY_ID)
    {
        switch(type)
        {
        case PROP_MAXHP:
            return uint32(BASE_RATE_NUM * (DifficultLevel-1) * 0.35f);
        case PROP_HP_REC_RATE:
            return BASE_RATE_NUM * (DifficultLevel-1);
        }
    }
    
    return 0;
}
