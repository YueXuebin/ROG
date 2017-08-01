#include "gameholder_pch.h"
#include "regular_single.h"
#include "regular_event.h"
#include "game_util.h"
#include "battle_unit.h"
#include "scene.h"
#include "dungeon_gain_config.h"
#include "player.h"
#include "event_define.h"
#include "battle_recorder.h"
#include "common_reward_config.h"
#include "center_handler.h"
#include "player_dungeon_manager.h"
#include "soul_dungeon_manager.h"
#include "title_manager.h"
#include "trigger.h"
#include "dungeon_data.h"
#include "monster.h"
#include "boss.h"
#include "dungeon.h"
#include "dungeon_rule.h"
#include "game_define.h"


CnImplementRTTI(RegularSingle, RegularDungeon);

RegularSingle::RegularSingle(RegularDoc* pDoc) :
RegularDungeon(pDoc)
{
    m_Difficulty = 1;
}

RegularSingle::~RegularSingle()
{
}

void RegularSingle::ParseRegular(TiXmlElement* RegularElement)
{
    RegularDungeon::ParseRegular(RegularElement);

}

void RegularSingle::FirstUpdate()
{
    RegularDungeon::FirstUpdate();

    // 关闭跳转下一个场景的trigger
    Scene::TriggerMap& triggers = m_pScene->GetTriggers();
    for (Scene::TriggerMap::iterator triggerIter = triggers.begin(); triggerIter != triggers.end(); ++triggerIter)
    {
        if (triggerIter->second->GetTriggerId() == 2)
        {
            triggerIter->second->SetEnable(false);
        }
    }
}

void RegularSingle::OneSecondUpdate( int32 deltatime )
{
    RegularDungeon::OneSecondUpdate(deltatime);

}

void RegularSingle::OnCreatureEnter( BattleUnit* creature )
{
    RegularDungeon::OnCreatureEnter(creature);

    if(creature->GetGroup() == 0)
        SetDefaultGroup(creature);
}

void RegularSingle::OnCreatureLeave( BattleUnit* creature )
{
    RegularDungeon::OnCreatureLeave(creature);
}

void RegularSingle::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

}
// 场景通关
void RegularSingle::OnSceneClear(bool success, std::vector<std::string>* arglist)
{
    RegularDungeon::OnSceneClear(success, arglist);

    if(!m_pScene->GetCanClose())
    {
        // 非结算场景,开启triggerID=2
        Scene::TriggerMap& triggers = m_pScene->GetTriggers();
        for (Scene::TriggerMap::iterator triggerIter = triggers.begin(); triggerIter != triggers.end(); ++triggerIter)
        {
            if (triggerIter->second->GetTriggerId() == 2)
            {
                triggerIter->second->SetEnable(true);
            }
        }
    }
}
// 副本通关
void RegularSingle::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{
    RegularDungeon::OnDungeonClear(success, arglist);

    uint32 dungeonID = m_pScene->GetDungeonID();
    uint32 dungeonTime = m_pScene->GetDungeon()->GetDungeonTime();

    uint64 masterPlayerID = GetMasterPlayerID();

    // 检查房主是否还在
    bool hasMaster = false;
    for (Scene::PlayerMap::iterator it= m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        Player* player = it->second;
        CnAssert(player);
        if(masterPlayerID == player->GetPlayerID())
        {
            hasMaster = true;
            break;
        }
    }

    // 通关
    for(Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        Player* player = it->second;
        CnAssert(player);

        bool isMaster = (masterPlayerID == player->GetPlayerID());      // 是否是房主

        // 计算副本评价
        int32 score = CalcDungeonScore(player, success);

        bool isRewardDiscount = false;          // 奖励是否要打折扣

        // 通关胜利的处理
        if(success)
        {
            uint32 dungeonClass = GetDungeonClass(dungeonID);
            // 冒险地图
            if(dungeonClass == DUNGEON_CLASS_PLOT_ID)
            {
                // 冒险地图通关记录
                player->GetPlayerDungeonManager()->OnDungeonClearRecord(dungeonID, score);

                // 通关开启新关卡判断
                if(player->GetPlayerDungeonManager()->IsDungeonOpen(dungeonID))
                {
                    // 通关的副本才能开启下一关
                    player->GetPlayerDungeonManager()->OpenNextDungeon(dungeonID);
                }

                // 所有人获得活跃度
                player->SendMyselfEvent(LIVENESS_EVENT_TARGET_FINISH_PLOT_DUNGEON, 1, 0, 0);
            }
            // 异界地图
            else if (dungeonClass == DUNGEON_CLASS_SOUL_ID)
            {
                SoulDungeonManager* pSoulDungeonManager = player->GetSoulDungeonManager();

                bool canRaid = false;           // 玩家是否还有通关次数
                if(pSoulDungeonManager->GetRaidsDungeonTimes(dungeonID) > 0)
                    canRaid = true;

                if(isMaster)
                {   
                    // 副本完成事件
                    player->SendMyselfEvent(DUNGEON_FINISH, dungeonID, 0, 0);
                }

                if(canRaid)
                {
                    // 减少护魂副本次数
                    player->GetSoulDungeonManager()->DecRaidsDungeonTimes(dungeonID);
                }
                else
                {
                    isRewardDiscount = true;            // 没有次数的情况，则奖励打折
                }

                // 活跃度
                player->SendMyselfEvent(LIVENESS_EVENT_TARGET_PROTECT_SOUL, 1, 0, 0);

                player->GetSoulDungeonManager()->SendSoulDungeonTimesList();
            }

        } // if(success)

        // 副本结算奖励(每次通关)
        OnDungeonClearNotify(player, success, score, dungeonTime, isRewardDiscount, hasMaster);

        // 获取池到奖励池
        player->GainPoolToRewardInfo();
    }


    // 停掉所有怪AI
    GetScene()->SetCanUseAI(false);
}

