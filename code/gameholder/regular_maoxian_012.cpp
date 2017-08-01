#include "gameholder_pch.h"
#include "regular_maoxian_012.h"
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

RegularMaoXian_012::RegularMaoXian_012(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_012::~RegularMaoXian_012()
{

}

void RegularMaoXian_012::FirstUpdate()
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
    PointTrigger* pPointTrigger11 = NULL;             // ����ʯ
	PointTrigger* pPointTrigger12 = NULL;             // ����ʯ
    PointTrigger* pPointTrigger13 = NULL;             // ����ʯ
    PointTrigger* pPointTrigger14 = NULL;             // ����ʯ
    /////////////////////////////////////////////////////////////////////////////////
    // �ؿ�����
    SetMapLevel(12);

    SetLifeCount(3); // ��������
	// ʤ������
	ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
	pEventMonsterDead->SetMonster(3312, 1);
	AddSucceedEvent(pEventMonsterDead); // ɱ����

    // ʧ������
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // �������

	// ��������
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // һ�����������һ��
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*120);            // 20��ͨ�ػ��һ��
	AddScoreEvent(pEventScore_Timer);

    /////////////////////////////////////////////////////////////////////////////////

	pPointMonster16= CnNew PointMonster(this, 23);
	pPointMonster16->SetRadius(400);
	pPointMonster16->SetMonster(3015, 4);

    // ����ʯ11
	pPointTrigger11 = CnNew PointTrigger(this, 11);
	pPointTrigger11->SetShowID(4002);
    pPointTrigger11->LinkTrigger(pPointMonster16);

	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3015, 2);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger11);

	pPointMonster11 = CnNew PointMonster(this, 16);
	pPointMonster11->SetRadius(300);
	pPointMonster11->SetMonster(3041, 4);
	pPointMonster11->SetDefaultMovie();
	pPointMonster11->LinkTrigger(pPointTrigger11);

    // ����ʯ12
	pPointTrigger12 = CnNew PointTrigger(this, 12);
	pPointTrigger12->SetShowID(4002);
    pPointTrigger12->LinkTrigger(pPointMonster11);

	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3015, 2);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger12);

	pPointMonster12 = CnNew PointMonster(this, 20);
	pPointMonster12->SetRadius(300);
	pPointMonster12->SetMonster(3041, 4);
	pPointMonster12->SetDefaultMovie();
	pPointMonster12->LinkTrigger(pPointTrigger12);

    // ����ʯ13
	pPointTrigger13 = CnNew PointTrigger(this, 13);
	pPointTrigger13->SetShowID(4002);
	pPointTrigger13->LinkTrigger(pPointMonster12);

	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3015, 2);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger13);

	pPointMonster13 = CnNew PointMonster(this, 22);
	pPointMonster13->SetRadius(300);
	pPointMonster13->SetMonster(3041, 4);
	pPointMonster13->SetDefaultMovie();
	pPointMonster13->LinkTrigger(pPointTrigger13);


    // ����ʯ14
	pPointTrigger14 = CnNew PointTrigger(this, 14);
	pPointTrigger14->SetShowID(4002);
    pPointTrigger14->LinkTrigger(pPointMonster13);

	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(150);
	pPointMonster->SetMonster(3015, 2);
	pPointMonster->SetDefaultMovie();
	pPointMonster->LinkTrigger(pPointTrigger14);

	pPointMonster14 = CnNew PointMonster(this, 18);
	pPointMonster14->SetRadius(300);
	pPointMonster14->SetMonster(3041, 4);
	pPointMonster14->SetDefaultMovie();
	pPointMonster14->LinkTrigger(pPointTrigger14);


	// boss

	pPointMonster15 = CnNew PointMonster(this, 23);
	pPointMonster15->SetRadius(0);
	pPointMonster15->SetMonster(3312, 1);
	pPointMonster15->SetDefaultMovie();
	pPointMonster15->SetQuality(QUALITY_GREEN);
    pPointMonster15->LinkTrigger(pPointMonster14);
}

