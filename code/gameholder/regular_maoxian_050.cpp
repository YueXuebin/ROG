#include "gameholder_pch.h"
#include "regular_maoxian_050.h"
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
RegularMaoXian_050::RegularMaoXian_050(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_050::~RegularMaoXian_050()
{

}

void RegularMaoXian_050::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointMonster* pPointMonster11  = NULL;            // 出怪点
	PointMonster* pPointMonster12  = NULL;            // 出怪点
	PointMonster* pPointMonster13  = NULL;            // 出怪点
	PointMonster* pPointMonster14  = NULL;            // 出怪点
	PointMonster* pPointMonster15  = NULL;            // 出怪点
	PointMonster* pPointMonster16  = NULL;            // 出怪点
	PointMonster* pPointMonster17  = NULL;            // 出怪点
	PointMonster* pPointMonster18  = NULL;            // 出怪点
	PointMonster* pPointMonster19  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(50);

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
	// 点10
	pPointMonster11 = CnNew PointMonster(this, 11);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3004, 6);         // 出什么怪(怪id, 数量)
	pPointMonster11->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster11->SetDefaultMovie();
	pPointMonster11->SetTriggerID(13);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 11);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster11);

		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 11);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 11);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3001), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,21);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}

	///////////点12
	pPointMonster12 = CnNew PointMonster(this, 12);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3004, 6);         // 出什么怪(怪id, 数量)
	pPointMonster12->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster12->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointMonster11);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 12);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster12);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 12);
			pPointMonster->SetRadius(300);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 12);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3009), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,22);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	/////////////////////点13
	pPointMonster13 = CnNew PointMonster(this, 13);
	pPointMonster13->SetRadius(300);
	pPointMonster13->SetMonster(3015, 6);         // 出什么怪(怪id, 数量)
	pPointMonster13->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster13->SetDefaultMovie();
	pPointMonster13->LinkTrigger(pPointMonster12);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 13);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster13);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 13);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 13);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3015), 4);
			pPointMonster->LinkTrigger(pPointTrigger);
		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,23);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////点14
	pPointMonster14 = CnNew PointMonster(this, 14);
	pPointMonster14->SetRadius(300);
	pPointMonster14->SetMonster(3018, 6);         // 出什么怪(怪id, 数量)
	pPointMonster14->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster14->SetDefaultMovie();
	pPointMonster14->LinkTrigger(pPointMonster13);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 14);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster14);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 14);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3036, 3027), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 14);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3023, 3039), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,24);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);
		}

	}

	///////////点15
	pPointMonster15 = CnNew PointMonster(this, 15);
	pPointMonster15->SetRadius(300);
	pPointMonster15->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster15->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster15->SetDefaultMovie();
	pPointMonster15->LinkTrigger(pPointMonster14);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 15);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster15);

		if (Crown::RandomPercent(70))
		{   

			pPointMonster = CnNew PointMonster(this, 15);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 15);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3051), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,25);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);
		}

	}

	///////////点16
	pPointMonster16 = CnNew PointMonster(this, 16);
	pPointMonster16->SetRadius(300);
	pPointMonster16->SetMonster(3018, 6);         // 出什么怪(怪id, 数量)
	pPointMonster16->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster16->SetDefaultMovie();
	pPointMonster16->LinkTrigger(pPointMonster15);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 16);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster16);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 16);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 16);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3051), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,26);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////点17
	pPointMonster17 = CnNew PointMonster(this, 17);
	pPointMonster17->SetRadius(300);
	pPointMonster17->SetMonster(3001, 6);         // 出什么怪(怪id, 数量)
	pPointMonster17->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster17->SetDefaultMovie();
	pPointMonster17->LinkTrigger(pPointMonster16);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 17);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster17);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 17);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3033, 3034), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 17);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3023, 3039), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,27);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////点18
	pPointMonster18 = CnNew PointMonster(this, 18);
	pPointMonster18->SetRadius(300);
	pPointMonster18->SetMonster(3039, 6);         // 出什么怪(怪id, 数量)
	pPointMonster18->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster18->SetDefaultMovie();
	pPointMonster18->LinkTrigger(pPointMonster17);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 18);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster18);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 18);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 18);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3051, 3001), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,28);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////点19
	pPointMonster19 = CnNew PointMonster(this, 19);
	pPointMonster19->SetRadius(300);
	pPointMonster19->SetMonster(3024, 1);         // 出什么怪(怪id, 数量)
	pPointMonster19->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster16->SetDefaultMovie();
	pPointMonster19->SetQuality(QUALITY_GREEN);
	pPointMonster19->LinkTrigger(pPointMonster18);

	pPointMonster11 = CnNew PointMonster(this, 11);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3049, 1);         // 出什么怪(怪id, 数量)
	pPointMonster11->SetQuality(QUALITY_GREEN);
	pPointMonster11->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster11->SetDefaultMovie();
	pPointMonster11->LinkTrigger(pPointMonster19);

	pPointMonster12 = CnNew PointMonster(this, 12);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3021, 1);         // 出什么怪(怪id, 数量)
	pPointMonster12->SetQuality(QUALITY_GREEN);
	pPointMonster12->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster11->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointMonster11);



	// 点20
	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,20);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster12);

	}

}