void RegularSingle::OnDungeonClearNotify( Player* player, bool success, int32 score, uint32 dungeonTime, bool isDiscount, bool hasMaster)
{
    CnAssert(player);

    CLEAR_DUNGEON_NTF sendMsg;
    sendMsg.score = score;
    sendMsg.clear_time = dungeonTime / 1000;    // 秒
    sendMsg.combo_kill_num = player->GetBattleRecorder()->GetComboKillNumber();
    sendMsg.combo_kill_max = player->GetBattleRecorder()->GetComboKillMax();
    sendMsg.on_hit_num = player->GetBattleRecorder()->GetOnHitCount();
    sendMsg.absorb_soul = player->GetBattleRecorder()->GetAbsorbSoul();

    if(!success)
    {   // 通关失败
        sendMsg.errcode = ERR_FAILED;
        player->SendToGate(MSG_CLEAR_DUNGEON_NTF, &sendMsg);
        return;
    }
    else
        sendMsg.errcode = ERR_SUCCEED;

    int32 reward_times = 1;   // 需要根据等级计算
    int32 rand_times = 6;

    uint32 dungeonID = m_pScene->GetDungeonID();
    const DungeonGainTemplate* dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(dungeonID);         // 副本模板
    rand_times = (rand_times > reward_times) ? (rand_times - reward_times): 0;

    sendMsg.reward_times = reward_times;

    // 副本奖励
    if (dungeonTmpl)
    {
        // 获取额外奖励百分比
        real extraExpPercent = 0;
        real extraSoulPercent = 0;
        real extraGamePointPercent = 0;
        DungeonGainConfig::Instance()->GetExtraRewardPercent(sendMsg.score, player->GetVipLevel(), extraExpPercent, extraSoulPercent, extraGamePointPercent);

        // 计算总奖励奖励
        int32 exp = (int32)(dungeonTmpl->exp * (1 + extraExpPercent));
        int32 soul = (int32)(dungeonTmpl->soul * (1 + extraSoulPercent));
        int32 gamepoint = (int32)(dungeonTmpl->game_point * (1 + extraGamePointPercent));

        std::vector<REWARD_ITEM> gain_item_list;
        std::vector<REWARD_ITEM> rand_item_list;
        std::vector<REWARD_ITEM> vip_gain_item_list;
        std::vector<REWARD_ITEM> vip_rand_item_list;

        // 固定奖励
        reward_times = player->GetFcmGainValue(reward_times);
        for(int32 i = 0; i < reward_times; ++i)
        {
            DungeonGainConfig::Instance()->GetDungeonGainReward(dungeonTmpl, 1, gain_item_list);
        }
        // 随机奖励(仅翻牌显示用)
        rand_times = player->GetFcmGainValue(rand_times);
        for(int32 i = 0; i < rand_times; ++i)
        {
            DungeonGainConfig::Instance()->GetDungeonGainReward(dungeonTmpl, 1, rand_item_list);
        }

        // vip玩家或体验vip玩家
        if (player->GetVipLevel() > VIP_0 || player->IsExperienceVIP())
        {
            uint32 dungeonClass = GetDungeonClass(dungeonID);
            // 副本类型为剧情本或任务本,有第二次翻牌机会
            //if ((dungeonClass == DUNGEON_CLASS_PLOT_ID) || (dungeonClass == DUNGEON_CLASS_TASK_ID))
            //{
            //    // 固定奖励
            //    for(int32 i = 0; i < reward_times; ++i)
            //    {
            //        // 获取非上一次获得过的副本奖励
            //        DungeonGainConfig::Instance()->GetDungeonGainRewardEx(dungeonTmpl, 1, vip_gain_item_list, gain_item_list);
            //    }
            //    // 随机奖励(仅翻牌显示用)
            //    for(int32 i = 0; i < rand_times; ++i)
            //    {
            //        DungeonGainConfig::Instance()->GetDungeonGainReward(dungeonTmpl, 1, vip_rand_item_list);
            //    }
            //}
        }

        // 奖励折扣
        if(isDiscount)
        {
            MultiRewardItems(&gain_item_list, g_TeamMateRewardDisCount);
            MultiRewardItems(&rand_item_list, g_TeamMateRewardDisCount);
            MultiRewardItems(&vip_gain_item_list, g_TeamMateRewardDisCount);
            MultiRewardItems(&vip_rand_item_list, g_TeamMateRewardDisCount);
        }

        // 奖励发送给玩家
        player->GainExp(exp);
        player->GainGamePoint(gamepoint);
        player->GainSoulPoint(soul);

        for (std::vector<REWARD_ITEM>::const_iterator gainIt = gain_item_list.begin(); gainIt != gain_item_list.end(); ++gainIt)
        {
            PushDungenGain(player, *gainIt, sendMsg.gain_item_list);
        }
        for(std::vector<REWARD_ITEM>::iterator gainIt = rand_item_list.begin(); gainIt != rand_item_list.end(); ++gainIt)
        {
            PushDungenGain(NULL, *gainIt, sendMsg.rand_item_list);
        }
        for (std::vector<REWARD_ITEM>::const_iterator gainIt = vip_gain_item_list.begin(); gainIt != vip_gain_item_list.end(); ++gainIt)
        {
            PushDungenGain(player, *gainIt, sendMsg.vip_gain_item_list);
        }
        for(std::vector<REWARD_ITEM>::iterator gainIt = vip_rand_item_list.begin(); gainIt != vip_rand_item_list.end(); ++gainIt)
        {
            PushDungenGain(NULL, *gainIt, sendMsg.vip_rand_item_list);
        }

        // 通过副本召唤宠物条件
        player->SendMyselfEvent(EVENT_ACIIVATION_PET, dungeonTmpl->id, ACTIVATION_DUGEON, 0);
    }

    // 发送副本结算消息
    //sendMsg.gain_exp = player->GetGainPool().exp;
    //sendMsg.gain_gp = player->GetGainPool().game_point;
    //sendMsg.gain_soul = player->GetGainPool().soul_point;

    // 副本结算通知
    player->SendToGate(MSG_CLEAR_DUNGEON_NTF, &sendMsg);
}

