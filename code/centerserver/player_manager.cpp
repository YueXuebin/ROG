#include "centerserver_pch.h"
#include "player_manager.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player.h"
#include "player_net_handler.h"
#include "player_config.h"
#include "config_loader.h"
#include "team_manager.h"
#include "cross_team_manager.h"
#include "dungeon_manager.h"
#include "dbjob.h"
#include "platform_callbacker.h"


// 用账号和服务器名,生成唯一名字
std::string MakeUnionName(const std::string& account, const std::string& loginServerName)
{
    return loginServerName+"&"+account;
}

IMPLEMENT_SINGLETON(PlayerManager)

PlayerManager::PlayerManager()
{
    m_LogCounter = 0;
}

PlayerManager::~PlayerManager()
{

}

void PlayerManager::Update()
{
    for(std::map<uint64, Player*>::iterator it=m_players.begin(); it != m_players.end(); ++it)
    {
        Player* player = it->second;
        player->Update();
    }

    m_LogCounter++;
    
#ifdef WIN
    const uint32 timer = g_ServerUpdateFrequency * 5;
#else
    const uint32 timer = g_ServerUpdateFrequency * 60;
#endif
    if(m_LogCounter >= timer)
    {
        CnDbg("playernum = %d %d\n", GetPlayerNum(), DungeonManager::Instance()->GetPlayerNum());
        m_LogCounter = 0;
    }
}

Player* PlayerManager::GetPlayerByPlayerID(uint64 id)
{
    std::map<uint64, Player*>::iterator it;
    if((it = m_players.find(id)) == m_players.end())
    {
        return NULL;
    }
    return it->second;
}

Player* PlayerManager::GetPlayerByAccount(const std::string& account, const std::string& loginServerName)
{
    std::map<std::string, Player*>::iterator it;
    if((it = m_accountMap.find(MakeUnionName(account, loginServerName))) == m_accountMap.end())
    {
        return NULL;
    }
    return it->second;
}

Player* PlayerManager::GetPlayerByName(const std::string& name)
{
    for(std::map<uint64, Player*>::iterator it=m_players.begin(); it != m_players.end(); ++it)
    {
        if((it->second->GetName() == name) && it->second->GetState() == Player::kStateInGame)
        {
            return it->second;
        }
    }
    return NULL;
}

Player* PlayerManager::GetPlayerByLoginID(uint64 loginID)
{
    std::map<uint64, Player*>::iterator it = m_loginIDMap.find(loginID);
    if(it == m_loginIDMap.end())
    {
        return NULL;
    }
    return it->second;
}

// ==================================================
// 创建角色唯一ID(player id)
// |------------高32位-------------|----低前16位-----|----低后16位-----|
//              时间戳                   序列号         CenterServerID  
// ==================================================
uint64 PlayerManager::GenPlayerUUID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetCenterID() & 0xFFFF;   // CenterServerID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

