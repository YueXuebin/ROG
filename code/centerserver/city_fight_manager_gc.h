/*
 *	城市争夺战管理器
 */
#ifndef city_fight_manager_gc_h__
#define city_fight_manager_gc_h__

#include "message_encoder.h"

class Player;

class CityFightManager
{
public:
    CityFightManager();
    ~CityFightManager();

    DECLARE_SINGLETON(CityFightManager)

    void Init();

    void InitFinish();

    void OnGSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void OnTSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    CITY_FIGHT_INFO_NTF& GetCityFightSaveInfo();

private:
    CITY_FIGHT_INFO_NTF m_city_fight_info;

    bool m_init_finish;

};

#endif // city_fight_manager_gc_h__
