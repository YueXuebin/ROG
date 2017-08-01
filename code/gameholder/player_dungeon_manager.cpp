#include "gameholder_pch.h"
#include "player_dungeon_manager.h"
#include "state_define.h"
#include "player.h"
#include "action_controller.h"
#include "task_manager.h"
#include "dungeon_data.h"
#include "item_base.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "trigger.h"
#include "vip_config.h"
#include "game_util.h"
#include "event_define.h"
#include "dungeon_gain_config.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "soul_dungeon_manager.h"
#include "team_manager.h"
#include "dungeon.h"
#include "dungeon_type.h"
#include "faction_manager.h"
#include "activity_manager.h"
#include "ashura_manager.h"
#include "goldtree_manager.h"
#include "treasury_manager.h"
#include "dungeon_times_manager.h"
#include "guild_manager.h"
#include "plot_dungeon_manager.h"
#include "soul_dungeon_manager.h"
#include "daily_dungeon_manager.h"
#include "capture_flag_manager.h"
#include "zombie_manager.h"
#include "void_field_manager.h"
#include "void_field_solo_manager.h"
#include "action_player_controller.h"
#include "drop_object.h"
#include "answer_manager.h"
#include "center_handler.h"
#include "regular_dungeon.h"
#include "state_controller.h"
#include "bag_manager.h"
#include "dungeon_config_manager.h"
#include "item_manager.h"
#include "game_define.h"


PlayerDungeonManager::PlayerDungeonManager(Player* player):
m_player(player)
{
    m_plotDungeonMgr = CnNew PlotDungeonManager(player);
    m_soulDungeonMgr = CnNew SoulDungeonManager(player);
    m_dailyDungeonMgr = CnNew DailyDungeonManager(player);
}

PlayerDungeonManager::~PlayerDungeonManager()
{
    if(m_soulDungeonMgr)
    {
        CnDelete m_soulDungeonMgr;
        m_soulDungeonMgr = NULL;
    }

    if(m_plotDungeonMgr)
    {
        CnDelete m_plotDungeonMgr;
        m_plotDungeonMgr = NULL;
    }

    if(m_dailyDungeonMgr)
    {
        CnDelete m_dailyDungeonMgr;
        m_dailyDungeonMgr = NULL;
    }
}

void PlayerDungeonManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MOVE_REQ:          // 移动
        {
            OnMove(data);
        }
        break;
    case MSG_JUMP_REQ:          // jump
        {
            OnJump(data);
        }
        break;
    case MSG_JUMP_DUNGEON_REQ:          // 跳地图请求
        {
            OnJumpDungeonReq(data);
        }
        break;
    case MSG_CHANGE_DUNGEON_REQ:        // 跳副本请求
        {
            OnChangeDungeonReq(data);
        }
        break;
    case MSG_CHANGE_SCENE_REQ:          // 跳场景请求
        {
            OnChangeScene(data);
        }
        break;
    case MSG_DUNGEON_SCORE_INFO_REQ:
        {
            OnDungeonScoreInfoReq(data);
        }
        break;
    case MSG_GAME_START_REQ:            // 客户端准备好,通知服务器可以开始逻辑
        {
            OnGameStart(data);
        }
        break;
    case MSG_SCENE_INFO_REQ:            // 请求场景信息(频道...)
        {
            OnSceneInfoReq(data);
        }
        break;
    case MSG_NOTIFY_PICK_DROP_OJBECT:   // 拾取物品
        {
           OnPickUpDropItem(data);
        }
        break;
    case MSG_DUNGEON_RUN_TIME_INFO_REQ:
        {
            OnDungeonRunTimeInfoReq(data);
        }
        break;
    case MSG_DUNGEON_CONDITION_REQ:     // 获取副本通关条件
        {
            OnDungeonConditionReq(data);
        }
        break;
    default:
        break;
    }

}
// 拾取掉落物品
void PlayerDungeonManager::OnPickUpDropItem(void* data)
{
    Scene* scene = m_player->GetScene();
    if(!scene) return;

    NOTIFY_PICK_DROP_OJBECT_REQ* recvMsg = (NOTIFY_PICK_DROP_OJBECT_REQ*)data;
    if(!recvMsg) return;

    REWARD_TABLE reward_table;
    reward_table.reason = REWARD_REASON_PICK_DROP_OBJECT;

    bool b_exe = false;
    std::vector<PICK_DROP_OBJECT>::iterator iter;
    for (iter = recvMsg->drop_object_list.begin(); iter!= recvMsg->drop_object_list.end(); iter++)
    {
        DropObject* dropObj = scene->GetDropItem((*iter).object_id);
        if(dropObj)
        {
            if(dropObj->GetOwnerID() == 0 || (dropObj->GetOwnerID() == m_player->GetId()))
            {
                ItemBase* pItem = dropObj->ExtractItem();
                if(pItem)
                {
                    if (ERR_SUCCEED == m_player->GetItemManager()->AddToBag(REASON_ITEM_ADD_DROP, pItem))
                    {
                        dropObj->TransferItemOwnership();
                        dropObj->DestroyDropObject();
                        
                        // 拾取通知
                        PICK_DROP_OBJECT_NTF ntf;
                        ntf.drop_object_id = dropObj->GetId();
                        ntf.owner_id = m_player->GetId();
                        m_player->GetScene()->NotifyMessageToAll(MSG_PICK_DROP_OBJECT_NTF, &ntf);

                        b_exe = true;
                    }
                }
                else
                {
                    // 掉落物品已经被拾取过了
                }
            }
            else
                break;
        }
    }

    if(reward_table.items.size() > 0 && b_exe)
       m_player->TakeInRewardTable(&reward_table);
}
// ==================================================
// 刷新副本次数
// ==================================================
void PlayerDungeonManager::DungeonTimesRefresh()
{
    m_plotDungeonMgr->Refresh();
    m_soulDungeonMgr->Refresh();
    m_dailyDungeonMgr->Refresh();
}

