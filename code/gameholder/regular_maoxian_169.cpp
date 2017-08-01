#include "gameholder_pch.h"
#include "regular_maoxian_169.h"
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

RegularMaoXian_169::RegularMaoXian_169(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_169::~RegularMaoXian_169()
{

}

void RegularMaoXian_169::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(69);

	SetLifeCount(0); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3024, 1);
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
	///////////////////////////////////////////////////////////////////////////////// 01
	// 点11
	pPointMonster = CnNew PointMonster(this,11);
	pPointMonster->SetRadius(200);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3053,2);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(50);                // 出怪间隔300ms

	// 点12
	pPointMonster = CnNew PointMonster(this,12);
	pPointMonster->SetRadius(200);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3013,3);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(50);                // 出怪间隔300ms

	//点17
	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(1000);                  // 延时出怪
	pPointMonster->SetMonster(3019,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发




	///////////////////////////////////////////////////////////////////////////////// 02
	//点13
	pPointMonster = CnNew PointMonster(this,13);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                  // 延时出怪
	pPointMonster->SetMonster(3002,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发


	//点14
	pPointMonster = CnNew PointMonster(this,14);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                  // 延时出怪
	pPointMonster->SetMonster(3002,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发

	//点15
	pPointMonster = CnNew PointMonster(this,15);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(1000);                  // 延时出怪
	pPointMonster->SetMonster(3002,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发

	//点16
	pPointMonster = CnNew PointMonster(this,16);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(1500);                  // 延时出怪
	pPointMonster->SetMonster(3002,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发




	///////////////////////////////////////////////////////////////////////////////// 03


	//点18
	pPointMonster = CnNew PointMonster(this,18);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3035,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发

	//点19
	pPointMonster = CnNew PointMonster(this,19);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3016,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发

	//点20
	pPointMonster = CnNew PointMonster(this,20);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3016,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发


	///////////////////////////////////////////////////////////////////////////////// 04

	//点21
	pPointMonster = CnNew PointMonster(this,21);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	//pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	//点22
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(600);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	//pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	//点23
	pPointMonster = CnNew PointMonster(this,23);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(1100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	//pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	//点24
	pPointMonster = CnNew PointMonster(this,24);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(2000);                  // 延时出怪
	pPointMonster->SetMonster(3044,1);                // 怪物类型 数量
	pPointMonster->SetQuality(QUALITY_GREEN);
	//pPointMonster->SetDefaultMovie();                 // 出场演绎
	//pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	//pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 25);         // 宝箱
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);
	}


	///////////////////////////////////////////////////////////////////////////////// 05


	//点31
	pPointMonster = CnNew PointMonster(this,31);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3052,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(11);

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 34);         // 宝箱
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);
	}


	//点32
	pPointMonster = CnNew PointMonster(this,32);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3015,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(11);

	//点33
	pPointMonster = CnNew PointMonster(this,33);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3015,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(11);




	///////////////////////////////////////////////////////////////////////////////// 06


	//点35
	pPointMonster = CnNew PointMonster(this,35);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(30);

	//点36
	pPointMonster = CnNew PointMonster(this,36);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(30);

	//点42
	pPointMonster = CnNew PointMonster(this,42);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3041,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(30);

	///////////////////////////////////////////////////////////////////////////////// 07


	//点37
	pPointMonster = CnNew PointMonster(this,37);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(40);
	//点42
	pPointMonster = CnNew PointMonster(this,42);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3015,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(40);
	//点43
	pPointMonster = CnNew PointMonster(this,43);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3041,5);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(18);                  // Trigger点事件触发
	pPointMonster->SetAIGroup(40);

	///////////////////////////////////////////////////////////////////////////////// 08

	//点38
	pPointMonster = CnNew PointMonster(this,38);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3051,1);                // 怪物类型 数量
	pPointMonster->SetTriggerID(19);                  // Trigger点事件触发
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	//点39
	pPointMonster = CnNew PointMonster(this,39);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3051,1);                // 怪物类型 数量
	pPointMonster->SetTriggerID(19);                  // Trigger点事件触发
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms




	///////////////////////////////////////////////////////////////////////////////// BOSS




	//点40
	pPointMonster = CnNew PointMonster(this,40);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                  // 延时出怪
	pPointMonster->SetMonster(3024,1);                // 怪物类型 数量
	pPointMonster->SetTriggerID(19);                  // Trigger点事件触发
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms

	// 点41
	pPointTreasure = CnNew PointTreasure(this, 41);                      
	pPointTreasure->SetShowID(4001);                                    //宝箱外形
	pPointTreasure->LinkTrigger(pPointMonster);                         //触发条件  结束 17


	///////////////////////////////////////////////////////////////////////////////// 采集点

	//点26
	pPointTouch = CnNew PointTouch(this,26);
	pPointTouch->SetShowID(4003);

	//点27
	pPointTouch = CnNew PointTouch(this,27);
	pPointTouch->SetShowID(4003);

	//点28
	pPointTouch = CnNew PointTouch(this,28);
	pPointTouch->SetShowID(4003);

	//点29
	pPointTouch = CnNew PointTouch(this,29);
	pPointTouch->SetShowID(4003);

	//点30
	pPointTouch = CnNew PointTouch(this,30);
	pPointTouch->SetShowID(4003);


}

