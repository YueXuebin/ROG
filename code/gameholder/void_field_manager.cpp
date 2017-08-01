#include "gameholder_pch.h"
#include "void_field_manager.h"
#include "scene.h"
#include "player.h"
#include "player_manager.h"
#include "regular.h"
#include "mail_title_config.h"
#include "mail_manager.h"
#include "ranking_manager.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "game_util.h"
#include "gate_handler.h"
#include "center_handler.h"
#include "event_define.h"
#include "dungeon.h"
#include "dungeon_manager.h"
#include "server_config.h"
#include "void_field_config.h"
#include "regular_void_field.h"
#include "game_setting_manager.h"
#include "channel_manager.h"


IMPLEMENT_SINGLETON(VoidFieldManager)

VoidFieldManager::VoidFieldManager()
{
    m_world_level = 0;

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    m_OpenTimeList.resize(VOID_FIELD_TYPE_MAX);
    m_OpenTimeList[VOID_FIELD_TYPE_NORMAL] = 0;
    SetOpenTime(VOID_FIELD_TYPE_BOSS);
    SetOpenTime(VOID_FIELD_TYPE_MOONLESS);
    //m_OpenTimeList[VOID_FIELD_TYPE_BOSS] = now + GameSettingManager::Instance()->GetVoidFieldBossCDTime();
    //m_OpenTimeList[VOID_FIELD_TYPE_MOONLESS] = now +  GameSettingManager::Instance()->GetVoidFieldMoonlessCDTime();

    m_BossChannelList.clear();
    m_MoonlessChannelList.clear();
}

VoidFieldManager::~VoidFieldManager()
{

}

void VoidFieldManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_CHANNEL_INFO_REQ:
        OnVoidFieldChannelInfoReq(data);
        break;
    default:
        break;
    }
}

void VoidFieldManager::OnPlayerRecv(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_CHANNEL_INFO_REQ:
        OnPlayerChannelInfoReq(player, data);
        break;
    case MSG_VOID_FIELD_SET_HOOK_POS_REQ:
        OnSetHookPosReq(player, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void VoidFieldManager::Update()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 openPlayerNum = 0;
    uint32 openChannelNum = 0;

    //for(uint32 dungeonID=DUNGEON_VOID_FIELD_ID_START; dungeonID<=DUNGEON_VOID_FIELD_ID_END; dungeonID++)
    //{
    //    DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeonID);
    //    if (!pDungeonSet)
    //        return;

    //    ChannelManager* pChannelManager = pDungeonSet->GetChannelManager();
    //    if (!pChannelManager)
    //        return;

    //    uint32 channelNum = pChannelManager->GetChannelNum();

    //    for(uint32 mType = VOID_FIELD_TYPE_BOSS; mType<m_OpenTimeList.size(); mType++)
    //    {
    //        // 开启人数和开启频道数
    //        if(mType == VOID_FIELD_TYPE_BOSS)
    //        {
    //            openPlayerNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldRegularBossPlayerNum());
    //            openChannelNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldBossChannelNum()) - m_BossChannelList.size();
    //        }
    //        else if(mType == VOID_FIELD_TYPE_MOONLESS)
    //        {
    //            openPlayerNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldRegularMoonlessPlayerNum());
    //            openChannelNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldMoonlessChannelNum()) - m_MoonlessChannelList.size();
    //        }

    //        if (now > m_OpenTimeList[mType] && openChannelNum > 0 && CheckLastInChannelListState(mType, dungeonID) == true) // 检查开启时间和可开启频道数以及前一个已开启频道的状态是否是相同状态
    //        {
    //            // 获取满足开启人数条件的候选频道列表
    //            std::vector<uint32> candidateList;
    //            for (uint32 curChannel = 1; curChannel <= channelNum; curChannel++)
    //            {
    //                if (CheckInChannelList(mType, curChannel) == false)   // 剔除掉已经在 bossList 或 moonlessList里的频道
    //                {
    //                    Dungeon* pDungeon = DungeonManager::Instance()->GetDungeon(dungeonID, curChannel);
    //                    if (pDungeon)
    //                    {
    //                        uint32 curPlayerNum = pDungeon->GetScene(1)->GetPlayerCount();
    //                        if (curPlayerNum >= openPlayerNum)
    //                            candidateList.push_back(curChannel);
    //                    }
    //                }
    //            }

    //            // 根据珍兽或者月黑的最大频道数随机选出频道
    //            if (candidateList.size() > 0)
    //            {
    //                uint32 numMin = (candidateList.size() < openChannelNum ) ? candidateList.size():openChannelNum;
    //                for (uint32 i = 0; i < numMin; i++) // 随机抽取
    //                {
    //                    uint32 rIdx = Crown::SDRandom(candidateList.size());
    //                    uint32 rChannel = candidateList[rIdx];
    //                    candidateList.erase(candidateList.begin()+rIdx);

    //                    Dungeon* pDungeon = DungeonManager::Instance()->GetDungeon(dungeonID, rChannel);
    //                    if (pDungeon)
    //                    {
    //                        RegularVoidField *pVoidFieldRegular = (RegularVoidField *)(pDungeon->GetScene(1)->GetRegular());
    //                        if (pVoidFieldRegular && pVoidFieldRegular->GetCurrType() == VOID_FIELD_TYPE_NORMAL)
    //                        {
    //                            AddToChannelList(mType, rChannel);

    //                            pVoidFieldRegular->SetStateStartTime(mType);
    //                            pVoidFieldRegular->SetCurrType(mType);
    //                        }
    //                    }
    //                }
    //            }
    //        }

    //    } // for
    //}   // for
}

// ==================================================
// 虚空幻景频道信息请求
// ==================================================
void VoidFieldManager::OnVoidFieldChannelInfoReq(void* data)
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    if(!recvMsg)
        return;

    // 检查在此GameServer中是否开启了虚空幻境，如果有，则直接发送消息
    // 否则需要通过CenterServer转发到相应的GameServer上应答
    if(DungeonManager::Instance()->GetDungeonInfo(recvMsg->dungeon_id))
    {
        SendVoidFieldChannelInfoAck(ERR_SUCCEED, recvMsg->gate_id, 
                U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l), recvMsg->dungeon_id);
        return;
    }

    CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), MSG_VOID_FIELD_CHANNEL_INFO_REQ, recvMsg);
}

