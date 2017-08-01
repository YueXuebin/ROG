#include "gameholder_pch.h"
#include "regular_field_003.h"
#include "scene.h"
#include "checkpoint.h"
#include "event_define.h"
#include "point_monster.h"
#include "point_touch.h"
#include "point_treasure.h"
#include "point_barrel.h"

RegularField_003::RegularField_003(RegularDoc* pDoc) :
RegularFieldBase(pDoc)
{

}

RegularField_003::~RegularField_003()
{

}

void RegularField_003::FirstUpdate()
{
    RegularFieldBase::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
    PointTreasure* pPointTreasure = NULL;
    PointBarrel* pPointBarrel = NULL;
    PointTouch* pPointTouch = NULL;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ؿ�����
	SetMapLevel(5);



	// ��11
	pPointMonster = CnNew PointMonster(this, 11);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3721, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms


	// ��12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3722, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(2000);
	pPointMonster->SetMonster(3723, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3724, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3725, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��16
	pPointMonster = CnNew PointMonster(this, 16);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3726, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLevel(10);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��17
	pPointMonster = CnNew PointMonster(this, 17);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3727, 10);         // ��ʲô��(��id, ����)
	pPointMonster->SetLevel(10);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��18
	pPointMonster = CnNew PointMonster(this, 18);
	pPointMonster->SetRadius(1500);
	pPointMonster->SetMonster(3728, 10);         // ��ʲô��(��id, ����)	
	pPointMonster->SetLevel(10);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��19
	pPointMonster = CnNew PointMonster(this, 19);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3729, 10);         // ��ʲô��(��id, ����)	
	pPointMonster->SetLevel(10);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��20
	pPointMonster = CnNew PointMonster(this, 20);
	pPointMonster->SetRadius(1000);
	pPointMonster->SetMonster(3730, 10);         // ��ʲô��(��id, ����)	
	pPointMonster->SetLevel(10);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(10);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetSpawnSpeed(3000);          // ���ּ��300ms	

	// ��23
	pPointMonster = CnNew PointMonster(this, 23);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3787, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetLevel(20);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetQuality(QUALITY_BLUE);    // ����Ʒ��
	//pPointMonster->SetDelayTime(5000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(180000);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 ����
	//pPointMonster->SetAIGroup(1);

	// ��22
	pPointMonster = CnNew PointMonster(this, 22);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3788, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetLevel(15);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetQuality(QUALITY_GREEN);    // ����Ʒ��
	//pPointMonster->SetDelayTime(5000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(180000);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 ����
	//pPointMonster->SetAIGroup(1);

	// ��21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(100);
	pPointMonster->SetMonster(3789, 1);         // ��ʲô��(��id, ����)
	pPointMonster->SetLevel(15);                  // ����ȼ� 4��
	pPointMonster->SetLoop(true);
	pPointMonster->SetMax(1);
	pPointMonster->SetDefaultMovie();           // ��������
	pPointMonster->SetQuality(QUALITY_GREEN);    // ����Ʒ��
	//pPointMonster->SetDelayTime(5000);           // ��������ʱ500ms����
	pPointMonster->SetSpawnSpeed(180000);          // ���ּ��300ms
	//pPointMonster->SetTriggerID(11);             // trigger 2 ����
	//pPointMonster->SetAIGroup(1);


/*

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 51);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3804, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 52);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 53);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 54);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 55);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 56);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 57);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3806, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 58);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3805, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);

	// ��2��npc1
	pPointMonster = CnNew PointMonster(this, 59);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMax(1);
	pPointMonster->SetMonster(3805, 1);
	pPointMonster->SetGroup(FRIEND_GROUP);
	pPointMonster->SetTalk(true);
	pPointMonster->SetCantMove(true);
	pPointMonster->SetAlwaysShow(true);
*/




}