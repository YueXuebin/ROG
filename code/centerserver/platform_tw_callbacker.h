// 台湾平台接口
#ifndef platform_tw_callbacker_h__
#define platform_tw_callbacker_h__

#include "platform_callbacker.h"

class PlatformTwCallbacker : public PlatformCallbacker
{
public:
    PlatformTwCallbacker() {}
    virtual ~PlatformTwCallbacker() {}

    virtual void OnChat(Player* sender, Player* recviver, int channel, const std::string& content);
    void PushChat(Player* sender, int32 type, std::string toqid, std::string toname, const std::string& content);
};


#endif // platform_tw_callbacker_h__