// ==================================================
// 处理移动消息
// ==================================================
void PlayerDungeonManager::OnMove(void* data)
{
    CREATURE_MOVE* recvMsg = (CREATURE_MOVE*)data;

    bool canMove = true;
    if(recvMsg->is_moving)      // 持续移动
    {   // 进入移动状态
        canMove = m_player->CanChangeTo(STATE_RUN);
        if(canMove)
            m_player->ChangeState(STATE_RUN);
    }
    else                        // 停止移动
    {   // 进入idle状态
        canMove = m_player->CanChangeTo(STATE_IDLE);
        if(canMove)
            m_player->ChangeState(STATE_IDLE);
    }

    ActionController* actionController = m_player->GetActionController();
    actionController->AngleTo(recvMsg->angle);
    
    if(recvMsg->is_moving)
    {
        m_player->SetPhysicsVelocity(CoVec2(recvMsg->vel_x, recvMsg->vel_y));
        actionController->RunTo(recvMsg->pos_x, recvMsg->pos_y);
    }
    else
    {
        m_player->SetPhysicsVelocity(CoVec2(0, 0));
        ((ActionPlayerController*)actionController)->StopAt(recvMsg->pos_x, recvMsg->pos_y);
    }

}

void PlayerDungeonManager::OnJump(void* data)
{
    CREATURE_MOVE* recvMsg = (CREATURE_MOVE*)data;
    ActionController* actionController = m_player->GetActionController();
    actionController->AngleTo(recvMsg->angle);
    actionController->JumpTo(recvMsg->pos_x, recvMsg->pos_y);
}

