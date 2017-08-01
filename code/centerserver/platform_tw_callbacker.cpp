#include "centerserver_pch.h"
#include "platform_tw_callbacker.h"
#include "player.h"
#include "md5.h"
#include "config_loader.h"
#include "pusher_360.h"
#include "url_encode.h"

void PlatformTwCallbacker::OnChat(Player* sender, Player* recviver, int channel, const std::string& content)
{
    int32 type = 0;
    std::string toqid = "";
    std::string toname = "";

    switch (channel)
    {
    //case CHAT_CHANNEL_WORLD:
    //    type = 1;
    //    break;
    case CHAT_CHANNEL_PRIVATE:
        type = 1;
        break;
    default:
        return;
    }

    toname = ConfigLoader::Instance()->GetAnchorName();

    PushChat(sender, type, toqid, toname, content);
}

void PlatformTwCallbacker::PushChat(Player* sender, int32 type, std::string toqid, std::string toname, const std::string& content)
{
    std::string PostMsg = "";
    uint32 time = (uint32)Crown::SDNow().GetTimeValue();

    std::string serverIDStr = ConfigLoader::Instance()->GetServerName();

    std::string signStr = "";
    std::string signMd5 = "";
    std::string senderName = Crown::Base64Encode(sender->GetName());
    std::string message = Crown::Base64Encode(content);
    std::string playerAccount = sender->GetAccount();

    MD5 md5Coder;

    if(sender == NULL)
        return;

    // MD5
    signStr = Crown::ToString(type);
    signStr += serverIDStr;
    signStr += ConfigLoader::Instance()->GetTwMD5Key();
    signStr += senderName; 
    signStr += message;

    md5Coder.update(signStr);
    signMd5 = md5Coder.toString();

    PostMsg = PostMsg + "{\"sid\":\""+ serverIDStr +"\",\"type\":"+Crown::ToString(type)+",\"char\":\""+ senderName +"\",\"message\":\""+ message +"\",\"sign \":\""+ signStr +"\",\"time\":\""+Crown::ToString(time)+"\" ,\"game_account\":\""+ playerAccount +"\"}";
    PostMsg = Crown::Base64Encode(PostMsg);

    Pusher360Executor::Instance()->PushMsg(ConfigLoader::Instance()->GetTwUrl().c_str(), PUSH_TW_CHAT, PostMsg);
}
