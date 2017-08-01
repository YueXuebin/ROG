#include "centerserver_pch.h"
#include "offline_player_job.h"
#include "dbdriver.h"
#include "player.h"
#include "player_manager.h"
#include "team_manager.h"

IMPLEMENT_SINGLETON(OfflinePlayerJob)

void OfflinePlayerJob::SearchTeamOfflinePlayer( uint64 playerID, TEAM_OFFLINE_MEMBER_ADD_REQ* offline_member_add_req )
{
    char name[PLAYER_NAME_LEN] = {0};

    if(!offline_member_add_req) return;

    uint32 nameLength = offline_member_add_req->dst_name.length();

    // 接受者姓名长度异常,防止因越界导致堆栈异常,应该在收到协议处就对长度做检测了
    if (nameLength > PLAYER_NAME_LEN)
        return;

    m_dbExecutor->EscapeString(name, offline_member_add_req->dst_name.c_str(), nameLength);

    m_dbExecutor->Query(this, &OfflinePlayerJob::CBSearchTeamOfflinePlayer, playerID, *offline_member_add_req,
        "SELECT player_id FROM `offline_player` WHERE player_name='%s'", name);
}

void OfflinePlayerJob::CBSearchTeamOfflinePlayer( DBRecorder& res, uint64 playerID, TEAM_OFFLINE_MEMBER_ADD_REQ offline_member_add_req )
{
    Player* player = 0;
    uint64  offlinePlayerId = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(offlinePlayerId);
    }
}
