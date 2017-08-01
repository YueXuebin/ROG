#ifndef spawn_wave_h__
#define spawn_wave_h__

#include "regular_data.h"

/*
 *	出怪点，一波怪数据结构
 */
class SpawnWave
{
public:
    SpawnWave() {}
    ~SpawnWave() {}

    std::vector<MonsterData>  waveMonsters;     // 这一波的怪物列表
    uint32  delay;                              // 延时

    uint32 GetAllMonsterNum();

};


#endif // spawn_wave_h__
