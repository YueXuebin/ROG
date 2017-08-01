#include "gameholder_pch.h"
#include "sub_regular_hook.h"
#include "pet.h"
#include "event_define.h"
#include "scene.h"
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
#include "drop_object.h"
#include "world_map_config.h"
#include "boss.h"
#include "regular_event.h"
#include "event_define.h"
#include "battle_unit.h"
#include "scene.h"
#include "checkpoint.h"
#include "player.h"
#include "boss.h"
#include "ai_controller.h"
#include "enemy_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "guild_manager.h"
#include "world_boss_manager.h"
#include "game_util.h"
#include "regular_field_boss.h"
#include "void_field_config.h"

SubRegularHook::SubRegularHook(Regular* pRegular) :
SubRegular(pRegular)
{
    m_masterRegular = CnDynamicCast(RegularFiledBoss, pRegular);
    CnAssert(m_masterRegular);
}

SubRegularHook::~SubRegularHook()
{
}

void SubRegularHook::OneSecondUpdate( int32 deltatime )
{
    SubRegular::OneSecondUpdate(deltatime);

    RewardHookExp();

    PushMessage();
}

void SubRegularHook::OnCreatureEnter( BattleUnit* creature )
{
    SubRegular::OnCreatureEnter(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)creature;

        // �����ҵĺ���buff
        if (IsRedName(player))
        {
            AddPlayerRedNameBuff(player);
        }

        // ����Ĭ��ս��ģʽ
        EnemyManager* enemyMgr = (player->GetEnemyMgr());
        if(enemyMgr)
        {
            enemyMgr->SetFightMode(FIGHT_MODE_PEACE);
        }
    }
}

void SubRegularHook::OnCreatureLeave( BattleUnit* creature )
{
    SubRegular::OnCreatureLeave(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        // �����ҵĺ���buff
        ClearPlayerRedNameBuff((Player*)creature);
    }
}

void SubRegularHook::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    SubRegular::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_SCENE_ATTACK:
        {
            OnBattleEventAttack(arg1, arg2);
        }
        break;
    case BATTLE_EVENT_PLAYER_DEAD:
        {
            OnBattleEventKill(arg2, arg1);
        }
        break;
    case EVENT_PLAYER_RESURGENCE_REQ:       // ���ո�������
        {
            OnPlayerResurgenceReq(arg1);
        }
        break;
    case EVENT_EVIL_CHANGE_TO_RED:
        {
            Player* player = GetScene()->GetPlayerByObjectId((uint32)arg1);
            AddPlayerRedNameBuff(player);
        }
        break;
    case EVENT_EVIL_CHANGE_TO_WHITE:
        {
            Player* player = GetScene()->GetPlayerByObjectId((uint32)arg1);
            ClearPlayerRedNameBuff(player);
        }
        break;
    default:
        break;
    }
}

void SubRegularHook::Update(uint32 curr_tick)
{
    SubRegular::Update(curr_tick);
}

void SubRegularHook::RewardHookExp()
{
    int32 interval_time = WorldMapConfig::Instance()->GetHookExpInterval();
    int32 exp_value     = WorldMapConfig::Instance()->GetHookExpValue();
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    // ��Boss������©ʱ���ھ����б��ʲ���
    if (m_masterRegular->IsInBossLeakTime())
    {
        exp_value *= WorldMapConfig::Instance()->GetBossLeakExpMulti();
    }

    std::map<uint64, WorldMapPlayerRecord>& records = m_masterRegular->GetRecordMap();
    for (std::map<uint64, WorldMapPlayerRecord>::iterator iter = records.begin(); iter != records.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.player_id);
        if(!player)
            return;

        // �жϼӾ����ʱ��
        if ((0 < iter->second.add_exp_time_point) && (iter->second.add_exp_time_point <= now))
        {
            // ��Ӿ���
            player->AddExp(exp_value);

            // ������Ӿ����ʱ��
            iter->second.add_exp_time_point = now + interval_time;

            // ��Ӿ�����������
            AddPersonalExpInfo(player, &(iter->second.personal_info), exp_value);
        }
    }
}

void SubRegularHook::AddPersonalExpInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, int32 exp)
{
    REWARD_ITEM rewardItem;

    if(!player || !info)
        return;

    rewardItem.Clear();
    rewardItem.id = PROP_EXP;
    rewardItem.num = exp;

    info->reward_table.items.push_back(rewardItem);
}

void SubRegularHook::OnBattleEventAttack(uint32 attackerID, uint32 defencerID)
{
    Player* attackPlayer = 0;
    Player* defencePlayer = 0;
    EnemyManager* attackEnemyMgr = 0;
    EnemyManager* defenceEnemyMgr = 0;

    // ������
    attackPlayer = m_masterRegular->GetPlayerByUnitID(attackerID);
    if(!attackPlayer)
        return;

    // ������
    defencePlayer = m_masterRegular->GetPlayerByUnitID(defencerID);
    if(!defencePlayer)
        return;

    // ����ƶ�ϵͳ
    attackEnemyMgr = attackPlayer->GetEnemyMgr();
    defenceEnemyMgr = defencePlayer->GetEnemyMgr();
    if(!attackEnemyMgr || !defenceEnemyMgr)
        return;

    // ��ӹ������б�
    defenceEnemyMgr->AddAttacker(attackPlayer->GetId());

    // ��ӷ�����
    attackEnemyMgr->AddEnemy(defencePlayer->GetPlayerID(), ENEMY_FLAG_FIGHTBACK);

    // ��ӳ���
    defenceEnemyMgr->AddEnemy(attackPlayer->GetPlayerID(), ENEMY_FLAG_ATTACK);

    // ����ƶ�ֵ
    attackEnemyMgr->AddAttacEvilVal(defenceEnemyMgr);
}