// ==================================================
// 虚空幻景频道信息请求
// ==================================================
void VoidFieldManager::OnPlayerChannelInfoReq(Player* player, void* data)
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    if(!player || !data)
    {
        return;
    }

    // gateID, playerID, regionID 赋值
    recvMsg->gate_id        = player->GetGateSvrID();
    recvMsg->player_id.id_h = U64ID_HIGH(player->GetPlayerID());
    recvMsg->player_id.id_l = U64ID_LOW(player->GetPlayerID());
    recvMsg->dungeon_id     = GetVoidFieldDungeonIDByRegionID(player->GetRegion());

    OnRecv(MSG_VOID_FIELD_CHANNEL_INFO_REQ, data);
}

void VoidFieldManager::OnSetHookPosReq(Player* player, void* data)
{
    VOID_FIELD_SET_HOOK_POS_REQ* recvMsg = (VOID_FIELD_SET_HOOK_POS_REQ*)data;
    if (!player || ! recvMsg)
    {
        return;
    }

    //player->SetFieldHookX(recvMsg->hook_x);
    //player->SetFieldHookY(recvMsg->hook_y);
}

//void VoidFieldManager::CalDailyRefresh(Player* player)
//{
//    if(!player) return;
//    player->SetVoideFiedlTime(0);
//}
void VoidFieldManager::SendVoidFieldChannelInfoAck(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 dungeonID)
{
    VOID_FIELD_CHANNEL_INFO_ACK sendMsg;
    VOID_FIELD_CHANNEL_INFO channelInfo;
    VOID_FIELD_STATE_INFO   stateInfo;
    Scene* scene = NULL;
    Regular* regular = NULL;
    DungeonSet* dungeonSet = NULL;

    sendMsg.errcode = errcode;

    if(errcode != ERR_SUCCEED)
        goto _end;

    dungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeonID);
    if(!dungeonSet)
    {
        sendMsg.errcode = ERR_FAILED;
        goto _end;
    }
    else
    {
        // 世界等级
        sendMsg.world_level = CalWorldLevel();
        sendMsg.dungeon_id = dungeonID;

        std::map<uint32, Dungeon*>& dungeons = dungeonSet->GetDungeons();
        for(std::map<uint32, Dungeon*>::iterator it = dungeons.begin(); it != dungeons.end(); ++it)
        {
            channelInfo.Clear();
            channelInfo.channel_num = it->second->GetChannelID();
            channelInfo.player_num = it->second->GetPlayerNum();
            if((scene = it->second->GetScene(1)) && (regular = scene->GetRegular()))
            {
                if(regular->GetType() == REGULAR_TYPE_VOID_FIELD)
                {
                    channelInfo.void_field_type = ((RegularVoidField*)regular)->GetCurrType();
                }
            }
            sendMsg.channel_list.push_back(channelInfo);
        }

        stateInfo.Clear();
        stateInfo.type = VOID_FIELD_TYPE_BOSS;
        stateInfo.open_time = m_OpenTimeList[VOID_FIELD_TYPE_BOSS];
        stateInfo.open_channel_num = (uint32)(GameSettingManager::Instance()->GetVoidFieldBossChannelNum()) - m_BossChannelList.size();
        stateInfo.open_channel_max = (uint32)(GameSettingManager::Instance()->GetVoidFieldBossChannelNum());
        sendMsg.state_list.push_back(stateInfo);

        stateInfo.Clear();
        stateInfo.type = VOID_FIELD_TYPE_MOONLESS;
        stateInfo.open_time = m_OpenTimeList[VOID_FIELD_TYPE_MOONLESS];
        stateInfo.open_channel_num = (uint32)(GameSettingManager::Instance()->GetVoidFieldMoonlessChannelNum()) - m_MoonlessChannelList.size();
        stateInfo.open_channel_max = (uint32)(GameSettingManager::Instance()->GetVoidFieldMoonlessChannelNum());
        sendMsg.state_list.push_back(stateInfo);
    }

