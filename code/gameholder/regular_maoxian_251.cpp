#include "gameholder_pch.h"
#include "regular_maoxian_251.h"
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

// 关卡类的构造函数
RegularMaoXian_251::RegularMaoXian_251(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// 关卡类的析构函数
RegularMaoXian_251::~RegularMaoXian_251()
{

}
// 关卡开始时的,关卡内容配置
void RegularMaoXian_251::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石

	/////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(51);

	SetLifeCount(3); // 设置命数

	// 胜利条件
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3036, 2);
	AddSucceedEvent(pEventMonsterDead); // 杀死怪

	// 失败条件
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // 玩家死亡


	////////////////////////////////////////////////////////////////////////////
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 4);         // 1个巨骷髅

	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetDelayTime(500);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);          // 出怪间隔300ms
	pPointMonster->SetTriggerID(13);             // trigger 3 触发

	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 5);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(1000);
	pPointMonster->SetSpawnSpeed(300);
	pPointMonster->SetTriggerID(13);


	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3002, 6);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetTriggerID(7);


	////////////////////////////////////////////////////////////////////////////


	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3003, 2);

	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetSpawnSpeed(300);
	pPointMonster->SetTriggerID(12);             // trigger 3 触发


	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3002, 3);
	pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(600);
	pPointMonster->SetSpawnSpeed(400);
	pPointMonster->SetTriggerID(12);             // trigger 3 触发

	////////////////////////////////////////////////////////////////////////////

	pPointBarrel = CnNew PointBarrel(this, 40);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 40);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 41);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 41);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 42);     // 木桶
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6个

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 42);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // 出场演绎
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 27);
		pPointTrigger->SetShowID(4002);

		//随机触发
		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 27);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 27);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3031), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// 出宝箱
			pPointTreasure = CnNew PointTreasure(this,30);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}






	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3003, 2);


	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3004, 2);


	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 3);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetSpawnSpeed(300);


	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3002, 4);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetDelayTime(100);
	pPointMonster->SetSpawnSpeed(200);




	////////////////////////////////////////////////////////////////////////////

	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3004, 2);
	pPointMonster->SetTriggerID(14);



	////////////////////////////////////////////////////////////////////////////
	// boss
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 8);
	pPointMonster->SetMax(2);                   // 一次1个，共4个
	//pPointMonster->SetDefaultMovie();
	//pPointMonster->SetDelayTime(100);
	pPointMonster->SetSpawnSpeed(200);
	pPointMonster->SetTriggerID(14);


	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3036, 2);
	//pPointMonster->SetDefaultMovie();
	pPointMonster->SetQuality(QUALITY_BLUE);
	pPointMonster->SetTriggerID(14);

	if (Crown::RandomPercent(90))
	{   

		pPointTreasure = CnNew PointTreasure(this,31);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);

	}


}
