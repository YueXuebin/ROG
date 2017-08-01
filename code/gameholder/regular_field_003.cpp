#include "gameholder_pch.h"
#include "regular_field_003.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_003::RegularField_003(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_003::~RegularField_003()
{

}

void RegularField_003::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 关卡配置
	SetMapLevel(5);



	// 点11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3721, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms


	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3722, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(2000);
	pPointMonster->SetMonster(3723, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3724, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3725, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3726, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(10);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3727, 10);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(10);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3728, 10);         // 出什么怪(怪id, 数量)	
	pPointMonster->SetLevel(10);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3729, 10);         // 出什么怪(怪id, 数量)	
	pPointMonster->SetLevel(10);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3730, 10);         // 出什么怪(怪id, 数量)	
	pPointMonster->SetLevel(10);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetSpawnSpeed(3000);          // 出怪间隔300ms	

	// 点23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3787, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(20);                  // 怪物等级 4级
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
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3788, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(15);                  // 怪物等级 4级
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
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3789, 1);         // 出什么怪(怪id, 数量)
	pPointMonster->SetLevel(15);                  // 怪物等级 4级
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // 出场演绎
	pPointMonster->SetQuality(QUALITY_GREEN);    // 怪物品质
	//pPointMonster->SetDelayTime(5000);           // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(180000);          // 出怪间隔300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 触发
	//pPointMonster->SetAIGroup(1);


/*

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 51);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3804, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 54);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 55);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 56);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 57);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 58);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3805, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// 点2：npc1
	pPointMonster = CnNew PointMonster(this, 59);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3805, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
*/




}