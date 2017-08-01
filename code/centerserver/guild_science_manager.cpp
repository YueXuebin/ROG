#include "centerserver_pch.h"
#include "guild_science_config.h"
#include "guild_science_manager.h"
#include "player_manager.h"
#include "guild_manager.h"
#include "guild_job.h"
#include "game_handler.h"

IMPLEMENT_SINGLETON(GuildScienceManager)

GuildScienceManager::GuildScienceManager()
{

}

GuildScienceManager::~GuildScienceManager()
{

}

void GuildScienceManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_GUILD_ADDITION_PROMOTE_TO_CENTER:
        onAdditionPromote(playerID, data);
        break;
    }
}

void GuildScienceManager::onAdditionPromote(uint64 playerID, void* data)
{
    GUILD_ADDITION_PROMOTE_TO_CENTER* recvMsg = (GUILD_ADDITION_PROMOTE_TO_CENTER*)data;
    GUILD_ADDITION_PROMOTE_TO_GAME sendMsg;
    sendMsg.err_code = ERR_FAILED;
    sendMsg.addition_id = recvMsg->addition_id;
    sendMsg.cost_type = recvMsg->cost_type;
    sendMsg.times = recvMsg->times;
    sendMsg.point_num = recvMsg->point_num;

    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!pPlayer)
        return;

    // 公会信息
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(pPlayer->GetGuildID());
    if(!guildInfo)
    {
        sendMsg.err_code = ERR_FAILED;
        SendAdditionPromoteToGame(pPlayer, sendMsg);
        return;
    }

    // 科技加成信息
    GUILD_ADDITION* additionItem = GetAdditionItem(pPlayer->GetGuildID(), recvMsg->addition_id);
    if(!additionItem)
    {
        sendMsg.err_code = ERR_FAILED;
        CnError("guild science addition item is not exist!guildID:%d, playerID:%d, additionID:%d", pPlayer->GetGuildID(), pPlayer->GetPlayerID(), recvMsg->addition_id);
        SendAdditionPromoteToGame(pPlayer, sendMsg);
        return;
    }

    // 公会科技等级不高于公会等级
    if(additionItem->level > guildInfo->level)
    {
        sendMsg.err_code = ERR_GUILD_ADD_GUILD_LEVEL_LIMIT;
        SendAdditionPromoteToGame(pPlayer, sendMsg);
        return;
    }

    const GuildAdditionTmpl* additionLevelTmp = GuildScienceConfig::Instance()->GetScienceAdditionTmpl(recvMsg->addition_id);
    // 科技等级达到最高
    if(!additionLevelTmp || additionItem->level >= additionLevelTmp->maxlevel)
    {
        sendMsg.err_code = ERR_GUILD_SCIENCE_MAX_LEVEL;
        SendAdditionPromoteToGame(pPlayer, sendMsg);
        return;
    }

    if(recvMsg->cost_type == PROP_GAMEPOINT)
    {
        const GuildPromoteDonatePointTmpl* PromoteGamepointTmp = GuildScienceConfig::Instance()->GetPromoteDonateGamePointTmpl(recvMsg->times);
        if(!PromoteGamepointTmp)
        {
            sendMsg.err_code = ERR_FAILED;
            SendAdditionPromoteToGame(pPlayer, sendMsg);
            return;
        }

        AddGuildAdditionExp(additionItem, PromoteGamepointTmp->exp);
        additionItem->promote_count_gamepoint++;    // 无用 记录加成总次数
    }
    else if(recvMsg->cost_type == PROP_MONEYPOINT)
    {
        const GuildPromoteDonatePointTmpl* PromoteMoneypointTmp = GuildScienceConfig::Instance()->GetPromoteDonateMoneyPointTmpl(recvMsg->times);
        if(!PromoteMoneypointTmp)
        {
            sendMsg.err_code = ERR_FAILED;
            SendAdditionPromoteToGame(pPlayer, sendMsg);
            return;
        }

        AddGuildAdditionExp(additionItem, PromoteMoneypointTmp->exp);
        additionItem->promote_count_moneypoint++;    // 无用 记录加成总次数
    }
    else
        CnAssert(false);

    sendMsg.err_code = ERR_SUCCEED;
    SendAdditionPromoteToGame(pPlayer, sendMsg);

    // 保存
    GuildJob::Instance()->SetGuildAddition(pPlayer->GetGuildID(), guildInfo->addition_list);

    // 发给公会所有成员
    GUILD_SCIENCE_INFO scienceInfo;
    scienceInfo.guild_addition = guildInfo->addition_list;
    GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_GUILD_SCIENCE_INFO_NTF, &scienceInfo);

    // 通知所有的GameServer
    GameHandler::Instance()->SendToAll(MSG_GUILD_INFO_NTF, guildInfo);
}

void GuildScienceManager::SendAdditionPromoteToGame(Player* pPlayer, GUILD_ADDITION_PROMOTE_TO_GAME& sendMsg)
{
    pPlayer->SendToGame(MSG_GUILD_ADDITION_PROMOTE_TO_GAME, &sendMsg);
}

GUILD_ADDITION* GuildScienceManager::GetAdditionItem(uint64 guildID, int32 additionID)
{
    std::vector<GUILD_ADDITION>* additionList = GetScienceAdditionList(guildID);
    if(!additionList)
        return NULL;

    for(std::vector<GUILD_ADDITION>::iterator additionItem = additionList->begin(); additionItem != additionList->end(); ++additionItem)
    {
        if(additionItem->id == additionID)
            return &(*additionItem);
    }
    return NULL;
}

std::vector<GUILD_ADDITION>* GuildScienceManager::GetScienceAdditionList(uint64 guildID)
{
    return GuildManager::Instance()->GetScienceAdditionList(guildID);
}

void GuildScienceManager::AddGuildAdditionExp(GUILD_ADDITION* additionItem, int32 exp)
{
    if(!additionItem)
        return;

    additionItem->exp += exp;

    const GuildPromoteLevelTmpl* propmoteLevelTmp = GuildScienceConfig::Instance()->GetPromoteLevelTmpl(additionItem->id, additionItem->level);
    if(!propmoteLevelTmp)
    {
        CnAssert(false);
        return;
    }

    const GuildAdditionTmpl* additionTmp = GuildScienceConfig::Instance()->GetScienceAdditionTmpl(additionItem->id);
    CnAssert(additionTmp);

    while(additionItem->exp >= propmoteLevelTmp->exp)
    {
        additionItem->level++;
        additionItem->exp -= propmoteLevelTmp->exp;

        // 科技等级达到最大
        if(additionItem->level >= additionTmp->maxlevel)
        {
            additionItem->exp = 0;
            return;
        }
    }

}
