#include "gameholder_pch.h"
#include "regular_multiple.h"

CnImplementRTTI(RegularMultiple, RegularDungeon);

RegularMultiple::RegularMultiple(RegularDoc* pDoc) :
RegularDungeon(pDoc)
{

}

RegularMultiple::~RegularMultiple()
{

}

bool RegularMultiple::IsGameStart()
{
    return IsAnyoneStart();     // ����ҿ��ƣ���ʼ�ͽ���
}
