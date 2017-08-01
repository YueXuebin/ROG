/*
 *	怪物生成器
 */
#ifndef spawn_generator_h__
#define spawn_generator_h__

#include "regular_data.h"

class Spawn;

class SpawnController
{
public:
    SpawnController(Spawn* spawn, uint32 type);
    virtual ~SpawnController();

    virtual bool Init(const SpawnWaveControlData& waveControlData) = 0;

    virtual void FirstUpdate() = 0;
    virtual void Update() = 0;

    virtual int32 FiltSpawnNum(int32 num) = 0;

protected:
    Spawn*  m_pSpawn;
    uint32  m_type;

    
};


#endif // spawn_generator_h__
