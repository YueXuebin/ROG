#include "gameholder_pch.h"
#include "regular_maoxian_006.h"
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

// �ؿ���Ĺ��캯��
RegularMaoXian_006::RegularMaoXian_006(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}
// �ؿ������������
RegularMaoXian_006::~RegularMaoXian_006()
{

}
// �ؿ���ʼʱ��,�ؿ���������
void RegularMaoXian_006::FirstUpdate()
{
	RegularMaoXianBase::FirstUpdate();

	PointMonster* pPointMonster  = NULL;            // ���ֵ�
	PointMonster* pPointMonster1  = NULL;            // ���ֵ�
	PointTreasure* pPointTreasure = NULL;           // ����
	PointBarrel* pPointBarrel = NULL;               // ľͰ
	PointTouch* pPointTouch = NULL;                 // �ɼ���
	PointTrigger* pPointTrigger = NULL;             // ����ʯ

	/////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(6);

	SetLifeCount(3); // ��������

	// ʤ������
	ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
	AddSucceedEvent(pEventMonsterClear); // ���ȫ����

	// ʧ������
	ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
	AddFailedEvent(pEventPlayerDead); // �������

	// ��������
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // һ�����������һ��
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*120);            // 20��ͨ�ػ��һ��
	AddScoreEvent(pEventScore_Timer);


	////////////////////////////////////////////////////////////////////////////

	//��11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3082, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3068, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3082, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����

	////////////////////////////////////////////////////////////////////////////


	//��14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3102, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 ����


	//��15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 ����


	//��16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 ����


	//��17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3102, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(12);             // trigger 3 ����

	////////////////////////////////////////////////////////////////////////////


	//��18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(1);                // AI��


	//��19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(1);                // AI��


	//��20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3108, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(2);                // AI��


	//��21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3066, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(2);                // AI��


	//��22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3108, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	////////////////////////////////////////////////////////////////////////////


	//��23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��24
	pPointMonster = CnNew PointMonster(this, 24);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3102, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��25
	pPointMonster = CnNew PointMonster(this, 25);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��26
	pPointMonster = CnNew PointMonster(this, 26);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3103, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����

	////////////////////////////////////////////////////////////////////////////


	//��27
	pPointMonster = CnNew PointMonster(this, 27);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(3);                // AI��


	//��28
	pPointMonster = CnNew PointMonster(this, 28);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(3);                // AI��


	//��29
	pPointMonster = CnNew PointMonster(this, 29);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(3);                // AI��


	//��30
	pPointMonster = CnNew PointMonster(this, 30);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3108, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(3);                // AI��


	//��31
	pPointMonster = CnNew PointMonster(this, 31);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3066, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(4);                // AI��


	//��32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3066, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(4);                // AI��


	//��33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3105, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(4);                // AI��

	////////////////////////////////////////////////////////////////////////////

	//��34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(5);                // AI��
	//��34
	pPointMonster = CnNew PointMonster(this, 34);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3103, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(5);                // AI��


	//��35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(400);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(6);                // AI��
	//��35
	pPointMonster = CnNew PointMonster(this, 35);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3103, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(6);                // AI��

	////////////////////////////////////////////////////////////////////////////

	//��36
	pPointMonster = CnNew PointMonster(this, 36);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����


	//��37
	pPointMonster = CnNew PointMonster(this, 37);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3070, 2);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����

	////////////////////////////////////////////////////////////////////////////BOSS


	





	//��40
	pPointMonster = CnNew PointMonster(this, 40);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(7);                // AI��


	//��41
	pPointMonster = CnNew PointMonster(this, 41);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(7);                // AI��


	//��42
	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3122, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster->SetAIGroup(7);                // AI��

	//��39
	pPointMonster1 = CnNew PointMonster(this, 39);
	pPointMonster1->SetRadius(100);
	pPointMonster1->SetMonster(3121, 1);
	//pPointMonster->SetDefaultMovie();           // ��������
	//pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
	//pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 3 ����
	pPointMonster1->SetAIGroup(7);                // AI��

	//��38
	pPointMonster = CnNew PointMonster(this, 38);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3306, 1);
	pPointMonster->SetQuality(QUALITY_GREEN);    // ����Ʒ��
    pPointMonster->SetDefaultMovie();            // ��������
	pPointMonster->LinkTrigger(pPointMonster1);
}

