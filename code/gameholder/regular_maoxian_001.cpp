#include "gameholder_pch.h"
#include "regular_maoxian_001.h"
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
#include "rog_regular_event_monster_clear.h"  //怪物清空
#include "rog_regular_event_player_dead.h"
#include "rog_regular_event_timer.h"


RegularMaoXian_001::RegularMaoXian_001(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_001::~RegularMaoXian_001()
{

}

void RegularMaoXian_001::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(02);

	SetLifeCount(3); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterClear* pEventSucceed_MonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
    AddSucceedEvent(pEventSucceed_MonsterClear); // 清空全部怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventFailed_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this);       // 玩家死亡
	AddFailedEvent(pEventFailed_PlayerDead);

    // 评价条件
    ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // 一条命不死获得一星
    AddScoreEvent(pEventScore_PlayerDead);

    ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
    pEventScore_Timer->SetTime(1000*120);            // 20秒通关获得一星
    AddScoreEvent(pEventScore_Timer);


	/////////////////////////////////////////////////////////////////////////////////
	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(200);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3018, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);             // trigger 2 触发


	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3019, 4);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
    pPointMonster->SetTriggerID(15);             // trigger 2 触发 


	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3082, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发 

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3082, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发 

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3027, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);             // trigger 2 触发 

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3019, 4);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(17);             // trigger 2 触发 

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);             // trigger 2 触发 

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(19);             // trigger 2 触发 


	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(1500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(20);             // trigger 2 触发 

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(1500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(20);             // trigger 2 触发

	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3001, 2);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(3000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(20);             // trigger 2 触发




	// 点31
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3301, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetTriggerID(18);

	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);
	//pPointMonster->SetMax(2);
	//pPointMonster->SetLoop(true);
	pPointMonster->SetTriggerID(18);

	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);
	//pPointMonster->SetMax(2);
	//pPointMonster->SetLoop(true);
	pPointMonster->SetTriggerID(18);

	pPointTouch = CnNew PointTouch (this, 15);
	pPointTouch->SetShowID(4006);


	// 点46
	pPointMonster = CnNew PointMonster(this, 46);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3801, 1);         // npc艾瑞贝丝
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetAttackable(false);

	//pPointMonster = CnNew PointMonster(this, 46);
	//pPointMonster->SetRadius(0);
	//pPointMonster->SetMonster(3101, 1);

	pPointTouch = CnNew PointTouch (this, 46);
	pPointTouch->SetShowID(4007);



}

