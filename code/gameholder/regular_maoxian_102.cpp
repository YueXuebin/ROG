#include "gameholder_pch.h"
#include "regular_maoxian_102.h"
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

RegularMaoXian_102::RegularMaoXian_102(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_102::~RegularMaoXian_102()
{

}

void RegularMaoXian_102::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;
	PointTrigger* pPointTrigger = NULL;             // ����ʯ

	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(2);

	SetLifeCount(0); // ��������

	// ʤ������
	ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
	AddSucceedEvent(pEventMonsterClear); // ���ȫ����

	// ʧ������
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // �������

	ROGRegularEvent_Timer* pEventFailed_Timer = CnNew ROGRegularEvent_Timer(this);
	pEventFailed_Timer->SetTime(1000*300);            // n����ʱ
	AddFailedEvent(pEventFailed_Timer);

	// ��������
	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*200);            // n��ͨ�ػ��һ��
	AddScoreEvent(pEventScore_Timer);

	ROGRegularEvent_ComboKill* pEventScore_ComboKill = CnNew ROGRegularEvent_ComboKill(this);
	pEventScore_ComboKill->SetComboKill(5);         // n��ɱ
	AddScoreEvent(pEventScore_ComboKill);
	/////////////////////////////////////////////////////////////////////////////////
	// ��72
	pPointMonster = CnNew PointMonster(this, 72);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3016, Crown::SDRandom(4,10));         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(10);             // trigger 10 ����
	pPointMonster->SetAIGroup(1);

	// ��12
	pPointMonster = CnNew PointMonster(this, 74);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 2);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(10);             // trigger 10 ����
	pPointMonster->SetAIGroup(1);


	// ��13
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3002, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��14
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 1);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��15
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3002, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(13);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	pPointBarrel = CnNew PointBarrel(this, 15);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6��

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 15);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // ��������
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 70);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6��

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 70);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // ��������
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 73);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6��

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 73);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // ��������
		pPointMonster->LinkTrigger(pPointBarrel);

	}


	// ��22
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3041,2);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(17);             // trigger 13 ����

	// ��23
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3001, 5);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(17);             // trigger 13 ����

	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 32);         // ����
		pPointTreasure->SetShowID(4001);
	}


	// ��25
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3016, 2);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3041, 2);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointBarrel = CnNew PointBarrel(this, 36);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6��

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 36);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // ��������
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointBarrel = CnNew PointBarrel(this, 43);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 6��

	if (Crown::RandomPercent(70))
	{   
		pPointMonster = CnNew PointMonster(this, 43);
		pPointMonster->SetRadius(0);
		pPointMonster->SetMonster(3001, 1);
		pPointMonster->SetDefaultMovie();           // ��������
		pPointMonster->LinkTrigger(pPointBarrel);

	}

	pPointMonster = CnNew PointMonster(this, 77);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 54);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 55);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 56);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(20);

	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 62);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 78);
		pPointTrigger->SetShowID(4002);

		//�������
		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 80);
			pPointMonster->SetRadius(150);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 80);
			pPointMonster->SetRadius(150);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3002), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,80);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}

	pPointMonster = CnNew PointMonster(this, 61);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms

	pPointMonster = CnNew PointMonster(this, 75);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3002, 3);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(18);             // trigger 13 ����
	pPointMonster->SetAIGroup(12);

	pPointMonster = CnNew PointMonster(this, 76);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 3);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(18);             // trigger 13 ����
	pPointMonster->SetAIGroup(12);


	// ��31
	pPointMonster = CnNew PointMonster(this, 71);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3021, 1);
	//pPointMonster->SetDefaultMovie();
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetTriggerID(18);
	pPointMonster->SetAIGroup(12);

	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,71);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster);

	}




}
