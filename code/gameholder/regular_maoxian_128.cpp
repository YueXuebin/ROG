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
// �ؿ���Ĺ��캯��
RegularMaoXian_128::RegularMaoXian_128(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// �ؿ������������
RegularMaoXian_128::~RegularMaoXian_128()
{

}
// �ؿ���ʼʱ��,�ؿ���������
void RegularMaoXian_128::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // ���ֵ�
	PointTreasure* pPointTreasure = NULL;           // ����
	PointBarrel* pPointBarrel = NULL;               // ľͰ
	PointTouch* pPointTouch = NULL;                 // �ɼ���
	PointTrigger* pPointTrigger = NULL;             // ����ʯ

	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(28);

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
	////////////////////////////////////////////////////////////////////////////
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3045, 3);         // 1��������

	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(11);             // trigger 3 ����

	pPointTrigger = CnNew PointTrigger(this, 13);        //����ʯ
	pPointTrigger->SetShowID(4002);
	pPointTrigger->LinkTrigger(pPointMonster);

	//�������

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
		// ������
		pPointTreasure = CnNew PointTreasure(this,30);
		pPointTreasure->SetShowID(4001);
		pPointTreasure->LinkTrigger(pPointTrigger);

	}


	pPointBarrel = CnNew PointBarrel(this, 40);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3��

	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointBarrel);

	pPointBarrel = CnNew PointBarrel(this, 41);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3��

	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointBarrel);

	pPointBarrel = CnNew PointBarrel(this, 42);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 3��

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

	pPointTouch = CnNew PointTouch(this, 18);      // �ɼ���
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

