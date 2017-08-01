#include "gameholder_pch.h"
#include "regular_maoxian_154.h"
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
#include "rog_regular_event_combo_kill.h"
RegularMaoXian_154::RegularMaoXian_154(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_154::~RegularMaoXian_154()
{

}

void RegularMaoXian_154::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;     // 怪物
	PointTreasure* pPointTreasure = NULL;    // 宝箱
	PointBarrel* pPointBarrel = NULL;        // 木桶
	PointTouch* pPointTouch = NULL;          // 采集点
	PointTrigger* pPointTrigger = NULL;      // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(54);

	SetLifeCount(0); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3010, 1);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

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
	////////////////////////////////////////////////////////////////////////////
	//点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3004, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(2500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms 
	pPointMonster->SetTriggerID(11);              // trigger 11 触发
	pPointMonster->SetAIGroup(2);

	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);            // trigger 11 触发
	pPointMonster->SetAIGroup(2);


	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001,4);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(100);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                                             // trigger 12 触发

	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3003,3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(2000);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                                             // trigger 12 触发



	// 点24 出宝箱
	pPointTreasure = CnNew PointTreasure(this, 24);                      
	pPointTreasure->SetShowID(4001);                                    //宝箱外形
	pPointTreasure->LinkTrigger(pPointMonster);                         //触发条件  结束 17


	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 4);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(300);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	//pPointMonster->SetTriggerID(13);

	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3012,2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(300);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);





	// 点20 触摸石
	if (Crown::RandomPercent(100))
	{   
		pPointTrigger = CnNew PointTrigger(this, 20);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster);


		if (Crown::RandomPercent(70))
		{   

			pPointMonster = CnNew PointMonster(this, 26);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
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




	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3014,3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(300);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);

	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3012,2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();                                          // 出场演绎
	pPointMonster->SetDelayTime(3000);                                            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                                           // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);

	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3015, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3015, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3010, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	// 出宝箱
	pPointTreasure = CnNew PointTreasure(this,18);
	pPointTreasure->SetShowID(4001);
	pPointTreasure->LinkTrigger(pPointMonster);





	//////////////////////////////////////////////////////////////////////////////
	// 点13
	pPointBarrel = CnNew PointBarrel(this, 13);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1


	// 点14
	pPointBarrel = CnNew PointBarrel(this, 14);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1


	// 点15
	pPointBarrel = CnNew PointBarrel(this, 15);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1


	// 点21
	pPointBarrel = CnNew PointBarrel(this, 21);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1


	// 点22
	pPointBarrel = CnNew PointBarrel(this, 22);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1


	// 点23
	pPointBarrel = CnNew PointBarrel(this, 23);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1





}

