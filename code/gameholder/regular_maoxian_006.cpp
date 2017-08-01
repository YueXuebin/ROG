#include "gameholder_pch.h"
#include "regular_maoxian_006.h"
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

// 关卡类的构造函数
RegularMaoXian_006::RegularMaoXian_006(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_006::~RegularMaoXian_006()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_006::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointMonster* pPointMonster1  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(6);

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
	pEventScore_Timer->SetTime(1000*120);            // 20秒通关获得一星
	AddScoreEvent(pEventScore_Timer);


	////////////////////////////////////////////////////////////////////////////

	//点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3082, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3068, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3082, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发

	////////////////////////////////////////////////////////////////////////////


	//点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3102, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 触发


	//点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 触发


	//点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 触发


	//点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3102, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 触发

	////////////////////////////////////////////////////////////////////////////


	//点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(1);                // AI组


	//点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(1);                // AI组


	//点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3108, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(2);                // AI组


	//点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3066, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(2);                // AI组


	//点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3108, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	////////////////////////////////////////////////////////////////////////////


	//点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发

	////////////////////////////////////////////////////////////////////////////


	//点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(3);                // AI组


	//点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(3);                // AI组


	//点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(3);                // AI组


	//点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(3);                // AI组


	//点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3066, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(4);                // AI组


	//点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3066, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(4);                // AI组


	//点33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3105, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(4);                // AI组

	////////////////////////////////////////////////////////////////////////////

	//点34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(5);                // AI组
	//点34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3103, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(5);                // AI组


	//点35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(6);                // AI组
	//点35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3103, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(6);                // AI组

	////////////////////////////////////////////////////////////////////////////

	//点36
	pPointMonster = CnNew PointMonster(this, 36);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发


	//点37
	pPointMonster = CnNew PointMonster(this, 37);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发

	////////////////////////////////////////////////////////////////////////////BOSS


	





	//点40
	pPointMonster = CnNew PointMonster(this, 40);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(7);                // AI组


	//点41
	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(7);                // AI组


	//点42
	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster->SetAIGroup(7);                // AI组

	//点39
	pPointMonster1 = CnNew PointMonster(this, 39);
	pPointMonster1->SetRadius(100);
	pPointMonster1->SetMonster(3121, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 触发
	pPointMonster1->SetAIGroup(7);                // AI组

	//点38
	pPointMonster = CnNew PointMonster(this, 38);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3306, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
    pPointMonster->SetDefaultMovie();            // 出场演绎
	pPointMonster->LinkTrigger(pPointMonster1);
}

