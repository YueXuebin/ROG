#include "gameholder_pch.h"
#include "spawn_controller.h"
#include "spawn.h"
#include "game_define.h"

SpawnController::SpawnController(Spawn* spawn, uint32 type) :
m_pSpawn(spawn),
m_type(type)
{

}

SpawnController::~SpawnController()
{

}
