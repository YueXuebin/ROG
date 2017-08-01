#include "gameholder_pch.h"
#include "regular_maoxian_225.h"
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


RegularMaoXian_225::RegularMaoXian_225(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_225::~RegularMaoXian_225()
{

}

void RegularMaoXian_225::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(25);

	SetLifeCount(3); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3049, 1);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // 玩家死亡


	/////////////////////////////////////////////////////////////////////////////////
	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(200);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);

	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3023, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(200);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);



	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);             // trigger 2 触发



	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3015, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发


	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3002, 6);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 2 触发
	//pPointMonster->SetAIGroup(2);

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3039, 6);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3023, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3003, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(3);

	// 点21
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(3);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(13);             // trigger 3 触发


	// 点22
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3015,5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);             // trigger 3 触发

	// 点23
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3039, 4);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(4);

	// 点23
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3002, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(4);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 44);
		pPointTrigger->SetShowID(4002);

		//随机触发
		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 44);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 44);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3031), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,45);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}



	// 点25
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3039, 5);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(14);             // trigger 4 触发




	// 点29
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(4);


	// 点32
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3003, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(4);

	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3009, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetAIGroup(5);

	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(5);

	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)


	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3040, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetAIGroup(6);

	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetAIGroup(6);








	// 点31
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3049, 1);
	//pPointMonster->SetDefaultMovie();
	pPointMonster->SetQuality(QUALITY_BLUE);
	pPointMonster->SetTriggerID(18);

	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,30);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);

	}

	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3015, 4);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetMax(2);
	//pPointMonster->SetLoop(true);
	pPointMonster->SetTriggerID(18);

	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 4);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetMax(2);
	//pPointMonster->SetLoop(true);
	pPointMonster->SetTriggerID(18);

	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 40);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 41);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3039, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 42);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 42);
		pPointMonster->SetRadius(50);
		pPointMonster->SetMonster(3039, 2);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 43);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 43);
		pPointMonster->SetRadius(50);
		pPointMonster->SetMonster(3039, 2);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}





}

