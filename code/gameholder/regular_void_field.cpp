#include "gameholder_pch.h"
#include "regular_void_field.h"
#include "void_field_manager.h"
#include "void_field_config.h"
#include "pet.h"
#include "event_define.h"
#include "scene.h"
#include "dungeon.h"
#include "enemy_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "game_util.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "item_manager.h"
#include "trigger.h"
#include "monster.h"
#include "spawn.h"
#include "setting_manager.h"
#include "regular_dungeon.h"
#include "game_setting_manager.h"
#include "checkpoint.h"
#include "boss.h"
#include "ai_controller.h"
#include "guild_manager.h"
#include "world_boss_manager.h"
#include "dungeon_manager.h"


void VoidFieldPlayerRecord::AddChestsNum(int32 val)
{
    int64 chests_num = 0;
    CnAssert(val >= 0);
    chests_num = m_chests_num + val;
    if(chests_num < 0)
    {
        chests_num = 0;
    }
    else if(chests_num > MAX_VOID_FIELD_CHESTS_NUM)
    {
        chests_num = MAX_VOID_FIELD_CHESTS_NUM;
    }
    m_chests_num = (int32)chests_num;
}

void VoidFieldPlayerRecord::SubChestsNum(int32 val)
{
    CnAssert(val >= 0);
    if(val < 0) return;
    if(m_chests_num > val)
    {
        m_chests_num -= val;
    }
    else
    {
        m_chests_num = 0;
    }
}

RegularVoidField::RegularVoidField(RegularDoc* pDoc) :
RegularWorld(pDoc)
{
    // 初始化类型条件表       // m_openCondList的id就是规则类型
    VoidFieldOpenConditon cdRecord;
    // 普通
    m_openCondList.push_back(cdRecord);
    // 珍兽
    cdRecord.m_start_time = 0;
    cdRecord.m_run_time = VoidFieldConfig::Instance()->GetRegularBossRunTime();
    m_openCondList.push_back(cdRecord);
    // 月黑
    cdRecord.m_start_time = 0;
    cdRecord.m_run_time = VoidFieldConfig::Instance()->GetRegularMoonlessRunTime();
    m_openCondList.push_back(cdRecord);

    // 设置初始状态为常态
    m_currType = VOID_FIELD_TYPE_NORMAL;

    m_into_time = 0;

    //---------------------------------------
    m_outRadius         = 0;
    ClearBossData();
}

RegularVoidField::~RegularVoidField()
{

}

void RegularVoidField::OneSecondUpdate( int32 deltatime )
{
    RegularWorld::OneSecondUpdate(deltatime);

    RewardPlayerExp();
    PushMessage();

    UpdateRegularState();   // 检查是否可切换副本状态

    if (m_currType == VOID_FIELD_TYPE_BOSS)
    {
        if(m_bossID == 0)
        {
            CreateBoss();
        }
        else
        {
            // 超出BOSS攻击范围计算计时
            if (m_outRadius)
            {
                CheckOverRange();
            }
            UpdateDamage();
        }
    }
    else if (m_currType == VOID_FIELD_TYPE_MOONLESS)
    {
        RewardPlayerBoxInMoonless();    // 为月黑才发奖励
    }
}

void RegularVoidField::OnCreatureEnter( BattleUnit* creature )
{
    if(!creature)
        return;

    RegularWorld::OnCreatureEnter(creature);

    if(creature->GetObjectType() != OBJ_PLAYER)
        return;

    Player* player = (Player*)creature;

    EnemyManager* enemyMgr = ((Player*)creature)->GetEnemyMgr();
    if(enemyMgr)
    {
        if (m_currType == VOID_FIELD_TYPE_NORMAL)
        {
            enemyMgr->SetFightMode(FIGHT_MODE_PEACE);
        }
        else if (m_currType == VOID_FIELD_TYPE_BOSS)
        {
            enemyMgr->SetFightMode(FIGHT_MODE_GUILD);
        }
        else if (m_currType == VOID_FIELD_TYPE_MOONLESS)
        {
            enemyMgr->SetFightMode(FIGHT_MODE_MASSACRE);
        }
    }

    AddRecord(player);
    NotifyRegularType(player);

    if (m_currType == VOID_FIELD_TYPE_BOSS)
        TriggerTaskEvent(TASK_EVENT_VOID_FIELD_BOSS_PAR);
    else if (m_currType == VOID_FIELD_TYPE_MOONLESS)
        TriggerTaskEvent(TASK_EVENT_VOID_FIELD_MOONLESS_PAR);

    // 添加玩家的红名buff
    CheckPlayerEvilState(player);

    m_into_time = (uint32)Crown::SDNow().GetTimeValue(); 

    player->SendMyselfEvent(LIVENESS_EVENT_TARGET_JOIN_VOID_FIELD, 1, 0, 0);
}

void RegularVoidField::OnCreatureLeave( BattleUnit* creature )
{
    if(!creature)
        return;

    if(creature->GetObjectType() != OBJ_PLAYER)
        return;

    RegularWorld::OnCreatureLeave(creature);

    Player* player = (Player*)creature;

    if (m_currType == VOID_FIELD_TYPE_MOONLESS)
    {
        DeductPlayerChestsNum(player);   // 判断是否是中途离开月黑
    }
    else if (m_currType == VOID_FIELD_TYPE_BOSS)
    {
        PlayerEscapeProcess(player);
    }

    SettleChests(player);
    DelRecord(player);

    // 清除玩家的红名buff
    ClearPlayerRedNameBuff(player);

    CalIntoTime(player);

    VoidFieldPlayerRecord* pRecord = GetRecord(player);
    CnAssert(!pRecord || pRecord->GetChestsNum() == 0);         //  确保玩家退出时，虚空宝箱数为0
}

void RegularVoidField::CalIntoTime(Player* player)
{
    if(!player)
        return;

    uint32 quitetime = (uint32)Crown::SDNow().GetTimeValue();
    if(quitetime > m_into_time)
    {
        uint32 time = quitetime - m_into_time;
        if(player && time)
        {
            player->AddVoideFiedlTime(time);
            m_into_time = (uint32)Crown::SDNow().GetTimeValue();
        }
    }
}
void RegularVoidField::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularWorld::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_SCENE_ATTACK:
        {
            OnBattleEventAttack(arg1, arg2);
        }
        break;
    case BATTLE_EVENT_PLAYER_DEAD:
        {
            if (m_currType == VOID_FIELD_TYPE_MOONLESS)
            {
                OnKillPlayer((uint32)arg1, (uint32)arg2);
            }
            else if (m_currType == VOID_FIELD_TYPE_BOSS)
            {
                OnBossModeBattleEventKill(arg2, arg1);
            }
            OnBattleEventKill(arg2, arg1);
        }
        break;
    case BATTLE_EVENT_MONSTER_DEAD:
        {
            OnBattleEventMonsterDead((uint32)arg1, (uint32)arg2);
        }
        break;
    case EVENT_PLAYER_RESURGENCE_REQ:       // 接收复活请求
        {
            OnPlayerResurgenceReq(arg1);
        }
        break;
    case EVENT_EVIL_CHANGE_TO_RED:
        {
            Player* player = m_pScene->GetPlayerByObjectId((uint32)arg1);
            AddPlayerRedNameBuff(player);
        }
        break;
    case EVENT_EVIL_CHANGE_TO_WHITE:
        {
            Player* player = m_pScene->GetPlayerByObjectId((uint32)arg1);
            ClearPlayerRedNameBuff(player);
        }
        break;
    case EVENT_VOID_FIELD_CLEAR_INTOTIME:
        {
             m_into_time = (uint32)Crown::SDNow().GetTimeValue();
        }
        break;
    case BATTLE_EVENT_DAMAGE:
        {
            if (m_currType == VOID_FIELD_TYPE_BOSS)
            {
                uint32 defencerID = arg3;
                if(defencerID == m_bossCreatureID)
                {
                    OnDamage(arg1, arg2);
                }
            }
        }
        break;
    case BATTLE_EVENT_BOSS_DEAD:
        {
            if (m_currType == VOID_FIELD_TYPE_BOSS)
            {
                if((uint32)arg1 == m_bossID)
                {
                    m_killed = true;
                    m_killerID = (uint32)arg2;
                    OnDead(arg2);
                    SendKillerRewardMail(m_killerID);
                    BossKillProcess(m_killerID);
                    // ------强制结束珍兽状态回NORMAL------
                    GotoNormalState();
                }
            }
        }
        break;
    default:
        break;
    }
}

