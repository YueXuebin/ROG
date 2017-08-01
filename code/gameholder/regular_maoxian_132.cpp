#include "gameholder_pch.h"
#include "regular_maoxian_132.h"
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

// 关卡类的构造函数
RegularMaoXian_132::RegularMaoXian_132(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_132::~RegularMaoXian_132()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_132::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointMonster* pPointMonster11  = NULL;            // 出怪点
	PointMonster* pPointMonster12  = NULL;            // 出怪点
	PointMonster* pPointMonster13  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(32);

	SetLifeCount(0); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3049, 1);
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
	////////////////////////////////////////////////////////////////////////////01


	//点11
	pPointMonster = CnNew PointMonster(this,11);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3048,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(50);                // 出怪间隔300ms


	//点12
	pPointMonster = CnNew PointMonster(this,12);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3011,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms



	//点13
	pPointMonster = CnNew PointMonster(this,13);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3043,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms


	//点14
	pPointMonster = CnNew PointMonster(this,14);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3011,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms


	//点15
	pPointMonster = CnNew PointMonster(this,15);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3043,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms

	////////////////////////////////////////////////////////////////////////////////////////////02


	//点16
	// 出宝箱
	pPointTreasure = CnNew PointTreasure(this,16);
	pPointTreasure->SetShowID(4001);


	//点17
	pPointMonster11 = CnNew PointMonster(this,17);
	pPointMonster11->SetRadius(300);                    // 区域
	pPointMonster11->SetDelayTime(50);                  // 延时出怪
	pPointMonster11->SetMonster(3001,2);                // 怪物类型 数量
	pPointMonster11->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster11->SetTriggerID(14);                  // Trigger点事件触发

	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(1000);                // 延时出怪
	pPointMonster->SetMonster(3001,4);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster11);


	//点18
	pPointMonster = CnNew PointMonster(this,18);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(100);                 // 延时出怪
	pPointMonster->SetMonster(3040,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster11);


	////////////////////////////////////////////////////////////////////////////////////////////03


	//点19
	pPointMonster12 = CnNew PointMonster(this,19);
	pPointMonster12->SetRadius(200);                    // 区域
	pPointMonster12->SetDelayTime(500);                // 延时出怪
	pPointMonster12->SetMonster(3014,2);                // 怪物类型 数量
	pPointMonster12->SetSpawnSpeed(300);                // 出怪间隔300ms

	//点36
	pPointMonster = CnNew PointMonster(this,36);
	pPointMonster->SetRadius(200);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3001,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(1000);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster12);

	//点37
	pPointMonster = CnNew PointMonster(this,37);
	pPointMonster->SetRadius(200);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3003,2);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster12);

	////////////////////////////////////////////////////////////////////////////////////////////04





	//点20
	pPointBarrel = CnNew PointBarrel(this, 20);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1

	pPointMonster = CnNew PointMonster(this,20);
	pPointMonster->SetRadius(50);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3001,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointBarrel);


	//点21
	pPointBarrel = CnNew PointBarrel(this, 21);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1

	pPointMonster = CnNew PointMonster(this,21);
	pPointMonster->SetRadius(50);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3001,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointBarrel);

	//点43
	pPointBarrel = CnNew PointBarrel(this, 43);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1

	pPointMonster = CnNew PointMonster(this,43);
	pPointMonster->SetRadius(50);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3001,1);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointBarrel);




	////////////////////////////////////////////////////////////////////////////////////////////05
	//点22
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3012,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发


	//点23
	pPointMonster = CnNew PointMonster(this,23);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3012,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发


	//点24
	pPointMonster = CnNew PointMonster(this,24);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3012,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发


	//点25
	pPointMonster = CnNew PointMonster(this,25);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3012,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetTriggerID(13);                  // Trigger点事件触发

	///////////////////////////////////////////////////////////////////////////////////////////06



	//点26
	pPointMonster = CnNew PointMonster(this,26);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(50);                  // 延时出怪
	pPointMonster->SetMonster(3001,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发
	//点38
	pPointMonster = CnNew PointMonster(this,38);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(2000);                // 延时出怪
	pPointMonster->SetMonster(3001,4);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(500);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发
	//点39
	pPointMonster = CnNew PointMonster(this,39);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(5000);                // 延时出怪
	pPointMonster->SetMonster(3039,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(500);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发



	//点40
	pPointMonster = CnNew PointMonster(this,40);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(50);                  // 延时出怪
	pPointMonster->SetMonster(3039,2);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
	//点41
	pPointMonster = CnNew PointMonster(this,41);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(1000);                // 延时出怪
	pPointMonster->SetMonster(3001,3);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
	//点42
	pPointMonster = CnNew PointMonster(this,42);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(1500);                // 延时出怪
	pPointMonster->SetMonster(3039,2);                // 怪物类型 数量
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发





	///////////////////////////////////////////////////////////////////////////////////////////07








	//点27
	pPointMonster = CnNew PointMonster(this,27);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3030,1);                // 怪物类型 数量
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);                  // Trigger点事件触发

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 27);         // 宝箱
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);
	}





	//点28
	pPointMonster = CnNew PointMonster(this,28);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(50);                // 延时出怪
	pPointMonster->SetMonster(3023,2);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);                  // Trigger点事件触发



	//点29
	pPointMonster = CnNew PointMonster(this,29);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(4000);                // 延时出怪
	pPointMonster->SetMonster(3023,2);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster);





	///////////////////////////////////////////////////////////////////////////////////////////08

	//点30
	pPointMonster = CnNew PointMonster(this,30);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3048,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms


	//点31
	pPointMonster = CnNew PointMonster(this,31);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(500);                // 延时出怪
	pPointMonster->SetMonster(3043,2);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms



	//////////////////////////////////////////////////////////////////////////////////////////09




	//点32
	pPointMonster = CnNew PointMonster(this,32);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(300);                // 延时出怪
	pPointMonster->SetMonster(3047,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);                  // Trigger点事件触发


	//点33
	pPointMonster = CnNew PointMonster(this,33);
	pPointMonster->SetRadius(300);                    // 区域
	pPointMonster->SetDelayTime(300);                // 延时出怪
	pPointMonster->SetMonster(3047,1);                // 怪物类型 数量
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);                  // Trigger点事件触发

	//点34
	pPointMonster13 = CnNew PointMonster(this,34);
	pPointMonster13->SetRadius(300);                    // 区域
	pPointMonster13->SetDelayTime(100);                // 延时出怪
	pPointMonster13->SetMonster(3038,1);                // 怪物类型 数量
	pPointMonster13->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster13->SetTriggerID(17);                  // Trigger点事件触发


	//////////////////////////////////////////////////////////////////////////////////////BOSS


	//点35
	pPointMonster = CnNew PointMonster(this,35);
	pPointMonster->SetRadius(100);                    // 区域
	pPointMonster->SetDelayTime(2000);                // 延时出怪
	pPointMonster->SetMonster(3049,1);                // 怪物类型 数量
	pPointMonster->SetQuality(QUALITY_BLUE);          // 怪物品质
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster13);



	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 44);         // 宝箱
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);
	}











}
