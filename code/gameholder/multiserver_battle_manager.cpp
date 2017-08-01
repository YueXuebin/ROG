#include "gameholder_pch.h"
#include "multiserver_battle_manager.h"
#include "dungeon_manager.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "server_config.h"
#include "player.h"
#include "dungeon_set.h"
#include "channel_manager.h"
#include "multiserver_battle_channel_manager.h"
#include "chaos_faction_channel_manager.h"
#include "chaos_faction_config.h"
#include "player_manager.h"
#include "multiserver_battle_info_manager.h"

// 跨服战斗匹配管理器
IMPLEMENT_SINGLETON(MultiserverBattleManager)


MultiserverBattleManager::MultiserverBattleManager(void)
{
    cTick = 0;
    m_OneApplyList.clear();
    m_ThreeApplyList.clear();
    m_last_time = 0;
    m_matchMap.clear();
    m_seasonInfo.Clear();
}

MultiserverBattleManager::~MultiserverBattleManager(void)
{
}

void MultiserverBattleManager::OnRecvFromPlayer( uint32 msgID, void* data )
{
    // 检查在此GameServer中是否开启了跨服组队.如果有,则直接发送消息,否则需要通过CenterServer转发到相应的GameServer上应答
    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;

    bool needToCenter = false;      // 是否需要转发
    switch(msgID)
    {
    case MSG_MULTISERVER_BATTLE_APPLY_REQ:
    case MSG_MULTISERVER_BATTLE_QUIT_REQ:
        //if((recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_ONE && DungeonManager::Instance()->GetDungeonInfo(DUNGEON_MULTISERVER_BATTLE_ID)) ||
        //    (recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_THREE && DungeonManager::Instance()->GetDungeonInfo(CHAOS_MAP)))
        //{
        //    needToCenter = true;
        //}
        break;
    default:
        CnAssert(false);        // 未判断是否需要转发
        break;

    }

    if(needToCenter)
    {   // 协议发送给Center
        CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), msgID, data);
    }
    else
    {
        OnRecvFromCenter(msgID, data);
    }
}

