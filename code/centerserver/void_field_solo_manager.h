/*
 *	虚空幻境solo管理
 */
#ifndef void_field_solo_manager_h__
#define void_field_solo_manager_h__

#include "message_encoder.h"

class VoidFieldSoloManager
{
public:
    VoidFieldSoloManager();
    ~VoidFieldSoloManager();

    DECLARE_SINGLETON(VoidFieldSoloManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

private:
    void RecvVoidFieldChannelInfoReq(void* data);
};


#endif // void_field_solo_manager_h__