void RegularVoidField::Update(uint32 curr_tick)
{
    RegularWorld::Update(curr_tick);
}

bool RegularVoidField::IsEnemy(BattleUnit* attacker, BattleUnit* defencer)
{
    Player* attackPlayer    = 0;
    Player* defencePlayer   = 0;
    Pet* pet          = 0;
    ENEMY_RECORD* record    = 0;
    EnemyManager* attackEnemy     = 0;

    if(!attacker || !defencer || (attacker == defencer))
    {
        return false;
    }

    switch(attacker->GetObjectType())
    {
    case OBJ_PLAYER:            // 玩家
    case OBJ_OFFLINE_PLAYER:    // 离线玩家
    case OBJ_PET:        // 战宠
        {
            if((defencer->GetObjectType() == OBJ_MONSTER) || 
                (defencer->GetObjectType() == OBJ_BOSS))
            {
                return true;
            }
        }
        break;
    case OBJ_MONSTER:           // 怪物
    case OBJ_BOSS:              // BOSS
        {
            if((defencer->GetObjectType() == OBJ_MONSTER) || 
               (defencer->GetObjectType() == OBJ_BOSS))
            {
                return false;
            }
            return true;
        }
        break;
    default:                    // 不应该
        {
            CnAssert(false);
            return false;
        }
        break;
    }

    switch(defencer->GetObjectType())
    {
    case OBJ_PLAYER:            // 玩家
    case OBJ_OFFLINE_PLAYER:    // 离线玩家
    case OBJ_PET:        // 战宠
        break;
    default:                    // 不应该
        {
            CnAssert(false);
            return false;
        }
        break;
    }

    // 获得攻击者指针
    if(attacker->GetObjectType() == OBJ_PET)
    {
        pet = (Pet*) attacker;
        attackPlayer = (Player*)pet->GetMaster();
    }
    else
    {
        attackPlayer = (Player*)attacker;
    }

    // 获得防御者指针
    if(defencer->GetObjectType() == OBJ_PET)
    {
        pet = (Pet*) defencer;
        defencePlayer = (Player*)pet->GetMaster();
    }
    else
    {
        defencePlayer = (Player*)defencer;
    }

    if(!attackPlayer || !defencePlayer)
    {
        return false;
    }

    if (attackPlayer == defencePlayer)
    {
        return false;
    }

    // 珍兽状态时，仇人列表暂时失效
    if (m_currType == VOID_FIELD_TYPE_BOSS)
    {
        // 无工会，则与任何人都敌对
        if(!attackPlayer->GetGuildID() || !defencePlayer->GetGuildID())
        {
            return true;
        }
        // 检查公会是否相同
        if(attackPlayer->GetGuildID() == defencePlayer->GetGuildID())
        {
            return false;
        }
    }

    attackEnemy = attackPlayer->GetEnemyMgr();
    if(!attackEnemy)
    {
        return false;
    }

    record = attackEnemy->GetEnemyRecord(defencePlayer->GetPlayerID());
    if(record)
    {
        if(record->flag == ENEMY_FLAG_ATTACK || record->flag == ENEMY_FLAG_FIGHTBACK)
        {
            return true;
        }
    }

    switch(attackEnemy->GetFightMode())
    {
    case FIGHT_MODE_PEACE:         // 和平模式
        {
            return false;
        }
        break;
    case FIGHT_MODE_KILLEVIL:      // 除恶模式
        {   // 检查是否为红名玩家
            if(!defencePlayer->IsRedName())
            {
                return false;
            }
        }
        break;
    case FIGHT_MODE_GUILD:         // 帮会模式
        {   // 无工会，则与任何人都敌对
            if(!attackPlayer->GetGuildID() || !defencePlayer->GetGuildID())
            {
                return true;
            }
            // 检查公会是否相同
            if(attackPlayer->GetGuildID() == defencePlayer->GetGuildID())
            {
                return false;
            }
        }
        break;
    case FIGHT_MODE_MASSACRE:      // 屠杀模式
        break;
    default:
        return false;
        break;
    }

    return true;
}

// ==================================================
// 设置规则的场景
// ==================================================
void RegularVoidField::SetScene(Scene* pScene)
{
    RegularWorld::SetScene(pScene);
}

void RegularVoidField::OnStart()
{
    RegularWorld::OnStart();

    if (m_currType == VOID_FIELD_TYPE_NORMAL)
    {
        ResetAddExpTime();
        RecoverRefreshMonsters();
    }
    else if (m_currType == VOID_FIELD_TYPE_BOSS)
    {
        ResetAddExpTime();
        KillAllMonsters();
        StopRefreshMonsters();
    }
    else if (m_currType == VOID_FIELD_TYPE_MOONLESS)
    {
        ResetAddExpTime();
        ResetAddBoxTime();
        KillAllMonsters();
        StopRefreshMonsters();
    }
}

// ==================================================
// 填充开始位置
// ==================================================
void RegularVoidField::FillStartPostion(Player* player, uint32 triggerID)
{
    Trigger* trigger = 0;

    if(!player) return;

    // 如果设置了断线重连进入虚空，则设置玩家上次掉线的坐标
    if ((player->GetSettingManager()->GetVoidFieldSettingInfo().reconnect_on_loss))
    {
        //player->SetX(player->GetFieldX());
        //player->SetY(player->GetFieldY());

        // 位置校验
        if(GetScene()->TestCircleWithTriangles(player->GetPosition(), 0))
        {
            Trigger* FirstTrigger = GetScene()->GetFirstTrigger();
            if(!FirstTrigger)
            {
                CnWarn("first trigger is not exist! scene id:%d!", GetScene()->GetSceneID());
            }
            else
            {
                player->SetX(FirstTrigger->GetX());
                player->SetY(FirstTrigger->GetY());
            }
        }
    }
    else
    {
        triggerID = CalPlayerTriggerID(player);
        trigger = m_pScene->GetTriggerByID(triggerID);
        if(!trigger)
        {   // 如果策划少配了出生点, 就从出生点 1 出生
            trigger = m_pScene->GetTriggerByID(1);
        }

        if(!trigger)
        {
            CnAssert(false);
            return;
        }

        CoVec2 randPos = GetScene()->FindValidPositionInCircle(trigger->GetPosition(), trigger->GetRadius(), player->GetRadius()); // trigger范围内查找一个有用的随机点
        player->SetPosition((int32)randPos.x, (int32)randPos.y);
        player->SetAngle(trigger->GetAngle());
    }
}

// ==================================================
// 获得当前类型
// ==================================================
int32 RegularVoidField::GetCurrType()
{
    return m_currType;
}

