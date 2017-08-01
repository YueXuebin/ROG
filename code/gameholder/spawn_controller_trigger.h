/*
 *	´¥·¢Æ÷´¥·¢
 */
#ifndef spawn_controller_trigger_h__
#define spawn_controller_trigger_h__

#include "spawn_controller.h"

class SpawnController_Trigger : public SpawnController
{
public:
    SpawnController_Trigger(Spawn* spawn);
    virtual ~SpawnController_Trigger();

    virtual bool Init(const SpawnWaveControlData& waveControlData);
    virtual void FirstUpdate();
    virtual void Update();

    virtual int32 FiltSpawnNum(int32 num);


private:
    uint32  m_TriggerId;
    bool    m_Active;
};

#endif // spawn_controller_trigger_h__
