#include "gameholder_pch.h"
#include "regular_maoxian_020.h"
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

RegularMaoXian_020::RegularMaoXian_020(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_020::~RegularMaoXian_020()
{

}

void RegularMaoXian_020::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(20);

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
	pEventScore_Timer->SetTime(1000*180);            // 20秒通关获得一星
	AddScoreEvent(pEventScore_Timer);


	/////////////////////////////////////////////////////////////////////////////////
	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3051, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);             // trigger 2 触发
	pPointMonster->SetAIGroup(1);

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3002, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);             // trigger 2 触发
	pPointMonster->SetAIGroup(1);

	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);



	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3039, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);             // trigger 2 触发
	pPointMonster->SetAIGroup(2);

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3009, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetQuality(QUALITY_GREEN);
	//pPointMonster->SetTriggerID(13);             // trigger 3 触发

	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001,5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 3 触发

	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001,5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 3 触发

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3051, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发

	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);             // trigger 4 触发

	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms



	// 点27
	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 27);
		pPointTrigger->SetShowID(4002);

		//随机触发
		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 29);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 32);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3031), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,29);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}




	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3320, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);
	//pPointMonster->SetDefaultMovie();
	pPointMonster->SetTriggerID(14);




}
