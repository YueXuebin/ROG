#include "gameholder_pch.h"
#include "regular_daily_001.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"
#include "rog_regular_event_monster_clear.h"
#include "rog_regular_event_player_dead.h"

RegularDaily_001::RegularDaily_001(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_001::~RegularDaily_001()
{

}

void RegularDaily_001::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;

    /////////////////////////////////////////////////////////////////////////////////
    // 关卡配置
    SetMapLevel(30);

    SetLifeCount(1); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
    AddSucceedEvent(pEventMonsterClear); // 清空全部怪

    // 失败条件
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // 玩家死亡


    /////////////////////////////////////////////////////////////////////////////////
    // 点11
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3004, 4);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3051, 4);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3015, 4);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetAIGroup(12);






}