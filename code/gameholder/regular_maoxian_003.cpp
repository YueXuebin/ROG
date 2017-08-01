#include "gameholder_pch.h"
#include "regular_maoxian_003.h"
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

RegularMaoXian_003::RegularMaoXian_003(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_003::~RegularMaoXian_003()
{

}

void RegularMaoXian_003::FirstUpdate()
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
	PointTrigger* pPointTrigger11 = NULL;             // 触摸石
	PointTrigger* pPointTrigger12 = NULL;             // 触摸石
	PointTrigger* pPointTrigger13 = NULL;             // 触摸石
	PointTrigger* pPointTrigger14 = NULL;             // 触摸石
	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(3);

	SetLifeCount(3); // 设置命数
	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3303, 1);
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

	/////////////////////////////////////////////////////////////////////////////////

	pPointMonster16= CnNew PointMonster(this, 23);
	pPointMonster16->SetRadius(400);
	pPointMonster16->SetMonster(3013, 4);

	pPointMonster18= CnNew PointMonster(this, 24);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组

	pPointMonster18= CnNew PointMonster(this, 25);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组

	pPointMonster18= CnNew PointMonster(this, 26);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组

	pPointMonster18= CnNew PointMonster(this, 27);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组

	pPointMonster18= CnNew PointMonster(this, 28);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组

	pPointMonster18= CnNew PointMonster(this, 29);
	pPointMonster18->SetRadius(10);
	pPointMonster18->SetMonster(3125, 1);
	pPointMonster18->SetAIGroup(15);                // AI组











	// 触摸石11
	pPointTrigger11 = CnNew PointTrigger(this, 11);
	pPointTrigger11->SetShowID(4002);
	pPointTrigger11->LinkTrigger(pPointMonster16);

	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3083,1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger11);

	pPointMonster11 = CnNew PointMonster(this, 16);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3013, 2);
	pPointMonster11->SetDefaultMovie();
	pPointMonster11->LinkTrigger(pPointTrigger11);

	// 触摸石12
	pPointTrigger12 = CnNew PointTrigger(this, 12);
	pPointTrigger12->SetShowID(4002);
	pPointTrigger12->LinkTrigger(pPointMonster11);

	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3032, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger12);

	pPointMonster12 = CnNew PointMonster(this, 20);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3013, 2);
	pPointMonster12->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointTrigger12);

	// 触摸石13
	pPointTrigger13 = CnNew PointTrigger(this, 13);
	pPointTrigger13->SetShowID(4002);
	pPointTrigger13->LinkTrigger(pPointMonster12);

	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3036, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger13);

	pPointMonster13 = CnNew PointMonster(this, 22);
	pPointMonster13->SetRadius(300);
	pPointMonster13->SetMonster(3013, 2);
	pPointMonster13->SetDefaultMovie();
	pPointMonster13->LinkTrigger(pPointTrigger13);


	// 触摸石14
	pPointTrigger14 = CnNew PointTrigger(this, 14);
	pPointTrigger14->SetShowID(4002);
	pPointTrigger14->LinkTrigger(pPointMonster13);

	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3037, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger14);

	pPointMonster14 = CnNew PointMonster(this, 18);
	pPointMonster14->SetRadius(300);
	pPointMonster14->SetMonster(3013, 2);
	pPointMonster14->SetDefaultMovie();
	pPointMonster14->LinkTrigger(pPointTrigger14);


	// boss

	pPointMonster15 = CnNew PointMonster(this, 23);
	pPointMonster15->SetRadius(0);
	pPointMonster15->SetMonster(3076, 1);
	pPointMonster15->SetDefaultMovie();
	pPointMonster15->LinkTrigger(pPointMonster14);
	pPointMonster15->SetAIGroup(15);                // AI组



	pPointMonster17 = CnNew PointMonster(this, 23);
	pPointMonster17->SetRadius(0);
	pPointMonster17->SetMonster(3303, 1);
	pPointMonster17->SetDefaultMovie();
	pPointMonster17->SetQuality(QUALITY_GREEN);
	pPointMonster17->LinkTrigger(pPointMonster15);
}

