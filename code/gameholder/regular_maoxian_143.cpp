#include "gameholder_pch.h"
#include "regular_maoxian_143.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"
#include "rog_regular_event_monster_clear.h"
#include "rog_regular_event_player_dead.h"
#include "rog_regular_event_timer.h"
#include "rog_regular_event_combo_kill.h"

RegularMaoXian_143::RegularMaoXian_143(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_143::~RegularMaoXian_143()
{

}

void RegularMaoXian_143::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;
	PointTreasure* pPointTreasure = NULL;
	PointBarrel* pPointBarrel = NULL;
	PointTouch* pPointTouch = NULL;

	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(43);

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
	// ��11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 3);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms


	// ��12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(12);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);


	// ��13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3001, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(2000);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(12);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3012, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(4000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(12);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(6000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(12);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3001, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(8000);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(12);             // trigger 12 ����
	pPointMonster->SetAIGroup(2);

	// ��17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3018, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(10000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(16);             // trigger 12 ����

	// ��21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3013, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(13);             // trigger 13 ����

	// ��30
	if (Crown::RandomPercent(50))
	{   
		pPointTreasure = CnNew PointTreasure(this, 30);         // ����
		pPointTreasure->SetShowID(4001);
	}

	// ��22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3018,2);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(13);             // trigger 13 ����

	// ��23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3001, 5);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(17);             // trigger 13 ����

	// ��24
	pPointBarrel = CnNew PointBarrel(this, 24);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 4��

	// ��25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3018, 3);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(12);
	pPointMonster->SetTriggerID(14);             // trigger 14 ����

	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3036, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(1000);           // ��������ʱ1000ms����
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetAIGroup(12);
	pPointMonster->SetTriggerID(14);             // trigger 14 ����

	// ��26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3013, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(10000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(16);             // trigger 12 ����


	// ��27
	pPointBarrel = CnNew PointBarrel(this, 27);     // ľͰ
	pPointBarrel->SetRadius(0);
	pPointBarrel->SetNumber(1);                     // 5��

	// ��28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3001, 4);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	pPointMonster->SetTriggerID(15);             // trigger 12 ����

	// ��31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3033, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetAIGroup(12);
	pPointMonster->SetTriggerID(14);

	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3013, 2);
	//pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetDefaultMovie();
	pPointMonster->SetAIGroup(12);
	pPointMonster->SetTriggerID(14);




}


