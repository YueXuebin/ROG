#include "gameholder_pch.h"
#include "regular_maoxian_002.h"
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

RegularMaoXian_002::RegularMaoXian_002(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_002::~RegularMaoXian_002()
{

}

void RegularMaoXian_002::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(3);

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
	pEventScore_Timer->SetTime(1000*120);            // 20秒通关获得一星
	AddScoreEvent(pEventScore_Timer);

	/////////////////////////////////////////////////////////////////////////////////

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3122, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(10);             // trigger 10 触发
	pPointMonster->SetAIGroup(1);


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3013, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(10);             // trigger 10 触发
	pPointMonster->SetAIGroup(2);



	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3011, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);             // trigger 10 触发
	pPointMonster->SetAIGroup(2);


	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3011, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);             // trigger 10 触发
	pPointMonster->SetAIGroup(2);



	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3019, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 10 触发
	pPointMonster->SetAIGroup(3);


	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3019, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 10 触发
	pPointMonster->SetAIGroup(3);


	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3011, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 10 触发
	pPointMonster->SetAIGroup(4);

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3041, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 10 触发
	pPointMonster->SetAIGroup(4);

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3041, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 10 触发
	pPointMonster->SetAIGroup(4);


	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3011, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(4);             // trigger 10 触发
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 10 触发
	//pPointMonster->SetAIGroup(4);


	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);             // trigger 10 触发
	pPointMonster->SetAIGroup(1);



	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(2000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);             // trigger 10 触发
	pPointMonster->SetAIGroup(1);


	pPointTouch = CnNew PointTouch (this, 23);
	pPointTouch->SetShowID(4006);


	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3015, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(15);             // trigger 10 触发
	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3041, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(15);             // trigger 10 触发
	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3015, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(15);             // trigger 10 触发
	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3041, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(15);             // trigger 10 触发

	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪

	// 点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪

	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3302, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	pPointMonster->SetAIGroup(5);                // AI组
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪

	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3107, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);                // AI组
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪

	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3107, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);                // AI组
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪






}
