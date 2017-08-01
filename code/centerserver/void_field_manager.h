/*
 *	虚空幻境管理
 */
#ifndef void_field_manager_h__
#define void_field_manager_h__

#include "message_encoder.h"

class VoidFieldManager
{
public:
    VoidFieldManager();
    ~VoidFieldManager();

    DECLARE_SINGLETON(VoidFieldManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

private:
    void RecvVoidFieldChannelInfoReq(void* data);
};


#endif // void_field_manager_h__
