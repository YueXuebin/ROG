#include "gameholder_pch.h"
#include "regular_maoxian_064.h"
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

RegularMaoXian_064::RegularMaoXian_064(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_064::~RegularMaoXian_064()
{

}

void RegularMaoXian_064::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;
    PointTrigger* pPointTrigger = NULL;

    /////////////////////////////////////////////////////////////////////////////////
    // �ؿ�����
    SetMapLevel(64);

    SetLifeCount(3); // ��������

    // ʤ������
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3025, 1);
    AddSucceedEvent(pEventMonsterDead); // ɱ����

    // ʧ������
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // �������

	// ��������
	ROGRegularEvent_PlayerDead* pEventScore_PlayerDead = CnNew ROGRegularEvent_PlayerDead(this, 0, true);       // һ�����������һ��
	AddScoreEvent(pEventScore_PlayerDead);

	ROGRegularEvent_Timer* pEventScore_Timer = CnNew ROGRegularEvent_Timer(this, true);
	pEventScore_Timer->SetTime(1000*20);            // 20��ͨ�ػ��һ��
	AddScoreEvent(pEventScore_Timer);

    ////////////////////////////////////////////////////////////////////////////
    // ��25
    pPointMonster = CnNew PointMonster(this, 25);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3001, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetTriggerID(11);             // trigger 12 ����
    pPointMonster->SetAIGroup(1);

    if (Crown::RandomPercent(60))
    {   
        pPointTrigger = CnNew PointTrigger(this, 24);
        pPointTrigger->SetShowID(4002);

        if (Crown::RandomPercent(50))
        {   

            pPointMonster = CnNew PointMonster(this, 24);
            pPointMonster->SetRadius(300);
            pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3025, 3009), 1);
            pPointMonster->LinkTrigger(pPointTrigger);


            pPointMonster = CnNew PointMonster(this, 24);
            pPointMonster->SetRadius(300);
            pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3001, 3023), 2);
            pPointMonster->LinkTrigger(pPointTrigger);

        }
        else
        {
            // ������
            pPointTreasure = CnNew PointTreasure(this,50);
            pPointTreasure->SetShowID(4001);
            pPointTreasure->LinkTrigger(pPointTrigger);

        }
    }

    // ��23
    pPointMonster = CnNew PointMonster(this, 23);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3045, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(13);             // trigger 13 ����
    //pPointMonster->SetAIGroup(2);

    // ��21
    pPointMonster = CnNew PointMonster(this, 21);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3045, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(14);             // trigger 14 ����
    //pPointMonster->SetAIGroup(2);

    // ��20
    pPointMonster = CnNew PointMonster(this, 20);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3040, 1);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetTriggerID(12);             // trigger 14 ����
    //pPointMonster->SetAIGroup(2);

    // ��20
    pPointMonster = CnNew PointMonster(this, 20);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3039, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetTriggerID(12);             // trigger 14 ����
    //pPointMonster->SetAIGroup(2);

    // ��19
    pPointMonster = CnNew PointMonster(this, 19);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3014, 3);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(15);             // trigger 2 ����
    //pPointMonster->SetAIGroup(2);

    // ��18
    pPointMonster = CnNew PointMonster(this, 18);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3039, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(15);             // trigger 15 ����
    //pPointMonster->SetAIGroup(2);

    // ��17
    pPointMonster = CnNew PointMonster(this, 17);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3039, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(16);             // trigger 16 ����
    //pPointMonster->SetAIGroup(2);

    // ��17
    pPointMonster = CnNew PointMonster(this, 17);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3001, 1);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(16);             // trigger 16 ����
    //pPointMonster->SetAIGroup(2);

    // ��16
    pPointMonster = CnNew PointMonster(this, 16);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3001, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(12);             // trigger 12 ����
    //pPointMonster->SetAIGroup(1);

    // ��42
    pPointMonster = CnNew PointMonster(this, 15);
    pPointMonster->SetRadius(150);
    pPointMonster->SetMonster(3003, 2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(2000);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(14);             // trigger 14 ����
    //pPointMonster->SetAIGroup(2);

    // ��43
    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3002, 3);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    //pPointMonster->SetTriggerID(16);             // trigger 16 ����
    //pPointMonster->SetAIGroup(2);

    // ��51
    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3001, 1);
    pPointMonster->SetDefaultMovie();
    pPointMonster->SetMax(1);
    pPointMonster->SetLoop(true);
    pPointMonster->SetTriggerID(13);


    // ��61
    pPointMonster = CnNew PointMonster(this, 30);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3039, 1);         // ��ʲô��(��id, ����)
    pPointMonster->SetMax(1);
    pPointMonster->SetLoop(true);
    pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetTriggerID(13);             // trigger 12 ����

    // ��62
    pPointMonster = CnNew PointMonster(this, 31);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3045, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetMax(2);
    pPointMonster->SetLoop(true);
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetTriggerID(13);             // trigger 12 ����


    // ��63
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3025, 1);         // ��ʲô��(��id, ����)
    pPointMonster->SetQuality(QUALITY_BLUE);
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetTriggerID(13);             // trigger 12 ����

    pPointTreasure = CnNew PointTreasure(this,51);
    pPointTreasure->SetShowID(4001);
    pPointTreasure->LinkTrigger(pPointMonster);


    // ��31
    pPointBarrel = CnNew PointBarrel(this, 40);     // ľͰ
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 1��

    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 40);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // ��������
        pPointMonster->LinkTrigger(pPointBarrel);

    }

    // ��32
    pPointBarrel = CnNew PointBarrel(this, 41);     // ľͰ
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 1��

    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 41);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // ��������
        pPointMonster->LinkTrigger(pPointBarrel);

    }



    // ��33
    pPointBarrel = CnNew PointBarrel(this, 42);     // ľͰ
    pPointBarrel->SetRadius(0);
    pPointBarrel->SetNumber(1);                     // 1��


    if (Crown::RandomPercent(70))
    {   
        pPointMonster = CnNew PointMonster(this, 42);
        pPointMonster->SetRadius(0);
        pPointMonster->SetMonster(3001, 1);
        pPointMonster->SetDefaultMovie();           // ��������
        pPointMonster->LinkTrigger(pPointBarrel);

    }


    
}


