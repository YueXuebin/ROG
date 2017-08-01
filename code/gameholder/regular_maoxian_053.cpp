#include "gameholder_pch.h"
#include "regular_maoxian_053.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"
#include "point_trigger.h"
#include "monster.h"
#include "spawn.h"
#include "point_barrel.h"
#include "rog_regular_event_monster_clear.h"
#include "rog_regular_event_player_dead.h"
#include "rog_regular_event_timer.h"

RegularMaoXian_053::RegularMaoXian_053(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_053::~RegularMaoXian_053()
{

}

void RegularMaoXian_053::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;
    PointTrigger* pPointTrigger = NULL;             // 触摸石
    uint32 randomNumber = 0;    // 随机数

    /////////////////////////////////////////////////////////////////////////////////
    // 关卡配置
    SetMapLevel(53);

    SetLifeCount(3); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
    AddSucceedEvent(pEventMonsterClear); // 清空全部怪

    // 失败条件
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // 玩家死亡


	// 评价条件
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // 一条命不死获得一星
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*20);            // 20秒通关获得一星
	AddScoreEvent(pEventScore_Timer);

    /////////////////////////////////////////////////////////////////////////////////
    // 点11
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3051, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(11);             // trigger 2 触发
    pPointMonster->SetAIGroup(1);

    // 点11
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(11);             // trigger 2 触发
    pPointMonster->SetAIGroup(1);

    // 点12
    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3051, 1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(12);             // trigger 2 触发
    pPointMonster->SetAIGroup(2);


    // 点13
    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(12);             // trigger 2 触发
    pPointMonster->SetAIGroup(2);

    // 点14
    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3014, 3);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(12);             // trigger 2 触发
    //pPointMonster->SetAIGroup(2);

    // 点15
    pPointMonster = CnNew PointMonster(this, 15);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3045, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(12);             // trigger 2 触发
    //pPointMonster->SetAIGroup(2);

    // 点16
    pPointMonster = CnNew PointMonster(this, 16);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(13);             // trigger 2 触发
    //pPointMonster->SetAIGroup(2);

    pPointMonster = CnNew PointMonster(this, 16);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3051, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(13);             // trigger 2 触发
    //pPointMonster->SetAIGroup(2);

    // 点17
    pPointMonster = CnNew PointMonster(this, 17);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3014, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(12);             // trigger 2 触发
    //pPointMonster->SetAIGroup(2);

    // 点21
    pPointMonster = CnNew PointMonster(this, 18);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(13);             // trigger 3 触发

    pPointMonster = CnNew PointMonster(this, 18);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3009, 1);         // 出什么怪(怪id, 数量)
    pPointMonster->SetQuality(QUALITY_GREEN);
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(13);             // trigger 3 触发



    // 点22
    pPointMonster = CnNew PointMonster(this, 19);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3014,3);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(13);             // trigger 3 触发

    pPointMonster = CnNew PointMonster(this, 19);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3051,1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(13);             // trigger 3 触发

    // 点23
    pPointMonster = CnNew PointMonster(this, 20);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3002, 3);         // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(14);             // trigger 3 触发

    // 点23
    pPointMonster = CnNew PointMonster(this, 41);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3029, 1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(14);             // trigger 3 触发

    // 点24
    pPointBarrel = CnNew PointBarrel(this, 51);     // 木桶
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 6个

    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 51);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // 出场演绎
        pPointMonster->LinkTrigger(pPointBarrel);

    }

    pPointBarrel = CnNew PointBarrel(this, 52);     // 木桶
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 6个

    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 52);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // 出场演绎
        pPointMonster->LinkTrigger(pPointBarrel);

    }

    pPointBarrel = CnNew PointBarrel(this, 53);     // 木桶
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 6个

    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 53);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // 出场演绎
        pPointMonster->LinkTrigger(pPointBarrel);

    }



    // 点25
    pPointMonster = CnNew PointMonster(this, 42);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3051, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    //pPointMonster->SetTriggerID(14);             // trigger 4 触发



    // 点29
    pPointMonster = CnNew PointMonster(this, 43);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetTriggerID(pPointTrigger->GetTriggerID());

    // 点32
    pPointMonster = CnNew PointMonster(this, 44);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3002, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetTriggerID(pPointTrigger->GetTriggerID());



    // 点31
    pPointMonster = CnNew PointMonster(this, 45);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3051, 3);
    //pPointMonster->SetDefaultMovie();
    //pPointMonster->SetTriggerID(14);

    pPointMonster = CnNew PointMonster(this, 31);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3014, 2);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(15);             // trigger 2 触发
    pPointMonster->SetAIGroup(10);

    pPointMonster = CnNew PointMonster(this, 32);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3034, 1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetTriggerID(15);             // trigger 2 触发
    pPointMonster->SetAIGroup(10);

    pPointMonster = CnNew PointMonster(this, 33);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 1);         // 出什么怪(怪id, 数量)
    //pPointMonster->SetDefaultMovie();           // 出场演绎
    //pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
    //pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetTriggerID(15);             // trigger 2 触发
    pPointMonster->SetAIGroup(10);


    
        
}