// ==================================================
// 设置当前类型
// ==================================================
void RegularVoidField::SetCurrType(int32 mType)
{
    int32 prevType = m_currType;

    if (prevType == VOID_FIELD_TYPE_BOSS)
    {
        // 删除 BOSS
        if(m_bossCreatureID && !m_killed)
        {
            m_pScene->DelBoss(m_bossCreatureID);
            BossEscapeProcess();
        }

        // 清除 BOSS 数据
        m_outRadius         = 0;
        ClearBossData();
    }

    if((0 <= mType) && (mType < VOID_FIELD_TYPE_MAX))
        m_currType = mType;

    if (m_currType == VOID_FIELD_TYPE_NORMAL)
    {
        ResetAddExpTime();
        RecoverRefreshMonsters();

        for (PlayerRecordMap::iterator iter = m_recordMap.begin(); iter != m_recordMap.end(); ++iter)
        {
            iter->second.SetBossParFlag(false);
            iter->second.SetMoonlessParFlag(false);
        }
    }
    else if (m_currType == VOID_FIELD_TYPE_BOSS)
    {
        ResetAddExpTime();
        KillAllMonsters();
        StopRefreshMonsters();

        TriggerTaskEvent(TASK_EVENT_VOID_FIELD_BOSS_PAR);
    }
    else if (m_currType == VOID_FIELD_TYPE_MOONLESS)
    {
        ResetAddExpTime();
        ResetAddBoxTime();
        KillAllMonsters();
        StopRefreshMonsters();

        TriggerTaskEvent(TASK_EVENT_VOID_FIELD_MOONLESS_PAR);
    }

    SetPlayersFightMode();

    NotifyRegularType();

    SendRegularTypeMarquee(m_currType, m_pScene->GetChannelID());
}

// ==================================================
// 切换规则
// ==================================================
void RegularVoidField::UpdateRegularState()
{
    // 检查运行
    if (m_currType != VOID_FIELD_TYPE_NORMAL)
    {
        uint32 now = (uint32)Crown::SDNow().GetTimeValue();
        if (now - m_openCondList[m_currType].m_start_time >= m_openCondList[m_currType].m_run_time)  // 运行时间到
        {
            GotoNormalState();
        }
    }

}

void RegularVoidField::GotoNormalState()
{
    // 如果是本轮的最后一个频道
    if (VoidFieldManager::Instance()->IsLastInChannelList(m_currType, m_pScene->GetChannelID()) == true)
    {
        // 设置open time
        VoidFieldManager::Instance()->SetOpenTime(m_currType);
        // 清空列表
        VoidFieldManager::Instance()->ClearChannelList(m_currType);
    }

    // 切换回 NORMAL
    SetCurrType(VOID_FIELD_TYPE_NORMAL);
}

void RegularVoidField::SetStateStartTime(int32 mType)
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    m_openCondList[mType].m_start_time = now;
}

// ==================================================
// 通知角色宝箱数量
// ==================================================
void RegularVoidField::SendVoidFieldChestsNum(Player* player, int32 num)
{
    VOID_FIELD_CHESTS_NUM sendMsg;

    if(!player)
        return;

    sendMsg.chests_num = num;
    player->SendToGate(MSG_VOID_FIELD_CHESTS_NUM_NTF, &sendMsg);
}

// ==================================================
// 抢夺宝箱
// ==================================================
void RegularVoidField::RobChests(uint32 deadID, uint32 killerID, int32 rate)
{
    Player* deadPlayer = NULL;
    Player* killPlayer = NULL;
    VoidFieldPlayerRecord* deadRecord = NULL;
    VoidFieldPlayerRecord* killRecord = NULL;
    int64 rob_num = 0;

    deadPlayer = m_pScene->GetPlayerByObjectId(deadID);
    if(!deadPlayer)
        return;

    killPlayer = m_pScene->GetPlayerByObjectId(killerID);
    if(!killPlayer)
        return;

    deadRecord = GetRecord(deadPlayer);
    if(!deadRecord)
        return;

    killRecord = GetRecord(killPlayer);
    if(!killRecord)
        return;

    if(deadRecord->GetChestsNum() < 1)
        return;

    rob_num = deadRecord->GetChestsNum() * (int64)rate;
    rob_num /= BASE_RATE_NUM;

    deadRecord->SubChestsNum((int32)rob_num);
    killRecord->AddChestsNum((int32)rob_num);

    SendVoidFieldChestsNum(deadPlayer, deadRecord->GetChestsNum());
    SendVoidFieldChestsNum(killPlayer, killRecord->GetChestsNum());
}

void RegularVoidField::ResetAddExpTime()
{
    int32 interval_time = VoidFieldConfig::Instance()->GetRewardExpInterval(m_currType);
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    PlayerRecordMap::iterator playerIt;
    for (playerIt = m_recordMap.begin(); playerIt != m_recordMap.end(); ++playerIt)
    {
        playerIt->second.SetAddExpTime(now + interval_time);
    }
}

void RegularVoidField::ResetAddBoxTime()
{
    int32 interval_time = VoidFieldConfig::Instance()->GetMoonlessRewardBoxInterval();
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    PlayerRecordMap::iterator playerIt;
    for (playerIt = m_recordMap.begin(); playerIt != m_recordMap.end(); ++playerIt)
    {
        playerIt->second.SetAddBoxTime(now + interval_time);
    }
}

void RegularVoidField::TriggerTaskEvent(uint32 taskEventId)
{
    for (PlayerRecordMap::iterator iter = m_recordMap.begin(); iter != m_recordMap.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.GetPlayerID());
        if (player) // 玩家在虚空内
        {
            if (taskEventId == TASK_EVENT_VOID_FIELD_BOSS_PAR)
            {
                if (iter->second.GetBossParFlag() == false)
                {
                    iter->second.SetBossParFlag(true);
                    player->SendTaskEvent(taskEventId, 1, 0);
                }
            }
            else if (taskEventId == TASK_EVENT_VOID_FIELD_MOONLESS_PAR)
            {
                if (iter->second.GetMoonlessParFlag() == false)
                {
                    iter->second.SetMoonlessParFlag(true);
                    player->SendTaskEvent(taskEventId, 1, 0);
                }
            }
        }
    }
}

void RegularVoidField::PushMessage()
{
    for (PlayerRecordMap::iterator iter = m_recordMap.begin(); iter != m_recordMap.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.GetPlayerID());
        if(player)
        {
            // 判断是否推送个人虚空幻境信息
            if((iter->second.m_personal_info.battle_list.size() > 0) || 
                (iter->second.m_personal_info.reward_table.items.size() > 0))
            {
                SendVoidFieldInfoNtf(player, &(iter->second.m_personal_info));
                iter->second.m_personal_info.Clear();
            }
        }
    }
}


void RegularVoidField::RewardPlayerExp()
{
    int32 interval_time = VoidFieldConfig::Instance()->GetRewardExpInterval(m_currType);
    int32 exp_value     = VoidFieldConfig::Instance()->GetRewardExpNum(m_currType);
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();


    for (PlayerRecordMap::iterator iter = m_recordMap.begin(); iter != m_recordMap.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.GetPlayerID());
        if(player)
        {
            // 判断加经验的时间
            if ((0 < iter->second.GetAddExpTime()) && (iter->second.GetAddExpTime() <= now))
            {
                // 添加经验
                player->AddExp(exp_value);

                // 设置添加经验的时间
                iter->second.SetAddExpTime(now + interval_time);

                // 添加经验推送数据
                AddPersonalExpInfo(player, &(iter->second.m_personal_info), exp_value);
            }
        }
    }
}

void RegularVoidField::RewardPlayerBoxInMoonless()
{
    uint32 interval_time = VoidFieldConfig::Instance()->GetMoonlessRewardBoxInterval();
    uint32 box_value     = VoidFieldConfig::Instance()->GetMoonlessRewardBoxNum();
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    std::vector<uint64> addBoxPlayerID;
    addBoxPlayerID.clear();

    bool is_add_box = false;
    for (PlayerRecordMap::iterator iter = m_recordMap.begin(); iter != m_recordMap.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.GetPlayerID());
        if(player)
        {
            // 判断加宝箱的时间
            if ((0 < iter->second.GetAddBoxTime()) && (iter->second.GetAddBoxTime() <= now))
            {
                is_add_box = true;
                // 添加宝箱
                iter->second.AddChestsNum(box_value);

                // 设置添加宝箱的时间
                iter->second.SetAddBoxTime(now + interval_time);

                SendVoidFieldChestsNum(player, iter->second.GetChestsNum());

                addBoxPlayerID.push_back(player->GetPlayerID());
            }
        }
    }

    NONE sendMsg;
    Scene::PlayerMap& players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        for (std::vector<uint64>::iterator curplayer = addBoxPlayerID.begin(); curplayer != addBoxPlayerID.end(); curplayer++)
        {
            if((*curplayer) == it->second->GetPlayerID())
            {
                 it->second->SendToGate(MSG_VOID_FIELD_REWARD_BOX_NTF, &sendMsg);
            }
        }
    }
}

