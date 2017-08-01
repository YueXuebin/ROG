#include "gameholder_pch.h"
#include "regular_daily_001.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"
#include "rog_regular_event_monster_clear.h"
#include "rog_regular_event_player_dead.h"

RegularDaily_001::RegularDaily_001(RegularDoc* pDoc) :
RegularDailyBase(pDoc)
{

}

RegularDaily_001::~RegularDaily_001()
{

}

void RegularDaily_001::FirstUpdate()
{
    RegularDailyBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;

    /////////////////////////////////////////////////////////////////////////////////
    // �ؿ�����
    SetMapLevel(30);

    SetLifeCount(1); // ��������

    // ʤ������
    ROGRegularEvent_MonsterClear* pEventMonsterClear = CnNew ROGRegularEvent_MonsterClear(this);
    AddSucceedEvent(pEventMonsterClear); // ���ȫ����

    // ʧ������
    ROGRegularEvent_PlayerDead* pEventPlayerDead = CnNew ROGRegularEvent_PlayerDead(this);
    AddFailedEvent(pEventPlayerDead); // �������


    /////////////////////////////////////////////////////////////////////////////////
    // ��11
    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3001, 4);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(1000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3004, 4);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(1000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3051, 4);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(1000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3015, 4);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetDelayTime(1000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_GREEN);
    pPointMonster->SetDelayTime(20000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(40000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 11);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3009, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 12);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3031, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 13);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3052, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);

    pPointMonster = CnNew PointMonster(this, 14);
    pPointMonster->SetRadius(200);
    pPointMonster->SetMonster(3027, 3);         // ��ʲô��(��id, ����)
    pPointMonster->SetDefaultMovie();           // ��������
    pPointMonster->SetQuality(QUALITY_BLUE);
    pPointMonster->SetDelayTime(60000);           // ��������ʱ500ms����
    pPointMonster->SetSpawnSpeed(300);          // ���ּ��300ms
    pPointMonster->SetAIGroup(12);






}