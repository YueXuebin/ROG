#include "gameholder_pch.h"
#include "void_field_solo_manager.h"
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
#include "void_field_solo_config.h"
#include "regular_void_field_solo.h"
#include "game_setting_manager.h"
#include "channel_manager.h"


IMPLEMENT_SINGLETON(VoidFieldSoloManager)

VoidFieldSoloManager::VoidFieldSoloManager()
{
    m_world_level = 0;

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    m_OpenTimeList.resize(VOID_FIELD_SOLO_TYPE_MAX);
    m_OpenTimeList[VOID_FIELD_TYPE_NORMAL] = 0;
    SetOpenTime(VOID_FIELD_TYPE_BOSS);

    m_BossChannelList.clear();
}

VoidFieldSoloManager::~VoidFieldSoloManager()
{

}

void VoidFieldSoloManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ:
        OnVoidFieldChannelInfoReq(data);
        break;
    default:
        break;
    }
}

void VoidFieldSoloManager::Update()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 openPlayerNum = 0;
    uint32 openChannelNum = 0;

    DungeonSet* pDungeonSet = NULL;
    //DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(DUNGEON_VOID_FIELD_SOLO_ID);
    //if (!pDungeonSet)
    //    return;
    return;

    ChannelManager* pChannelManager = pDungeonSet->GetChannelManager();
    if (!pChannelManager)
        return;

    uint32 channelNum = pChannelManager->GetChannelNum();

    for(uint32 mType = VOID_FIELD_TYPE_BOSS; mType<m_OpenTimeList.size(); mType++)
    {
        // 开启人数和开启频道数
        if(mType == VOID_FIELD_TYPE_BOSS)
        {
            openPlayerNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldSoloRegularBossPlayerNum());
            openChannelNum = (uint32)(GameSettingManager::Instance()->GetVoidFieldSoloBossChannelNum()) - m_BossChannelList.size();
        }

        if (now > m_OpenTimeList[mType] && openChannelNum > 0 && CheckLastInChannelListState(mType) == true) // 检查开启时间和可开启频道数以及前一个已开启频道的状态是否是相同状态
        {
            // 获取满足开启人数条件的候选频道列表
            std::vector<uint32> candidateList;
            for (uint32 curChannel = 1; curChannel <= channelNum; curChannel++)
            {
                if (CheckInChannelList(mType, curChannel) == false)   // 剔除掉已经在 bossList
                {
                    //Dungeon* pDungeon = DungeonManager::Instance()->GetDungeon(DUNGEON_VOID_FIELD_SOLO_ID, curChannel);
                    //if (pDungeon)
                    //{
                    //    uint32 curPlayerNum = pDungeon->GetScene(1)->GetPlayerCount();
                    //    if (curPlayerNum >= openPlayerNum)
                    //        candidateList.push_back(curChannel);
                    //}
                }
            }

            // 根据珍兽的最大频道数随机选出频道
            if (candidateList.size() > 0)
            {
                uint32 numMin = (candidateList.size() < openChannelNum ) ? candidateList.size():openChannelNum;
                for (uint32 i = 0; i < numMin; i++) // 随机抽取
                {
                    uint32 rIdx = Crown::SDRandom(candidateList.size());
                    uint32 rChannel = candidateList[rIdx];
                    candidateList.erase(candidateList.begin()+rIdx);

                    //Dungeon* pDungeon = DungeonManager::Instance()->GetDungeon(DUNGEON_VOID_FIELD_SOLO_ID, rChannel);
                    //if (pDungeon)
                    //{
                    //    RegularVoidFieldSolo *pVoidFieldRegular = (RegularVoidFieldSolo *)(pDungeon->GetScene(1)->GetRegular());
                    //    if (pVoidFieldRegular && pVoidFieldRegular->GetCurrType() == VOID_FIELD_TYPE_NORMAL)
                    //    {
                    //        AddToChannelList(mType, rChannel);

                    //        pVoidFieldRegular->SetStateStartTime(mType);
                    //        pVoidFieldRegular->SetCurrType(mType);
                    //    }
                    //}
                }
            }
        }
    }
}

// ==================================================
// 虚空幻景频道信息请求
// ==================================================
void VoidFieldSoloManager::OnVoidFieldChannelInfoReq(void* data)
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    if(!data)
    {
        return;
    }

    // 检查在此GameServer中是否开启了虚空幻境，如果有，则直接发送消息
    // 否则需要通过CenterServer转发到相应的GameServer上应答
    //if(DungeonManager::Instance()->GetDungeonInfo(DUNGEON_VOID_FIELD_SOLO_ID))
    //{
    //    SendVoidFieldChannelInfoAck(ERR_SUCCEED, recvMsg->gate_id, 
    //            U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l));
    //    return;
    //}

    CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ, recvMsg);
}

// ==================================================
// 虚空幻景频道信息请求
// ==================================================
void VoidFieldSoloManager::OnPlayerChannelInfoReq(Player* player, void* data)
{
    VOID_FIELD_CHANNEL_INFO_REQ* recvMsg = (VOID_FIELD_CHANNEL_INFO_REQ*)data;
    if(!player || !data)
    {
        return;
    }

    // 对角色的 gateID 和 playerID 赋值
    recvMsg->gate_id = player->GetGateSvrID();
    recvMsg->player_id.id_h = U64ID_HIGH(player->GetPlayerID());
    recvMsg->player_id.id_l = U64ID_LOW(player->GetPlayerID());

    OnRecv(MSG_VOID_FIELD_SOLO_CHANNEL_INFO_REQ, data);
}

