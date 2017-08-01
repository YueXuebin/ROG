/*
 *	√ø≤®—” ±
 */
#ifndef spawn_controller_wave_delay_h__
#define spawn_controller_wave_delay_h__

#include "spawn_controller.h"

class SpawnController_WaveDelay : public SpawnController
{
public:
    SpawnController_WaveDelay(Spawn* spawn);
    virtual ~SpawnController_WaveDelay();

    virtual bool Init(const SpawnWaveControlData& waveControlData);
    virtual void FirstUpdate();
    virtual void Update();

    virtual int32 FiltSpawnNum(int32 num);

private:
    void    EnterDelay();
    void    LeaveDelay();

private:
    int32   m_DelayTime;
    int32   m_DelayTimer;

    bool    m_InDelayState;
    int32   m_DelayedWaveIndex;
};

#endif // spawn_controller_wave_delay_h__