void PlayerDungeonManager::OnJumpDungeonReq(void* data)
{
    CHANGE_DUNGEON_REQ* recvMsg = (CHANGE_DUNGEON_REQ*)data;

    // 消耗传送石
    // ...


    // 判断是否为同副本
    if(recvMsg->dungeon_id == DUNGEON_ID(m_player->GetUnionMapID()))
    {
        int32 newX = recvMsg->pos_x;
        int32 newY = recvMsg->pos_y;
        CoVec2 newPos = m_player->GetScene()->FindValidPositionInCircle(CoVec2(newX, newY), g_TeamFlyNearDistance, m_player->GetRadius());
        int32 newAngle = m_player->GetAngle();
        m_player->GetActionController()->JumpTo(int32(newPos.x), int32(newPos.y));
        m_player->GetActionController()->AngleTo(newAngle);
        return;
    }
    else
    {
        OnChangeDungeonReq(recvMsg);
    }
}
// ==================================================
// 处理切换副本
// ==================================================
void PlayerDungeonManager::OnChangeDungeonReq(void* data)
{
    CHANGE_DUNGEON_REQ* recvMsg = (CHANGE_DUNGEON_REQ*)data;
    DungeonTemplate* dungeonTmpl = 0;
    uint16 errCode = ERR_FAILED;
    int32 vigor = 0;
    
    if(recvMsg->jump_type != DUNGEON_JUMP_TRIGGER
        && recvMsg->jump_type != DUNGEON_JUMP_TEAM
        && recvMsg->jump_type != DUNGEON_JUMP_POS
        && recvMsg->jump_type != DUNGEON_JUMP_PLAYER)
    {
        CnAssert(false);        // 跳副本类型错误
        goto _ERR;
    }

    // 判断条件
    errCode = CanChangeToDungeon(m_player, recvMsg->dungeon_id, recvMsg->scene_id);
    if(errCode != ERR_SUCCEED)
    {
        goto _ERR;
    }

    // 进入其他副本前处理
    m_player->ReadyChangeToDungeon(recvMsg);

    // 如果发送的 dungeon_id 为 0 则表明角色希望退出副本，返回大地图
    if (recvMsg->dungeon_id == 0)
    {
        uint32 backUnionID = m_player->GetCityMapID();
        if(DUNGEON_ID(backUnionID) == 0)
        {
            CnAssert(false);
            backUnionID = UNION_ID(0, CITY_MAIN_CITY_MAP, 0);
        }
        recvMsg->dungeon_id = DUNGEON_ID(backUnionID);
        recvMsg->scene_id   = SCENE_ID(backUnionID);
        recvMsg->channel_id = CHANNEL_ID(backUnionID);

        // 清空获取池
        m_player->ClearGainPool();
    }
    else
    {

    }

    m_player->SendToCenter(MSG_CHANGE_DUNGEON_REQ, recvMsg);

    return;

_ERR:
    m_player->SendChangeDungeonErrorAck(errCode);
}

// ==================================================
// 处理请求副本分数信息
// ==================================================
void PlayerDungeonManager::OnDungeonScoreInfoReq(void* data)
{
    if(!data) return;

    m_player->SendToCenter(MSG_DUNGEON_SCORE_INFO_REQ, data);
}

// ==================================================
// 处理切换场景
// ==================================================
void PlayerDungeonManager::OnChangeScene(void* data)
{
    CHANGE_SCENE_REQ* recvMsg = (CHANGE_SCENE_REQ*)data;

    uint32 channelID = 0;
    uint32 dungeonID = 0;
    uint32 sceneID = 0;
    uint32 newUnionMapID = 0;
    int32 newX = 0;
    int32 newY = 0;
    int32 newAngle = 0;
    Scene* pNewScene = NULL;
    
    Trigger* pTrigger = NULL;
    Teleport* pTeleport = NULL;
    Trigger* pTargetTirgger = NULL;
    uint16 errCode = ERR_FAILED;

    // 获得当前场景
    Scene* pOldScene = m_player->GetScene();

    channelID = CHANNEL_ID(m_player->GetUnionMapID());
    dungeonID = DUNGEON_ID(m_player->GetUnionMapID());
    sceneID = recvMsg->scene_id;

    if(sceneID == pOldScene->GetSceneID())
    {   // 同场景跳转
        CnWarn("jump to same scene\n");
        goto _ERROR;
    }

    // 新场景ID
    newUnionMapID = UNION_ID(channelID, dungeonID, sceneID);

    // 获得新场景
    pNewScene = DungeonManager::Instance()->GetScene(newUnionMapID);
    if(!pNewScene)
    {
        CnWarn("can't find dungeon %u\n", m_player->GetUnionMapID());
        goto _ERROR;
    }

    // 获得目标位置
    pTargetTirgger = pNewScene->GetTriggerByID(recvMsg->trigger_id);
    if(pTargetTirgger)
    {
        newX = pTargetTirgger->GetX();
        newY = pTargetTirgger->GetY();
        newAngle = pTargetTirgger->GetAngle();
    }

    // 发送跳场景成功
    errCode = ERR_SUCCEED;
    m_player->SendChangeSceneAck(errCode, newUnionMapID, newX, newY, newAngle);

    // 切换场景前处理
    ReadyChangeScene();

    // 从原场景中删除角色
    pOldScene->DelPlayer(m_player->GetId());

    // 将角色添加到新的场景中
    m_player->SetX(newX);
    m_player->SetY(newY);
    pNewScene->AddPlayer(m_player);

    // 则保存地图ID和X,Y
    CenterHandler::Instance()->SavePlayerDungeonPos(pNewScene->GetDungeon(), m_player);

    // 跳场景后的处理
    m_player->SendToCenterTeamMemberProp();

    return;

_ERROR:
    m_player->SendChangeSceneAck(errCode, 0, 0, 0, 0);
    return;
}

