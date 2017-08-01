#include "centerserver_pch.h"
#include "activecode_job.h"
#include "dbdriver.h"
#include "player.h"
#include "player_manager.h"
#include "game_handler.h"
#include "json_helper.h"

IMPLEMENT_SINGLETON(ActiveCodeJob)


void ActiveCodeJob::UseGiftCode(uint64 playerID, std::string giftCode)
{
    if(IsInUse(playerID))
    {
        // 该玩家正在领取中
        return;
    }
    EnterUse(playerID);

    uint32 giftType = 2;
    m_dbGlobalExecutor->Query(this, &ActiveCodeJob::CBGetUsedGiftCode, playerID, giftCode, giftType,
        "SELECT `flag`, `gift_id`, t2.title FROM `gift_code` as t1 JOIN gift_code_item as t2 on t1.gift_id=t2.uid WHERE `player_id`=%llu", playerID);
    /*
    if(giftCode.find(ACTIVATION_PREFIX) != giftCode.npos)
    {
        giftType = 2;
        // 先查找该玩家已经领过的激活码
        m_dbGlobalExecutor->Query(this, &ActiveCodeJob::CBGetUsedGiftCode, playerID, giftCode, giftType,
            "SELECT `flag`, `gift_id` FROM `gift_code` WHERE `player_id`=%llu", playerID);
    }
    else
    {
        giftType = 1;
        // 先查找该玩家已经领过的激活码
        m_dbGameExecutor->Query(this, &ActiveCodeJob::CBGetUsedGiftCode, playerID, giftCode, giftType,
            "SELECT `flag`, `gift_id` FROM `gift_code` WHERE `player_id`=%llu", playerID);
    }
    */
   
}

void ActiveCodeJob::CBGetUsedGiftCode(DBRecorder& res, uint64 playerID, std::string giftCode, uint32 giftType)
{
    int32 flag = 0;
    int32 giftID = 0;
    std::string title;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) 
        return;

    //std::vector<uint32> usedGiftIds;
    std::vector<std::string> usedGiftIds;
    for(uint32 i=0; i<res.GetRowNum(); i++)
    {
        res.GetRecord(flag);            // flag
        res.GetRecord(giftID);          // gift id
        res.GetRecord(title);          // gift id

        if(flag != 0)
        {
            usedGiftIds.push_back(title);      // 记录已经领过的gift i=
        }
    }

    GetGiftCodeInfo(playerID, giftCode, usedGiftIds, giftType);
}

// ==================================================
// 获得兑换码信息
// ==================================================
void ActiveCodeJob::GetGiftCodeInfo(uint64 playerID, std::string giftCode, std::vector<std::string> usedGiftIDs, uint32 giftType)
{
    char gift_code[MAX_ACTIVE_CODE_LEN*2+1] = {0};

    if(giftCode.length() > MAX_ACTIVE_CODE_LEN)
    {
        LeaveUse(playerID);
        return;
    }
    if(giftType == 2)
    {
        m_dbGlobalExecutor->EscapeString(gift_code, giftCode.c_str(), giftCode.length());
        m_dbGlobalExecutor->Query(this, &ActiveCodeJob::CBGetGiftCodeInfo, playerID, giftCode, usedGiftIDs, giftType,
            "SELECT `flag`, `gift_id`, t2.title FROM `gift_code` as t1 JOIN gift_code_item as t2 on t1.gift_id=t2.uid WHERE `active_code`='%s'", gift_code);
    }
    else{
        m_dbGameExecutor->EscapeString(gift_code, giftCode.c_str(), giftCode.length());
        m_dbGameExecutor->Query(this, &ActiveCodeJob::CBGetGiftCodeInfo, playerID, giftCode, usedGiftIDs, giftType,
            "SELECT `flag`, `gift_id`, t2.title FROM `gift_code`  as t1 JOIN gift_code_item as t2 on t1.gift_id=t2.uid WHERE `active_code`='%s'", gift_code);
    }
}

// ==================================================
// 获得激活码信息回调
// ==================================================
void ActiveCodeJob::CBGetGiftCodeInfo(DBRecorder& res, uint64 playerID, std::string activeCode, std::vector<std::string> usedGiftIDs, uint32 giftType)
{
    Player* player = NULL;
    int32 flag = 0;
    int32 giftID = 0;
    std::string title;
    uint16 errCode = ERR_ACTIVE_CODE_NOT_FOUND;
    bool isUsed = false;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(res.GetRowNum() < 1)
    {
        goto _ERROR;
    }

    res.GetRecord(flag);            // flag
    res.GetRecord(giftID);          // gift id
    res.GetRecord(title);          // gift id

    // 检查此激活码是否已使用
    if(flag != 0)
    {
        errCode = ERR_ACTIVE_CODE_USED;
        goto _ERROR;
    }

    // 检查该玩家是否已经领过该批次的礼品
    for(uint32 i=0; i<usedGiftIDs.size(); i++)
    {
        if(usedGiftIDs[i] == title)
        {
            isUsed = true;
            break;
        }
    }

    if(isUsed)
    {
        errCode = ERR_ACTIVE_CODE_GIFT_ID_USED;     // 该批礼品已经领过了
        goto _ERROR;
    }

    _UseGiftCode(playerID, player->GetName(), player->GetCentGameID(), activeCode, giftID, giftType);

    return;

_ERROR:
    player->NotifyErrorCode(errCode);
    LeaveUse(playerID);
}

