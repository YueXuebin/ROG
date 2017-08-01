/*
 *	等一波死完再出一波
 */
#ifndef spawn_controller_wait_wave_dead_h__
#define spawn_controller_wait_wave_dead_h__


#include "spawn_controller.h"

class SpawnController_WaitWaveDead : public SpawnController
{
public:
    SpawnController_WaitWaveDead(Spawn* spawn);
    virtual ~SpawnController_WaitWaveDead();

    virtual bool Init(const SpawnWaveControlData& waveControlData);
    virtual void FirstUpdate();
    virtual void Update();

    virtual int32 FiltSpawnNum(int32 num);

private:
    bool    m_IsWaitDead;
};



#endif // spawn_controller_wait_wave_dead_h__
