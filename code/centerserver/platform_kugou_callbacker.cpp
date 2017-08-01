#include "centerserver_pch.h"
#include "platform_kugou_callbacker.h"
#include "pusher_360.h"
#include "kugou_login.h"
#include "player.h"



void PlatformKugouCallbacker::OnCreateCharacter(uint64 loginID, uint64 playerID, const std::string& uid, const std::string& uname, const PROPERTY_INFO& info)
{
    std::string kgext = GetKugouKgext(loginID);
    if(kgext.length() == 0)
    {
        CnWarn("player %s create no kugou kgext\n", info.name.c_str());
        return;
    }

    uint32 time = (uint32)Crown::SDNow().GetTimeValue();

    CnDbg("kugou create character uid=%s serverid=%d roleid=%llu rolename=%s\n", uid.c_str(), info.region, playerID, info.name.c_str());

    //std::string url = "http://cp.game.kugou.com/statistic/create_role_test.php";      // 测试地址
    std::string url = "http://cp.game.kugou.com/statistic/create_role.html";        // 正式地址
    url += "?";
    url += "gameid=231";
    url += "&uid="+uid;
    url += "&serverid="+Crown::ToString(info.region);
    url += "&roleid="+Crown::ToString(playerID);
    url += "&rolename="+info.name;
    url += "&time="+Crown::ToString(time);
    url += "&kgext="+Crown::UrlEncode(kgext);
    CnDbg("%s\n", url.c_str());

    Pusher360Executor::Instance()->PushMsg(url, PUSH_KUGOU_CREATE_CHARACTER, "", true);
}

void PlatformKugouCallbacker::OnDeletePlayer(Player* player)
{
    RemoveKugouKgext(player->GetLoginID());
}
