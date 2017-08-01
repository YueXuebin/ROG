#include "gameholder_pch.h"
#include "regular_maoxian_128.h"
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
#include "rog_regular_event_combo_kill.h"
// 关卡类的构造函数
RegularMaoXian_128::RegularMaoXian_128(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_128::~RegularMaoXian_128()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_128::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(28);

	SetLifeCount(0); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
	AddSucceedEvent(pEventMonsterClear); // 清空全部怪

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
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3045, 3);         // 1个巨骷髅

	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(11);             // trigger 3 触发

	pPointTrigger = CnNew PointTrigger(this, 13);        //触摸石
	pPointTrigger->SetShowID(4002);
	pPointTrigger->LinkTrigger(pPointMonster);

	//随机触发

	if (Crown::RandomPercent(50))
	{   

		pPointMonster = CnNew PointMonster(this, 30);
		pPointMonster->SetRadius(300);
		pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
		pPointMonster->LinkTrigger(pPointTrigger);


		pPointMonster = CnNew PointMonster(this, 31);
		pPointMonster->SetRadius(300);
		pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3002), 3);
		pPointMonster->LinkTrigger(pPointTrigger);

	}
	else
	{
		// 出宝箱
		pPointTreasure = CnNew PointTreasure(this,30);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointTrigger);

	}


	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3个

	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointBarrel);

	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3个

	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointBarrel);

	pPointBarrel = CnNew PointBarrel(this, 42);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3个

	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointBarrel);



	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3045, 3);
	//pPointMonster->SetDefaultMovie();
	//pPointMonster->SetTriggerID(7);

	pPointTouch = CnNew PointTouch(this, 18);      // 采集点
	pPointTouch->SetShowID(4003);
	pPointTouch->SetTouchTime(2000);

	////////////////////////////////////////////////////////////////////////////



	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 3);

	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(200);


	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(600);
	pPointMonster->SetSpawnSpeed(400);
	//pPointMonster->SetTriggerID(pPointTrigger->GetTriggerID());

	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3045, 2);

	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(600);
	pPointMonster->SetSpawnSpeed(400);
	//pPointMonster->SetTriggerID(pPointTrigger->GetTriggerID());

	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3019, 3);
	//pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(600);
	//pPointMonster->SetSpawnSpeed(400);
	//pPointMonster->SetTriggerID(pPointTrigger->GetTriggerID());

	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 4);

	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3046, 2);
	pPointMonster->SetQuality(QUALITY_BLUE);


	pPointTreasure = CnNew PointTreasure(this,25);
	pPointTreasure->SetShowID(4001);
	pPointTreasure->LinkTrigger(pPointMonster);


}

