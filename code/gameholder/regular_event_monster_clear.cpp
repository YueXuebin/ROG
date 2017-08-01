#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_monster_clear.h"


RegularEvent_MonsterClear::RegularEvent_MonsterClear(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_MONSTER_CLEAR)
{

}

RegularEvent_MonsterClear::~RegularEvent_MonsterClear()
{

}

void RegularEvent_MonsterClear::OneSecondUpdate( int32 deltatime )
{
    if(GetActive())
        return;

    // ����Ƿ����й��ﶼ����
    Scene* pScene = GetScene();

    if(IsSceneEnemyClear(pScene))
    {   
        SetActive(true);
    }
}
