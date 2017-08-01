#ifndef platform_callbacker_h__
#define platform_callbacker_h__

class Player;

/*
 *	平台回调接口类
 */
class PlatformCallbacker
{
public:
    PlatformCallbacker() {}
    virtual ~PlatformCallbacker() {}

    virtual void OnCreateCharacter(uint64 loginID, uint64 playerID, const std::string& uid, const std::string& uname, const PROPERTY_INFO& info) {}        // 创角回调

    virtual void OnAddPlayer(Player* player) {}
    virtual void OnDeletePlayer(Player* player) {}

    virtual void OnChat(Player* sender, Player* recviver, int channel, const std::string& content) {}

    virtual void OnGameServerPushNtf(PUSH_360_NTF* recvMsg) {}

};

bool    InitPlatformCallbacker();
void    UninitPlatformCallbacker();

PlatformCallbacker* GetPlatformCallbacker();
void SetPlatformCallbacker(PlatformCallbacker* pCallbacker);


#endif // platform_callbacker_h__
