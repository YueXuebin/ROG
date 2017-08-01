#include "centerserver_pch.h"
#include "platform_360_callbacker.h"
#include "pusher_360.h"
#include "config_loader.h"
#include "player.h"
#include "md5.h"


// 360聊天推送
void Platform360Callbacker::OnChat(Player* sender, Player* recviver, int channel, const std::string& content)
{

    int type = 0;
    std::string toqid = "";
    std::string toname = "";

    // 1 私聊；2 喇叭；3 邮件；4 世界；5 国家；6 工会/帮会；7 队伍；8 附近；9 其他
    switch(channel)
    {
        //case CHAT_CHANNEL_SYSTEM:       // 系统聊天
        //    type = 4;
        //    break;
    case CHAT_CHANNEL_WORLD:        // 世界频道
    case CHAT_CHANNEL_HORN:         // 喇叭聊天
        type = 4;
        break;
    case CHAT_CHANNEL_TEAM:         // 队伍
        type = 7;
        break;
    case CHAT_CHANNEL_PRIVATE:      // 私聊
        type = 1;
        break;
    case CHAT_CHANNEL_GUILD:        // 公会
        type = 6;
        break;
        //case CHAT_CHANNEL_MARQUEE:      // 跑马灯
        //    type = 9;
        //    break;
        //case CHAT_CHANNEL_EMERGENCY:    // 应急频道
        //    type = 9;
        //    break;
    default:
        return;
        break;
    }

    if(recviver)
    {
        toqid = recviver->GetAccount();
        toname =recviver->GetName(); 
    }

    Push360Chat(sender, type, toqid, toname, content);
}

// 推送 360 聊天信息
void Platform360Callbacker::Push360Chat(Player* sender, int type, const std::string& toqid, const std::string& toname, const std::string& content)
{
    PUSH_360_NTF sendMsg;
    uint32 time = (uint32)Crown::SDNow().GetTimeValue();

    std::string signStr = "";
    std::string signMd5 = "";

    MD5 md5Coder;

    if(sender == NULL)
        return;

    sendMsg.type = PUSH_360_CHAT;

    // sign = md5($gkey.$server_id.$qid.$name.$type.$toqid.$toname.$content.$time.$ip.$key);
    //如果post里没有的数据，在sign中也要相应的去

    // 合区的时候可能会有问题，因为当一个区一个帐号有多个角色时需要传递playerid
    signStr = ConfigLoader::Instance()->GetGameName()+ConfigLoader::Instance()->GetServerName();
    signStr += sender->GetAccount()+sender->GetName()+Crown::ToString(type);
    signStr += toqid+toname+content+Crown::ToString(time);
    signStr += Crown::GetIpStr(sender->GetLoginIP())+ConfigLoader::Instance()->GetLogin360Key();
    // 生成 MD5 码
    md5Coder.update(signStr);
    // 赋值md5码
    signMd5 = md5Coder.toString();

    // 角色信息
    sendMsg.msg += "gkey="+ConfigLoader::Instance()->GetGameName()+"&";
    sendMsg.msg += "server_id="+ConfigLoader::Instance()->GetServerName()+"&";
    sendMsg.msg += "qid="+sender->GetAccount()+"&";
    sendMsg.msg += "name="+sender->GetName()+"&";
    sendMsg.msg += "type="+Crown::ToString(type)+"&";
    sendMsg.msg += "toqid="+toqid+"&";
    sendMsg.msg += "toname="+toname+"&";
    sendMsg.msg += "content="+content+"&";
    sendMsg.msg += "time="+Crown::ToString(time)+"&";
    sendMsg.msg += "ip="+Crown::GetIpStr(sender->GetLoginIP())+"&";
    sendMsg.msg += "sign="+signMd5;

    Pusher360Executor::Instance()->PushMsg(ConfigLoader::Instance()->GetPush360ChatUrl(), sendMsg.type, sendMsg.msg);
}


void Platform360Callbacker::OnGameServerPushNtf(PUSH_360_NTF* recvMsg)
{
    if(recvMsg->type == PUSH_360_LEVEL)
    {
        Pusher360Executor::Instance()->PushMsg(ConfigLoader::Instance()->GetPush360LevelUrl(), recvMsg->type, recvMsg->msg);
    }
}
