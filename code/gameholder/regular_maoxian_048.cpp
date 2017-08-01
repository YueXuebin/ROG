#include "gameholder_pch.h"
#include "regular_maoxian_048.h"
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

RegularMaoXian_048::RegularMaoXian_048(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_048::~RegularMaoXian_048()
{

}

void RegularMaoXian_048::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;
    PointTrigger* pPointTrigger11 = NULL;             // 触摸石
    PointTrigger* pPointTrigger12 = NULL;             // 触摸石
    PointTrigger* pPointTrigger = NULL;             // 触摸石
    /////////////////////////////////////////////////////////////////////////////////
    // 关卡配置
    SetMapLevel(48);

    SetLifeCount(3); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3020, 1);
    AddSucceedEvent(pEventMonsterDead); // 杀死怪


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
	
	//点11
	pPointMonster = CnNew PointMonster(this,11);
    pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(2000);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	//点12
	pPointMonster = CnNew PointMonster(this,12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(2000);
	pPointMonster->SetMonster(3012,2);
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	
	//点13
	pPointMonster = CnNew PointMonster(this,13);
	pPointMonster->SetRadius(300);
   pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetTriggerID(11);
	
	//点14
	pPointTouch = CnNew PointTouch(this,14);
	pPointTouch->SetShowID(4003);
	
	//点15
	pPointTouch = CnNew PointTouch(this,15);
	pPointTouch->SetShowID(4003);
	
	//点16
	pPointTouch = CnNew PointTouch(this,16);
	pPointTouch->SetShowID(4003);

	//点17
	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(300);
   pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(100);                               // 出怪间隔300ms
	pPointMonster->SetAIGroup(11);
	pPointMonster->SetTriggerID(12);

	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetDelayTime(2000);
	pPointMonster->SetMonster(3028,1);
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetSpawnSpeed(300);                                // 出怪间隔300ms
	pPointMonster->SetAIGroup(11);
	pPointMonster->SetTriggerID(12);


	//点18
	pPointMonster = CnNew PointMonster(this,18);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);                                // 出怪间隔300ms
	//pPointMonster->SetAIGroup(21);
	//pPointMonster->SetTriggerID(13);

	//点19
	pPointMonster = CnNew PointMonster(this,19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //吸血鬼                   
	pPointMonster->SetSpawnSpeed(300);                                 // 出怪间隔300ms
	//pPointMonster->SetAIGroup(21);
	//pPointMonster->SetTriggerID(13);

	//点20
	pPointTouch = CnNew PointTouch(this,20);
	pPointTouch->SetShowID(4003);

	//点25
	pPointMonster = CnNew PointMonster(this,25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(100);
	pPointMonster->SetMonster(3012,2);
	pPointMonster->SetSpawnSpeed(300);                                  // 出怪间隔300ms
	//pPointMonster->SetAIGroup(21);
	pPointMonster->SetTriggerID(14);

	//点24
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(150);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);                                  // 出怪间隔300ms

	//点22
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);                                  // 出怪间隔300ms

	// 点23 出宝箱
	pPointTreasure = CnNew PointTreasure(this, 23);                      
	pPointTreasure->SetShowID(4001);                                    //宝箱外形
	pPointTreasure->LinkTrigger(pPointMonster);                         //触发条件  结束 17

	//点21
	pPointTouch = CnNew PointTouch(this,21);
	pPointTouch->SetShowID(4003);

	//点26
	pPointMonster = CnNew PointMonster(this,26);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3014), 2);
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	//点27
	pPointMonster = CnNew PointMonster(this,27);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	//点28
	pPointMonster = CnNew PointMonster(this,28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms



	// 点29 触摸石
	if (Crown::RandomPercent(100))
	{   
		pPointTrigger = CnNew PointTrigger(this, 29);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster);


		if (Crown::RandomPercent(70))
		{   

			pPointMonster = CnNew PointMonster(this, 26);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3028, 3034), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
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


    //点30
	pPointMonster = CnNew PointMonster(this,30);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetSpawnSpeed(2000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);

	//点31
	pPointMonster = CnNew PointMonster(this,31);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
    pPointMonster->SetMonster(3014,2);   //吸血鬼
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetSpawnSpeed(2000);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(15);

	//点99
	pPointMonster = CnNew PointMonster(this,99);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetMonster(3020,1);
	pPointMonster->SetQuality(QUALITY_BLUE);
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms

	// 点99 出宝箱
	pPointTreasure = CnNew PointTreasure(this, 98);                      
	pPointTreasure->SetShowID(4001);                                    //宝箱外形
	pPointTreasure->LinkTrigger(pPointMonster);                         //触发条件  结束 17



           
}