// ==================================================
// 使用激活码
// ==================================================
void ActiveCodeJob::_UseGiftCode(uint64 playerID, std::string playerName, uint32 gameSvrID, std::string giftCode, int32 giftID, uint32 giftType)
{
    char gift_code[MAX_ACTIVE_CODE_LEN*2+1] = {0};

    if(giftCode.length() > MAX_ACTIVE_CODE_LEN)
    {
        LeaveUse(playerID);
        return;
    }

    if(giftType == 2)
    {
        m_dbGlobalExecutor->Query(this, &ActiveCodeJob::CBUseGiftCode, playerID, playerName, gameSvrID, giftCode, giftID, giftType,
            "SELECT `title`,`items` FROM `gift_code_item` WHERE `uid`='%d'", giftID);
    }
    else
    {
        m_dbGameExecutor->Query(this, &ActiveCodeJob::CBUseGiftCode, playerID, playerName, gameSvrID, giftCode, giftID, giftType,
            "SELECT `title`,`items` FROM `gift_code_item` WHERE `uid`='%d'", giftID);
    }
    
}
// ==================================================
// 使用兑换码信息回调
// ==================================================
void ActiveCodeJob::CBUseGiftCode(DBRecorder& res, uint64 playerID, std::string playerName, uint32 gameSvrID, std::string giftCode, int32 giftID, uint32 giftType)
{
    Player* player = 0;
    ACTIVECODE_USE_ACK sendMsg;
    uint16 errCode = ERR_ACTIVE_CODE_USED;

    Json::Reader reader;
    Json::Value gift_json;
    REWARD_ITEM gift_item;

    std::string gift_info;
    std::string gift_title;
    uint32 gitf_count = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    if(res.GetAffNum() < 1)
    {
        goto _ERROR;
    }

    res.GetRecord(gift_title);
    res.GetRecord(gift_info);

    if (!reader.parse(gift_info, gift_json, false))
    {
        errCode = ERR_ACTIVE_CODE_NOT_FOUND;
        goto _ERROR;
    }

    // 解析奖品
    gitf_count = gift_json.size();
    for(uint32 i=0; i<gitf_count; ++i)
    {
        gift_item.Clear();
        Json::Value& item_json = gift_json[i];
        read_REWARD_ITEM(gift_item, item_json);

        sendMsg.items.items.push_back(gift_item);
    }
    sendMsg.items.reason = REWARD_REASON_GIFT;

    sendMsg.player_name = playerName;
    sendMsg.title = gift_title;

    GameHandler::Instance()->SendByID(gameSvrID, MSG_ACTIVECODE_USE_ACK, &sendMsg, playerID);

    SetActiveCodeUseInfo(giftCode, playerID, giftType);
    return;

_ERROR:
    if(player)
    {
        player->NotifyErrorCode(errCode);
        LeaveUse(playerID);
    }
}

// ==================================================
// 更新激活码使用信息
// ==================================================
void ActiveCodeJob::SetActiveCodeUseInfo(std::string giftCode, uint64 playerID, uint32 giftType)
{
    char gift_code[MAX_ACTIVE_CODE_LEN*2+1] = {0};

    if(giftCode.length() > MAX_ACTIVE_CODE_LEN)
    {
        LeaveUse(playerID);
        return;
    }
    if(giftType == 2)
    {
        m_dbGlobalExecutor->EscapeString(gift_code, giftCode.c_str(), giftCode.length());
        m_dbGlobalExecutor->Query(this, &ActiveCodeJob::CBSetActiveCodeUseInfo, playerID,
            "UPDATE `gift_code` SET `flag`=1, `player_id`=%llu, use_time=CURRENT_TIMESTAMP WHERE `active_code`='%s'", playerID, gift_code);
    }else{
        m_dbGameExecutor->EscapeString(gift_code, giftCode.c_str(), giftCode.length());
        m_dbGameExecutor->Query(this, &ActiveCodeJob::CBSetActiveCodeUseInfo, playerID,
            "UPDATE `gift_code` SET `flag`=1, `player_id`=%llu, use_time=CURRENT_TIMESTAMP WHERE `active_code`='%s'", playerID, gift_code);
    }
   
}

// ==================================================
// 更新激活码使用信息回调
// ==================================================
void ActiveCodeJob::CBSetActiveCodeUseInfo(DBRecorder& res, uint64 playerID)
{
    LeaveUse(playerID);
}

void ActiveCodeJob::EnterUse(uint64 playerID)
{
    if(IsInUse(playerID))
        return;

    m_InUsedPlayers.push_back(playerID);
}

void ActiveCodeJob::LeaveUse(uint64 playerID)
{
    for(uint32 i=0; i<m_InUsedPlayers.size(); i++)
    {
        if(playerID == m_InUsedPlayers[i])
        {
            m_InUsedPlayers.erase(m_InUsedPlayers.begin()+i);
            break;
        }
    }
}

bool ActiveCodeJob::IsInUse(uint64 playerID)
{
    for(uint32 i=0; i<m_InUsedPlayers.size(); i++)
    {
        if(playerID == m_InUsedPlayers[i])
            return true;
    }
    return false;
}