// 处理游戏开始
void PlayerDungeonManager::OnGameStart(void* data)
{
    GAME_START* pGameStart = (GAME_START*)data;
    m_player->SetGameStart(pGameStart->start!=0);
}

// 请求场景信息
void PlayerDungeonManager::OnSceneInfoReq(void* data)
{
    SCENE_INFO_ACK ack;
    m_player->GetScene()->GetRegular()->FillSceneInfo(ack);
    m_player->SendToGate(MSG_SCENE_INFO_ACK, &ack);
}

void PlayerDungeonManager::FillPlayerDungeonInfo(PLAYER_DUNGEON_INFO& info) const
{
    info.open_dungeon_info = m_OpenDungeonInfo;
    info.dungeon_record = m_DungeonRecord;

    m_plotDungeonMgr->FillPlotDungeonInfo(info.plot_dungeon_info);
    m_soulDungeonMgr->FillSoulDungeonInfo(info.soul_dungeon_info);
    m_dailyDungeonMgr->FillDailyDungeonInfo(info.daily_dungeon_info);
}

void PlayerDungeonManager::LoadPlayerDungeonInfo(const PLAYER_DUNGEON_INFO& info)
{
    m_OpenDungeonInfo = info.open_dungeon_info;
    m_DungeonRecord = info.dungeon_record;

    m_plotDungeonMgr->LoadPlotDungeonInfo(info.plot_dungeon_info);
    m_soulDungeonMgr->LoadSoulDungeonInfo(info.soul_dungeon_info);
    m_dailyDungeonMgr->LoadDailyDungeonInfo(info.daily_dungeon_info);

    // 默认开启的地图
    if(!IsDungeonOpen(2001))            // 冒险地图关卡1
    {
        OpenDungeon(2001);
    }
    if(!IsDungeonOpen(1101))            // 野外地图1
    {
        OpenDungeon(1101);
    }
    CheckFieldDungeonOpen();
    CheckSoulDungeonOpen();
    CheckDailyDungeonOpen();
}

// ==================================================
// 副本是否开启
// ==================================================
bool PlayerDungeonManager::IsDungeonOpen(uint32 dungeonID)
{
    for(std::vector<uint32>::iterator iter = m_OpenDungeonInfo.open_dungeon_list.begin(); iter != m_OpenDungeonInfo.open_dungeon_list.end(); iter++)
    {
        uint32 openDungeonId = *iter;
        if(dungeonID == openDungeonId)
            return true;
    }
    return false;
}

void PlayerDungeonManager::OpenDungeon(uint32 dungeonID)
{
    bool isOpen = false;
    for(std::vector<uint32>::iterator iter = m_OpenDungeonInfo.open_dungeon_list.begin(); iter != m_OpenDungeonInfo.open_dungeon_list.end(); iter++)
    {
        uint32 openDungeonId = *iter;
        if(dungeonID == openDungeonId)
        {
            isOpen = true;
        }
    }

    if(!isOpen)
    {
        m_OpenDungeonInfo.open_dungeon_list.push_back(dungeonID);
        // 通知客户端
        m_player->SendToGate(MSG_OPEN_DUNGEON_NTF, &m_OpenDungeonInfo);
    }
}

void PlayerDungeonManager::CloseDungeon(uint32 dungeonID)
{
    bool isClose = false;
    for(std::vector<uint32>::iterator iter = m_OpenDungeonInfo.open_dungeon_list.begin(); iter != m_OpenDungeonInfo.open_dungeon_list.end(); iter++)
    {
        uint32 openDungeonId = *iter;
        if(dungeonID == openDungeonId)
        {
            m_OpenDungeonInfo.open_dungeon_list.erase(iter);
            isClose = true;
            break;
        }
    }

    if(!isClose)
    {
        m_OpenDungeonInfo.open_dungeon_list.push_back(dungeonID);
        // 通知客户端
        m_player->SendToGate(MSG_OPEN_DUNGEON_NTF, &m_OpenDungeonInfo);
    }
}

