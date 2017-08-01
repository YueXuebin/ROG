#include "gameholder_pch.h"
#include "regular_maoxian_007.h"
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
RegularMaoXian_007::RegularMaoXian_007(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_007::~RegularMaoXian_007()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_007::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;            // 出怪点
    PointTreasure* pPointTreasure = NULL;           // 宝箱
    PointBarrel* pPointBarrel = NULL;               // 木桶
    PointTouch* pPointTouch = NULL;                 // 采集点
    PointTrigger* pPointTrigger = NULL;             // 触摸石

    /////////////////////////////////////////////////////////////////////////////////
    // 关卡配置
    SetMapLevel(7);

    SetLifeCount(3); // 设置命数

    // 胜利条件
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3307, 1);
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
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3082, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3018, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3082, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3082, 3);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3018, 2);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(4000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 3);           // 出什么怪(怪id, 数量)
	//pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(6000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(1000);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(12);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)



///////////////////////////////////////////////////////////////////////////////////////////////////////////////



	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(2000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(4000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1)

	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3013, 1);           // 出什么怪(怪id, 数量)
	pPointMonster->SetDefaultMovie();             // 出场演绎
	//pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	pPointMonster->SetDelayTime(6000);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);              // trigger 15 触发
	//pPointMonster->SetAIGroup(1) 

	////////////////////////////////////////////////////////////////////////////////////////////

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3307, 1);         // 出什么怪(怪id, 数量)
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质  BLUE  GREEN
	//pPointMonster->SetDelayTime(200);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(500);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 触发
	//pPointMonster->SetAIGroup(1)





////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 点37
	pPointBarrel = CnNew PointBarrel(this, 37);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1
	
	// 点38
	pPointBarrel = CnNew PointBarrel(this, 38);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点39
	pPointBarrel = CnNew PointBarrel(this, 39);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点40
	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	// 点41
	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // 数量1

	 pPointTouch = CnNew PointTouch (this, 50);
	 pPointTouch->SetShowID(4006);



}