// ==================================================
// 通知当前副本规则类型
// ==================================================
void RegularVoidField::AddPlayerChests(uint32 creatureID)
{
    Player* player = m_pScene->GetPlayerByObjectId(creatureID);
    if(!player)
        return;

    AddChestsNum(player);
}

// ==================================================
// 添加虚空宝箱数量
// ==================================================
void RegularVoidField::AddChestsNum(Player* player)
{
    PlayerRecordMap::iterator it;

    if(!player)
        return;

    it = m_recordMap.find(player->GetPlayerID());
    if(it == m_recordMap.end())
        return;

    it->second.AddChestsNum(1);

    SendVoidFieldChestsNum(player, it->second.GetChestsNum());
}

void RegularVoidField::AddDropItem(Player* player)
{
    if(!player)
        return;

    PlayerRecordMap::iterator it = m_recordMap.find(player->GetPlayerID());
    if(it == m_recordMap.end())
        return;

    // 从掉落物品列表中随机一个物品
    int32 randNum = Crown::SDRandom(BASE_RATE_NUM);
    REWARD_ITEM* rewardItem = VoidFieldConfig::Instance()->GetDropRandItem(randNum);
    if(rewardItem)
    {
        REWARD_TABLE rewardTable;
        rewardTable.reason = REWARD_REASON_VOID_FIELD_DROP;
        MergeRewardTableItemList(rewardTable, *rewardItem);
        //player->GetItemManager()->AddRewardTableWithEmail(player->GetCenterSvrID(), player->GetPlayerID(), rewardTable);

        // 添加掉落物品到推送数据
        AddPersonalDropItemInfo(player, &(it->second.m_personal_info), rewardItem);
    }
}

bool RegularVoidField::CanMonsterDrop(int32 diff_level)
{
    int32 rate = VoidFieldConfig::Instance()->GetDiffLevelDropRate(diff_level);
    int32 rand_num = Crown::SDRandom(BASE_RATE_NUM);
    return rand_num < rate;
}

bool RegularVoidField::CanStateDrop()
{
    int32 rate = 0;
    int32 rand_num = 0;

    switch(m_currType)
    {
    case VOID_FIELD_TYPE_NORMAL:
        rate = VoidFieldConfig::Instance()->GetNormalChestsRate();
        break;
    case VOID_FIELD_TYPE_BOSS:
        rate = VoidFieldConfig::Instance()->GetBossChestsRate();
        break;
    case VOID_FIELD_TYPE_MOONLESS:
        rate = VoidFieldConfig::Instance()->GetMoonlessChestsRate();
        break;
    default:
        break;
    }

    rand_num = Crown::SDRandom(BASE_RATE_NUM);
    return rand_num < rate;
}

//虚空宝箱的掉落概率
bool RegularVoidField::CanChestsProbability(Player* player,int32 lower)
{
    if(!player) return false;

    //uint32 time = player->GetVoidFieldTime();

    int32 rate = 0;
    int32 rand_num = 0;

    switch(m_currType)
    {
    case VOID_FIELD_TYPE_NORMAL:
        rate = VoidFieldConfig::Instance()->GetNormalChestsRate();
        break;
    case VOID_FIELD_TYPE_BOSS:
        rate = VoidFieldConfig::Instance()->GetBossChestsRate();
        break;
    case VOID_FIELD_TYPE_MOONLESS:
        rate = VoidFieldConfig::Instance()->GetMoonlessChestsRate();
        break;
    default:
        break;
    }

    if(rate >= lower)
        rate = rate - lower;
    else
        rate = 0;

    rand_num = Crown::SDRandom(BASE_RATE_NUM);
    return rand_num <= rate;
}
// ==================================================
// 设置玩家的战斗状态
// ==================================================
void RegularVoidField::SetPlayersFightMode()
{
    Scene::PlayerMap& players = m_pScene->GetPlayers();
    int32 fightMode = FIGHT_MODE_PEACE;

    switch(m_currType)
    {
    case VOID_FIELD_TYPE_NORMAL:
        fightMode = FIGHT_MODE_PEACE;
        break;
    case VOID_FIELD_TYPE_BOSS:
        fightMode = FIGHT_MODE_GUILD;
        break;
    case VOID_FIELD_TYPE_MOONLESS:
        fightMode = FIGHT_MODE_MASSACRE;
        break;
    default:
        break;
    }

    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        it->second->GetEnemyMgr()->SetFightMode(fightMode);
    }
}

// ==================================================
// 通知当前副本规则类型
// ==================================================
void RegularVoidField::NotifyRegularType(Player* player)
{
    VOID_FIELD_REGULAR_TYPE_NTF sendMsg;

    sendMsg.void_field_type = m_currType;

    sendMsg.start_time = 0;
    if (m_currType != VOID_FIELD_TYPE_NORMAL)
    {
        sendMsg.start_time = m_openCondList[m_currType].m_start_time;
        sendMsg.run_time = m_openCondList[m_currType].m_run_time;
    }

    if(player)
    {
        player->SendToGate(MSG_VOID_FIELD_REGULAR_TYPE_NTF, &sendMsg);
    }
    else
    {
        m_pScene->NotifyMessageToAll(MSG_VOID_FIELD_REGULAR_TYPE_NTF, &sendMsg);
    }
}

// ==================================================
// 战斗攻击事件
// ==================================================
void RegularVoidField::OnBattleEventAttack(uint32 attackerID, uint32 defencerID)
{
    Player* attackPlayer = 0;
    Player* defencePlayer = 0;
    EnemyManager* attackEnemyMgr = 0;
    EnemyManager* defenceEnemyMgr = 0;

    // 攻击者
    /*attackPlayer = GetScene()->GetPlayerByUnitId(attackerID);*/
    attackPlayer = GetPlayerByUnitID(attackerID);
    if(!attackPlayer)
        return;

    // 防御者
    /*defencePlayer = GetScene()->GetPlayerByUnitId(defencerID);*/
    defencePlayer = GetPlayerByUnitID(defencerID);
    if(!defencePlayer)
        return;

    // 获得善恶系统
    attackEnemyMgr = attackPlayer->GetEnemyMgr();
    defenceEnemyMgr = defencePlayer->GetEnemyMgr();
    if(!attackEnemyMgr || !defenceEnemyMgr)
        return;

    // 添加攻击者列表
    defenceEnemyMgr->AddAttacker(attackPlayer->GetId());

    // 月黑模式不记录罪恶值; 调整为只有常态下才计算罪恶值
    //if(m_currType != VOID_FIELD_TYPE_MOONLESS)
    if (m_currType == VOID_FIELD_TYPE_NORMAL)
    {
        // 添加反击人
        attackEnemyMgr->AddEnemy(defencePlayer->GetPlayerID(), ENEMY_FLAG_FIGHTBACK);

        // 添加仇人
        defenceEnemyMgr->AddEnemy(attackPlayer->GetPlayerID(), ENEMY_FLAG_ATTACK);

        // 添加善恶值
        attackEnemyMgr->AddAttacEvilVal(defenceEnemyMgr);
    }
}

