#include "centerserver_pch.h"
#include "360_login.h"
#include "player.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "player_job.h"
#include "md5.h"
#include "config_loader.h"
#include "string_parser.h"


uint16 Parse360LoginParam(const std::string& token, std::string& uid, std::string& loginServerId);

// ==================================================
// 360登录模式
// ==================================================
void Verify360ModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort)
{
    Player* player = 0;
    std::string uid;

    std::string loginServerName;
    uint16 errcode = ERR_FAILED;

    errcode = Parse360LoginParam(token, uid, loginServerName);
    if(errcode != ERR_SUCCEED)
    {
        GateHandler::Instance()->SendLoginGameAck(errcode, gateSvrID, loginID);
        return;
    }
    
    if(!PlayerManager::Instance()->AddVerifyPlayer(gateSvrID, loginID, uid, addrIP, addrPort, uid, uid, ConfigLoader::Instance()->GetPlatformName(), loginServerName))
    {
        GateHandler::Instance()->SendLoginGameAck(ERR_FAILED, gateSvrID, loginID);
        return;
    }
}

// ==================================================
// 解析 360 登陆参数
// ==================================================
uint16 Parse360LoginParam(const std::string& token, std::string& uid, std::string& loginServerName)
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
            if(key_value[0] != "server_id")
                CommonLib::ToLower(key_value[1]);
            paramMap[key_value[0]] = key_value[1];
        }
    }

    // 用户id
    if(paramMap.find("qid") == paramMap.end())
    {
        CnWarn("360 login token no qid\n");
        return ERR_LACK_QID;
    }

    uid = paramMap["qid"];

    // 区服id
    if(paramMap.find("server_id") == paramMap.end())
    {
        CnWarn("360 login token no server_id\n");
        return ERR_LACK_SERVERID;
    }

    loginServerName = paramMap["server_id"];
    loginServerName = MakeUpServerName(loginServerName);

    // 登录时间
    if(paramMap.find("time") == paramMap.end())
    {
        CnWarn("360 login token no time\n");
        return ERR_LACK_TIME;
    }

    // 校验登陆时间（同一个sign过15分钟算超时）
    loginTime = Crown::SDAtou(paramMap["time"].c_str());
    //if(now > (loginTime + 900)) // 15分钟 = 900秒
    //{
    //    CnWarn("360 login sign time out\n");
    //    return false;
    //}

    // 登录签名
    if(paramMap.find("sign") == paramMap.end())
    {
        CnWarn("360 login token no sign\n");
        return ERR_LACK_SIGN;
    }

    // 实名制标识
    if(paramMap.find("isAdult") == paramMap.end())
    {
        CnWarn("360 login token no isAdult\n");
        return ERR_LACK_ISADULT;
    }

    // 拼接签名字符串
    signStr = "qid="+ paramMap["qid"] + "&time=" + paramMap["time"];
    signStr += "&server_id="+ paramMap["server_id"] + ConfigLoader::Instance()->GetLogin360Key();

    // 生成 MD5 码
    md5Coder.update(signStr);

    // 赋值md5码
    md5Str = md5Coder.toString();

    // 检查计算出的MD5与用户发来的是否相同，相同则通过校验
    if(md5Str == paramMap["sign"])
    {
        CnInfo("360 login %s %s\n", loginServerName.c_str(), uid.c_str());
        return ERR_SUCCEED;
    }

    CnWarn("360 login token sign error\n");

    return ERR_MISMATCH_SIGN;
}