_end:
    GateHandler::Instance()->SendByGateID(gate_id, MSG_VOID_FIELD_CHANNEL_INFO_ACK, &sendMsg, playerID);
}

// ==================================================
// 检查能否进入虚空幻境
// ==================================================
uint16 VoidFieldManager::CanEnterVoidField(Player* player)
{
    if(!player)
        return ERR_FAILED;

    // 世界等级未达到
    if (CalWorldLevel() < GameSettingManager::Instance()->GetVoidFieldWorldLevelNeed())
    {
        return ERR_VOID_FIELD_WORLD_LEVEL_NOT_ENOUGH;
    }

    // 玩家等级未达到
    if (player->GetLevel() < VoidFieldConfig::Instance()->GetPlayerLevelNeed())
    {
        return ERR_VOID_FIELD_PlAYER_LEVEL_NOT_ENOUGH;
    }

    return ERR_SUCCEED;
}

bool VoidFieldManager::IsInBoosScene(Player* player)
{
    Scene* pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
    if (!pScene)
    {
        return false;
    }

    RegularVoidField* pRegular = (RegularVoidField*)pScene->GetRegular();
    if (!pRegular)
    {
        return false;
    }

    if (pRegular->GetCurrType() == VOID_FIELD_TYPE_BOSS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int32 VoidFieldManager::CalWorldLevel()
{
    uint32 total_level = 0;
    uint32 world_level = 0;

    std::vector<TOP_RANK_INFO>* top_rank_list = RankingManager::Instance()->GetTopPlayerLevelRankList();
    if (top_rank_list && top_rank_list->size() > 0)
    {
        // TODO 世界等级技术规则暂定，暂时去前20名的平均等级
        std::vector<TOP_RANK_INFO>::const_iterator iter;
        for (iter = top_rank_list->begin(); iter != top_rank_list->end(); ++iter)
        {
//             uint64 playerId = U64ID(iter->player_id.id_h, iter->player_id.id_l);
//             Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerId);
//             if (player)
//             {
//                 total_level += player->GetLevel();
//             }
            total_level += iter->player_level;
        }

        world_level = total_level / top_rank_list->size();
    }

    SetWorldLevel(world_level);
    return world_level;
}

void VoidFieldManager::SetOpenTime(int32 mType)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if (mType == VOID_FIELD_TYPE_BOSS)
        m_OpenTimeList[VOID_FIELD_TYPE_BOSS] = now + GameSettingManager::Instance()->GetVoidFieldBossCDTime();
    else if (mType = VOID_FIELD_TYPE_MOONLESS)
        m_OpenTimeList[VOID_FIELD_TYPE_MOONLESS] = now +  GameSettingManager::Instance()->GetVoidFieldMoonlessCDTime();
}

bool VoidFieldManager::IsLastInChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        if (m_BossChannelList.size() == 0)
            return false;

        if (m_BossChannelList.at(m_BossChannelList.size()-1) == mChannelId)
            return true;
    }
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
    {
        if (m_MoonlessChannelList.size() == 0)
            return false;

        if (m_MoonlessChannelList.at(m_MoonlessChannelList.size()-1) == mChannelId)
            return true;
    }

    return false;
}

