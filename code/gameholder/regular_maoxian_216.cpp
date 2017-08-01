#include "gameholder_pch.h"
#include "regular_maoxian_216.h"
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


RegularMaoXian_216::RegularMaoXian_216(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_216::~RegularMaoXian_216()
{

}

void RegularMaoXian_216::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(16);

	SetLifeCount(3); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3025, 1);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // 玩家死亡


	////////////////////////////////////////////////////////////////////////////
	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);             // trigger 12 触发
	pPointMonster->SetAIGroup(1);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 24);
		pPointTrigger->SetShowID(4002);

		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 24);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3025, 3009), 1);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 24);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3023), 2);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,50);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3045, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(13);             // trigger 13 触发
	//pPointMonster->SetAIGroup(2);

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3045, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(14);             // trigger 14 触发
	//pPointMonster->SetAIGroup(2);

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3040, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetTriggerID(12);             // trigger 14 触发
	//pPointMonster->SetAIGroup(2);

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 14 触发
	//pPointMonster->SetAIGroup(2);

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3014, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);             // trigger 2 触发
	//pPointMonster->SetAIGroup(2);

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);             // trigger 15 触发
	//pPointMonster->SetAIGroup(2);

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3039, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(16);             // trigger 16 触发
	//pPointMonster->SetAIGroup(2);

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(16);             // trigger 16 触发
	//pPointMonster->SetAIGroup(2);

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 12 触发
	//pPointMonster->SetAIGroup(1);

	// 点42
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3003, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(14);             // trigger 14 触发
	//pPointMonster->SetAIGroup(2);

	// 点43
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(16);             // trigger 16 触发
	//pPointMonster->SetAIGroup(2);

	// 点51
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetMax(1);
	pPointMonster->SetLoop(true);
	pPointMonster->SetTriggerID(13);


	// 点61
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetMax(1);
	pPointMonster->SetLoop(true);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发

	// 点62
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3045, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetMax(2);
	pPointMonster->SetLoop(true);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发


	// 点63
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3025, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetQuality(QUALITY_BLUE);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发

	pPointTreasure = CnNew PointTreasure(this,51);
	pPointTreasure->SetShowID(4001);
	pPointTreasure->LinkTrigger(pPointMonster);


	// 点31
	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 40);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	// 点32
	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 41);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}



	// 点33
	pPointBarrel = CnNew PointBarrel(this, 42);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个


	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 42);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}



}