// ==================================================
// 检查重复登录
// ==================================================
bool PlayerManager::CheckDuplicateLogin(uint32 gateID, uint64 loginID, const std::string& account, const std::string& loginServerName)
{
    Player* player = NULL;
    char idstr[30] = {0};

    // 检查服务器上是否已存在同名账户
    player = GetPlayerByAccount( account, loginServerName );
    if(player)
    {
        // 有同账户角色在踢下线
        Crown::SDUtoa64<30>(idstr, loginID);
        CnDbg("Player repeat login: loginID:%s, loginServerName:%s, account:%s\n", idstr, loginServerName.c_str(), account.c_str());

        GateHandler::Instance()->SendLoginGameAck(ERR_REPEAT_LOGIN, gateID, loginID);       // 通知新登陆的客户端，重复登陆
        GateHandler::Instance()->SendLoginGameAck(ERR_REPEAT_LOGIN, player->GetCentGateID(), player->GetLoginID());      // 通知已经登陆的客户端，重复登陆

        player->GetNetHandler()->OnLoginRepeat();

        return false;
    }

    return true;
}
// ==================================================
// 添加验证玩家
// ==================================================
bool PlayerManager::AddVerifyPlayer(uint32 centGateID, uint64 loginID, const std::string& origin_account, 
                                        uint32 addrIP, uint32 addrPort, 
                                        const std::string& uid, const std::string& uname, const std::string& platform, const std::string& loginServerName)
{
    Player* player = NULL;

    std::string account = Crown::SDStrTrim(origin_account);            // 去掉账号前后的空格

    // 检查重复登录
    if(!CheckDuplicateLogin(centGateID, loginID, account, loginServerName))
    {
        CnInfo("player %s duplicate login\n", account.c_str());
        return false;
    }

    int32 region_id = ServerNameToRegionID(loginServerName);        // 根据服务器名算区号(服务器名 S342 -> 区号 342)

    player = CnNew Player;
    player->Clear();

    player->SetCentGateID(centGateID);           // 设置RegionID
    player->SetLoginID(loginID);                // 设置登陆ID
    player->SetAccount(account);                // 设置账号
    player->SetPlatformUID(uid);                // 平台账户ID
    player->SetPlatformUName(uname);            // 平台账户名
    player->SetPlatform(platform);              // 平台名
    player->SetPlatformServerName(loginServerName);         // 平台登陆服务器名
    player->SetRegion(region_id);               // 设置区号
    player->SetLoginIP(addrIP);                 // 地址IP
    player->SetLoginPort(addrPort);             // 地址Port
    player->GetNetHandler()->OnVerify();        // 设置验证状态

    if(!PlayerManager::Instance()->AddPlayerForLogin(player))
    {
        CnInfo("player %s add for login failed\n", account.c_str());
        // 发送登陆失败的消息
        GateHandler::Instance()->SendLoginGameAck(ERR_FAILED, centGateID, loginID);
        // 删除角色数据
        CnDelete player;
        player = nullptr;
        return false;
    }
    else
    {
        CnInfo("player %s %s login\n", loginServerName.c_str(), account.c_str());
    }

    // 登陆成功通知
    GateHandler::Instance()->SendLoginGameAck(ERR_SUCCEED, centGateID, loginID);

    return true;
}

bool PlayerManager::AddPlayerForLogin(Player* player)
{
    if(!player)
    {
        CnAssert(false);
        return false;
    }

    if(m_loginIDMap.find(player->GetLoginID()) != m_loginIDMap.end())
    {
        CnDbg("player %s loginID already exist\n", player->GetAccount().c_str());
        // 有相同LoginID的玩家存在
        CnAssert(false);
        return false;
    }

    if(m_accountMap.find( MakeUnionName(player->GetAccount(), player->GetPlatformServerName()) ) != m_accountMap.end())
    {
        // 有相同账户的玩家存在
        CnDbg("player loginServerName:%s,account:%s already exist\n", player->GetPlatformServerName().c_str(), player->GetAccount().c_str());
        CnAssert(false);
        return false;
    }

    GetPlatformCallbacker()->OnAddPlayer(player);

    m_loginIDMap[player->GetLoginID()] = player;
    m_accountMap[ MakeUnionName(player->GetAccount(), player->GetPlatformServerName()) ] = player;

    CnDbg("AddPlayerForLogin server_name=%s account=%s\n", player->GetPlatformServerName().c_str(), player->GetAccount().c_str());

    CnAssert(m_accountMap.size() == m_loginIDMap.size());

    return true;
}

bool PlayerManager::AddPlayerForLoaded(Player* player)
{
    if(!player)
    {
        CnAssert(false);
        return false;
    }

    if(m_players.find(player->GetPlayerID()) != m_players.end())
    {
        // 有相同PlayerID的玩家存在
        CnAssert(false);
        return false;
    }

    if(m_nameMap.find(player->GetName()) != m_nameMap.end())
    {
        // 有相同名字的玩家存在
        CnAssert(false);
        return false;
    }

    m_players[player->GetPlayerID()] = player;
    m_nameMap[player->GetName()] = player;

    CnAssert(m_players.size() == m_nameMap.size());

	CnDbg("AddPlayer %s\n", player->GetName().c_str());

    return true;
}

