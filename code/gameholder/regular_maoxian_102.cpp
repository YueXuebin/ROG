#include "gameholder_pch.h"
#include "regular_maoxian_102.h"
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
#include "rog_regular_event_combo_kill.h"

RegularMaoXian_102::RegularMaoXian_102(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_102::~RegularMaoXian_102()
{

}

void RegularMaoXian_102::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(2);

	SetLifeCount(0); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
	AddSucceedEvent(pEventMonsterClear); // 清空全部怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // 玩家死亡

	ROGRegularEvent_Timer* pEventFailed_Timer = CnNew ROGRegularEvent_Timer(this);
	pEventFailed_Timer->SetTime(1000*300);            // n秒限时
	AddFailedEvent(pEventFailed_Timer);

	// 评价条件
	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*200);            // n秒通关获得一星
	AddScoreEvent(pEventScore_Timer);

	ROGRegularEvent_ComboKill* pEventScore_ComboKill = CnNew ROGRegularEvent_ComboKill(this);
	pEventScore_ComboKill->SetComboKill(5);         // n连杀
	AddScoreEvent(pEventScore_ComboKill);
	/////////////////////////////////////////////////////////////////////////////////
	// 点72
	pPointMonster = CnNew PointMonster(this, 72);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3016, Crown::SDRandom(4,10));         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(10);             // trigger 10 触发
	pPointMonster->SetAIGroup(1);

	// 点12
	pPointMonster = CnNew PointMonster(this, 74);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(10);             // trigger 10 触发
	pPointMonster->SetAIGroup(1);


	// 点13
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发
	pPointMonster->SetAIGroup(2);

	// 点14
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发
	pPointMonster->SetAIGroup(2);

	// 点15
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3002, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 触发
	pPointMonster->SetAIGroup(2);

	pPointBarrel = CnNew PointBarrel(this, 15);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 15);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 70);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 70);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 73);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 73);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}


	// 点22
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3041,2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);             // trigger 13 触发

	// 点23
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3001, 5);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);             // trigger 13 触发

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 32);         // 宝箱
		pPointTreasure->SetShowID(4001);
	}


	// 点25
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3016, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3041, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointBarrel = CnNew PointBarrel(this, 36);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 36);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 43);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 43);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointMonster = CnNew PointMonster(this, 77);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 54);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 55);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 56);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 62);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 78);
		pPointTrigger->SetShowID(4002);

		//随机触发
		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 80);
			pPointMonster->SetRadius(150);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 80);
			pPointMonster->SetRadius(150);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3002), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,80);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}

	pPointMonster = CnNew PointMonster(this, 61);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	pPointMonster = CnNew PointMonster(this, 75);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);             // trigger 13 触发
	pPointMonster->SetAIGroup(12);

	pPointMonster = CnNew PointMonster(this, 76);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1000);           // 触发后延时1000ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);             // trigger 13 触发
	pPointMonster->SetAIGroup(12);


	// 点31
	pPointMonster = CnNew PointMonster(this, 71);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3021, 1);
	//pPointMonster->SetDefaultMovie();
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetTriggerID(18);
	pPointMonster->SetAIGroup(12);

	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,71);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);

	}




}
