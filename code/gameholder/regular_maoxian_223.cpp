#include "gameholder_pch.h"
#include "regular_maoxian_223.h"
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


RegularMaoXian_223::RegularMaoXian_223(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_223::~RegularMaoXian_223()
{

}

void RegularMaoXian_223::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(23);

	SetLifeCount(3); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3049, 1);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // 玩家死亡

	////////////////////////////////////////////////////////////////////////////

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3003, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3003, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 2);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 2);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(1000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 2);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 2);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(3000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3004, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3004, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3004, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3004, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1) 

	////////////////////////////////////////////////////////////////////////////////////////////

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3009, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质  BLUE  GREEN
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 23);         // 宝箱
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////

	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001,4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	//pPointMonster->SetDelayTime(200);         // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	//pPointMonster->SetDelayTime(200);         // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	pPointMonster->SetDelayTime(2000);          // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 点55
	pPointMonster = CnNew PointMonster(this, 55);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点56
	pPointMonster = CnNew PointMonster(this, 56);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点57
	pPointMonster = CnNew PointMonster(this, 57);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点54
	pPointMonster = CnNew PointMonster(this, 54);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 点50
	pPointMonster = CnNew PointMonster(this, 50);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	pPointMonster->SetDelayTime(500);          // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点50
	pPointMonster = CnNew PointMonster(this, 50);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点51
	pPointMonster = CnNew PointMonster(this, 51);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	pPointMonster->SetDelayTime(2000);          // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点51
	pPointMonster = CnNew PointMonster(this, 51);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	// 点52
	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	pPointMonster->SetDelayTime(500);          // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点52
	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点53
	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);  // 怪物品质
	pPointMonster->SetDelayTime(2000);          // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点53
	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)




	////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3031, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发

	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3003, 2);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发

	// 点33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3003, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);              // trigger 15 触发

	// 点34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3003, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3049, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);            // trigger 13 触发
	//pPointMonster->SetAIGroup(1)

	// 出宝箱
	pPointTreasure = CnNew PointTreasure(this,36);
	pPointTreasure->SetShowID(4001);
	pPointTreasure->LinkTrigger(pPointMonster);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 点37
	pPointBarrel = CnNew PointBarrel(this, 37);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点38
	pPointBarrel = CnNew PointBarrel(this, 38);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点39
	pPointBarrel = CnNew PointBarrel(this, 39);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点40
	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点41
	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1



}