void PlayerManager::DelPlayer(uint64 playerID)
{
    CheckError();

    std::map<uint64, Player*>::iterator it = m_players.find(playerID);
    if(it == m_players.end())
    {
        return;
    }

    Player* delPlayer = it->second;

    CnAssert(m_nameMap.find(delPlayer->GetName()) != m_nameMap.end());
    m_nameMap.erase(delPlayer->GetName());
	
	CnDbg("DelPlayer %s\n", delPlayer->GetName().c_str());

    CnAssert(m_accountMap.find( MakeUnionName( delPlayer->GetAccount(), delPlayer->GetPlatformServerName() )) != m_accountMap.end());
    m_accountMap.erase( MakeUnionName(delPlayer->GetAccount(), delPlayer->GetPlatformServerName()) );

    CnAssert(m_loginIDMap.find(delPlayer->GetLoginID()) != m_loginIDMap.end());
    m_loginIDMap.erase(delPlayer->GetLoginID());

    GetPlatformCallbacker()->OnDeletePlayer(delPlayer);

    CnDelete delPlayer;
    m_players.erase(it);
}

void PlayerManager::DelPlayerByLoginID(uint64 loginID)
{
    Player* delPlayer = NULL;

    std::map<uint64, Player*>::iterator loginIt = m_loginIDMap.find(loginID);
    if(loginIt == m_loginIDMap.end())
    {
        CnAssert(false);
        return;
    }

    delPlayer = loginIt->second;

    if(delPlayer->GetState() > Player::kStateVerifyOk)
        CheckError();

    m_loginIDMap.erase(delPlayer->GetLoginID());

    GetPlatformCallbacker()->OnDeletePlayer(delPlayer);

    CnAssert(m_accountMap.find( MakeUnionName(delPlayer->GetAccount(), delPlayer->GetPlatformServerName()) ) != m_accountMap.end());
    m_accountMap.erase( MakeUnionName(delPlayer->GetAccount(), delPlayer->GetPlatformServerName()) );

    if(delPlayer->GetState() <= Player::kStateVerifyOk)
    {
        goto _END;
    }

    CnAssert(m_nameMap.find(delPlayer->GetName()) != m_nameMap.end());
    m_nameMap.erase(delPlayer->GetName());
    CnAssert(m_players.find(delPlayer->GetPlayerID()) != m_players.end());
    m_players.erase(delPlayer->GetPlayerID());

_END:
    CnDelete delPlayer;
    delPlayer = 0;
}

void PlayerManager::CheckError()
{
    // 逻辑错误检测
#ifdef DEBUG
    TeamManager::Instance()->ValidCheck();

    CnAssert(m_accountMap.size() == m_loginIDMap.size());

    if(m_players.size() != m_nameMap.size())
    {
        CnAssert(false);
        for(std::map<std::string, Player*>::iterator diter = m_nameMap.begin(); diter != m_nameMap.end(); diter++)
        {
            Player* player = diter->second;
            std::map<uint64, Player*>::iterator diter2 = m_players.find(player->GetPlayerID());
            if(diter2 == m_players.end())
            {
                CnAssert(false);
            }
        }
    }
#endif
}

uint32 PlayerManager::GetAccountNum()
{
    return m_accountMap.size();
}

uint32 PlayerManager::GetPlayerNum()
{
    return m_players.size();
}