void PlayerDungeonManager::OpenNextDungeon(uint32 dungeonID)
{
    uint32 dungeonClass = GetDungeonClass(dungeonID);
    // 剧情副本地图
    if(dungeonClass == DUNGEON_CLASS_PLOT_ID)
    {
        // 开启难度2
        if(dungeonID >= 2001 && dungeonID <= 2099)
        {
            OpenDungeon(dungeonID+100);
        }
        // 开启难度3
        else if(dungeonID >= 2101 && dungeonID <= 2199)
        {
            OpenDungeon(dungeonID+100);
        }

        OpenDungeon(dungeonID+1);   // 开启冒险地图下一关
    }
    // 异界地图
    else if(dungeonClass == DUNGEON_CLASS_SOUL_ID)
    {

    }
    // 日常地图
    else if(dungeonClass == DUNGEON_CLASS_DAILY_ID)
    {
        
    }

    CheckFieldDungeonOpen();
    CheckSoulDungeonOpen();
    CheckDailyDungeonOpen();
}


void PlayerDungeonManager::OnDungeonClearRecord(uint32 dungeonID, int32 score)
{
    // 副本奖励
    REWARD_TABLE rewards;
    bool needSend = false;
    int32 oldScore = REGULAR_SCORE_F;
    int32 addExp(0);

    // 通关奖励
    const SDungeonClearanceBonusConfig* pConfig = DungeonConfigManager::Instance()->FindClearanceBonusByID(dungeonID);
    if (pConfig)
    {
        rewards = pConfig->ClearanceBonus;
        addExp = pConfig->pStruct->clearance_exp;
    }

    uint32 index = 0;
    for (index=0; index<m_DungeonRecord.dungeon_record_list.size(); ++index)
    {
        if (m_DungeonRecord.dungeon_record_list[index].dungeon_id == dungeonID)
        {
            oldScore = m_DungeonRecord.dungeon_record_list[index].score;
            break;
        }
    }

    if(index >= m_DungeonRecord.dungeon_record_list.size())
    {
        CnAssert(oldScore == REGULAR_SCORE_F);
        needSend = true;

        DUNGEON_CLEAR_RECORD dungeonRecord;
        dungeonRecord.dungeon_id = dungeonID;
        dungeonRecord.score = oldScore;
        dungeonRecord.exp = addExp;
        m_DungeonRecord.dungeon_record_list.push_back(dungeonRecord);
    }
    else
    {
        m_DungeonRecord.dungeon_record_list[index].exp += addExp;
    }

    if(score > oldScore)
    {
        needSend = true;

        CnAssert(score > m_DungeonRecord.dungeon_record_list[index].score);
        m_DungeonRecord.dungeon_record_list[index].score = score;

        if(oldScore <= REGULAR_SCORE_F && score >= REGULAR_SCORE_C)
        {   // 通关奖励,一星奖励

        }
        if(oldScore <= REGULAR_SCORE_C && score >= REGULAR_SCORE_B)
        {   // 两星奖励
            if (pConfig)
            {
                MergeRewardTable(rewards, pConfig->TwoStarBonus);
            }
        }
        if(oldScore <= REGULAR_SCORE_B && score >= REGULAR_SCORE_A)
        {   // 三星奖励
            if (pConfig)
            {
                MergeRewardTable(rewards, pConfig->ThreeStarBonus);
            }
        }
    }

    m_player->GetItemManager()->AddRewardTable(&rewards);

    if (needSend)
    {
        m_player->SendToGate(MSG_DUNGEON_RECORD_NTF, &m_DungeonRecord);
    }
}

void PlayerDungeonManager::CheckFieldDungeonOpen()
{
    // 野外地图依照冒险地图开启
    if(IsDungeonOpen(2013))
        OpenDungeon(1102);
    if(IsDungeonOpen(2025))
        OpenDungeon(1103);
    if(IsDungeonOpen(2037))
        OpenDungeon(1104);
    if(IsDungeonOpen(2049))
        OpenDungeon(1105);
    if(IsDungeonOpen(2061))
        OpenDungeon(1106);
}

void PlayerDungeonManager::CheckSoulDungeonOpen()
{

}

void PlayerDungeonManager::CheckDailyDungeonOpen()
{
    //if(!IsDungeonOpen(2901))
    //{
    //    OpenDungeon(2901);
    //}
    //if(!IsDungeonOpen(2902))
    //{
    //    OpenDungeon(2902);
    //}
    //if(!IsDungeonOpen(2903))
    //{
    //    OpenDungeon(2903);
    //}
    //if(!IsDungeonOpen(2904))
    //{
    //    OpenDungeon(2904);
    //}
    //if(!IsDungeonOpen(2905))
    //{
    //    OpenDungeon(2905);
    //}
    //if(!IsDungeonOpen(2906))
    //{
    //    OpenDungeon(2906);
    //}
    //if(!IsDungeonOpen(2907))
    //{
    //    OpenDungeon(2907);
    //}
    //if(!IsDungeonOpen(2908))
    //{
    //    OpenDungeon(2908);
    //}

}

