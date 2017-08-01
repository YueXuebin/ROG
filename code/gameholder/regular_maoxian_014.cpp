#include "gameholder_pch.h"
#include "regular_maoxian_014.h"
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
#include "rog_regular_event_timer.h"


RegularMaoXian_014::RegularMaoXian_014(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_014::~RegularMaoXian_014()
{

}

void RegularMaoXian_014::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;
    PointTrigger* pPointTrigger = NULL;

    /////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(14);

    SetLifeCount(3); // ��������

    // ʤ������
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3314, 1);
    AddSucceedEvent(pEventMonsterDead); // ɱ����

    // ʧ������
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // �������

	// ��������
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // һ�����������һ��
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*180);            // 20��ͨ�ػ��һ��
	AddScoreEvent(pEventScore_Timer);

	////////////////////////////////////////////////////////////////////////////

	// ��11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetQuality(QUALITY_BLUE);    // ����Ʒ��
	//pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 ����
	//pPointMonster->SetAIGroup(1)

	// ��12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // ��ʲô��(��id, ����)


	////////////////////////////////////////////////////////////////////////////
	// ��13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(1);

	// ��14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(1);

	// ��15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(1);

	// ��16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(1);

	////////////////////////////////////////////////////////////////////////////

	// ��17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3116, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(2);

	// ��18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 4);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(2);

	////////////////////////////////////////////////////////////////////////////


	// ��19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 4);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(3);

	// ��19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3110, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(3);

	////////////////////////////////////////////////////////////////////////////


	// ��20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(4);

	// ��21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(4);

	// ��22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3111, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(4);


	// ��23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3041, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetTriggerID(11);            // trigger 15 ����
	pPointMonster->SetAIGroup(4);

	// ��24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3041, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetTriggerID(11);            // trigger 15 ����
	pPointMonster->SetAIGroup(4);

	// ��25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3113, 1);         // ��ʲô��(��id, ����)

	// ��25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3045, 4);         // ��ʲô��(��id, ����)

	////////////////////////////////////////////////////////////////////////////

	// ��26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3118, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(7);
	// ��26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3116, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(7);

	// ��27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3118, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(8);
	// ��27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3116, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(8);

	// ��28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3118, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(9);
	// ��28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(500);
	pPointMonster->SetMonster(3116, 2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(9);
	////////////////////////////////////////////////////////////////////////////

	// ��29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)
	// ��30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)

	// ��31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3110,1);         // ��ʲô��(��id, ����)


	////////////////////////////////////////////////////////////////////////////

	// ��32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3112, 2);         // ��ʲô��(��id, ����)

	// ��32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3110, 2);         // ��ʲô��(��id, ����)

	////////////////////////////////////////////////////////////////////////////

	// ��33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)
	// ��34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)
	////////////////////////////////////////////////////////////////////////////

	// ��35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(5);

	// ��36
	pPointMonster = CnNew PointMonster(this, 36);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3118,1);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(5);
	// ��37
	pPointMonster = CnNew PointMonster(this, 37);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3110,2);         // ��ʲô��(��id, ����)
	pPointMonster->SetAIGroup(5);


	////////////////////////////////////////////////////////////////////////////BOSS

	// ��38
	pPointMonster = CnNew PointMonster(this,38);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3314, 1);         // ��ʲô��(��id, ����)
	//pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetQuality(QUALITY_GREEN);    // ����Ʒ��
	//pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(15);            // trigger 15 ����
	//pPointMonster->SetAIGroup(1)

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ��37
	pPointBarrel = CnNew PointBarrel(this, 39);     // ľͰ
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // ����1

	// ��38
	pPointBarrel = CnNew PointBarrel(this, 40);     // ľͰ
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // ����1

	// ��39
	pPointBarrel = CnNew PointBarrel(this, 41);     // ľͰ
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // ����1

	// ��40
	pPointBarrel = CnNew PointBarrel(this, 42);     // ľͰ
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // ����1

	// ��41
	pPointBarrel = CnNew PointBarrel(this, 43);     // ľͰ
	pPointBarrel->SetRadius(50);
	pPointBarrel->SetNumber(1);                     // ����1



}

