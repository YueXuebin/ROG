#include "gameholder_pch.h"
#include "world_map_manager.h"
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
#include "regular_field_boss.h"


IMPLEMENT_SINGLETON(WorldMapManager)

WorldMapManager::WorldMapManager()
{
    m_world_level = 0;
}

WorldMapManager::~WorldMapManager()
{

}

void WorldMapManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FIELD_BOSS_INFO_REQ:
        OnFieldBossInfoReq(data);
        break;
    case MSG_FIELD_BOSS_PLAYER_IS_OFFLINE_ACK:
        PlayerIsOnLine(data);
        break;
    default:
        break;
    }
}

void WorldMapManager::PlayerIsOnLine(void* data)
{
    FIELD_BOSS_PLAYER_IS_OFFLINE_ACK* recMsg = (FIELD_BOSS_PLAYER_IS_OFFLINE_ACK*)data;
    if(!recMsg) return;

    //临时用
    for (uint32 dungeonId = WORLD_MAP_START_DUGEON_ID; dungeonId <= WORLD_MAP_END_DUGEON_ID; dungeonId++)
    {
        DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeonId);
        if(pDungeonSet) 
        {
            if(pDungeonSet->GetType() != DUNGEON_TYPE_WORLD) continue;

            std::map<uint32, Dungeon*>& dungeons = pDungeonSet->GetDungeons();
            for (std::map<uint32, Dungeon*>::const_iterator dungenonIter = dungeons.begin(); dungenonIter != dungeons.end(); ++dungenonIter)
            {
                std::map<uint32, Scene*>& scenes = dungenonIter->second->GetScenes();
                for(std::map<uint32, Scene*>::iterator sceneIt = scenes.begin(); sceneIt != scenes.end(); ++sceneIt)
                {
                    sceneIt->second->SendEvent(0, WORD_MAP_PLYAER_OFFLINE, recMsg->player_id.id_h, recMsg->player_id.id_l, 0);
                }
            }
        }
    }
}
void WorldMapManager::OnPlayerRecv(Player* player, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FIELD_BOSS_INFO_REQ:
        {
            FIELD_BOSS_INFO_REQ* recvMsg = (FIELD_BOSS_INFO_REQ*)data;
            if(!player || !data)
            {
                return;
            }

            // 对角色的 gateID 和 playerID 赋值
            recvMsg->gate_id = player->GetGateSvrID();
            recvMsg->player_id.id_h = U64ID_HIGH(player->GetPlayerID());
            recvMsg->player_id.id_l = U64ID_LOW(player->GetPlayerID());
            OnFieldBossInfoReq(data);
        }
        break;
    case MSG_FIELD_BOSS_SET_HOOK_POS_REQ:
        {
            if(!player) return;
            OnSetHookPosReq(player, data);
        }
         break;
    //case MSG_FIELD_BOSS_FRESH_TIME_REQ: //处理逻辑其他消息
    //    {
    //        FIELD_BOSS_FRESH_TIME_ACK sendMsg;
    //        //player->GetScene()->GetRegular()
    //        sendMsg.time = GetFreshBossTime();
    //        player->SendToGate(MSG_FIELD_BOSS_FRESH_TIME_ACK, &sendMsg);
    //    }
    //    break;
    default:
      //  OnRegularRecv(player, msgID, data);  //默认处理规则
        break;
    }

}
void WorldMapManager::Update(uint32 curr_tick)
{

}

void WorldMapManager::OnFieldBossInfoReq(void* data)
{
    FIELD_BOSS_INFO_REQ* recvMsg = (FIELD_BOSS_INFO_REQ*)data;
    if(!data)
    {
        return;
    }

    
    // 检查在此GameServer中是否开启了野外Boss，如果有，则直接发送消息
    // 否则需要通过CenterServer转发到相应的GameServer上应答
    if(DungeonManager::Instance()->GetDungeonInfo(recvMsg->dungeon_id))
    {
        SendFieldBossInfoAck(ERR_SUCCEED,   recvMsg->gate_id, 
            U64ID(recvMsg->player_id.id_h, recvMsg->player_id.id_l), recvMsg->dungeon_id);
        return;
    }

    CenterHandler::Instance()->SendByCenterID(ServerConfig::Instance()->GetCenterID(), MSG_FIELD_BOSS_INFO_REQ, recvMsg);
}

void WorldMapManager::SendFieldBossInfoAck(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 dungeon_id)
{
    FIELD_BOSS_INFO_ACK sendMsg;

    DungeonSet* pDungeonSet = DungeonManager::Instance()->GetDungeonSet(dungeon_id);
    std::map<uint32, Dungeon*>& dungeons = pDungeonSet->GetDungeons();
    for (std::map<uint32, Dungeon*>::const_iterator dungenonIter = dungeons.begin(); dungenonIter != dungeons.end(); ++dungenonIter)
    {
        // 无场景
        Scene* pScene = dungenonIter->second->GetDefaultScene();
        if (!pScene)
        {
            continue;
        }

        // 无规则
        RegularFiledBoss* pRegular = static_cast<RegularFiledBoss*>(pScene->GetRegular());
        if (!pRegular)
        {
            continue;
        }

        // Boss信息
        FIELD_BOSS_INFO bossInfo;
        bossInfo.dungeon_id = pScene->GetDungeonID();
        bossInfo.scene_id = pScene->GetSceneID();
        bossInfo.channel_id = pScene->GetChannelID();
        bossInfo.killer_name = pRegular->GetBossKillerName();

        sendMsg.boss_list.push_back(bossInfo);
        sendMsg.fresh_boss_time = pRegular->GetFreshBossTime();

    }

    GateHandler::Instance()->SendByGateID(gate_id, MSG_FIELD_BOSS_INFO_ACK, &sendMsg, playerID);
}

void WorldMapManager::OnRegularRecv(Player* player, uint32 msgID, void* data)
{
    RegularFiledBoss* pRegular = GetPlayerRegular(player);
    if (pRegular)
    {
        pRegular->OnPlayerRecv(player, msgID, data);
    }
}

RegularFiledBoss* WorldMapManager::GetPlayerRegular(Player* player)
{
    if (!player)
    {
        return NULL;
    }

    Scene* pScene = DungeonManager::Instance()->GetScene(player->GetUnionMapID());
    if (!pScene)
    {
        return NULL;
    }

    RegularFiledBoss* pRegular = (RegularFiledBoss*)pScene->GetRegular();
    return pRegular;
}

uint16 WorldMapManager::CanEnterFieldBoss(Player* player)
{
    if(!player)
        return ERR_FAILED;

    return ERR_SUCCEED;
}

int32 WorldMapManager::CalWorldLevel()
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
            total_level += iter->player_level;
        }

        world_level = total_level / top_rank_list->size();
    }

    SetWorldLevel(world_level);
    return world_level;
}


void WorldMapManager::OnSetHookPosReq(Player* player, void* data)
{
    FIELD_BOSS_SET_HOOK_POS_REQ* recvMsg = (FIELD_BOSS_SET_HOOK_POS_REQ*)data;
    if (!player || ! recvMsg)
    {
        return;
    }

    //player->SetFieldHookX(recvMsg->hook_x);
    //player->SetFieldHookY(recvMsg->hook_y);
}