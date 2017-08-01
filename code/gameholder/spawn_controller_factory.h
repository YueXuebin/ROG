#ifndef spawn_controller_factory_h__
#define spawn_controller_factory_h__

#include "regular_data.h"

class SpawnController;
class Spawn;

SpawnController* CreateSpawnController(Spawn* pSpawn, const SpawnWaveControlData& waveControlData);


#endif // spawn_controller_factory_h__
