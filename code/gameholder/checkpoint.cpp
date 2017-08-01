#include "gameholder_pch.h"
#include "checkpoint.h"
#include "performance_analysis.h"

CnImplementRTTI(Checkpoint, SceneObject);

Checkpoint::Checkpoint():
SceneObject(OBJ_CHECKPOINT, false)
{

}

Checkpoint::~Checkpoint()
{

}

void Checkpoint::onEnterScene(Scene * pScene)
{
    SceneObject::onEnterScene(pScene);
}

void Checkpoint::onLeaveScene(Scene * pScene)
{
    SceneObject::onLeaveScene(pScene);
}

void Checkpoint::Update(uint32 curr_time)
{
    PERFORM_HELPER;
    SceneObject::Update(curr_time);
}