// ==================================================
// 是否能够进入副本
// ==================================================
uint16 PlayerDungeonManager::CanChangeToDungeon(Player* player, uint32 dungeonID, uint32 sceneID)
{
    uint16 errCode = ERR_SUCCEED;

    if(!player)
    {
        CnError("Error: no map\n");
        return ERR_FAILED;
    }

    // 允许GM随便跳转副本
    if(player->IsGMJump())
    {
        return ERR_SUCCEED;
    }

    uint32 dungeonClass = GetDungeonClass(dungeonID);

    if(dungeonClass == DUNGEON_CLASS_PLOT_ID)
    {   // 冒险地图
        //return ChangePlotDungeon();
    }

    //// 进入次数是否足够
    //if(!m_player->GetDungeonTimesManager()->HasEnterCount(dungeonID))
    //{
    //    errCode = ERR_CHANGE_DUNGEON_NO_COUNT;
    //    goto _END;
    //}

    // 如果发送的 dungeonID 为 0 则表明角色希望退出副本，返回城镇
    // 因为组队需要下副本，所以允许客户端做返回城镇的操作
    // 检查返回城镇的ID有效性
    if (dungeonID == 0 && !m_player->GetCityMapID())
    {
        errCode = ERR_CHANGE_DUNGEON_NOT_BACKCITY;
        goto _END;
    }

    // 检查副本是否存在
    if (dungeonID != 0)
    {
        DungeonTemplate* pDungeonTemplate = DungeonManager::Instance()->GetDungeonTemplate(dungeonID);
        if(!pDungeonTemplate)
        {
            errCode = ERR_CHANGE_DUNGEON_NO_DUNGEON;
            goto _END;
        }
        else if(sceneID != 0)
        {   // 检查场景是否存在
            bool isSceneExit = false;
            for(std::list<SceneTemplate>::iterator iter = pDungeonTemplate->m_scene_list.begin(); iter != pDungeonTemplate->m_scene_list.end(); iter++)
            {
                if(iter->m_scene_id == sceneID)
                    isSceneExit = true;
            }

            if(!isSceneExit)
            {
                errCode = ERR_CHANGE_DUNGEON_NO_DUNGEON;
                goto _END;
            }
        }
    }

_END:
    return errCode;
}

uint32 PlayerDungeonManager::GetPlotDungonScore()
{
    std::vector<DUNGEON_CLEAR_RECORD>::const_iterator cIter =   m_DungeonRecord.dungeon_record_list.begin();
    uint32 totalScore = 0;

    for(cIter; cIter!= m_DungeonRecord.dungeon_record_list.end(); cIter++)
    {
        if(GetDungeonClass(cIter->dungeon_id) == DUNGEON_CLASS_PLOT_ID)
        {
            uint32 score = cIter->score;
            /*switch (cIter->score)
            {
                case 'A':
                    score = 3;
                    break;
                case 'B':
                    score = 2;
                    break;
                case 'C':
                    score = 1;
                    break;
            }*/
            
            totalScore += score;
        }
    }

    return totalScore;
}

void PlayerDungeonManager::ReadyChangeScene()
{
    // 跳场景前把玩家置为待机状态
    m_player->GetActionController()->Stop();
}

void PlayerDungeonManager::OnDungeonRunTimeInfoReq( void* data )
{
    ERROR_CODE* recvMsg = (ERROR_CODE*)data;
    if(!recvMsg) return;
    
    DUNGEON_RUN_ACK sendMsg; 
    sendMsg.type = recvMsg->errcode;

    //if(recvMsg->errcode == OPEN_SYS_ONLINE_REWARD)
    //{
    //    RegularDungeon *pDungeonRegular = (RegularDungeon *)(m_player->GetScene()->GetRegular());
    //    sendMsg.time = pDungeonRegular->GetCurrentTime() / 1000;
    //}

    m_player->SendToGate(MSG_DUNGEON_RUN_TIME_INFO_ACK, &sendMsg);
    
}

void PlayerDungeonManager::OnDungeonConditionReq( void* data )
{
    RegularDungeon *pDungeonRegular = (RegularDungeon *)(m_player->GetScene()->GetRegular());
    pDungeonRegular->NotifyDungeonCondition(m_player);
}
