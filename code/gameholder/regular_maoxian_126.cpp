#include "gameholder_pch.h"
#include "regular_maoxian_126.h"
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
RegularMaoXian_126::RegularMaoXian_126(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_126::~RegularMaoXian_126()
{

}

void RegularMaoXian_126::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // ���ֵ�
	PointMonster* pPointMonster11  = NULL;            // ���ֵ�
	PointMonster* pPointMonster12  = NULL;            // ���ֵ�
	PointMonster* pPointMonster13  = NULL;            // ���ֵ�
	PointMonster* pPointMonster14  = NULL;            // ���ֵ�
	PointMonster* pPointMonster15  = NULL;            // ���ֵ�
	PointMonster* pPointMonster16  = NULL;            // ���ֵ�
	PointMonster* pPointMonster17  = NULL;            // ���ֵ�
	PointMonster* pPointMonster18  = NULL;            // ���ֵ�
	PointMonster* pPointMonster19  = NULL;            // ���ֵ�
	PointTreasure* pPointTreasure = NULL;           // ����
	PointBarrel* pPointBarrel = NULL;               // ľͰ
	PointTouch* pPointTouch = NULL;                 // �ɼ���
	PointTrigger* pPointTrigger = NULL;             // ����ʯ

	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(26);

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
	// ��10
	pPointMonster11 = CnNew PointMonster(this, 11);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3004, 6);         // ��ʲô��(��id, ����)
	pPointMonster11->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster11->SetDefaultMovie();
	pPointMonster11->SetTriggerID(13);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 11);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster11);

		if (Crown::RandomPercent(50))
		{   

			pPointMonster = CnNew PointMonster(this, 11);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 11);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3001), 3);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,21);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}
	}

	///////////��12
	pPointMonster12 = CnNew PointMonster(this, 12);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3004, 6);         // ��ʲô��(��id, ����)
	pPointMonster12->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster12->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointMonster11);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 12);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster12);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 12);
			pPointMonster->SetRadius(300);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 12);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3009), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,22);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	/////////////////////��13
	pPointMonster13 = CnNew PointMonster(this, 13);
	pPointMonster13->SetRadius(300);
	pPointMonster13->SetMonster(3015, 6);         // ��ʲô��(��id, ����)
	pPointMonster13->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster13->SetDefaultMovie();
	pPointMonster13->LinkTrigger(pPointMonster12);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 13);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster13);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 13);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 13);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3015), 4);
			pPointMonster->LinkTrigger(pPointTrigger);
		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,23);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////��14
	pPointMonster14 = CnNew PointMonster(this, 14);
	pPointMonster14->SetRadius(300);
	pPointMonster14->SetMonster(3018, 6);         // ��ʲô��(��id, ����)
	pPointMonster14->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster14->SetDefaultMovie();
	pPointMonster14->LinkTrigger(pPointMonster13);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 14);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster14);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 14);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3036, 3027), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 14);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3023, 3039), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,24);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);
		}

	}

	///////////��15
	pPointMonster15 = CnNew PointMonster(this, 15);
	pPointMonster15->SetRadius(300);
	pPointMonster15->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster15->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster15->SetDefaultMovie();
	pPointMonster15->LinkTrigger(pPointMonster14);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 15);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster15);

		if (Crown::RandomPercent(70))
		{   

			pPointMonster = CnNew PointMonster(this, 15);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 15);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3051), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,25);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);
		}

	}

	///////////��16
	pPointMonster16 = CnNew PointMonster(this, 16);
	pPointMonster16->SetRadius(300);
	pPointMonster16->SetMonster(3018, 6);         // ��ʲô��(��id, ����)
	pPointMonster16->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster16->SetDefaultMovie();
	pPointMonster16->LinkTrigger(pPointMonster15);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 16);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster16);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 16);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3029, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);


			pPointMonster = CnNew PointMonster(this, 16);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3002, 3051), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,26);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////��17
	pPointMonster17 = CnNew PointMonster(this, 17);
	pPointMonster17->SetRadius(300);
	pPointMonster17->SetMonster(3001, 6);         // ��ʲô��(��id, ����)
	pPointMonster17->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster17->SetDefaultMovie();
	pPointMonster17->LinkTrigger(pPointMonster16);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 17);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster17);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 17);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3033, 3034), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 17);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3023, 3039), 4);
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

	///////////��18
	pPointMonster18 = CnNew PointMonster(this, 18);
	pPointMonster18->SetRadius(300);
	pPointMonster18->SetMonster(3039, 6);         // ��ʲô��(��id, ����)
	pPointMonster18->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster18->SetDefaultMovie();
	pPointMonster18->LinkTrigger(pPointMonster17);

	if (Crown::RandomPercent(60))
	{   
		pPointTrigger = CnNew PointTrigger(this, 18);
		pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster18);

		if (Crown::RandomPercent(70))
		{   
			pPointMonster = CnNew PointMonster(this, 18);
			pPointMonster->SetRadius(300);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
			pPointMonster->SetQuality(QUALITY_GREEN);
			pPointMonster->LinkTrigger(pPointTrigger);

			pPointMonster = CnNew PointMonster(this, 18);
			pPointMonster->SetRadius(200);
			pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3051, 3001), 4);
			pPointMonster->LinkTrigger(pPointTrigger);

		}
		else
		{
			// ������
			pPointTreasure = CnNew PointTreasure(this,28);
			pPointTreasure->SetShowID(4001);
			pPointTreasure->LinkTrigger(pPointTrigger);

		}

	}

	///////////��19
	pPointMonster19 = CnNew PointMonster(this, 19);
	pPointMonster19->SetRadius(300);
	pPointMonster19->SetMonster(3024, 1);         // ��ʲô��(��id, ����)
	pPointMonster19->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster16->SetDefaultMovie();
	pPointMonster19->SetQuality(QUALITY_GREEN);
	pPointMonster19->LinkTrigger(pPointMonster18);

	pPointMonster11 = CnNew PointMonster(this, 11);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3049, 1);         // ��ʲô��(��id, ����)
	pPointMonster11->SetQuality(QUALITY_GREEN);
	pPointMonster11->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster11->SetDefaultMovie();
	pPointMonster11->LinkTrigger(pPointMonster19);

	pPointMonster12 = CnNew PointMonster(this, 12);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3021, 1);         // ��ʲô��(��id, ����)
	pPointMonster12->SetQuality(QUALITY_GREEN);
	pPointMonster12->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster11->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointMonster11);



	// ��20
	if (Crown::RandomPercent(50))
	{   

		pPointTreasure = CnNew PointTreasure(this,20);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointMonster12);

	}

}

