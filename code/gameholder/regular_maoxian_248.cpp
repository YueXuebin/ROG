#include "gameholder_pch.h"
#include "regular_maoxian_248.h"
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


RegularMaoXian_248::RegularMaoXian_248(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_248::~RegularMaoXian_248()
{

}

void RegularMaoXian_248::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger11 = NULL;             // ����ʯ
	PointTrigger* pPointTrigger12 = NULL;             // ����ʯ
	PointTrigger* pPointTrigger = NULL;             // ����ʯ
	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(48);

	SetLifeCount(3); // ��������

	// ʤ������
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3020, 1);
	AddSucceedEvent(pEventMonsterDead); // ɱ����


	// ʧ������
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // �������



	/////////////////////////////////////////////////////////////////////////////////

	//��11
	pPointMonster = CnNew PointMonster(this,11);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(2000);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	//��12
	pPointMonster = CnNew PointMonster(this,12);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(2000);
	pPointMonster->SetMonster(3012,2);
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	//��13
	pPointMonster = CnNew PointMonster(this,13);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetTriggerID(11);

	//��14
	pPointTouch = CnNew PointTouch(this,14);
	pPointTouch->SetShowID(4003);

	//��15
	pPointTouch = CnNew PointTouch(this,15);
	pPointTouch->SetShowID(4003);

	//��16
	pPointTouch = CnNew PointTouch(this,16);
	pPointTouch->SetShowID(4003);

	//��17
	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(300);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(100);                               // ���ּ��300ms
	pPointMonster->SetAIGroup(11);
	pPointMonster->SetTriggerID(12);

	pPointMonster = CnNew PointMonster(this,17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetDelayTime(2000);
	pPointMonster->SetMonster(3028,1);
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetSpawnSpeed(300);                                // ���ּ��300ms
	pPointMonster->SetAIGroup(11);
	pPointMonster->SetTriggerID(12);


	//��18
	pPointMonster = CnNew PointMonster(this,18);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);                                // ���ּ��300ms
	//pPointMonster->SetAIGroup(21);
	//pPointMonster->SetTriggerID(13);

	//��19
	pPointMonster = CnNew PointMonster(this,19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��                   
	pPointMonster->SetSpawnSpeed(300);                                 // ���ּ��300ms
	//pPointMonster->SetAIGroup(21);
	//pPointMonster->SetTriggerID(13);

	//��20
	pPointTouch = CnNew PointTouch(this,20);
	pPointTouch->SetShowID(4003);

	//��25
	pPointMonster = CnNew PointMonster(this,25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(100);
	pPointMonster->SetMonster(3012,2);
	pPointMonster->SetSpawnSpeed(300);                                  // ���ּ��300ms
	//pPointMonster->SetAIGroup(21);
	pPointMonster->SetTriggerID(14);

	//��24
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(150);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);                                  // ���ּ��300ms

	//��22
	pPointMonster = CnNew PointMonster(this,22);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);                                  // ���ּ��300ms

	// ��23 ������
	pPointTreasure = CnNew PointTreasure(this, 23);                      
	pPointTreasure->SetShowID(4001);                                    //��������
	pPointTreasure->LinkTrigger(pPointMonster);                         //��������  ���� 17

	//��21
	pPointTouch = CnNew PointTouch(this,21);
	pPointTouch->SetShowID(4003);

	//��26
	pPointMonster = CnNew PointMonster(this,26);
	pPointMonster->SetRadius(300);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3014), 2);
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	//��27
	pPointMonster = CnNew PointMonster(this,27);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	//��28
	pPointMonster = CnNew PointMonster(this,28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetDelayTime(500);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms



	// ��29 ����ʯ
	if (Crown::RandomPercent(100))
	{   
		pPointTrigger = CnNew PointTrigger(this, 29);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster);


		if (Crown::RandomPercent(70))
		{   

			pPointMonster = CnNew PointMonster(this, 26);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3028, 3034), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,27);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);
		}

	}


	//��30
	pPointMonster = CnNew PointMonster(this,30);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetSpawnSpeed(2000);          // ���ּ��300ms
	pPointMonster->SetTriggerID(15);

	//��31
	pPointMonster = CnNew PointMonster(this,31);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetMonster(3014,2);   //��Ѫ��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetSpawnSpeed(2000);          // ���ּ��300ms
	pPointMonster->SetTriggerID(15);

	//��99
	pPointMonster = CnNew PointMonster(this,99);
	pPointMonster->SetRadius(20);
	pPointMonster->SetDelayTime(200);
	pPointMonster->SetMonster(3020,1);
	pPointMonster->SetQuality(QUALITY_BLUE);
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	// ��99 ������
	pPointTreasure = CnNew PointTreasure(this, 98);                      
	pPointTreasure->SetShowID(4001);                                    //��������
	pPointTreasure->LinkTrigger(pPointMonster);                         //��������  ���� 17




}

