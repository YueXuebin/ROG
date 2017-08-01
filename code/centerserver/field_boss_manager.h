/*
 *	野外Boss管理
 */
#ifndef field_boss_manager_h__
#define field_boss_manager_h__

#include "message_encoder.h"

class FieldBossManager
{
public:
    FieldBossManager();
    ~FieldBossManager();

    DECLARE_SINGLETON(FieldBossManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

private:
    void RecvFieldBossInfoReq(void* data);
    void RecPlayerIsOnLine(void* data); //玩家是否下线的判断
};


#endif // field_boss_manager_h__
