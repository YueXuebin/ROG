#include "gameholder_pch.h"
#include "regular_field_001.h"
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


RegularField_001::RegularField_001(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_001::~RegularField_001()
{

}

void RegularField_001::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // 出怪点
	PointTreasure* pPointTreasure = NULL;           // 宝箱
	PointBarrel* pPointBarrel = NULL;               // 木桶
	PointTouch* pPointTouch = NULL;                 // 采集点
	PointTrigger* pPointTrigger = NULL;             // 触摸石




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(1);



   // return;
	/////////////////////////////////////////////////////////////////////////////////


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(1200);
	pPointMonster->SetMonster(3701, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);



	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3702, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);



	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(1300);
	pPointMonster->SetMonster(3703, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);




	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3704, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);



	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3782, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);





	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(900);
	pPointMonster->SetMonster(3705, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);





	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3706, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);




	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(1400);
	pPointMonster->SetMonster(3707, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);




	// 点22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3708, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(6);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);


	// 点31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(900);
	pPointMonster->SetMonster(3709, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(3);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);



	// 点24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(2300);
	pPointMonster->SetMonster(3710, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(5);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);
	
	// 点26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(600);
	pPointMonster->SetMonster(3783, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);


	// 点28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(600);
	pPointMonster->SetMonster(3781, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_BLUE);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);


}