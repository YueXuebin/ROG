#ifndef platform_kugou_callbacker_h__
#define platform_kugou_callbacker_h__

#include "platform_callbacker.h"

class PlatformKugouCallbacker : public PlatformCallbacker
{
public:
    PlatformKugouCallbacker() {}
    virtual ~PlatformKugouCallbacker() {}

    virtual void OnCreateCharacter(uint64 loginID, uint64 playerID, const std::string& uid, const std::string& uname, const PROPERTY_INFO& info);        // 创角回调


    virtual void OnDeletePlayer(Player* player);
};


#endif // platform_kugou_callbacker_h__