// ==================================================
// 接收角色死亡事件
// ==================================================
void RegularVoidField::OnBattleEventKill(uint32 attackerID, uint32 defencerID)
{
    Player* attackPlayer = 0;
    Player* defencePlayer = 0;
    EnemyManager* attackEnemyMgr = 0;
    EnemyManager* defenceEnemyMgr = 0;

    // 防御者
    defencePlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!defencePlayer)
        return;

    // 发送复活请求
    SendResurgenceAsk(defencePlayer);

    // 攻击者
    attackPlayer = GetScene()->GetPlayerByObjectId(attackerID);
    if(!attackPlayer)
        return;

    // 月黑模式不记录罪恶值; 调整为只有常态下才计算罪恶值
    //if(m_currType != VOID_FIELD_TYPE_MOONLESS)
    if (m_currType == VOID_FIELD_TYPE_NORMAL)
    {
        // 获得善恶系统
        attackEnemyMgr = attackPlayer->GetEnemyMgr();
        defenceEnemyMgr = defencePlayer->GetEnemyMgr();
        if(!attackEnemyMgr || !defenceEnemyMgr)
            return;

        // 添加善恶值
        attackEnemyMgr->AddKillEvilVal(defenceEnemyMgr);
    }

    // 添加个人击杀信息
    AddPersonalKillInfo(attackPlayer, defencePlayer);
}

void RegularVoidField::OnBattleEventMonsterDead(uint32 monsterID, uint32 killerID)
{
    //Player* player = GetPlayerByUnitID(killerID);
    //const MonsterTemplate* monsterTmpl = MonsterConfig::Instance()->GetMonsterTemplate(monsterID);
    //if (monsterTmpl && player)
    //{
    //    CalIntoTime(player);

    //    if (CanMonsterDrop(player->GetLevel() - monsterTmpl->level))
    //    {
    //        //宝箱掉落概率的计算
    //        if(player->GetVoidFieldTime() <= GameSettingManager::Instance()->GetVoidFieldDropChestTime())
    //        {
    //            if(CanStateDrop())
    //            {
    //                AddPlayerChests(killerID);
    //            }
    //        }
    //        else
    //        {
    //            std::map<uint32, int32> curmap = VoidFieldConfig::Instance()->GetChestsProbabilityByTime();
    //            std::map<uint32, int32>::iterator iter = curmap.begin();

    //            if(iter != curmap.end())
    //            {
    //                int32  time = player->GetVoidFieldTime() - GameSettingManager::Instance()->GetVoidFieldDropChestTime();
    //                int32 num = time /iter->first;
    //                if(num) 
    //                {
    //                    int32 lowernum = num* iter->second;
    //                    CanChestsProbability(player, lowernum);
    //                }
    //                else
    //                {
    //                    if(CanStateDrop())
    //                    {
    //                        AddPlayerChests(killerID);
    //                    }
    //                }
    //            }
    //            else
    //            {
    //                CnAssert(false);
    //            }
    //        }
    //    }

    //    if (CanMonsterDrop(player->GetLevel() - monsterTmpl->level) && CanStateDrop())
    //    {
    //        AddDropItem(player);
    //    }
    //}
}

// ==================================================
// 获得记录
// ==================================================
VoidFieldPlayerRecord* RegularVoidField::GetRecord(Player* player)
{
    PlayerRecordMap::iterator playerIt;

    if(!player)
        return NULL;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt != m_recordMap.end())
    {
        return &playerIt->second;
    }

    return NULL;
}

Player* RegularVoidField::GetPlayerByUnitID(uint32 uintID)
{
    BattleUnit* battleUnit = NULL;
    Pet*  battlePet  = NULL;
    Player*     battlePlayer = NULL;

    battleUnit = GetScene()->GetBattleUnit(uintID);
    if (!battleUnit)
    {
        return battlePlayer;
    }

    if (battleUnit->GetObjectType() == OBJ_PLAYER || battleUnit->GetObjectType() == OBJ_OFFLINE_PLAYER)
    {
        battlePlayer = (Player*)battleUnit;
    }
    else if (battleUnit->GetObjectType() == OBJ_PET)
    {
        battlePet = (Pet*)battleUnit;
        if (battlePet)
        {
            battlePlayer = (Player*)battlePet->GetMaster();
        }
    }

    return battlePlayer;
}

void RegularVoidField::KillAllMonsters()
{
    Scene::MonsterMap& monsterMap = m_pScene->GetMonsters();
    for(Scene::MonsterMap::iterator iter = monsterMap.begin(); iter != monsterMap.end(); iter++)
    {
        Monster* pkillMonster = iter->second;
        if(!pkillMonster->IsDead())
        {
            pkillMonster->Kill(NULL);
        }
    }
}

void RegularVoidField::StopRefreshMonsters()
{
    // 停止刷怪
    Scene::SpawnMap& spawnMap = GetScene()->GetSpawn();
    for(Scene::SpawnMap::iterator spawnIt = spawnMap.begin(); spawnIt != spawnMap.end(); ++spawnIt)
    {
        spawnIt->second->SetClearAllMosters();
    }
}

void RegularVoidField::RecoverRefreshMonsters()
{
    Scene::SpawnMap& spawnMap = GetScene()->GetSpawn();
    for(Scene::SpawnMap::iterator spawnIt = spawnMap.begin(); spawnIt != spawnMap.end(); ++spawnIt)
    {
        spawnIt->second->SetLoop(true);
    }
}

// ==================================================
// 添加记录
// ==================================================
void RegularVoidField::AddRecord(Player* player)
{
    int32 interval_time = VoidFieldConfig::Instance()->GetRewardExpInterval(m_currType);
    uint32 box_interval_time = VoidFieldConfig::Instance()->GetMoonlessRewardBoxInterval();
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    PlayerRecordMap::iterator playerIt;
    VoidFieldPlayerRecord record;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt == m_recordMap.end())
    {
        record.Clear();
        record.SetPlayerID(player->GetPlayerID());
        record.SetAddExpTime(now + interval_time);
        record.SetAddBoxTime(now + box_interval_time);
        record.SetBossParFlag(false);
        record.SetMoonlessParFlag(false);
        m_recordMap[player->GetPlayerID()] = record;

        SendVoidFieldChestsNum(player, record.GetChestsNum());
    }
    else
    {
        //CnAssert(false);
        //CnError("Duplicate player record\n");

        playerIt->second.SetAddExpTime(now + interval_time);
        playerIt->second.SetAddBoxTime(now + box_interval_time);

        SendVoidFieldChestsNum(player, playerIt->second.GetChestsNum());
    }
}

// ==================================================
// 删除记录
// ==================================================
void RegularVoidField::DelRecord(Player* player)
{
    PlayerRecordMap::iterator playerIt;

    playerIt = m_recordMap.find(player->GetPlayerID());
    if(playerIt != m_recordMap.end())
    {
        //m_recordMap.erase(playerIt);  // 不再真正删除该玩家，只是删除该玩家的某些记录，但保留该玩家是否参加了当轮的珍兽或者月黑标志
        playerIt->second.SetAddExpTime(0);
        playerIt->second.SetAddBoxTime(0);
        playerIt->second.SetChestsNum(0);           // 清空宝箱数
    }
    else
    {
        CnAssert(false);
        CnError("Duplicate player record\n");
    }
}

// ==================================================
// 结算宝箱
// ==================================================
void RegularVoidField::SettleChests(Player* player)
{
    VoidFieldPlayerRecord* record = NULL;
    REWARD_TABLE rewardTable;
    REWARD_ITEM* rewardItem = NULL;
    int32 randNum = 0;

    record = GetRecord(player);
    if(!record || (record->GetChestsNum() < 1))
       return;

    rewardTable.reason = REWARD_REASON_VOID_FIELD_CHESTS;

    for(int32 i=0; i<record->GetChestsNum(); ++i)
    {
        randNum = Crown::SDRandom(BASE_RATE_NUM);
        rewardItem = VoidFieldConfig::Instance()->GetChestsRandItem(randNum);
        if(rewardItem)
        {
            MergeRewardTableItemList(rewardTable, *rewardItem);
        }
    }

    if(rewardTable.items.size() < 1)
        return;

    /*
    MailManager::SendMail(player->GetCenterSvrID(), player->GetPlayerID(), "", 
        MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_CHESTS), 
        MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_CHESTS, record->GetChestsNum()), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, player->GetPlayerID());
        */
    MailManager::SendMail(player->GetCenterSvrID(), player->GetPlayerID(), "", "", 
        Crown::ToString(MAIL_TITLE_ID_VOID_FIELD_CHESTS) + "|" + Crown::ToString(record->GetChestsNum()), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, player->GetPlayerID());
}

