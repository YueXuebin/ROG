#include "gameholder_pch.h"
#include "game_util.h"
#include "rog_regular_event_monster_clear.h"
#include "scene.h"
#include "monster.h"
#include "point_base.h"
#include "point_monster.h"
#include "regular_dungeon.h"

CnImplementRTTI(ROGRegularEvent_MonsterClear, ROGRegularEvent);

ROGRegularEvent_MonsterClear::ROGRegularEvent_MonsterClear(RegularDungeon* pRegular) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_MONSTER_CLEAR)
{
    
}

ROGRegularEvent_MonsterClear::~ROGRegularEvent_MonsterClear()
{

}

void ROGRegularEvent_MonsterClear::OneSecondUpdate( int32 deltatime )
{
    if(GetActive())
        return;

    // 检查是否所有怪物都死亡
    if(IsAllDead())
    {   
        GetRegular()->SetConditionChanged();
        SetActive(true);
    }
}

bool ROGRegularEvent_MonsterClear::IsAllDead()
{
    Scene* pScene = GetScene();

    Scene::MonsterMap& Monsters = pScene->GetMonsters();

    // 条件1：场景里已有的敌对怪全部死亡
    bool MonsterClear = true;
    for(Scene::MonsterMap::iterator msIter = Monsters.begin(); msIter != Monsters.end(); msIter++)
    {
        Monster* pMonster = msIter->second;
        // 是否是木桶
        if(IsBarrel(pMonster->GetCreatureId()))
            continue;

        //怪在敌对组内，且只要有一个怪没死,就返回false
        if(pMonster->GetGroup() == ENEMY_GROUP && !pMonster->IsDead())
        {
            MonsterClear = false;
            break;
        }
    }

    // 条件2：场景里的非循环出怪点(排除触摸石,要trigger触发的),出怪死完
    std::vector<PointBase*> pPointBaseList = GetRegular()->GetBusinessPoints();
    bool PointClear = true;

    for (uint32 i=0; i<pPointBaseList.size(); i++)
    {
        // 只检查trigger点出发和正常出怪
        PointBase* pPointBase = pPointBaseList[i];

        // 还有没有正常怪点没有出完怪
        if (!pPointBase->IsKindOf(&PointMonster::ms_RTTI))
            continue;

        PointMonster* pPointMonster = CnDynamicCast(PointMonster, pPointBase);
        if(!pPointMonster) 
        {
            CnAssert(false);
            return false;
        }

        // 排除非怪点触发型（受链式触发的影响）
        if(pPointMonster->GetEventPointType() && pPointMonster->GetEventPointType() != POINT_TYPE_MONSTER)
            continue;

        Spawn* pSpawn = pPointMonster->GetSpawn();
        // 排除循环怪点
        if(pSpawn->GetLoop())
            continue;

        if(!pSpawn->IsSpawnEnd())
        {
            PointClear = false;
            break;
        }
    }

    return (MonsterClear && PointClear);
}