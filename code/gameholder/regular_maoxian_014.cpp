#include "gameholder_pch.h"
#include "regular_maoxian_014.h"
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
#include "rog_regular_event_monster_dead.h"
#include "rog_regular_event_player_dead.h"
#include "rog_regular_event_timer.h"


RegularMaoXian_014::RegularMaoXian_014(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_014::~RegularMaoXian_014()
{

}

void RegularMaoXian_014::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;
    PointTrigger* pPointTrigger = NULL;

    /////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(14);

    SetLifeCount(3); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3314, 1);
    AddSucceedEvent(pEventMonsterDead); // 杀死怪

    // 失败条件
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // 玩家死亡

	// 评价条件
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // 一条命不死获得一星
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*180);            // 20秒通关获得一星
	AddScoreEvent(pEventScore_Timer);

	////////////////////////////////////////////////////////////////////////////

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // 出什么怪(怪id, 数量)


	////////////////////////////////////////////////////////////////////////////
	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(1);

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(1);

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(1);

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(1);

	////////////////////////////////////////////////////////////////////////////

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(2);

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(2);

	////////////////////////////////////////////////////////////////////////////


	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(3);

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3110, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(3);

	////////////////////////////////////////////////////////////////////////////


	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(4);

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(4);

	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3111, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(4);


	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3041, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetTriggerID(11);            // trigger 15 触发
	pPointMonster->SetAIGroup(4);

	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3041, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetTriggerID(11);            // trigger 15 触发
	pPointMonster->SetAIGroup(4);

	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3113, 1);         // 出什么怪(怪id, 数量)

	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 4);         // 出什么怪(怪id, 数量)

	////////////////////////////////////////////////////////////////////////////

	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3118, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(7);
	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3116, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(7);

	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3118, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(8);
	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3116, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(8);

	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3118, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(9);
	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3116, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(9);
	////////////////////////////////////////////////////////////////////////////

	// 点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)
	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)

	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3110,1);         // 出什么怪(怪id, 数量)


	////////////////////////////////////////////////////////////////////////////

	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // 出什么怪(怪id, 数量)

	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3110, 2);         // 出什么怪(怪id, 数量)

	////////////////////////////////////////////////////////////////////////////

	// 点33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)
	// 点34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)
	////////////////////////////////////////////////////////////////////////////

	// 点35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);

	// 点36
	pPointMonster = CnNew PointMonster(this, 36);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);
	// 点37
	pPointMonster = CnNew PointMonster(this, 37);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3110,2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);


	////////////////////////////////////////////////////////////////////////////BOSS

	// 点38
	pPointMonster = CnNew PointMonster(this,38);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3314, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 点37
	pPointBarrel = CnNew PointBarrel(this, 39);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点38
	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点39
	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点40
	pPointBarrel = CnNew PointBarrel(this, 42);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点41
	pPointBarrel = CnNew PointBarrel(this, 43);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1



}