void VoidFieldManager::AddToChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        m_BossChannelList.push_back(mChannelId);
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
        m_MoonlessChannelList.push_back(mChannelId);
}

void VoidFieldManager::RemoveFromChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        for (std::vector<uint32>::iterator it = m_BossChannelList.begin(); it != m_BossChannelList.end(); ++it)
        {
            if (*it == mChannelId)
            {
                m_BossChannelList.erase(it);
                return;
            }
        }
    }
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
    {
        for (std::vector<uint32>::iterator it = m_MoonlessChannelList.begin(); it != m_MoonlessChannelList.end(); ++it)
        {
            if (*it == mChannelId)
            {
                m_MoonlessChannelList.erase(it);
                return;
            }
        }
    }
}

void VoidFieldManager::ClearChannelList(int32 mType)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        m_BossChannelList.clear();
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
        m_MoonlessChannelList.clear();
}

bool VoidFieldManager::CheckLastInChannelListState(int32 mType, uint32 dungeonID)
{
    uint32 channelId = 0;

    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        if (m_BossChannelList.size() == 0)
            return true;

        channelId = m_BossChannelList.at(m_BossChannelList.size()-1);
    }
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
    {
        if (m_MoonlessChannelList.size() == 0)
            return true;

        channelId = m_MoonlessChannelList.at(m_MoonlessChannelList.size()-1);
    }

    RegularVoidField *pVoidFieldRegular = (RegularVoidField *)(DungeonManager::Instance()->GetDungeon(dungeonID, channelId)->GetScene(1)->GetRegular());
    if (pVoidFieldRegular)
    {
        if (pVoidFieldRegular->GetCurrType() == mType)
            return true;
    }

    return false;
}

bool VoidFieldManager::CheckInChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        for (std::vector<uint32>::iterator it = m_BossChannelList.begin(); it != m_BossChannelList.end(); ++it)
        {
            if (*it == mChannelId)
            {
                return true;
            }
        }
    }
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
    {
        for (std::vector<uint32>::iterator it = m_MoonlessChannelList.begin(); it != m_MoonlessChannelList.end(); ++it)
        {
            if (*it == mChannelId)
            {
                return true;
            }
        }
    }

    return false;
}

uint32 VoidFieldManager::GetChannelListNum(int32 mType)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        return m_BossChannelList.size();
    else if (mType == VOID_FIELD_TYPE_MOONLESS)
        return m_MoonlessChannelList.size();

    return 0;
}

uint32  VoidFieldManager::GetVoidFieldDungeonIDByRegionID(uint32 region_id)
{
    for(uint32 i=0; i<m_VoidFieldGroupList.size(); i++)
    {
        VoidFieldGroup& group = m_VoidFieldGroupList[i];
        if(region_id >= group.region_id_min && region_id <= group.region_id_max)
            return group.dungeonId;
    }
    
    CnWarn("GetVoidFieldDungeonIDByRegionID region_id=%d not valid\n", region_id);
    //return DUNGEON_VOID_FIELD_ID_START;     // 如果没有在任何分组里面，则分配到虚空幻境1里面
    return 0;
}

void    VoidFieldManager::AddVoidFieldGroup(uint32 dungeonId, uint32 region_id_min, uint32 region_id_max)
{
    //CnAssert(IsVoidFieldCross(dungeonId));

    //VoidFieldGroup group;
    //group.dungeonId = dungeonId;
    //group.region_id_max = region_id_max;
    //group.region_id_min = region_id_min;

    //m_VoidFieldGroupList.push_back(group);
}