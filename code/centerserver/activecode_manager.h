/*
*   组队管理器
*/

#ifndef activation_code_manager_h__
#define activation_code_manager_h__

#include "message_encoder.h"

class Player;

class ActiveCodeManager
{
public:
    ActiveCodeManager();
    ~ActiveCodeManager();

    DECLARE_SINGLETON(ActiveCodeManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);       ///< 接收网络消息

private:
    void OnActiveCodeUseReq(uint64 playerID, void* data);                       ///< 使用激活码获得物品
};


#endif // team_manager_h__