void SubRegularHook::OnBattleEventKill(uint32 attackerID, uint32 defencerID)
{
    Player* attackPlayer = 0;
    Player* defencePlayer = 0;
    EnemyManager* attackEnemyMgr = 0;
    EnemyManager* defenceEnemyMgr = 0;

    // ������
    defencePlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!defencePlayer)
        return;

    // ���͸�������
    SendResurgenceAsk(defencePlayer);

    // ������
    attackPlayer = GetScene()->GetPlayerByObjectId(attackerID);
    if(!attackPlayer)
        return;

    // ����ƶ�ϵͳ
    attackEnemyMgr = attackPlayer->GetEnemyMgr();
    defenceEnemyMgr = defencePlayer->GetEnemyMgr();
    if(!attackEnemyMgr || !defenceEnemyMgr)
        return;

    // ����ƶ�ֵ
    attackEnemyMgr->AddKillEvilVal(defenceEnemyMgr);

    // ��Ӹ��˻�ɱ��Ϣ
    AddPersonalKillInfo(attackPlayer, defencePlayer);
}

void SubRegularHook::AddPersonalKillInfo(Player* attacker, Player* defencer)
{
    VOID_FIELD_BATTLE_INFO  battleInfo;
    WorldMapPlayerRecord*  record = NULL;

    if(!attacker || !defencer)
        return;

    // ��ӹ���֪ͨ
    record = m_masterRegular->GetRecord(attacker);
    if(record)
    {
        battleInfo.type = VOID_FIELD_BATTLE_DEFEAT;
        battleInfo.crature_id = defencer->GetId();
        record->personal_info.battle_list.push_back(battleInfo);
    }

    // ��ӱ�����֪ͨ
    record = m_masterRegular->GetRecord(defencer);
    if(record)
    {
        battleInfo.type = VOID_FIELD_BATTLE_KILLED;
        battleInfo.crature_id = attacker->GetId();
        record->personal_info.battle_list.push_back(battleInfo);
    }
}

void SubRegularHook::SendResurgenceAsk(Player* player)
{
    REGULAR_PLAYER_DEAD sendMsg;

    if(!player)
        return;

    sendMsg.regular_type = m_Regular->GetType();
    sendMsg.dead_action = DEAD_ACTION_ITEM_RESURGENCE;
    sendMsg.count_down = VoidFieldConfig::Instance()->GetResurgenceTime();
    player->SendToGate(MSG_PLAYER_DEAD, &sendMsg);
}

void SubRegularHook::OnPlayerResurgenceReq(int32 arg1)
{
    Player* player = GetScene()->FindPlayer(arg1);
    if(!player || !player->IsDead())
        return;

    uint16 err = player->SubItemOne(REWARD_REASON_RESURGENCE, PROP_ITEM, 1, RESURGENCE_ITEM);
    if(err != ERR_SUCCEED)
    {
        player->NotifyErrorCode(err);
        return;
    }

    uint32 triggerID = m_masterRegular->CalPlayerTriggerID(player);

    // ������ԭ�ظ���
    if (player->GetSettingManager()->GetVoidFieldSettingInfo().current_revival)
    {
        triggerID = 0;
    }

    m_Regular->Resurgence(player, triggerID, 0);

    if (IsRedName(player))
    {
        AddPlayerRedNameBuff(player);
    }
}


bool SubRegularHook::IsRedName(Player* player)
{
    EnemyManager* enemy_mrg = player->GetEnemyMgr();
    if (enemy_mrg && enemy_mrg->IsRedName())
    {
        return true;
    }

    return false;
}


void SubRegularHook::AddPlayerRedNameBuff(Player* player)
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

void SubRegularHook::ClearPlayerRedNameBuff(Player* player)
{
    if (!player)
    {
        return;
    }
    //player->DelBuff(GameParamConfig::Instance()->GetRedNameBuffID());
}

void SubRegularHook::PushMessage()
{
    std::map<uint64, WorldMapPlayerRecord>& records = m_masterRegular->GetRecordMap();
    for (std::map<uint64, WorldMapPlayerRecord>::iterator iter = records.begin(); iter != records.end(); ++iter)
    {
        Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(iter->second.player_id);
        if(!player)
            return;

        // �ж��Ƿ����͸�����ջþ���Ϣ
        if((iter->second.personal_info.battle_list.size() > 0) || 
            (iter->second.personal_info.reward_table.items.size() > 0))
        {
            player->SendToGate(MSG_FIELD_BOSS_PERSONAL_INFO_NTF, &(iter->second.personal_info));

            // �����Ϣ
            iter->second.personal_info.Clear();
        }
    }
}
