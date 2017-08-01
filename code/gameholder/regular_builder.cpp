#include "gameholder_pch.h"
#include "regular_builder.h"
#include "point_base.h"
#include "event_define.h"
#include "scene.h"
#include "battle_unit.h"
#include "drop_object.h"
#include "drop_ruler.h"
#include "regular_single.h"
#include "monster.h"
#include "dungeon_config_manager.h"
#include "buff_controller.h"
#include "player.h"
#include "monster_config_manager.h"
#include "random_package_manager.h"
#include "soldier.h"

CnImplementRTTI(RegularBuilder, Regular);

RegularBuilder::RegularBuilder(RegularDoc *pDoc) : Regular(pDoc),
m_IsBuild(false)
{
    m_GenerateTriggerIDCounter = 10000;

    m_DropRuler = CnNew DropRuler(this);
}

RegularBuilder::~RegularBuilder()
{
    for(std::vector<PointBase*>::iterator iter = m_BusinessPoints.begin(); iter != m_BusinessPoints.end(); iter++)
    {
        PointBase* pBusinessPoint = *iter;
        CnDelete pBusinessPoint;
    }

    CnDelete m_DropRuler;
}

void RegularBuilder::Update(uint32 curr_tick)
{
    Regular::Update(curr_tick);

    if(!m_IsBuild)
    {
        m_IsBuild = true;

        // 构建所有的事件点
        for(std::vector<PointBase*>::iterator iter = m_BusinessPoints.begin(); iter != m_BusinessPoints.end(); iter++)
        {
            PointBase* pBusinessPoint = *iter;
            if(!pBusinessPoint->Build())
            {
                CnWarn("regular %s build point failed\n", m_FileName.c_str());
            }
        }
    }

    for(std::vector<PointBase*>::iterator iter = m_BusinessPoints.begin(); iter != m_BusinessPoints.end(); iter++)
    {
        PointBase* pBusinessPoint = *iter;
        pBusinessPoint->Update(curr_tick);
    }

}

void RegularBuilder::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    Regular::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    // 事件点需要处理的事件
    for(std::vector<PointBase*>::iterator iter = m_BusinessPoints.begin(); iter != m_BusinessPoints.end(); iter++)
    {
        PointBase* pBusinessPoint = *iter;
        pBusinessPoint->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }

    if(eventId == BATTLE_EVENT_MONSTER_DEAD)
    {
        OnMonsterDrop(arg1, arg2);
    }
}

bool RegularBuilder::IsEnemy(BattleUnit* attacker, BattleUnit* defencer)
{
    // 队友判断
    uint64 teamID_attacker = attacker->GetTeamID();
    uint64 teamID_defencer = defencer->GetTeamID();
    if(teamID_attacker && teamID_defencer && teamID_attacker == teamID_defencer)
        return false;
    
    return Regular::IsEnemy(attacker, defencer);
}

void RegularBuilder::AddPoint(PointBase* point)
{
    m_BusinessPoints.push_back(point);
}

uint32 RegularBuilder::GenerateValidTriggerID()
{
    uint32 newTriggerID = ++m_GenerateTriggerIDCounter;

    while(m_pScene->GetTriggerByID(newTriggerID))
    {
        newTriggerID++;
    }
    // m_GenerateTriggerIDCounter永远是最新的TriggerID
    m_GenerateTriggerIDCounter = newTriggerID;

    return m_GenerateTriggerIDCounter;
}

void RegularBuilder::OnMonsterDrop(int32 nMonsterID, int32 nKillerID)
{
    Monster* pMonster = CnDynamicCast(Monster, GetScene()->GetBattleUnit(nMonsterID));
    if (!pMonster)
    {
        return;
    }

    BattleUnit* pKiller = GetScene()->GetBattleUnit(nKillerID);
    if (!pKiller)
    {
        return;
    }

    Soldier* pPet = CnDynamicCast(Soldier, pKiller);
    if (pPet)
    {
        pKiller = pPet->GetMaster();
    }

    Player* pPlayer = CnDynamicCast(Player, pKiller);
    if (!pPlayer)
    {   
        return;
    }

    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* pMonsterStruct = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(pMonster->GetMonsterData().creatureId);
    if(!pMonsterStruct)
    {
        return;
    }

    uint32 package_id(0);

    if (pMonsterStruct->package_id)
    {
        package_id = pMonsterStruct->package_id;
    }
    else
    {
        const DUNGEON_DROP_ROOT_DROP_STRUCT* pDropStruct = DungeonConfigManager::Instance()->FindDropByID(GetScene()->GetDungeonID());
        if (!pDropStruct)
        {
            return;
        }

        if (Crown::SDRandom(BASE_RATE_NUM) >= (uint32)pDropStruct->drop_probability)
        {
            return;
        }

        package_id = pDropStruct->package_id;
    }

    if (!package_id)
    {
        return;
    }

    REWARD_TABLE table;

    pPlayer->GetRandomPackageMgr()->GetPackage(0, package_id, table);

    DropItem(table, pMonster->GetPosition());
}

void RegularBuilder::DropItem(const std::vector<RandomDropData>& data_list, const CoVec2& from_pos)
{
    DropData dropdata;
    ItemBase* pItemBase(NULL);
    for (size_t i=0; i<data_list.size(); ++i)
    {
        // 掉落物品
        pItemBase = m_DropRuler->DropItem(0, data_list[i]);
        if(!pItemBase)
        {
            continue;
        }

        dropdata.clear();
        dropdata.from_pos = from_pos;
        dropdata.pos = GetScene()->FindValidDropItemPostion(dropdata.from_pos, 250);            // 掉落范围
        dropdata.owner_id = 0;
        dropdata.lifetime = 20000;           // 20秒,掉落物品消失时间

        GetScene()->GetRegular()->AddDropObject(dropdata, pItemBase);
    }
}

void RegularBuilder::DropItem(const REWARD_TABLE& table, const CoVec2& from_pos)
{
    DropData dropdata;
    ItemBase* pItemBase(NULL);
    for (size_t i=0; i<table.items.size(); ++i)
    {
        // 掉落物品
        pItemBase = m_DropRuler->DropItem(0, table.items[i]);
        if(!pItemBase)
        {
            continue;
        }

        dropdata.clear();
        dropdata.from_pos = from_pos;
        dropdata.pos = GetScene()->FindValidDropItemPostion(dropdata.from_pos, 250);            // 掉落范围
        dropdata.owner_id = 0;
        dropdata.lifetime = 20000;           // 20秒,掉落物品消失时间

        GetScene()->GetRegular()->AddDropObject(dropdata, pItemBase);
    }
}