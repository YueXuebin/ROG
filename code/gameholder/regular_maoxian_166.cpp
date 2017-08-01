#include "gameholder_pch.h"
#include "regular_maoxian_166.h"
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
RegularMaoXian_166::RegularMaoXian_166(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_166::~RegularMaoXian_166()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_166::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(66);

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
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3025, 1);         // 1个精英狼人
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetTriggerID(15);

	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3023, 1);
	//pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	//pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(15);             // trigger 13 触发

	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 2);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(1000);
	//pPointMonster->SetSpawnSpeed(300);
	pPointMonster->SetTriggerID(15);

	pPointBarrel = CnNew PointBarrel(this, 51);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 51);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 52);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 52);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 53);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 1个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 53);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}




	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 45);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 2);
	//pPointMonster->SetDefaultMovie();
	//pPointMonster->SetTriggerID(17);

	pPointTouch = CnNew PointTouch(this, 43);      // 采集点
	pPointTouch->SetShowID(4004);
	pPointTouch->SetTouchTime(4000);

	////////////////////////////////////////////////////////////////////////////



	pPointMonster = CnNew PointMonster(this, 44);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3023, 2);


	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetSpawnSpeed(300);



	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3018, 1);
	pPointMonster->SetTriggerID(14);             // trigger 13 触发

	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3013, 1);
	pPointMonster->SetTriggerID(14);             // trigger 13 触发

	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3003, 1);
	pPointMonster->SetTriggerID(14);             // trigger 13 触发

	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3004, 1);
	pPointMonster->SetTriggerID(14);             // trigger 13 触发



	////////////////////////////////////////////////////////////////////////////




	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3025, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);

	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3023, 2);

	////////////////////////////////////////////////////////////////////////////


	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 2);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(100);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetTriggerID(13);

	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 2);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetTriggerID(13);

	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 2);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetSpawnSpeed(200);
	//pPointMonster->SetTriggerID(10);


	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 2);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetSpawnSpeed(200);
	//pPointMonster->SetTriggerID(10);

	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 2);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetTriggerID(12);

	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 3);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetTriggerID(12);

	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3021, 1);
	//pPointMonster->SetMax(1);                   // 一次1个，共4个
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetTriggerID(11);

	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,60);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);

	}









}