void PlayerManager::ChangePlayerName(uint64 playerID, const std::string& newName)
{
    Player* player = 0;
    std::map<std::string, Player*>::iterator it;

    player = GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(player->GetState() != Player::kStateInGame)
    {
        GameHandler::Instance()->SendChangePlayerNameAck(player->GetCentGameID(), playerID, ERR_FAILED, newName);
        return;
    }

    std::string oldName = player->GetName();
    player->SetName(newName);

    it = m_nameMap.find(oldName);
    if(it != m_nameMap.end())
    {
        m_nameMap.erase(it);
    }
    else
        CnAssert(false);

    CnAssert(m_nameMap.find(newName) == m_nameMap.end());       // 该名字不存在
    m_nameMap[newName] = player;
}

// 重新加载角色充值信息
void PlayerManager::ReloadPlayerRechargeInfo(const std::string& account, const std::string& loginServerName)
{
    Player* player = NULL;

    player = GetPlayerByAccount(account, loginServerName);   // 需要平台名 必须改
    if(!player)
    {
        return;
    }

    PayJob::Instance()->ReloadRechargeInfo(player->GetPlayerID());
}

void PlayerManager::AddSystemMail(MAIL_INFO& mail_info)
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second)
        {
            it->second->AddMail(mail_info);
        }
    }
}

void PlayerManager::DelExpiredMail(uint32 mail_id_h)
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if(it->second)
        {
            it->second->DelExpiredMail(mail_id_h);
        }
    }
}

// ==================================================
// 通知GameServer邮件
// ==================================================
void PlayerManager::SendGameMail(MAIL_INFO& mail_info)
{
    for(std::map<uint64, Player*>::iterator playerIt = m_players.begin();
        playerIt != m_players.end(); ++playerIt)
    {
        if(playerIt->second)
        {
            playerIt->second->SendToGame(MSG_MAIL_INFO_NTF, &mail_info);
        }
    }
}

void PlayerManager::LogoutGame(Player* player)
{ 
    if(!player)
    {
        CnWarn("player logout game not exist\n");
        return;
    }

    CnInfo("logout player %s %s %d\n", player->GetPlatformServerName().c_str(), player->GetAccount().c_str(), player->GetState());
    
    player->GetNetHandler()->OnLogout();
}

void PlayerManager::GetQuickFriendList(std::list<Player*>& list, Player* player)
{
    for(std::map<uint64, Player*>::iterator it = m_players.begin(); 
        (it != m_players.end()) && (list.size() < QUICK_ADD_FRIEND_COUNT); ++it)
    {
        if((it->first == player->GetPlayerID()) || (it->second->GetState() != Player::kStateInGame)) continue;
        if(player->HasFriend(it->second->GetName())) continue;
        list.push_back(it->second);
    }
}

void PlayerManager::OnCloseGateServer(uint32 gateSvrID)       // gateSvrID其实是RegionID
{
    std::list<Player*> list;
    Player* player = NULL;

    // 找到该Gate上的玩家
    for(std::map<uint64, Player*>::iterator it=m_players.begin(); it != m_players.end(); ++it)
    {
        Player* closePlayer = it->second;
        if(closePlayer->GetCentGateID() == gateSvrID)
        {
            list.push_back(closePlayer);
        }
    }

    // 如果在玩家创建角色时,gate断开,会执行如下代码。以确保对象正确释放
    if(m_loginIDMap.size() > m_players.size())
    {
        for(std::map<uint64, Player*>::iterator loginIt=m_loginIDMap.begin(); loginIt != m_loginIDMap.end(); ++loginIt)
        {
            player = loginIt->second;
            for(std::map<uint64, Player*>::iterator playerIt=m_players.begin(); playerIt != m_players.end(); ++playerIt)
            {
                if(loginIt->second == playerIt->second)
                {
                    player = NULL;
                    break;
                }
            }

            if(player)
            {   // 该player对象，只在m_loginIDMap中存在
                CnInfo("player %s no add but gate down\n", player->GetName().c_str());
                list.push_back(player);
            }
        }
    }
    else
        CnAssert(m_loginIDMap.size() == m_players.size());      // m_loginIDMap.size() 不可能 < m_players.size()

    // 该玩家下线处理
    for(std::list<Player*>::iterator it=list.begin(); it != list.end(); ++it)
    {
        LogoutGame(*it);
    }
}

