#include "centerserver_pch.h"
#include "9377_login.h"
#include "player.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "player_job.h"
#include "md5.h"
#include "config_loader.h"
#include "string_parser.h"



uint16 Parse9377LoginParam(const std::string& token, std::string& uid, std::string& login_serverName);

void Verify9377ModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort)
{
    Player* player = 0;
    std::string uid;

    std::string login_serverName;
    uint16 errcode = ERR_FAILED;

    errcode = Parse9377LoginParam(token, uid, login_serverName);
    if(errcode != ERR_SUCCEED)
    {
        GateHandler::Instance()->SendLoginGameAck(errcode, gateSvrID, loginID);
        return;
    }

    if(!PlayerManager::Instance()->AddVerifyPlayer(gateSvrID, loginID, uid, addrIP, addrPort, uid, uid, ConfigLoader::Instance()->GetPlatformName(), login_serverName))
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_FAILED, gateSvrID, loginID);
        return;
    }
}

// ==================================================
// 解析 9377 登陆参数
// ==================================================
uint16 Parse9377LoginParam(const std::string& token, std::string& uid, std::string& login_serverName)
{
    std::map<std::string, std::string> paramMap;
    std::string signStr = "";
    std::string md5Str ="";
    MD5 md5Coder;
    uint32 loginTime = 0;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    // 解析参数
    std::vector<std::string> params = Crown::SDSplitStrings(token, '&');
    for(uint32 i=0; i < params.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(params[i], '=');
        if(key_value.size() == 2)
        {
            // 转小写
            CommonLib::ToLower(key_value[1]);
            paramMap[key_value[0]] = key_value[1];
        }
    }

    // 用户id
    if(paramMap.find("username") == paramMap.end())
    {
        CnWarn("9377 login token no username\n");
        return ERR_LACK_USERNAME;
    }

    uid = paramMap["username"];

    // 区服id
    if(paramMap.find("server_id") == paramMap.end())
    {
        CnWarn("9377 login token no server_id\n");
        return ERR_LACK_SERVERID;
    }

    login_serverName = paramMap["server_id"];
    login_serverName = MakeUpServerName(login_serverName);

    // 防沉迷信息
    if(paramMap.find("fcm") == paramMap.end())
    {
        CnWarn("9377 login token no fcm\n");
        return ERR_LACK_FCM;
    }

    // 登录时间
    if(paramMap.find("time") == paramMap.end())
    {
        CnWarn("9377 login token no time\n");
        return ERR_LACK_TIME;
    }

    // 校验登陆时间（同一个sign过15分钟算超时）
    loginTime = Crown::SDAtou(paramMap["time"].c_str());
    //if(now > (loginTime + 900)) // 15分钟 = 900秒
    //{
    //    CnWarn("9377 login sign time out\n");     // 暂时不校验时间
    //    return false;
    //}

    // 登录签名
    if(paramMap.find("sign") == paramMap.end())
    {
        CnWarn("9377 login token no sign\n");
        return ERR_LACK_SIGN;
    }

    // 拼接签名字符串
    signStr = "username="+ paramMap["username"];
    signStr += "&server_id="+ paramMap["server_id"] + "&key=" + ConfigLoader::Instance()->GetLogin9377Key();
    signStr += "&time=" + paramMap["time"];

    // 生成 MD5 码
    md5Coder.update(signStr);

    // 赋值md5码
    md5Str = md5Coder.toString();

    std::string sign = paramMap["sign"];
    CommonLib::ToLower(sign);                   // 转小写

    // 检查计算出的MD5与用户发来的是否相同，相同则通过校验
    if(md5Str == sign)
    {
        CnInfo("9377 login %s %s\n", login_serverName.c_str(), uid.c_str());
        return ERR_SUCCEED;
    }

    CnWarn("9377 login token sign error\n");

    return ERR_MISMATCH_SIGN;
}