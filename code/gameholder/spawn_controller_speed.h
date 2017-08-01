/*
 *	出怪速度控制器
 */
#ifndef spawn_controller_speed_h__
#define spawn_controller_speed_h__

#include "spawn_controller.h"

class SpawnController_Speed : public SpawnController
{
public:
    SpawnController_Speed(Spawn* spawn);
    virtual ~SpawnController_Speed();

    virtual bool Init(const SpawnWaveControlData& waveControlData);
    virtual void FirstUpdate();
    virtual void Update();

    virtual int32 FiltSpawnNum(int32 num);

    void SetSpeedTime(int32 speed) {m_SpeedTime = speed;}
    int32 GetSpeedTime() {return m_SpeedTime;}

private:
    int32   m_SpeedTime;        // 多少毫秒出一个
    int32   m_UpdateTime;
};

#endif // spawn_controller_speed_h__
