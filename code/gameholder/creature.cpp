#include "gameholder_pch.h"
#include "creature.h"
#include "action_creature_controller.h"
#include "prop_notifier.h"
#include "scene.h"
#include "player.h"

CnImplementRTTI(Creature, BattleUnit);

Creature::Creature(uint32 objType) :
BattleUnit(objType)
{
    CreatureClear();
}

Creature::~Creature()
{

}

void Creature::onEnterScene( Scene* pScene )
{
    BattleUnit::onEnterScene(pScene);
}

void Creature::onLeaveScene( Scene* pScene )
{
    BattleUnit::onLeaveScene(pScene);
}

void Creature::Update(uint32 frame_count)
{
    BattleUnit::Update(frame_count);
}

void Creature::Bounce(int32 height, int32 time, bool exclude_me)
{
    BOUNCE_NTF ntf;
    ntf.object_id = GetId();
    ntf.height = height;
    ntf.time = time;

    if(exclude_me && GetObjectType() == OBJ_PLAYER)
        GetScene()->NotifyMessage(this, MSG_BOUNCE_NTF, &ntf, true);
    else
        GetScene()->NotifyMessage(this, MSG_BOUNCE_NTF, &ntf);
}
