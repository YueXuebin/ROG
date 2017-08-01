#include "gameholder_pch.h"
#include "regular_maoxian_011.h"
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
// 关卡类的构造函数
RegularMaoXian_011::RegularMaoXian_011(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_011::~RegularMaoXian_011()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_011::FirstUpdate()
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
	SetMapLevel(11);

	SetLifeCount(3); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3311, 1);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

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

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3123, 1);         // 出什么怪(怪id, 数量)


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3123, 1);         // 出什么怪(怪id, 数量)


	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3039, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(16);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108,1);         // 出什么怪(怪id, 数量)


	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)


	// 点29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(10);
	pPointMonster->SetMonster(3108, 1);         // 出什么怪(怪id, 数量)



	////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3122, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	pPointMonster->SetAIGroup(30);  
	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3122, 1);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	pPointMonster->SetAIGroup(30);  

	// 点32
	pPointMonster1 = CnNew PointMonster(this, 32);
	pPointMonster1->SetRadius(50);
	pPointMonster1->SetMonster(3124, 1);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	pPointMonster1->SetAIGroup(30);  


	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3064, 4);           // 出什么怪(怪id, 数量)
    pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
    pPointMonster->LinkTrigger(pPointMonster1);   // 触发条件击杀怪物

///////////////////////////////////////////////////////////////////////////////////

	// 点33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);              // trigger 15 触发

	// 点34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(14);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


/////////////////////////////////////////////////////////////////////////////////////////



	// 点37
	pPointMonster = CnNew PointMonster(this, 37);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3018, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)
	// 点38
	pPointMonster = CnNew PointMonster(this, 38);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3018, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


	////////////////////////////////////////////////////////////////////////////

	// 点35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3311, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);            // trigger 13 触发
	//pPointMonster->SetAIGroup(1)

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// 出宝箱
	pPointTreasure = CnNew PointTreasure(this,36);
	pPointTreasure->SetShowID(4001);
	pPointTreasure->LinkTrigger(pPointMonster);


}



