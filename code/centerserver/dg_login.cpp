#include "centerserver_pch.h"
#include "player_manager.h"
#include "player_job.h"
#include "config_loader.h"
#include "string_parser.h"


std::string filterAccount(std::string& old_accout)
{
    int32 p = old_accout.find("&");
    if(p < 0)
        return old_accout;

    CnDbg("filter account old:%s\n", old_accout.c_str());

    p = old_accout.find("username");
    std::string new_account = old_accout.substr(p+9);
    p = new_account.find("&");
    new_account = new_account.substr(0, p);

    CnDbg("filter account new:%s\n", new_account.c_str());

    return new_account;
}

// ==================================================
// 验证测试模式登录
// ==================================================
void VerifyTestModeLogin(uint32 centGateID, uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort)
{
    std::string account = "";
    std::string login_serverName = "";

    std::map<std::string, std::string> paramMap;
    std::vector<std::string> params = Crown::SDSplitStrings(token, '&');
    for(uint32 i=0; i < params.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(params[i], '=');
        if(key_value.size() == 2)
        {
            paramMap[key_value[0]] = key_value[1];
        }
    }

    // 用户id
    if(paramMap.find("username") == paramMap.end())
    {
        CnWarn("test mode login token no username\n");
        return ;
    }

    account = paramMap["username"];

    // 区服id
    if(paramMap.find("server_id") == paramMap.end())
    {
        CnWarn("test mode login token no server_id\n");
        return ;
    }

    login_serverName = paramMap["server_id"];
    login_serverName = MakeUpServerName(login_serverName);



    CnDbg("test mode login: server_name=%s account=%s\n", login_serverName.c_str(), account.c_str());


    // 账号转小写
    CommonLib::ToLower(account);

    // 测试模式默认验证通过
    if(!PlayerManager::Instance()->AddVerifyPlayer(centGateID, loginID, account, 
        addrIP, addrPort, account, account, ConfigLoader::Instance()->GetPlatformName(), login_serverName))
    {
        return;
    }
}

// ==================================================
// 验证AF模式登录
// ==================================================
void VerifyAfModeLogin(uint32 gateSvrID,  uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort)
{
    // AF 登陆模式
    PlayerJob::Instance()->LoginLoadAfAccount(gateSvrID, loginID, token, addrIP, addrPort);
}

void OnVerifyAfModeLogin( uint32 gateSvrID, uint64 loginID, const std::string& account, uint32 addrIP, uint16 addrPort )
{
    std::string login_serverName = "";      // 动艺官网默认是无serverName

    if(!PlayerManager::Instance()->AddVerifyPlayer(gateSvrID, loginID, account, 
        addrIP, addrPort, "", "", ConfigLoader::Instance()->GetPlatformName(), login_serverName))
    {
        return;
    }
}