// ==================================================
// 记录虚空幻境击杀信息
// ==================================================
void RegularVoidField::AddPersonalKillInfo(Player* attacker, Player* defencer)
{
    VOID_FIELD_BATTLE_INFO  battleInfo;
    VoidFieldPlayerRecord*  record = NULL;

    if(!attacker || !defencer)
        return;

    // 添加攻击通知
    record = GetRecord(attacker);
    if(record)
    {
        battleInfo.type = VOID_FIELD_BATTLE_DEFEAT;
        battleInfo.crature_id = defencer->GetId();
        record->m_personal_info.battle_list.push_back(battleInfo);
    }

    // 添加被攻击通知
    record = GetRecord(defencer);
    if(record)
    {
        battleInfo.type = VOID_FIELD_BATTLE_KILLED;
        battleInfo.crature_id = attacker->GetId();
        record->m_personal_info.battle_list.push_back(battleInfo);
    }
}

// ==================================================
// 添加虚空幻境经验信息
// ==================================================
void RegularVoidField::AddPersonalExpInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, int32 exp)
{
    REWARD_ITEM rewardItem;

    if(!player || !info)
        return;

    rewardItem.Clear();
    rewardItem.id = PROP_EXP;
    rewardItem.num = exp;

    info->reward_table.items.push_back(rewardItem);
}

void RegularVoidField::AddPersonalDropItemInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, REWARD_ITEM* drop_item)
{
    if (!player || !info || !drop_item)
    {
        return;
    }

    REWARD_ITEM rewardItem;
    rewardItem.Clear();
    rewardItem.flag = drop_item->flag;
    rewardItem.id = drop_item->id;
    rewardItem.num = drop_item->num;
    //rewardItem.type = drop_item->type;

    info->reward_table.items.push_back(rewardItem);
}

// ==================================================
// 发送虚空幻境信息
// ==================================================
void RegularVoidField::SendVoidFieldInfoNtf(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info)
{
    if(!player || !info)
        return;

    // 发送玩家信息
    player->SendToGate(MSG_VOID_FIELD_PERSONAL_INFO_NTF, info);

    // 清空信息
    info->Clear();
}

// ==================================================
// 接收复活请求
// ==================================================
void RegularVoidField::OnPlayerResurgenceReq(int32 arg1)
{
    Player* player = m_pScene->FindPlayer(arg1);
    if(!player || !player->IsDead())
        return;

    uint16 err = player->SubItemOne(REWARD_REASON_RESURGENCE, PROP_ITEM, 1, RESURGENCE_ITEM);
    if(err != ERR_SUCCEED)
    {
        player->NotifyErrorCode(err);
        return;
    }

    uint32 triggerID = CalPlayerTriggerID(player);

    // 设置了原地复活
    if (player->GetSettingManager()->GetVoidFieldSettingInfo().current_revival)
    {
        triggerID = 0;
    }

    Resurgence(player, triggerID, 0);

    CheckPlayerEvilState(player);
}

// ==================================================
// 计算TriggerID
// ==================================================
uint32 RegularVoidField::CalPlayerTriggerID(Player* player)
{
    uint32 triggerID = 0;

    // 根据等级随机出生点(从第11个Trigger点开始随机)
    if(player->GetLevel() < 50)
    {
        triggerID = Crown::SDRandom(4) + 11;
    }
    else if(player->GetLevel() < 60)
    {
        triggerID = Crown::SDRandom(3) + 15;
    }
    else
    {
        triggerID = Crown::SDRandom(2) + 18;
    }

    return triggerID;
}

// ==================================================
// 发送复活询问
// ==================================================
void RegularVoidField::SendResurgenceAsk(Player* player)
{
    REGULAR_PLAYER_DEAD sendMsg;

    if(!player)
        return;

    sendMsg.regular_type = GetType();
    sendMsg.dead_action = DEAD_ACTION_ITEM_RESURGENCE;
    sendMsg.count_down = VoidFieldConfig::Instance()->GetResurgenceTime();
    player->SendToGate(MSG_PLAYER_DEAD, &sendMsg);
}

// ==================================================
// 广播频道开启类型
// ==================================================
void RegularVoidField::SendRegularTypeMarquee(int32 regularType, uint32 channelNum)
{
    SERVER_MARQUEE sendMsg;
    std::string langID;

    sendMsg.id = MARQUEE_VOID_FIELD;

    // 根据副本类型获得语言包
    switch(regularType)
    {
    case VOID_FIELD_TYPE_BOSS:
        langID = VoidFieldConfig::Instance()->GetBossBroadcastID();
        break;
    case VOID_FIELD_TYPE_MOONLESS:
        langID = VoidFieldConfig::Instance()->GetMoonlessBroadcastID();
        break;
    default:
        return;
        break;
    }

    // 组织广播语言格式
    sendMsg.parame_list.push_back(MarQueeVoidFieldRegularType(langID, channelNum));

    // 广播公告
    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();
    for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
    {
        Player* player = playerIt->second;
        if(player->GetNetState() != Player::kStateInGame)
            continue;

        // 只给该跨服虚空副本对应的玩家发公告
        uint32 VoidFieldDungeonID = VoidFieldManager::Instance()->GetVoidFieldDungeonIDByRegionID(player->GetRegion());
        if( VoidFieldDungeonID == m_pScene->GetDungeonID() )
        {
            player->SendToGate(MSG_SERVER_MARQUEE, &sendMsg);
        }
    }

}

void RegularVoidField::CheckPlayerEvilState(Player* player)
{
    if (!player)
    {
        return;
    }

    EnemyManager* enemy_mrg = player->GetEnemyMgr();
    if (enemy_mrg && enemy_mrg->IsRedName())
    {
        AddPlayerRedNameBuff(player);
    }
}

void RegularVoidField::AddPlayerRedNameBuff(Player* player)
{
    if (!player)
    {
        return;
    }

    //int32 buff_id = GameParamConfig::Instance()->GetRedNameBuffID();
    //int32 buff_time = GameParamConfig::Instance()->GetRedNameBuffTime();
    //int32 buff_value = GameParamConfig::Instance()->GetRedNameBuffValue();
    //player->AddBuff(buff_id, buff_time, buff_value, NULL);
}

void RegularVoidField::ClearPlayerRedNameBuff(Player* player)
{
    if (!player)
    {
        return;
    }
    //player->DelBuff(GameParamConfig::Instance()->GetRedNameBuffID());
}

//====================================================================boss mode==========================================================

void RegularVoidField::ClearBossData()
{
    m_bossID            = 0;    ///< 珍兽Boss的ID
    m_killed            = false;///< 珍兽是否被杀标记
    m_killerID          = 0;    ///< 击杀者ID
    m_allDamage         = 0;    ///< 对Boss造成的伤害总和
    m_bossCreatureID    = 0;    ///< BOSS 的实例ID
    m_damageMap.clear();        ///< 伤害数据
    m_damageList.clear();       ///< 伤害排名列表
}

