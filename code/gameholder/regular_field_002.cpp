#include "gameholder_pch.h"
#include "regular_field_002.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_002::RegularField_002(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_002::~RegularField_002()
{

}

void RegularField_002::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	//PointTrigger* pPointTrigger = NULL;             // 触摸石



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(20);



    //return;
	/////////////////////////////////////////////////////////////////////////////////
	

	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(2000);
	pPointMonster->SetMonster(3711, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(1200);
	pPointMonster->SetMonster(3712, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3713, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(2000);
	pPointMonster->SetMonster(3714, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3715, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(1200);
	pPointMonster->SetMonster(3716, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(2000);
	pPointMonster->SetMonster(3717, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(1800);
	pPointMonster->SetMonster(3718, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3719, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3720, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(600);
	pPointMonster->SetMonster(3784, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);

	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(600);
	pPointMonster->SetMonster(3785, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);

	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(600);
	pPointMonster->SetMonster(3786, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);










}