void PlayerManager::OnCloseGameServer(uint32 gameSvrID)
{
    // 断开该GameServer上的玩家
    std::list<Player*> list;

    for(std::map<uint64, Player*>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        Player* closePlayer = it->second;
        if(CalGameIDByRegionID(closePlayer->GetCentGameID()) == gameSvrID)
        {
            if(closePlayer->m_InDungeonCheck)
                DungeonManager::Instance()->DecreasePlayer( DUNGEON_ID(closePlayer->GetUnionMapID()), closePlayer );

            closePlayer->GetNetHandler()->KickOut(ERR_GAMESVR_CLOSE);
            list.push_back(closePlayer);

            //CnAssert(false);            // GS非安全退出
        }
    }

    // 删除玩家对象
    for(std::list<Player*>::iterator it2=list.begin(); it2 != list.end(); ++it2)
    {
        Player* delPlayer = *it2;

        TeamManager::Instance()->LeaveTeam(delPlayer->GetPlayerID());       // 玩家下线离队

        CrossTeamManager::Instance()->LeaveTeam(delPlayer->GetPlayerID());  // 玩家下线离队

        PlayerJob::Instance()->SavePlayerDataAndDestory(delPlayer);         // 保存该玩家数据
    }
}

void PlayerManager::ReMapPlayerAccount( Player* player, const std::string& oldAccount, const std::string& newAccount )
{
    if(oldAccount == newAccount)
        return;

    CnAssert(player);
    CnAssert(oldAccount.length() > 0);
    CnAssert(newAccount.length() > 0);

    std::map<std::string, Player*>::iterator oldIter = m_accountMap.find( MakeUnionName(oldAccount, player->GetPlatformServerName()) );
    std::map<std::string, Player*>::iterator newIter = m_accountMap.find( MakeUnionName(newAccount, player->GetPlatformServerName()) );
    CnAssert(oldIter != m_accountMap.end());
    CnAssert(oldIter->second == player);

    m_accountMap.erase(oldIter);
    m_accountMap[ MakeUnionName(newAccount, player->GetPlatformServerName()) ] = player;

    CnInfo("remap loginServerName:%s account %s<->%s\n", player->GetPlatformServerName().c_str(), oldAccount.c_str(), newAccount.c_str());
}

int32 PlayerManager::OnLikeState( const std::string& user_id )
{
    //返回玩家信息列表
    std::map<std::string, Player*> p = GetAccountMap();
    for (std::map<std::string, Player*>::iterator it = p.begin(); it != p.end(); ++it)
    {
        if(user_id == it->second->GetAccount())
        {
            if(it->second->GetShorcutInfo().like_flag == 0)
            {
                CnDbg("Get Like OnLikeState %s\n", user_id.c_str());
                NONE startNtf;
                it->second->SendToGame(MSG_PLAYER_LIKE_REQ, &startNtf);
                return 1;
            }
            else
            {
                return 0;
            }
            break;
        }
    }
    return 0;
}

bool PlayerManager::OnSubItem( uint64 user_id, uint32 itemid, uint32 itemnum )
{
    ////返回玩家信息列表
    //std::map<std::string, Player*> p = GetAccountMap();
    //for (std::map<std::string, Player*>::iterator it = p.begin(); it != p.end(); ++it)
    //{
    //    if(user_id == it->second->GetPlayerID())
    //    {
    //        OPT_ITEM_REQ req;
    //        req.opt = ITEM_OPT_SUB;
    //        req.src_pos = itemid;
    //        req.dst_pos = itemnum;
    //        it->second->SendToGame(MSG_OPT_ITEM_REQ, &req);
    //        return true;
    //        break;
    //    }
    //}
    return false;
}