// ==================================================
// 创建 BOSS
// ==================================================
void RegularVoidField::CreateBoss()
{
    Checkpoint* pCheckPoint = m_pScene->GetCheckPoint(0);
    Boss* newBoss = NULL;

    if(!pCheckPoint)
        return;

    // 初始化范围半径
    if(m_outRadius == 0)
    {
        m_outRadius = pCheckPoint->GetRadius() * pCheckPoint->GetRadius();
    }

    // 检查是否有这个 BOSS
    m_bossID = VoidFieldConfig::Instance()->GetRareBossID();
    CnAssert(m_bossID);

    m_bossData.creatureId = m_bossID;
    CnAssert(m_bossData.creatureId != 0);
    m_bossData.x = pCheckPoint->GetX();
    m_bossData.y = pCheckPoint->GetY();
    m_bossData.angle = pCheckPoint->GetAngle();
    m_bossData.cantMove = true;
    m_bossData.cantBuff = true;
    m_bossData.bossui = true;
    m_bossData.worldboss = true;
    m_bossData.name = "void_field_" + Crown::ToString(m_bossData.creatureId);

    newBoss = AddBoss(m_bossData, "", false);

    if(newBoss == NULL)
    {
        CnError("world boss id:%d is not exist\n", m_bossData.creatureId);
        return;
    }

    // TODO:根据强壮等级，使Boss变强，临时使用和世界Boss一样的变强规则
    WorldBossManager::Instance()->StrongWorldBoss(newBoss);
    StrongBoss(newBoss);

    newBoss->GetAIController()->LoadAI("ai/world_boss_skill.aix");

    m_bossCreatureID = newBoss->GetId();
    m_startcreatebooss = (uint32)Crown::SDNow().GetTimeValue();
    m_outRadiusTime = 0;
}

void RegularVoidField::StrongBoss(Creature* battleUnit)
{
}

// ==================================================
// 接收角色死亡事件
// ==================================================
void RegularVoidField::OnBossModeBattleEventKill(uint32 attackerID, uint32 defencerID)
{
    Player* defencePlayer = NULL;
    VoidFieldBossDamageInfo* damageInfo = NULL;
    int64 val = 0;

    // 防御者
    defencePlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!defencePlayer)
        return;

    // 伤害信息
    damageInfo = GetBossDamageInfo(defencePlayer->GetPlayerID());
    if(!damageInfo)
        return;

    val = damageInfo->damage * VoidFieldConfig::Instance()->GetBossPlayerDeadRate();
    damageInfo->damage = (uint32)(val / BASE_RATE_NUM);
}

void RegularVoidField::OnDamage(uint32 attackerID, int32 damage)
{
    DamageMap::iterator damageIt;
    uint64  playerID = 0;
    Player* player = 0;

    player = m_pScene->GetPlayerByObjectId(attackerID);
    if(!player)
        return;

    playerID = player->GetPlayerID();

    damageIt = m_damageMap.find(playerID);
    if(damageIt == m_damageMap.end())
    {
        VoidFieldBossDamageInfo damageInfo;
        damageInfo.center_id    = player->GetCenterSvrID();
        damageInfo.player_id    = player->GetPlayerID();
        damageInfo.player_name  = player->GetName();
        damageInfo.guild_id     = player->GetGuildID();
        damageInfo.damage       = 0;
        damageInfo.battle_power = player->GetBattlePower();
        damageInfo.level        = player->GetLevel();

        m_damageMap[playerID]   = damageInfo;
        m_damageList.push_back(&m_damageMap[playerID]);
    }
    else
    {
        damageIt->second.damage += damage;
    }

    // 总伤害量
    m_allDamage += damage;
}

void RegularVoidField::UpdateDamage()
{
    WORLD_BOSS_DAMAGE_LIST topDamageList;
    WORLD_BOSS_DAMAGE_INFO guildDamageInfo;
    WORLD_BOSS_DAMAGE_LIST sendMsg;
    VoidFieldBossDamageInfo* pInfo = 0;
    Player* player = 0;
    int32 num = 0;

    // 前 10 排行
    num = Crown::SDMin((uint32)10, (uint32)m_damageList.size());

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    // 获得前10排行数据
    for(int32 i=0; i<num; i++)
    {
        guildDamageInfo.Clear();

        pInfo = m_damageList[i];
        guildDamageInfo.rank           = i+1;
        guildDamageInfo.damage         = pInfo->damage;
        guildDamageInfo.name           = pInfo->player_name;
        topDamageList.damage_list.push_back(guildDamageInfo);
    }

    if(topDamageList.damage_list.size() < 1)
        return;

    // 发送给所有玩家伤害排名
    Scene::PlayerMap& players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        pInfo = GetNotifyDamageInfo(topDamageList, it->second);
        if(pInfo)
        {
            guildDamageInfo.rank = 0;
            guildDamageInfo.damage         = pInfo->damage;
            guildDamageInfo.name           = pInfo->player_name;
            sendMsg = topDamageList;
            sendMsg.damage_list.push_back(guildDamageInfo);
            it->second->SendToGate(MSG_VOID_FIELD_BOSS_DAMAGE_NTF, &sendMsg);
        }
        else
        {
            it->second->SendToGate(MSG_VOID_FIELD_BOSS_DAMAGE_NTF, &topDamageList);
        }
    }
}

void RegularVoidField::OnDead(uint32 attackerID)
{
    if(attackerID)
    {
        // 计算奖励
        SettleReward();
    }

    BossLevelUp();

    m_startcreatebooss = 0;
}

void RegularVoidField::BossLevelUp()
{
    Crown::CSDDateTime now = Crown::SDNow();

    // TODO:
    WorldBossManager::Instance()->SetBoss(VOID_FIELD_BOSS_SAVE_TYPE_ID, m_bossID , (uint32)now.GetTimeValue());
    WorldBossManager::Instance()->LevelUpBoss(m_bossID , m_startcreatebooss, (uint64)(VoidFieldConfig::Instance()->GetBossKilledLevelupSeconds()));
    WorldBossManager::Instance()->SendWorldBossInfoToCenter();
}

// ==================================================
// 按由大到小排序
// ==================================================
bool RegularVoidField::SortDamage(VoidFieldBossDamageInfo* a, VoidFieldBossDamageInfo* b)
{
    return a->damage > b->damage;
}

// ==================================================
// 在列表中查找同名角色数据
// ==================================================
RegularVoidField::VoidFieldBossDamageInfo* RegularVoidField::GetNotifyDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player)
{
    DamageMap::iterator damageIt;

    if(!player) return 0;

    for(std::vector<WORLD_BOSS_DAMAGE_INFO>::iterator it = list.damage_list.begin(); it != list.damage_list.end(); ++it)
    {
        if(it->name == player->GetName())
        {
            return 0;
        }
    }

    damageIt = m_damageMap.find(player->GetPlayerID());
    if(damageIt == m_damageMap.end())
    {
        return 0;
    }

    return &(damageIt->second);
}

// ==================================================
// 获得角色的伤害值
// ==================================================
RegularVoidField::VoidFieldBossDamageInfo* RegularVoidField::GetBossDamageInfo(uint64 playerID)
{
    DamageMap::iterator it = m_damageMap.find(playerID);
    if(it != m_damageMap.end())
    {
        return &(it->second);
    }
    return NULL;
}

