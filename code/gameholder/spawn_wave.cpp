#include "gameholder_pch.h"
#include "spawn_wave.h"


uint32 SpawnWave::GetAllMonsterNum()
{
    uint32 allMonsterNum = 0;
    for(std::vector<MonsterData>::iterator iter = waveMonsters.begin(); iter != waveMonsters.end(); iter++)
    {
        allMonsterNum += iter->num;
    }
    return allMonsterNum;
}
