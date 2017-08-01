/*
 *	圣光生存战管理器
 */
#ifndef city_survival_manager_gc_h__
#define city_survival_manager_gc_h__

#include "message_encoder.h"

class Player;

class CitySurvivalManager
{
public:
    CitySurvivalManager();
    ~CitySurvivalManager();

    DECLARE_SINGLETON(CitySurvivalManager)

    void OnGSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void OnTSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

private:

};

#endif // city_survival_manager_gc_h__
