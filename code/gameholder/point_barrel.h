#ifndef point_box_h__
#define point_box_h__

#include "point_base.h"
#include "spawn.h"

class Spawn;
class SpawnController_Speed;
class SpawnController_MaxNum;

/*
 *	木桶(特殊小怪)
 */
class PointBarrel : public PointBase
{
    CnDeclareRTTI
public:
    PointBarrel(RegularBuilder* pRogRegular, uint32 CheckPointID);
    virtual ~PointBarrel();

    virtual bool Build();
    virtual void Update(uint32 curr_tick) {}

    void SetRadius(int32 max_radius, int32 min_radius = 0);     // 出木桶范围
    void SetLoop(bool loop);                                    // 循环出木桶
    void SetNumber(int32 num);                                  // 设置木桶数量
    Spawn* GetSpawn() { return m_pSpawn; }

protected:
    virtual void OnEventTrigger();

private:
    Spawn*  m_pSpawn;
    SpawnController_Speed*  m_pSpeedController;
    SpawnController_MaxNum* m_pMaxNumController;
};

#endif // point_box_h__
