/*
 *	活动管理
 */
#ifndef activity_manager_h__
#define activity_manager_h__

#include "message_encoder.h"

class ActivityManager
{
public:
    ActivityManager();
    ~ActivityManager();

    DECLARE_SINGLETON(ActivityManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

private:
};

#endif // activity_manager_h__
