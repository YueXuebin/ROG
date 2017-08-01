#ifndef platform_360_callbacker_h__
#define platform_360_callbacker_h__

#include "platform_callbacker.h"

class Platform360Callbacker : public PlatformCallbacker
{
public:
    Platform360Callbacker() {}
    virtual ~Platform360Callbacker() {}


    virtual void OnChat(Player* sender, Player* recviver, int channel, const std::string& content);

    virtual void OnGameServerPushNtf(PUSH_360_NTF* recvMsg);

private:
    void Push360Chat(Player* sender, int type, const std::string& toqid, const std::string& toname, const std::string& content);
};


#endif // platform_360_callbacker_h__