void RegularSingle::PushDungenGain(Player* player, const REWARD_ITEM& reward, std::vector<REWARD_ITEM>& list)
{
    if(player)
        player->GainItem(reward);       // 
    list.push_back(reward);   
}

bool RegularSingle::IsGameStart()
{
    return IsAnyoneStart();     // 由该玩家控制，开始和结束
}

void RegularSingle::OnChangeDifficulty(uint32 DifficultLevel)
{
    // 对关卡中的所有怪物调整难度
    Scene::MonsterMap& monsters = GetScene()->GetMonsters();
    for(Scene::MonsterMap::iterator mIter = monsters.begin(); mIter != monsters.end(); mIter++)
    {
        Monster* pMonster = mIter->second;
        ChangeStrong(pMonster);
    }

    Scene::BossMap& boss = GetScene()->GetBosses();
    for(Scene::BossMap::iterator bIter = boss.begin(); bIter != boss.end(); bIter++)
    {
        Boss* pBoss = bIter->second;
        ChangeStrong(pBoss);
    }
}

int32 RegularSingle::CalcDifficultyLevel(uint32 playerNum)
{
    // 根据人数决定难度
    uint32 newDifficulty = 1;
    if(playerNum <= 1)
        newDifficulty = 1;
    else if(playerNum == 2)
        newDifficulty = 2;
    else
        newDifficulty = 3;

    return newDifficulty;
}

