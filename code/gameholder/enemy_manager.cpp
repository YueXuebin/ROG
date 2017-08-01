#include "gameholder_pch.h"
#include "enemy_manager.h"
#include "player.h"
#include "scene.h"
#include "event_define.h"
#include "void_field_manager.h"

// ==================================================
// 构造函数
// ==================================================
EnemyManager::EnemyManager(Player* player) :
m_player(player)
{
    m_fight_mode = FIGHT_MODE_PEACE;
    m_enemy_info.Clear();
    m_notify_info.Clear();
    m_attacker_info.Clear();
}

// ==================================================
// 析构函数
// ==================================================
EnemyManager::~EnemyManager()
{
    
}

// ==================================================
// 加载仇人信息
// ==================================================
void EnemyManager::LoadInfo(const ENEMY_INFO& enemy_info)
{
    m_enemy_info = enemy_info;
}

// ==================================================
// 填充仇人信息
// ==================================================
void EnemyManager::FillInfo(ENEMY_INFO& enemy_info)
{
    enemy_info = m_enemy_info;
}

// ==================================================
// 消息接收
// ==================================================
void EnemyManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_FIGHT_MODE_REQ:
        OnFightModeReq(data);
        break;
    default:
        break;
    }
}

// ==================================================
// 每秒更新
// ==================================================
void EnemyManager::Update()
{
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    // 定时减少玩家在虚空幻境中的罪恶值
    //if ( now >= (m_enemy_info.update_time + GameParamConfig::Instance()->GetSubEvilInterval()))
    //{
    //    // 定时减少罪恶值
    //    if (m_enemy_info.evil_point >= GameParamConfig::Instance()->GetSubEvilValue())
    //    {
    //        m_enemy_info.update_time = now;
    //        SubEvilPoint(GameParamConfig::Instance()->GetSubEvilValue());
    //    }
    //}

    // 删除超过时间的仇人
    for (std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin();
            it != m_enemy_info.enemy_list.end();)
    {
        if(now >= it->record_time)
        {
            // 添加到通知列表
            it->opt = OPT_DEL_FLAG;
            m_notify_info.enemy_list.push_back(*it);
            // 从仇人名单中删除
            it = m_enemy_info.enemy_list.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // 如果通知列表大于0，则通知并清空
    if(m_notify_info.enemy_list.size() > 0)
    {
        m_player->SendToGate(MSG_EMEMY_UPDATE_NTF, &m_notify_info);
        m_notify_info.Clear();
    }

    // 攻击者列表
    if(m_attacker_info.list.size() > 0)
    {
        m_player->SendToGate(MSG_ENEMY_ATTACKER_NTF, &m_attacker_info);
        m_attacker_info.list.clear();
    }
}

// ==================================================
// player 是否红名玩家
// ==================================================
bool EnemyManager::IsRedName()
{
    //if(m_enemy_info.evil_point >= GameParamConfig::Instance()->GetRedNameEvil())
    //{
    //    return true;
    //}
    //return false;
    return false;
}

// ==================================================
// 是否在反击时间内
// ==================================================
bool EnemyManager::IsInBeatBackTime(uint64 playerID)
{
    uint64 enemy_id = 0;
    uint32 now = 0;

    if(IsRedName())
    {
        return false;
    }

    for(std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin();
            it != m_enemy_info.enemy_list.end(); ++it)
    {
        enemy_id = U64ID(it->player_id.id_h, it->player_id.id_l);
        if(enemy_id)
        {
            now = (uint32)Crown::SDNow().GetTimeValue();
            if(now < it->record_time)
            {
                return true;
            }
            return false;
        }
    }

    return false;
}

// ==================================================
// 获得善恶点
// ==================================================
int32 EnemyManager::GetEvilPoint()
{
    return m_enemy_info.evil_point;
}

// ==================================================
// 添加仇恨点
// ==================================================
void EnemyManager::AddEvilPoint(int32 val)
{
    int32 old_point = m_enemy_info.evil_point;
    m_enemy_info.evil_point += val;

    CnInfo("AddEvilPoint %d\n", val);

    // 检查两个值，判断红名状态
    //if((old_point < GameParamConfig::Instance()->GetRedNameEvil()) &&
    //   (m_enemy_info.evil_point >= GameParamConfig::Instance()->GetRedNameEvil()))
    //{
    //    CREATURE_EVIL creatureEvil;
    //    creatureEvil.object_id = m_player->GetId();
    //    creatureEvil.evil = m_enemy_info.evil_point;
    //    CREATURE_EVIL_LIST creatureEvilList;
    //    creatureEvilList.list.push_back(creatureEvil);
    //    m_player->GetScene()->NotifyMessage(m_player, MSG_CREATURE_EVIL_LIST_NTF, &creatureEvilList);

    //    m_player->GetScene()->SendEvent(0, EVENT_EVIL_CHANGE_TO_RED, m_player->GetId(), 0);
    //}

    // 善恶信息通知
    SendEnemyInfoNtf();
}

// ==================================================
// 减少罪恶点
// ==================================================
void EnemyManager::SubEvilPoint(int32 val)
{
    int32 old_point = m_enemy_info.evil_point;
    CnAssert(m_enemy_info.evil_point >= val);

    if(m_enemy_info.evil_point >= val)
        m_enemy_info.evil_point -= val;
    else
         m_enemy_info.evil_point = 0;

    //m_player->SendToGate(MSG_EVIL_CHANGED_NTF, &m_notify_info);

    // 检查两个值，判断红名状态
    //if((old_point >= GameParamConfig::Instance()->GetRedNameEvil()) &&
    //    (m_enemy_info.evil_point < GameParamConfig::Instance()->GetRedNameEvil()))
    //{
    //    CREATURE_EVIL creatureEvil;
    //    creatureEvil.object_id = m_player->GetId();
    //    creatureEvil.evil = m_enemy_info.evil_point;
    //    CREATURE_EVIL_LIST creatureEvilList;
    //    creatureEvilList.list.push_back(creatureEvil);
    //    m_player->GetScene()->NotifyMessage(m_player, MSG_CREATURE_EVIL_LIST_NTF, &creatureEvilList);

    //    m_player->GetScene()->SendEvent(0, EVENT_EVIL_CHANGE_TO_WHITE, m_player->GetId(), 0);
    //}

    // 善恶信息通知
    SendEnemyInfoNtf();
}

// ==================================================
// 获得仇人记录
// ==================================================
ENEMY_RECORD* EnemyManager::GetEnemyRecord(uint64 playerID)
{
    for(std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin();
        it != m_enemy_info.enemy_list.end(); ++it)
    {
        if((it->player_id.id_h == U64ID_HIGH(playerID)) &&
            (it->player_id.id_l == U64ID_LOW(playerID)))
        {
            return &(*it);
        }
    }
    return 0;
}

// ==================================================
// 获得仇人标记
// ==================================================
uint8 EnemyManager::GetEnemyFlag(uint64 playerID)
{
    for(std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin();
        it != m_enemy_info.enemy_list.end(); ++it)
    {
        if((it->player_id.id_h == U64ID_HIGH(playerID)) &&
            (it->player_id.id_l == U64ID_LOW(playerID)))
        {
            it->flag;
        }
    }
    return 0;
}

// ==================================================
// 判断仇人
// ==================================================
bool EnemyManager::IsEnemy(uint64 playerID, uint8 flag)
{
    for(std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin();
        it != m_enemy_info.enemy_list.end(); ++it)
    {
        if((it->player_id.id_h == U64ID_HIGH(playerID)) &&
           (it->player_id.id_l == U64ID_LOW(playerID)))
        {
            // 如果设置了标记，则检查标记是否相同
            if(flag)
            {
                if(it->flag == flag)
                    return true;
                else
                    return false;
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}

// ==================================================
// 把enemy作为仇人添加到victim的仇人列表中
// ==================================================
void EnemyManager::AddEnemy(uint64 playerID, uint8 flag)
{
    ENEMY_RECORD enemy_record;
    ENEMY_RECORD* record = 0;

    record = GetEnemyRecord(playerID);
    if(record)
    {
        //record->record_time = (uint32)Crown::SDNow().GetTimeValue() + GameParamConfig::Instance()->GetFightBackTime();
        return;
    }
    else
    {
        enemy_record.player_id.id_h = U64ID_HIGH(playerID);
        enemy_record.player_id.id_l = U64ID_LOW(playerID);
        enemy_record.flag = flag;
        //enemy_record.record_time = (uint32)Crown::SDNow().GetTimeValue() + GameParamConfig::Instance()->GetFightBackTime();

        // 添加到仇人名单
        m_enemy_info.enemy_list.push_back(enemy_record);

        // 添加到通知列表
        enemy_record.opt = OPT_ADD_FLAG;
        m_notify_info.enemy_list.push_back(enemy_record);
    }

    //CnInfo("~~~~~~~~~~~~~~~~~~~~\n");
    //CnInfo("add enemy list\n");
    //for(std::vector<ENEMY_RECORD>::iterator it = m_enemy_info.enemy_list.begin(); 
    //        it != m_enemy_info.enemy_list.end(); ++it)
    //{
    //    uint64 playerID = U64ID(it->player_id.id_h, it->player_id.id_l);
    //    CnInfo("enemy %llu\n", playerID);
    //}
}

// ==================================================
// 添加攻击者名单
// ==================================================
void EnemyManager::AddAttacker(uint32 creatureID)
{
    for(std::vector<uint32>::iterator it = m_attacker_info.list.begin(); it != m_attacker_info.list.end(); ++it)
    {
        if(*it == creatureID)
            return;
    }
    m_attacker_info.list.push_back(creatureID);
}

// ==================================================
// 添加因攻击影响的罪恶值
// ==================================================
void EnemyManager::AddAttacEvilVal(EnemyManager* defenceEnemyMgr)
{
    ENEMY_RECORD* record = 0;

    if(!defenceEnemyMgr)
        return;

    record = defenceEnemyMgr->GetEnemyRecord(m_player->GetPlayerID());
    if(!record)
        return;

    if(defenceEnemyMgr->IsRedName())
        return;

    //int32 attack_value = GameParamConfig::Instance()->GetAttackEvilVal();

    //if((record->flag == ENEMY_FLAG_ATTACK) && (record->state == 0))
    //{
    //    AddEvilPoint(attack_value);
    //    record->state = ENEMY_RECORD_STATE_ATTACK;
    //}
}

// ==================================================
// 添加击杀影响的罪恶值
// ==================================================
void EnemyManager::AddKillEvilVal(EnemyManager* defenceEnemyMgr)
{
    ENEMY_RECORD* record = 0;

    if(!defenceEnemyMgr)
        return;

    record = defenceEnemyMgr->GetEnemyRecord(m_player->GetPlayerID());
    if(!record)
        return;

    //int32 attack_value = (defenceEnemyMgr->IsRedName()) ? (0) : (GameParamConfig::Instance()->GetAttackEvilVal());
    //int32 kill_value = (defenceEnemyMgr->IsRedName()) ? (GameParamConfig::Instance()->GetKillRedEvilVal()) : (GameParamConfig::Instance()->GetKillEvilVal());

    //if(record->state == 0)
    //{
    //    AddEvilPoint(attack_value + kill_value);
    //}
    //else if(record->state == ENEMY_RECORD_STATE_ATTACK)
    //{
    //    AddEvilPoint(kill_value);
    //    record->state = 0;
    //}
}

// ==================================================
// 设置战斗模式
// ==================================================
void EnemyManager::SetFightMode(int32 val)
{
    if((FIGHT_MODE_PEACE <= val) && (val <= FIGHT_MODE_MASSACRE))
    {
        m_fight_mode = val;
    }
    else
    {
        m_fight_mode = FIGHT_MODE_PEACE;
    }

    SendFightModeAck(m_fight_mode);
}

// ==================================================
// 善恶信息请求
// ==================================================
void EnemyManager::SendEnemyInfoNtf()
{
    m_player->SendToGate(MSG_EMEMY_INFO_NTF, &m_enemy_info);
}

void EnemyManager::CalEvilPoint()
{
    uint32 last_time = m_enemy_info.update_time;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    //int32 sub_interval = GameParamConfig::Instance()->GetSubEvilInterval();
    //int32 sub_evil = GameParamConfig::Instance()->GetSubEvilValue();

    //uint32 interval_num = (now - last_time) / GameParamConfig::Instance()->GetSubEvilInterval();

    //if (interval_num > 0)
    //{
    //    m_enemy_info.evil_point -= interval_num*sub_evil;
    //    m_enemy_info.update_time = now;
    //}
    //m_enemy_info.evil_point = (m_enemy_info.evil_point < GameParamConfig::Instance()->GetMinEvilValue()) ? (GameParamConfig::Instance()->GetMinEvilValue()) : (m_enemy_info.evil_point);
}

// ==================================================
// 战斗模式请求
// ==================================================
void EnemyManager::OnFightModeReq(void* data)
{
    FIGHT_MODE_REQ* recvMsg = (FIGHT_MODE_REQ*)data;

    if(!recvMsg) return;

    // 屏蔽珍兽状态时的模式切换的请求
    if (VoidFieldManager::Instance()->IsInBoosScene(m_player))
    {
        uint16 errCode = ERR_VOID_FIELD_CANNOT_CHANGE_FIGHT_MODE;
        m_player->NotifyErrorCode(errCode);
        return;
    }

    SetFightMode(recvMsg->fight_mode);
}

// ==================================================
// 发送战斗模式
// ==================================================
void EnemyManager::SendFightModeAck(int32 fightMode)
{
    FIGHT_MODE_ACK sendMsg;
    sendMsg.fight_mode = fightMode;
    m_player->SendToGate(MSG_FIGHT_MODE_ACK, &sendMsg);
}


void EnemyManager::SendEvilInfoNtf()
{
    //EVIL_VALUE_INFO evil_info;
    //evil_info.evil_value = m_enemy_info.evil_point;
    //evil_info.last_time = m_enemy_info.update_time;
    //m_player->SendToGate(MSG_EVIL_CHANGED_NTF, &evil_info);
}