// ==================================================
// 检查每个玩家是否超出攻击范围
// ==================================================
void RegularVoidField::CheckOverRange()
{
    NONE sendMsg;
    DamageMap::iterator damageIt;
    int64 num = 0;
    int32 distanceSqu = 0;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 subTime = 0;

    Scene::PlayerMap& players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        distanceSqu = it->second->GetX() * it->second->GetX() + 
                      it->second->GetY() * it->second->GetY();
        if(distanceSqu < m_outRadius)
        {
            m_subScopeMap[it->second->GetPlayerID()] = 0;
            continue;
        }

        subTime = GetPlayerSubScoreTime(it->second->GetPlayerID());
        // TODO:暂时定位玩家刚出圈时，每5秒，扣玩家积分时，通知玩家超出范围
        if (subTime == 0)
        {
            m_subScopeMap[it->second->GetPlayerID()] = now + VoidFieldConfig::Instance()->GetBossOverRangeTime();
            it->second->SendToGate(MSG_VOID_FIELD_OVER_RANGE_NTF, &sendMsg);
        }
        else if (subTime > now && (subTime - now) % VoidFieldConfig::Instance()->GetOutScopeInterval() == 0)
        {
            it->second->SendToGate(MSG_VOID_FIELD_OVER_RANGE_NTF, &sendMsg);
        }

        if (0 < subTime && subTime <= now) 
        {
            damageIt = m_damageMap.find(it->second->GetPlayerID());
            if(damageIt != m_damageMap.end() && damageIt->second.damage)
            {
                num = damageIt->second.damage * VoidFieldConfig::Instance()->GetBossOverRangeRate();
                num /= BASE_RATE_NUM;
                damageIt->second.damage = (int32)num;
            }

            m_subScopeMap[it->second->GetPlayerID()] = now + VoidFieldConfig::Instance()->GetBossOverRangeTime();
            it->second->SendToGate(MSG_VOID_FIELD_OVER_RANGE_NTF, &sendMsg);
        }
    }
}

// ==================================================
// 结算奖励
// ==================================================
void RegularVoidField::SettleReward()
{
    VoidFieldBossDamageInfo* damageInfo = NULL;

    // 伤害排序
    std::sort(m_damageList.begin(), m_damageList.end(), SortDamage);

    if(m_damageList.size() < 1)
        return;

    damageInfo = m_damageList.front();
    if(!damageInfo)
        return;

    SendTopOneRewardMail(damageInfo->center_id, damageInfo->player_id);

    SendTopOneGuildRewardMail(damageInfo->guild_id);
}

// ==================================================
// 排名第一奖励
// ==================================================
void RegularVoidField::SendTopOneRewardMail(uint32 centerID, uint64 playerID)
{
    if(!playerID)
        return;

    REWARD_TABLE& rewardTable = VoidFieldConfig::Instance()->GetBossTopOneRewards();
    /*
    MailManager::SendMail(centerID, playerID, "", 
        MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
        MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, playerID);
    */
    MailManager::SendMail(centerID, playerID, "", "", 
        Crown::ToString(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, playerID);
}

// ==================================================
// 排名第一公会奖励
// ==================================================
void RegularVoidField::SendTopOneGuildRewardMail(uint64 guildID)
{
    uint64 playerID = 0;
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);

    if(!guildInfo)
        return;

    REWARD_TABLE& rewardTable = VoidFieldConfig::Instance()->GetBossTopOneGuildRewards();

    // 奖励第一公会中在现场的成员
    Scene::PlayerMap& players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::iterator it = players.begin(); it != players.end(); ++it)
    {
        if(it->second->GetGuildID() == guildID)
        {
            playerID = it->second->GetPlayerID();
            /*
            MailManager::SendMail(guildInfo->center_server_id, playerID, "", 
                MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE_GUILD), 
                MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE_GUILD), 
                MAIL_TYPE_PLAYER_SYS, rewardTable.items, playerID);
                */
            MailManager::SendMail(guildInfo->center_server_id, playerID, "", "", 
                Crown::ToString(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE_GUILD), 
                MAIL_TYPE_PLAYER_SYS, rewardTable.items, playerID);
        }
    }

    // 奖励给第一公会中所有的成员
    /*for(std::vector<GUILD_MEMBER>::iterator it = guildInfo->member_list.begin();
            it != guildInfo->member_list.end(); ++it)
    {
        playerID = U64ID(it->player_id_h, it->player_id_l);
        MailManager::SendMail(guildInfo->center_server_id, playerID, "", 
            MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE_GUILD), 
            MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_BOSS_TOPONE_GUILD), 
            MAIL_TYPE_PLAYER_SYS, rewardTable.items, playerID);
    }*/
}

// ==================================================
// 最后击杀者奖励
// ==================================================
void RegularVoidField::SendKillerRewardMail(uint32 uintID)
{
    Player* player = m_pScene->GetPlayerByObjectId(uintID);

    if(!player)
        return;

    player->SendTaskEvent(TASK_EVENT_VOID_FIELD_BOSS_KILL, 1, 0);

    REWARD_TABLE& rewardTable = VoidFieldConfig::Instance()->GetBossKillerRewards();
    /*
    MailManager::SendMail(player->GetCenterSvrID(), player->GetPlayerID(), "", 
        MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_VOID_FIELD_BOSS_KILLER), 
        MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_VOID_FIELD_BOSS_KILLER), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, player->GetPlayerID());
        */
    MailManager::SendMail(player->GetCenterSvrID(), player->GetPlayerID(), "", "", 
        Crown::ToString(MAIL_TITLE_ID_VOID_FIELD_BOSS_KILLER), 
        MAIL_TYPE_PLAYER_SYS, rewardTable.items, player->GetPlayerID());
}

uint32 RegularVoidField::GetPlayerSubScoreTime(uint64 playerID)
{
    SubScoreTimeMap::const_iterator iter = m_subScopeMap.find(playerID);
    if (iter != m_subScopeMap.end())
    {
        return iter->second;
    }

    return 0;
}

void RegularVoidField::BossKillProcess(uint32 uintID)
{
    Player* player = m_pScene->GetPlayerByObjectId(uintID);

    if(!player)
        return;

    // 公告最后一击
    if(player)
    {
        SERVER_MARQUEE sendMsg;
        sendMsg.Clear();
        sendMsg.id = MARQUEE_VOID_BOSS_LAST_KILL;
        sendMsg.parame_list.push_back(MarQueePlayerName(player->GetVipLevel(), player->GetName()));
        Player::OnMarQuee(sendMsg);
    }
}

void RegularVoidField::BossEscapeProcess()
{
    // 公告珍兽boss逃跑
    SERVER_MARQUEE sendMsg;
    sendMsg.Clear();
    sendMsg.id = MARQUEE_VOID_BOSS_ESCAPE;
    Player::OnMarQuee(sendMsg);
}

void RegularVoidField::PlayerEscapeProcess(Player* player)
{
    DamageMap::iterator damageIt;
    DamageList::iterator listIt;
    uint64  playerID = 0;

    if (!player)
    {
        return;
    }
    playerID = player->GetPlayerID();

    // 从排序表中删除逃跑玩家的伤害记录
    for (listIt = m_damageList.begin(); listIt != m_damageList.end();)
    {
        VoidFieldBossDamageInfo* damageInfo = *listIt;
        if (damageInfo && damageInfo->player_id == playerID)
        {
            listIt = m_damageList.erase(listIt);
        }
        else
        {
            ++listIt;
        }
    }

    // 从伤害表中删除逃跑玩家的伤害记录
    damageIt = m_damageMap.find(playerID);
    if(damageIt != m_damageMap.end())
    {
        m_damageMap.erase(damageIt);
    }
}

//====================================================================moon less mode=====================================================

// ==================================================
// 处理击杀玩家事件
// ==================================================
void RegularVoidField::OnKillPlayer(uint32 deadID, uint32 killerID)
{
    Player* deadPlayer = GetPlayerByUnitID(deadID);
    Player* killerPlayer = GetPlayerByUnitID(killerID);
    if (deadPlayer && killerPlayer)
    {
        int32 rate = VoidFieldConfig::Instance()->GetMoonlessRobRate();
        RobChests(deadID, killerID, rate);
    }
    else
    {
        DeductPlayerChestsNum(deadPlayer);
    }
}

void RegularVoidField::DeductPlayerChestsNum(Player* player)
{
    VoidFieldPlayerRecord* record = GetRecord(player);
    if (!player || !record)
    {
        return;
    }

    // 扣除玩家的宝箱数
    int64 drop_chest = record->GetChestsNum() * VoidFieldConfig::Instance()->GetMoonlessRobRate() / BASE_RATE_NUM;
    record->SubChestsNum((int32)drop_chest);
    if (drop_chest > 0)
    {
        SendVoidFieldChestsNum(player, record->GetChestsNum());
    }
}

