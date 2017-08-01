#include "gameholder_pch.h"
#include "regular_maoxian_054.h"
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

RegularMaoXian_054::RegularMaoXian_054(RegularDoc* pDoc) :
RegularMaoXianBase(pDoc)
{

}

RegularMaoXian_054::~RegularMaoXian_054()
{

}

void RegularMaoXian_054::FirstUpdate()
{
    RegularMaoXianBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;     // ����
    PointTreasure* pPointTreasure = NULL;    // ����
    PointBarrel* pPointBarrel = NULL;        // ľͰ
    PointTouch* pPointTouch = NULL;          // �ɼ���
    PointTrigger* pPointTrigger = NULL;      // ����ʯ

    /////////////////////////////////////////////////////////////////////////////////
    // �ؿ�����
    SetMapLevel(54);

    SetLifeCount(3); // ��������

    // ʤ������
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3010, 1);
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
	//��11
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3004, 3);           // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(2500);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms 
    pPointMonster->SetTriggerID(11);              // trigger 11 ����
	pPointMonster->SetAIGroup(2);

	 // ��12
    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3001, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
    pPointMonster->SetTriggerID(11);            // trigger 11 ����
    pPointMonster->SetAIGroup(2);


	// ��17
    pPointMonster = CnNew PointMonster(this, 17);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3001,4);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(100);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    pPointMonster->SetTriggerID(12);                                             // trigger 12 ����

    // ��27
    pPointMonster = CnNew PointMonster(this, 27);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3003,3);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(2000);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    pPointMonster->SetTriggerID(12);                                             // trigger 12 ����



    // ��24 ������
    pPointTreasure = CnNew PointTreasure(this, 24);                      
    pPointTreasure->SetShowID(4001);                                    //��������
	pPointTreasure->LinkTrigger(pPointMonster);                         //��������  ���� 17


    // ��25
    pPointMonster = CnNew PointMonster(this, 25);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3039, 4);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(300);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    //pPointMonster->SetTriggerID(13);

	// ��25
    pPointMonster = CnNew PointMonster(this, 25);
    pPointMonster->SetRadius(500);
    pPointMonster->SetMonster(3012,2);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(300);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    pPointMonster->SetTriggerID(13);




	
    // ��20 ����ʯ
    if (Crown::RandomPercent(100))
    {   
        pPointTrigger = CnNew PointTrigger(this, 20);
        pPointTrigger->SetShowID(4002);
		pPointTrigger->LinkTrigger(pPointMonster);


        if (Crown::RandomPercent(70))
        {   
         
            pPointMonster = CnNew PointMonster(this, 26);
            pPointMonster->SetRadius(300);
            pPointMonster->SetMonster(Crown::RandomOneNumber(2, 3052, 3009), 1);
            pPointMonster->SetQuality(QUALITY_GREEN);
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




    // ��26
    pPointMonster = CnNew PointMonster(this, 26);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3014,3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(300);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    pPointMonster->SetTriggerID(14);

	// ��30
    pPointMonster = CnNew PointMonster(this, 30);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3012,2);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();                                          // ��������
    pPointMonster->SetDelayTime(3000);                                            // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(500);                                           // ���ּ��300ms
    pPointMonster->SetTriggerID(14);
	
    // ��28
    pPointMonster = CnNew PointMonster(this, 28);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3015, 1);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
    pPointMonster->SetTriggerID(15);            // trigger 15 ����
    //pPointMonster->SetAIGroup(1)

	 // ��29
    pPointMonster = CnNew PointMonster(this, 29);
    pPointMonster->SetRadius(100);
    pPointMonster->SetMonster(3015, 1);         // ��ʲô��(��id, ����)
    //pPointMonster->SetDefaultMovie();           // ��������
    //pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
    pPointMonster->SetTriggerID(15);            // trigger 15 ����
    //pPointMonster->SetAIGroup(1)

	
    // ��18
    pPointMonster = CnNew PointMonster(this, 18);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3010, 1);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetQuality(QUALITY_BLUE);    // ����Ʒ��
    //pPointMonster->SetDelayTime(200);           // ��������ʱ500ms����
    //pPointMonster->SetSpawnSpeed(500);          // ���ּ��300ms
    pPointMonster->SetTriggerID(15);            // trigger 15 ����
    //pPointMonster->SetAIGroup(1)
	

    // ������
    pPointTreasure = CnNew PointTreasure(this,18);
    pPointTreasure->SetShowID(4001);
    pPointTreasure->LinkTrigger(pPointMonster);





    //////////////////////////////////////////////////////////////////////////////
	 // ��13
    pPointBarrel = CnNew PointBarrel(this, 13);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1


	// ��14
    pPointBarrel = CnNew PointBarrel(this, 14);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1


	// ��15
    pPointBarrel = CnNew PointBarrel(this, 15);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1


	 // ��21
    pPointBarrel = CnNew PointBarrel(this, 21);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1


	// ��22
    pPointBarrel = CnNew PointBarrel(this, 22);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1


	// ��23
    pPointBarrel = CnNew PointBarrel(this, 23);     // ľͰ
    pPointBarrel->SetRadius(50);
    pPointBarrel->SetNumber(1);                     // ����1





}

