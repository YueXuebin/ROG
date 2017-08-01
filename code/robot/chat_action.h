#pragma once

#include "action_base.h"

class ChatAction :public ActionBase
{
public:
    ChatAction(Robot* pRobot, int32 change, int32 dstName, int32 chat);
    ~ChatAction(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    int32        m_channel;
    std::string  m_dst_name;
    std::string  m_chat;

};