void MultiserverBattleManager::OnRecvFromCenter(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MULTISERVER_BATTLE_APPLY_REQ:      // 请求匹配
        {
            OnClearMatchCount();            // 每日凌晨重置匹配次数
            OnMultiserverBattleApplyHandle(data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_QUIT_REQ:       // 退出匹配
        {
            OnMultiserverBattleQuitHandle(data);
        }
        break;
    case MSG_MULTISERVER_BATTLE_SEASON_NTF:
        {
            OnSeasonNtf(data);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

void MultiserverBattleManager::OnSeasonNtf(void* data)
{
    MULTISERVER_BATTLE_SEASON_NTF* recvMsg = (MULTISERVER_BATTLE_SEASON_NTF*)data;
    
    if(recvMsg->season_time > m_seasonInfo.season_time)
    {
        m_seasonInfo.season_time = recvMsg->season_time;

        // 通知玩家赛季更新
        const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
        for(PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
        {
            playerIt->second->GetMultiserverBattleInfoManager()->CheckNewSeason();
        }
    }
}
// 请求匹配
void MultiserverBattleManager::OnMultiserverBattleApplyHandle( void* data )
{
    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;
    if(!data)
        return;
    // 匹配成功应答
    SendApplyAck(ERR_SUCCEED, recvMsg->gate_id, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l), recvMsg->battle_type);

    if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_ONE)
    {
        OneApplyReq(recvMsg);
    }
    else if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_THREE)
    {
        ThreeApplyReq(recvMsg);
    }
    else
    {
        CnAssert(false);    // 未知的匹配类型
    }
}

void MultiserverBattleManager::OneApplyReq(MULTISERVER_BATTLE_APPLY_REQ* recvMsg)
{
    // 是否已经请求过匹配
    bool newflag = true;
    for( ApplyList::iterator it = m_OneApplyList.begin(); it != m_OneApplyList.end(); it++)
    {
        if(IsSame64(it->player_id, recvMsg->player_id))
        {
            it->gate_id = recvMsg->gate_id;
            it->game_id = recvMsg->game_id;
            it->channel_id = 0;
            it->apply_time = recvMsg->apply_time;
            it->wait_time = recvMsg->wait_time;
            newflag = false;
            break;
        }
    }
    // 加入到请求列表中
    if(newflag)
    {
        MULTISERVER_BATTLE_APPLY_REQ sv;
        sv.player_id.id_h = recvMsg->player_id.id_h;
        sv.player_id.id_l = recvMsg->player_id.id_l;
        sv.gate_id = recvMsg->gate_id;
        sv.game_id = recvMsg->game_id;
        sv.battle_type = recvMsg->battle_type;
        //sv.battle_power = recvMsg->battle_power;
        sv.channel_id = 0;
        sv.apply_time = recvMsg->apply_time;
        sv.wait_time = recvMsg->wait_time;

        m_OneApplyList.push_back(sv);
    }
    // 判断与该玩家的匹配人数
    int32 count = 1;
    for( ApplyList::iterator it = m_OneApplyList.begin(); it != m_OneApplyList.end(); it++)
    {
        if(it->channel_id == 0)
        {
            if(IsSame64(it->player_id, recvMsg->player_id))     // 自己不能与自己匹配
                continue;

            bool cantMatch = JudgeMatchCount(recvMsg->player_id.id_h, recvMsg->player_id.id_l, it->player_id.id_h, it->player_id.id_l, MULTISERVER_BATTLE_MATCH_NUM);
            if(!cantMatch)
            {
                count++;
                break;
            }
        }
    }
    // 匹配到足够的人生
    if(count >= MULTISERVER_BATTLE_APPLY_ONE_NUM)
    {
        // 丢去战斗
        //DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(DUNGEON_MULTISERVER_BATTLE_ID);
        //CnAssert(dungeonSet);
        //uint32 channelID = ((MultiserverBattleChannelManager*)dungeonSet->GetChannelManager())->GetCreateChannelID(MULTISERVER_BATTLE_APPLY_ONE);
        //count = 0;

        //// 给匹配成功的两个玩家发送通知
        //int32 otherSend = 0;
        //uint32 ph = 0; 
        //uint32 pl = 0;
        //uint32 oh = 0; 
        //uint32 ol = 0;
        //for( ApplyList::iterator it = m_OneApplyList.begin(); it != m_OneApplyList.end(); it++)
        //{
        //    if(it->channel_id == 0)
        //    {
        //        if(it->player_id.id_h == recvMsg->player_id.id_h && it->player_id.id_l == recvMsg->player_id.id_l)
        //        {
        //            ph = it->player_id.id_h;
        //            pl = it->player_id.id_l;
        //            SendApplyNtf(ERR_SUCCEED, it->gate_id, U64ID(it->player_id.id_h, it->player_id.id_l), it->battle_type, channelID, MULTISERVER_BATTLE_APPLY_ENTER);
        //            it->channel_id = channelID;
        //            continue;
        //        }

        //        bool cantMatch = JudgeMatchCount(recvMsg->player_id.id_h, recvMsg->player_id.id_l, it->player_id.id_h, it->player_id.id_l, MULTISERVER_BATTLE_MATCH_NUM);
        //        if(!cantMatch && otherSend == 0)
        //        {
        //            otherSend++;
        //            oh = it->player_id.id_h;
        //            ol = it->player_id.id_l;
        //            SendApplyNtf(ERR_SUCCEED, it->gate_id, U64ID(it->player_id.id_h, it->player_id.id_l), it->battle_type, channelID, MULTISERVER_BATTLE_APPLY_ENTER);
        //            it->channel_id = channelID;

        //        }
        //    }
        //}
        //// 记录匹配次数
        //SaveMatchCount(ph, pl, oh, ol);
        //SaveMatchCount(oh, ol, ph, pl);
    }
}

// 3V3匹配请求
void MultiserverBattleManager::ThreeApplyReq(MULTISERVER_BATTLE_APPLY_REQ* recvMsg)
{
    // 判断是否已经请求过匹配
    bool alreadyApply = false;
    for(ApplyList::iterator threeApplyIt = m_ThreeApplyList.begin(); threeApplyIt != m_ThreeApplyList.end(); ++threeApplyIt)
    {
        if(IsSame64(threeApplyIt->player_id, recvMsg->player_id))
        {
            threeApplyIt->gate_id = recvMsg->gate_id;
            threeApplyIt->game_id = recvMsg->game_id;
            threeApplyIt->channel_id = 0;                       // 重新匹配
            threeApplyIt->apply_time = recvMsg->apply_time;
            threeApplyIt->wait_time = recvMsg->wait_time;
            threeApplyIt->battle_type = recvMsg->battle_type;
            threeApplyIt->battle_power = recvMsg->battle_power;
            alreadyApply = true;
            break;
        }
    }
    // 添加到请求列表
    if(!alreadyApply)
    {
        MULTISERVER_BATTLE_APPLY_REQ sv;
        sv = *recvMsg;
        sv.channel_id = 0;
        m_ThreeApplyList.push_back(sv);
    }

    // 计算匹配数
    int32 count = 0;
    for(ApplyList::iterator threeApplyIt = m_ThreeApplyList.begin(); threeApplyIt != m_ThreeApplyList.end(); threeApplyIt++)
    {
        if(threeApplyIt->channel_id == 0)
        {
            ++count;
        }
    }

    // 判断是否达到匹配人数
    if(count >= (int32)ChaosFactionConfig::Instance()->GetPlayerMax()) // 开始配对
    {
        // 丢去战斗
        //DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(CHAOS_MAP);
        //CnAssert(dungeonSet);
        //uint32 channelID = ((ChaosFactionChannelManager*)dungeonSet->GetChannelManager())->GetCreateChannelID();

        //int32 enterCount = 0;
        //for(ApplyList::iterator threeApplyIt = m_ThreeApplyList.begin(); threeApplyIt != m_ThreeApplyList.end(); threeApplyIt++)
        //{
        //    if(threeApplyIt->channel_id == 0)
        //    {
        //        SendApplyNtf(ERR_SUCCEED, threeApplyIt->gate_id, U64ID(threeApplyIt->player_id.id_h, threeApplyIt->player_id.id_l), threeApplyIt->battle_type, channelID, MULTISERVER_BATTLE_APPLY_ENTER);
        //        threeApplyIt->channel_id = channelID;
        //        ++enterCount;
        //    }

        //    if(enterCount > (int32)ChaosFactionConfig::Instance()->GetPlayerMax())
        //    {
        //        CnAssert(false);
        //    }
        //}
    }
}

void MultiserverBattleManager::SendApplyAck( uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType)
{
    MULTISERVER_BATTLE_APPLY_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.battle_type = battleType;

    GateHandler::Instance()->SendByGateID(gate_id, MSG_MULTISERVER_BATTLE_APPLY_ACK, &sendMsg, playerID);
}

// 退出匹配
void MultiserverBattleManager::OnMultiserverBattleQuitHandle( void* data )
{
    MULTISERVER_BATTLE_APPLY_REQ* recvMsg = (MULTISERVER_BATTLE_APPLY_REQ*)data;
    if(!data)
        return;

    uint16 errcode = ERR_SUCCEED;
    ApplyList::iterator it;

    if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_ONE)
    {
        for( it = m_OneApplyList.begin(); it != m_OneApplyList.end();)
        {
            if( IsSame64(it->player_id, recvMsg->player_id) )
            {
                // 删除玩家
                if(it->channel_id > 0)
                {
                    //DungeonSet* dset = DungeonManager::Instance()->GetDungeonSet(DUNGEON_MULTISERVER_BATTLE_ID);
                    //((MultiserverBattleChannelManager*)dset->GetChannelManager())->JudgeDungeonStart(it->channel_id);
                }
                it = m_OneApplyList.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
    else if(recvMsg->battle_type == MULTISERVER_BATTLE_APPLY_THREE)
    {
        ApplyList::iterator threeIt = m_ThreeApplyList.begin();
        for(; threeIt != m_ThreeApplyList.end(); ++threeIt)
        {
            if(IsSame64(threeIt->player_id, recvMsg->player_id))
                break;
        }

        if(threeIt == m_ThreeApplyList.end())
            return;

        if(threeIt->channel_id == 0)
            m_ThreeApplyList.erase(threeIt);
    }

    SendQuitAck(errcode, recvMsg->gate_id, U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l), recvMsg->battle_type);
}

void MultiserverBattleManager::SendQuitAck( uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType )
{
    MULTISERVER_BATTLE_APPLY_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.battle_type = battleType;

    GateHandler::Instance()->SendByGateID(gate_id, MSG_MULTISERVER_BATTLE_QUIT_ACK, &sendMsg, playerID);
}

void MultiserverBattleManager::SendApplyNtf( uint16 errcode, uint32 gate_id, uint64 playerID, uint32 battleType, uint32 channel_id, uint32 ackState)
{
    MULTISERVER_BATTLE_APPLY_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.battle_type = battleType;
    sendMsg.channel_id = channel_id;
    sendMsg.ack_state = ackState;

    GateHandler::Instance()->SendByGateID(gate_id, MSG_MULTISERVER_BATTLE_APPLY_NTF, &sendMsg, playerID);
}

void MultiserverBattleManager::Update( uint32 frame_count )
{
    // 每秒执行
    if(frame_count - cTick < g_ServerUpdateFrequency)
        return;

    cTick = frame_count;
    // 匹配超时判断
    int32 ctime = (int32)Crown::SDNow().GetTimeValue();
    ;
    for( ApplyList::iterator it = m_OneApplyList.begin(); it != m_OneApplyList.end(); it++)
    {
        if(it->channel_id == 0 && ctime - (int32)it->apply_time > (int32)it->wait_time)
        {
            it->channel_id = 9999;
            SendApplyNtf(ERR_SUCCEED, it->gate_id, U64ID(it->player_id.id_h, it->player_id.id_l), it->battle_type, 0, MULTISERVER_BATTLE_APPLY_TIME_OUT);
        }
    }

    for( ApplyList::iterator it = m_ThreeApplyList.begin(); it != m_ThreeApplyList.end();)
    {
        if(it->channel_id == 0 && ctime - (int32)it->apply_time > (int32)it->wait_time)
        {
            SendApplyNtf(ERR_SUCCEED, it->gate_id, U64ID(it->player_id.id_h, it->player_id.id_l), it->battle_type, 0, MULTISERVER_BATTLE_APPLY_TIME_OUT);
            it = m_ThreeApplyList.erase(it);
        }
        else
            ++it;
    }
}

void MultiserverBattleManager::SendWaitAck( uint32 ph, uint32 pl, uint32 type, uint32 gameID )
{
    MULTISERVER_BATTLE_WAIT_ACK wait;
    wait.player_id.id_h = ph;
    wait.player_id.id_l = pl;
    wait.battle_type = type;
    wait.game_server_id = gameID;
    CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), MSG_MULTISERVER_BATTLE_WAIT_ACK, &wait);
}

// 判断两个玩家的匹配次数是否超过上限
bool MultiserverBattleManager::JudgeMatchCount( uint32 mh, uint32 ml, uint32 ph, uint32 pl, uint32 maxCount )
{
    uint64 playerID = U64ID(mh, ml);
    bool countState = false;
    std::map<uint64, MatchList>::iterator matchIt;
    matchIt = m_matchMap.find(playerID);

    if(matchIt != m_matchMap.end())
    {
        for(MatchList::iterator itemIt = matchIt->second.begin();itemIt != matchIt->second.end(); ++itemIt)
        {
            if(itemIt->player_id.id_h == ph && itemIt->player_id.id_l == pl)
            {
                if(itemIt->count >= maxCount)
                {
                    countState = true;
                    break;
                }
            }
        }
    }

    return countState;
}
// 记录匹配
void MultiserverBattleManager::SaveMatchCount( uint32 ph, uint32 pl, uint32 oh, uint32 ol )
{
    uint64 playerID = U64ID(ph, pl);
    std::map<uint64, MatchList>::iterator matchIt = m_matchMap.find(playerID);

    if(matchIt != m_matchMap.end())
    {
        bool bFind = false;
        for(MatchList::iterator itemIt = matchIt->second.begin(); itemIt != matchIt->second.end(); ++itemIt)
        {
            if(itemIt->player_id.id_h == oh && itemIt->player_id.id_l == ol)
            {
                itemIt->count += 1;
                bFind = true;
                break;
            }
        }

        if(!bFind)
        {
            MULTISERVER_MATCH_COUNT countInfo;
            countInfo.player_id.id_h = oh;
            countInfo.player_id.id_l = ol;
            countInfo.count = 1;
            matchIt->second.push_back(countInfo);
        }
    }
    else 
    {
        MatchList matchList;
        MULTISERVER_MATCH_COUNT countInfo;
        countInfo.player_id.id_h = oh;
        countInfo.player_id.id_l = ol;
        countInfo.count = 1;
        matchList.push_back(countInfo);
        m_matchMap[playerID] = matchList;
    }
}

void MultiserverBattleManager::OnClearMatchCount()
{
    Crown::CSDDateTime today_time = Crown::SDNow(); // 凌晨清空匹配记录
    today_time.SetTime(0, 0, 0);

    if(m_last_time < (int32)today_time.GetTimeValue())
    {
        m_last_time = (int32)Crown::SDNow ().GetTimeValue();
        m_matchMap.clear();
    }
}

uint32 MultiserverBattleManager::GetChannelIDByThreeApplyList(uint64 playerID)
{
    for(ApplyList::iterator playerIt = m_ThreeApplyList.begin(); playerIt != m_ThreeApplyList.end(); ++playerIt)
    {
        if(U64ID(playerIt->player_id.id_h, playerIt->player_id.id_l) == playerID)
        {
            return playerIt->channel_id;
        }
    }

    return 0;
}

void MultiserverBattleManager::ThreeApplyListErase(uint64 playerID)
{
    for(ApplyList::iterator applyIt = m_ThreeApplyList.begin(); applyIt != m_ThreeApplyList.end();)
    {
        if(U64ID(applyIt->player_id.id_h, applyIt->player_id.id_l) == playerID)
        {
            applyIt = m_ThreeApplyList.erase(applyIt);
        }
        else
        {
            ++applyIt;
        }
    }
}