void VoidFieldSoloManager::OnSetHookPosReq(Player* player, void* data)
{
    VOID_FIELD_SET_HOOK_POS_REQ* recvMsg = (VOID_FIELD_SET_HOOK_POS_REQ*)data;
    if (!player || ! recvMsg)
    {
        return;
    }

    //player->SetFieldHookX(recvMsg->hook_x);
    //player->SetFieldHookY(recvMsg->hook_y);
}

void VoidFieldSoloManager::SendVoidFieldChannelInfoAck(uint16 errcode, uint32 gate_id, uint64 playerID)
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

    //dungeonSet = DungeonManager::Instance()->GetDungeonSet(DUNGEON_VOID_FIELD_SOLO_ID);
    if(!dungeonSet)
    {
        sendMsg.errcode = ERR_FAILED;
        goto _end;
    }
    else
    {
        // 世界等级
        sendMsg.world_level = CalWorldLevel();

        std::map<uint32, Dungeon*>& dungeons = dungeonSet->GetDungeons();
        for(std::map<uint32, Dungeon*>::iterator it = dungeons.begin(); it != dungeons.end(); ++it)
        {
            channelInfo.Clear();
            channelInfo.channel_num = it->second->GetChannelID();
            channelInfo.player_num = it->second->GetPlayerNum();
            if((scene = it->second->GetScene(1)) && (regular = scene->GetRegular()))
            {
                if(regular->GetType() == REGULAR_TYPE_VOID_FIELD_SOLO)
                {
                    channelInfo.void_field_type = ((RegularVoidFieldSolo*)regular)->GetCurrType();
                }
            }
            sendMsg.channel_list.push_back(channelInfo);
        }

        stateInfo.Clear();
        stateInfo.type = VOID_FIELD_TYPE_BOSS;
        stateInfo.open_time = m_OpenTimeList[VOID_FIELD_TYPE_BOSS];
        stateInfo.open_channel_num = (uint32)(GameSettingManager::Instance()->GetVoidFieldSoloBossChannelNum()) - m_BossChannelList.size();
        stateInfo.open_channel_max = (uint32)(GameSettingManager::Instance()->GetVoidFieldSoloBossChannelNum());
        sendMsg.state_list.push_back(stateInfo);
    }

_end:
    GateHandler::Instance()->SendByGateID(gate_id, MSG_VOID_FIELD_SOLO_CHANNEL_INFO_ACK, &sendMsg, playerID);
}

// ==================================================
// 检查能否进入虚空幻境
// ==================================================
uint16 VoidFieldSoloManager::CanEnterVoidField(Player* player)
{
    if(!player)
        return ERR_FAILED;

    // 世界等级未达到
    if (CalWorldLevel() < GameSettingManager::Instance()->GetVoidFieldSoloWorldLevelNeed())
    {
        return ERR_VOID_FIELD_WORLD_LEVEL_NOT_ENOUGH;
    }

    // 玩家等级未达到
    if (player->GetLevel() < VoidFieldSoloConfig::Instance()->GetPlayerLevelNeed())
    {
        return ERR_VOID_FIELD_PlAYER_LEVEL_NOT_ENOUGH;
    }

    return ERR_SUCCEED;
}

bool VoidFieldSoloManager::IsInBoosScene(Player* player)
{
    Scene* pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
    if (!pScene)
    {
        return false;
    }

    RegularVoidFieldSolo* pRegular = (RegularVoidFieldSolo*)pScene->GetRegular();
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

int32 VoidFieldSoloManager::CalWorldLevel()
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

void VoidFieldSoloManager::SetOpenTime(int32 mType)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if (mType == VOID_FIELD_TYPE_BOSS)
        m_OpenTimeList[VOID_FIELD_TYPE_BOSS] = now + GameSettingManager::Instance()->GetVoidFieldSoloBossCDTime();
}

bool VoidFieldSoloManager::IsLastInChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        if (m_BossChannelList.size() == 0)
            return false;

        if (m_BossChannelList.at(m_BossChannelList.size()-1) == mChannelId)
            return true;
    }

    return false;
}

void VoidFieldSoloManager::AddToChannelList(int32 mType, uint32 mChannelId)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        m_BossChannelList.push_back(mChannelId);
}

void VoidFieldSoloManager::RemoveFromChannelList(int32 mType, uint32 mChannelId)
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
}

void VoidFieldSoloManager::ClearChannelList(int32 mType)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        m_BossChannelList.clear();
}

bool VoidFieldSoloManager::CheckLastInChannelListState(int32 mType)
{
    uint32 channelId = 0;

    if (mType == VOID_FIELD_TYPE_BOSS)
    {
        if (m_BossChannelList.size() == 0)
            return true;

        channelId = m_BossChannelList.at(m_BossChannelList.size()-1);
    }

    //RegularVoidFieldSolo *pVoidFieldRegular = (RegularVoidFieldSolo *)(DungeonManager::Instance()->GetDungeon(DUNGEON_VOID_FIELD_SOLO_ID, channelId)->GetScene(1)->GetRegular());
    //if (pVoidFieldRegular)
    //{
    //    if (pVoidFieldRegular->GetCurrType() == mType)
    //        return true;
    //}

    return false;
}

bool VoidFieldSoloManager::CheckInChannelList(int32 mType, uint32 mChannelId)
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

    return false;
}

uint32 VoidFieldSoloManager::GetChannelListNum(int32 mType)
{
    if (mType == VOID_FIELD_TYPE_BOSS)
        return m_BossChannelList.size();

    return 0;
}