void RegularSingle::OnPlayerEnterDungeonNtf(uint64 playerID, const std::string& playerName)
{
    // 玩家进入改变难度
    Scene* pScene = GetScene();
    Dungeon* pDungeon = pScene->GetDungeon();
    uint32 playerNum = pDungeon->GetPlayerCount();
    if(playerNum == 1)
        return;

    // 计算难度
    int32 oldDifficulty = m_Difficulty;
    m_Difficulty = CalcDifficultyLevel(playerNum);
    int32 deltaDifficulty = m_Difficulty - oldDifficulty;

    if(deltaDifficulty != 0)
        OnChangeDifficulty(m_Difficulty);       // 调整关卡难度

    // 通知
    if(!pDungeon->IsClose())
        NotifyPlayerEnterDungeon(playerID, playerName, deltaDifficulty);
}

void RegularSingle::OnPlayerLeaveDungeonNtf(uint64 playerID, const std::string& playerName)
{
    // 玩家离开改变难度
    Scene* pScene = GetScene();
    Dungeon* pDungeon = pScene->GetDungeon();
    uint32 playerNum = pDungeon->GetPlayerCount();
    if(playerNum == 0)
        return;

    // 计算难度
    int32 oldDifficulty = m_Difficulty;
    m_Difficulty = CalcDifficultyLevel(playerNum);
    int32 deltaDifficulty = m_Difficulty - oldDifficulty;

    if(deltaDifficulty != 0)
        OnChangeDifficulty(m_Difficulty);       // 调整关卡难度

    // 通知
    if(!pDungeon->IsClose())
        NotifyPlayerLeaveDungeon(playerID, playerName, deltaDifficulty);
}

void RegularSingle::NotifyPlayerEnterDungeon(uint64 playerID, const std::string& playerName, int32 DeltaDifficultLevel)
{
    Scene* pScene = GetScene();
    // 通知客户端玩家进入副本
    TEAM_MEMBER_ENTER_NTF ntf;
    ntf.player_id_h = U64ID_HIGH(playerID);
    ntf.player_id_l = U64ID_LOW(playerID);
    ntf.player_name = playerName;
    ntf.delta_difficult_level = DeltaDifficultLevel;

    // 通知场景信息
    SCENE_INFO_ACK sceneInfoAck;
    FillSceneInfo(sceneInfoAck);

    const Scene::PlayerMap& players = pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* player = playerIt->second;
        player->SendToGate(MSG_TEAM_MEMBER_ENTER_NTF, &ntf);
        player->SendToGate(MSG_SCENE_INFO_ACK, &sceneInfoAck);
    }
}

void RegularSingle::NotifyPlayerLeaveDungeon(uint64 playerID, const std::string& playerName, int32 DeltaDifficultLevel)
{
    Scene* pScene = GetScene();
    // 通知客户端玩家离开副本
    TEAM_MEMBER_LEAVE_NTF ntf;
    ntf.player_id_h = U64ID_HIGH(playerID);
    ntf.player_id_l = U64ID_LOW(playerID);
    ntf.player_name = playerName;
    ntf.delta_difficult_level = DeltaDifficultLevel;

    // 通知场景信息
    SCENE_INFO_ACK sceneInfoAck;
    FillSceneInfo(sceneInfoAck);

    const Scene::PlayerMap& players = pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* other = playerIt->second;
        other->SendToGate(MSG_TEAM_MEMBER_LEAVE_NTF, &ntf);
        other->SendToGate(MSG_SCENE_INFO_ACK, &sceneInfoAck);
    }
}   

uint64 RegularSingle::GetMasterPlayerID()
{
    return m_pScene->GetDungeon()->GetDungeonRule()->GetMasterPlayerID();
}