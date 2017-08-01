/*
 *	最大数量控制
 */
#ifndef spawn_controller_max_num_h__
#define spawn_controller_max_num_h__

#include "spawn_controller.h"

class SpawnController_MaxNum : public SpawnController
{
public:
    SpawnController_MaxNum(Spawn* spawn);
    virtual ~SpawnController_MaxNum();

    virtual bool Init(const SpawnWaveControlData& waveControlData);
    virtual void FirstUpdate();
    virtual void Update();

    virtual int32 FiltSpawnNum(int32 num);

    void SetMaxNum(int32 num) {m_MaxNum = num;}
    int32 GetMaxNum() {return m_MaxNum;}

private:
    int32   m_MaxNum;
};


#endif // spawn_controller_max_num_h__
