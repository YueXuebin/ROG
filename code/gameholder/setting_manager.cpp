// 设置系统
#include "gameholder_pch.h"
#include "setting_manager.h"
#include "player.h"
#include "auto_fight_config.h"
#include "system_setting_config.h"
#include "void_field_setting_config.h"
#include "game_setting_manager.h"

SettingManager::SettingManager(Player* player) :
m_player(player)
{
    Clear();
}

SettingManager::~SettingManager()
{
}

void SettingManager::Clear()
{
    m_auto_fight_setting_info.Clear();
    m_system_setting_info.Clear();
    m_player_setting_info.Clear();
    m_autobuy_setting_info.Clear();
    m_void_field_setting_info.Clear();
}

void SettingManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_AUTO_FIGHT_SETTING_INFO_REQ:
        {
            OnAutoFightSettingInfoReq();
        }
        break;
    case MSG_SYSTEM_SETTING_INFO_REQ:
        {
            OnSystemSettingInfoReq();
        }
        break;
    case MSG_PLAYER_SETTING_INFO_REQ:
        {
            OnPlayerSettingInfoReq();
        }
        break;
    case MSG_AUTOBUY_SETTING_INFO_REQ:
        {
            OnAutobuySettingInfoReq();
        }
        break;
    case MSG_AUTO_FIGHT_SETTING_CHANGE_REQ:
        {
            OnAutoFightSettingChangeReq(data);
        }
        break;
    case MSG_AUTO_FIGHT_SETTING_RESET_REQ:
        {
            OnAutoFightSettingResetReq();
        }
        break;
    case MSG_SYSTEM_SETTING_CHANGE_REQ:
        {
            OnSystemSettingChangeReq(data);
        }
        break;
    case MSG_SYSTEM_SETTING_RESET_REQ:
        {
            OnSystemSettingResetReq();
        }
        break;
    case MSG_PLAYER_SETTING_CHANGE_REQ:
        {
            OnPlayerSettingChangeReq(data);
        }
        break;
    case MSG_AUTOBUY_SETTING_CHANGE_REQ:
        {
            OnAutobuySettingChangeReq(data);
        }
        break;
    case MSG_AUTOBUY_SETTING_RESET_REQ:
        {
            OnAutobuySettingResetReq();
        }
        break;
    case MSG_VOID_FIELD_SETTING_INFO_REQ:
        {
            OnVoidFieldSettingInfoReq();
        }
        break;
    case MSG_VOID_FIELD_SETTING_CHANGE_REQ:
        {
            OnVoidFieldSettingChangeReq(data);
        }
        break;
    case MSG_VOID_FIELD_SETTING_RESET_REQ:
        {
            OnVoidFieldSettingResetReq();
        }
        break;
    default:
        break;
    }
}

void SettingManager::LoadAutoFightSettingInfo(const AUTO_FIGHT_SETTING_INFO& info)
{
    m_auto_fight_setting_info = info;

    if (m_auto_fight_setting_info.hp == 0 && m_auto_fight_setting_info.mp == 0)   // init
    {
        m_auto_fight_setting_info.hp = AutoFightConfig::Instance()->GetHP();
        m_auto_fight_setting_info.hp_order = AutoFightConfig::Instance()->GetHPOrder();
        m_auto_fight_setting_info.mp = AutoFightConfig::Instance()->GetMP();
        m_auto_fight_setting_info.mp_order = AutoFightConfig::Instance()->GetMPOrder();
        m_auto_fight_setting_info.skills = AutoFightConfig::Instance()->GetSkills();
        m_auto_fight_setting_info.auto_revival = AutoFightConfig::Instance()->GetAutoRevival();
        m_auto_fight_setting_info.auto_fight = AutoFightConfig::Instance()->GetAutoFight();
        m_auto_fight_setting_info.auto_buy_hp_drug = AutoFightConfig::Instance()->GetAutoBuyHPDrug();
        m_auto_fight_setting_info.auto_buy_mp_drug = AutoFightConfig::Instance()->GetAutoBuyMPDrug();
        m_auto_fight_setting_info.auto_buy_attack_drug = AutoFightConfig::Instance()->GetAutoBuyAttackDrug();
        m_auto_fight_setting_info.auto_buy_defence_drug = AutoFightConfig::Instance()->GetAutoBuyDefenceDrug();
        m_auto_fight_setting_info.auto_buy_speed_drug = AutoFightConfig::Instance()->GetAutoBuySpeedDrug();
        m_auto_fight_setting_info.auto_use_attack_drug = AutoFightConfig::Instance()->GetAutoUseAttackDrug();
        m_auto_fight_setting_info.auto_use_defence_drug = AutoFightConfig::Instance()->GetAutoUseDefenceDrug();
        m_auto_fight_setting_info.auto_use_speed_drug = AutoFightConfig::Instance()->GetAutoUseSpeedDrug();
    }
}

void SettingManager::FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& info)
{
    info = m_auto_fight_setting_info;
}

void SettingManager::LoadSystemSettingInfo(const SYSTEM_SETTING_INFO& info)
{
    m_system_setting_info = info;

    if (m_system_setting_info.volume == 0 && m_system_setting_info.se_volume == 0)  // init
    {
        OnSystemSettingResetReq();
    }
}

void SettingManager::FillSystemSettingInfo(SYSTEM_SETTING_INFO& info)
{
    info = m_system_setting_info;
}

void SettingManager::LoadPlayerSettingInfo(const PLAYER_SETTING_INFO& info)
{
    m_player_setting_info = info;

    if (m_player_setting_info.hp == 0 && m_player_setting_info.mp == 0) //init
    {
        m_player_setting_info.hp = 200;
        m_player_setting_info.mp = 203;
    }
}

void SettingManager::FillPlayerSettingInfo(PLAYER_SETTING_INFO& info)
{
    info = m_player_setting_info;
}

void SettingManager::LoadAutobuySettingInfo(const AUTOBUY_SETTING_INFO& info)
{
    m_autobuy_setting_info = info;
}

void SettingManager::FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& info)
{
    // 自动购买不在服务器保存
    std::vector<int>::iterator iter;
    for(iter = m_autobuy_setting_info.autobuyList.begin(); iter != m_autobuy_setting_info.autobuyList.end(); iter++)
    {
        (*iter) = 0;
    }

    info = m_autobuy_setting_info;
}

void SettingManager::LoadVoidFieldSettingInfo(const VOID_FIELD_SETTING_INFO& info)
{
    m_void_field_setting_info = info;

    if (info.auto_buy_revival == 99 && info.auto_use_revival == 99 && info.current_revival == 99 && info.reconnect_on_loss == 99) // init
    {
        m_void_field_setting_info.auto_use_revival = VoidFieldSettingConfig::Instance()->GetAutoUseRevival();
        m_void_field_setting_info.auto_buy_revival = VoidFieldSettingConfig::Instance()->GetAutoBuyRevival();
        m_void_field_setting_info.current_revival = VoidFieldSettingConfig::Instance()->GetCurrentRevival();
        m_void_field_setting_info.reconnect_on_loss = VoidFieldSettingConfig::Instance()->GetReconnectOnLoss();
    }
}

void SettingManager::FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& info)
{
    info = m_void_field_setting_info;
}

void SettingManager::CancelCurrentHookSetting()
{
//     m_void_field_setting_info.current_hook = 0;
//     m_void_field_setting_info.field_hook_x = 0;
//     m_void_field_setting_info.field_hook_y = 0;

    // 通知client 原地挂机取消
    VOID_FIELD_SETTING_INFO sendMsg = m_void_field_setting_info;
    m_player->SendToGate(MSG_VOID_FIELD_SETTING_RESET_ACK, &sendMsg);
}

void SettingManager::OnAutoFightSettingInfoReq()
{
    AUTO_FIGHT_SETTING_INFO sendMsg = m_auto_fight_setting_info;

    m_player->SendToGate(MSG_AUTO_FIGHT_SETTING_INFO_ACK, &sendMsg);
}

void SettingManager::OnSystemSettingInfoReq()
{
    SYSTEM_SETTING_INFO sendMsg = m_system_setting_info;

    m_player->SendToGate(MSG_SYSTEM_SETTING_INFO_ACK, &sendMsg);
}

void SettingManager::OnPlayerSettingInfoReq()
{
    PLAYER_SETTING_INFO sendMsg = m_player_setting_info;

    m_player->SendToGate(MSG_PLAYER_SETTING_INFO_ACK, &sendMsg);
}

void SettingManager::OnAutobuySettingInfoReq()
{
    AUTOBUY_SETTING_INFO sendMsg = m_autobuy_setting_info;

    m_player->SendToGate(MSG_AUTOBUY_SETTING_INFO_ACK, &sendMsg);
}

void SettingManager::OnAutoFightSettingChangeReq(void* data)
{
    AUTO_FIGHT_SETTING_CHANGE_REQ* recvMsg = (AUTO_FIGHT_SETTING_CHANGE_REQ*)data;

    if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_HP)
    {
        m_auto_fight_setting_info.hp = recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_HP_ORDER)
    {
        m_auto_fight_setting_info.hp_order = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_MP)
    {
        m_auto_fight_setting_info.mp = recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_MP_ORDER)
    {
        m_auto_fight_setting_info.mp_order = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_SKILLS)
    {
        m_auto_fight_setting_info.skills = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_REVIVAL)
    {
        m_auto_fight_setting_info.auto_revival = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_FIGHT)
    {
        m_auto_fight_setting_info.auto_fight = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_BUY_HP_DRUG)
    {
        m_auto_fight_setting_info.auto_buy_hp_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_BUY_MP_DRUG)
    {
        m_auto_fight_setting_info.auto_buy_mp_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_BUY_ATTACK_DRUG)
    {
        m_auto_fight_setting_info.auto_buy_attack_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_BUY_DEFENCE_DRUG)
    {
        m_auto_fight_setting_info.auto_buy_defence_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_BUY_SPEED_DRUG)
    {
        m_auto_fight_setting_info.auto_buy_speed_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_USE_ATTACK_DRUG)
    {
        m_auto_fight_setting_info.auto_use_attack_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_USE_DEFENCE_DRUG)
    {
        m_auto_fight_setting_info.auto_use_defence_drug = (uint8)recvMsg->value;
    }
    else if (recvMsg->key == AUTO_FIGHT_SETTING_ENUM_AUTO_USE_SPEED_DRUG)
    {
        m_auto_fight_setting_info.auto_use_speed_drug = (uint8)recvMsg->value;
    }
}

void SettingManager::OnAutoFightSettingResetReq()
{
    m_auto_fight_setting_info.hp = AutoFightConfig::Instance()->GetHP();
    m_auto_fight_setting_info.hp_order = AutoFightConfig::Instance()->GetHPOrder();
    m_auto_fight_setting_info.mp = AutoFightConfig::Instance()->GetMP();
    m_auto_fight_setting_info.mp_order = AutoFightConfig::Instance()->GetMPOrder();
    m_auto_fight_setting_info.skills = AutoFightConfig::Instance()->GetSkills();
    m_auto_fight_setting_info.auto_revival = AutoFightConfig::Instance()->GetAutoRevival();
    m_auto_fight_setting_info.auto_fight = AutoFightConfig::Instance()->GetAutoFight();
    m_auto_fight_setting_info.auto_buy_hp_drug = AutoFightConfig::Instance()->GetAutoBuyHPDrug();
    m_auto_fight_setting_info.auto_buy_mp_drug = AutoFightConfig::Instance()->GetAutoBuyMPDrug();
    m_auto_fight_setting_info.auto_buy_attack_drug = AutoFightConfig::Instance()->GetAutoBuyAttackDrug();
    m_auto_fight_setting_info.auto_buy_defence_drug = AutoFightConfig::Instance()->GetAutoBuyDefenceDrug();
    m_auto_fight_setting_info.auto_buy_speed_drug = AutoFightConfig::Instance()->GetAutoBuySpeedDrug();
    m_auto_fight_setting_info.auto_use_attack_drug = AutoFightConfig::Instance()->GetAutoUseAttackDrug();
    m_auto_fight_setting_info.auto_use_defence_drug = AutoFightConfig::Instance()->GetAutoUseDefenceDrug();
    m_auto_fight_setting_info.auto_use_speed_drug = AutoFightConfig::Instance()->GetAutoUseSpeedDrug();
}

void SettingManager::OnSystemSettingChangeReq(void* data)
{
    SYSTEM_SETTING_CHANGE_REQ* recvMsg = (SYSTEM_SETTING_CHANGE_REQ*)data;

    if (recvMsg->key == SYSTEM_SETTING_ENUM_VOLUME)
    {
        m_system_setting_info.volume = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_SE_VOLUME)
    {
        m_system_setting_info.se_volume = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_SHOW_PLAYER)
    {
        m_system_setting_info.show_player = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_SHOW_PET)
    {
        m_system_setting_info.show_pet = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_SHOW_SHADOW)
    {
        m_system_setting_info.show_shadow = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_SHOW_PARTICLE)
    {
        m_system_setting_info.show_particle = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_MIPMAP)
    {
        m_system_setting_info.mipmap = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_RIDE_PET)
    {
        m_system_setting_info.ride_pet = recvMsg->value;
    }
    else if(recvMsg->key == SYSTEM_SETTING_ENUM_ILLUMINATION)
    {
        m_system_setting_info.illumination = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_LIGHT)
    {
         m_system_setting_info.light = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_EFFECT_LEVEL)
    {
        m_system_setting_info.effect_level = recvMsg->value;
    }
    else if(recvMsg->key == SYSTEM_SETTING_ENUM_VARIABILITY)
    {
        m_system_setting_info.fullscreen = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_FOG)
    {
        m_system_setting_info.fog = recvMsg->value;
    }
    else if (recvMsg->key == SYSTEM_SETTING_ENUM_FRAME)
    {
        m_system_setting_info.frame = recvMsg->value;
    }

    if(SYSTEM_SETTING_STATE_MIDDLE == recvMsg->quality_state ||
        SYSTEM_SETTING_STATE_HIGH == recvMsg->quality_state ||
        SYSTEM_SETTING_STATE_LOW == recvMsg->quality_state)
        m_system_setting_info.quality_state = recvMsg->quality_state;

}

void SettingManager::OnSystemSettingResetReq()
{
    m_system_setting_info.volume = SystemSettingConfig::Instance()->GetVolume();
    m_system_setting_info.se_volume = SystemSettingConfig::Instance()->GetSeVolume();
    m_system_setting_info.show_player = SystemSettingConfig::Instance()->GetShowPlayer();
    m_system_setting_info.show_pet = SystemSettingConfig::Instance()->GetShowPet();
    m_system_setting_info.show_shadow = SystemSettingConfig::Instance()->GetShowShadow();
    m_system_setting_info.show_particle = SystemSettingConfig::Instance()->GetShowParticle();
    m_system_setting_info.mipmap = SystemSettingConfig::Instance()->GetMipmap();
   // m_system_setting_info.fullscreen = SystemSettingConfig::Instance()->GetFullscreen();
    m_system_setting_info.fullscreen = GameSettingManager::Instance()->GetScreenParameter();
    m_system_setting_info.ride_pet = SystemSettingConfig::Instance()->GetRidePet();
    m_system_setting_info.illumination = SystemSettingConfig::Instance()->GetBeam();
    m_system_setting_info.light = SystemSettingConfig::Instance()->GetLight();
   // m_system_setting_info.quality_state = SYSTEM_SETTING_STATE_MIDDLE;         // 默认配置
     m_system_setting_info.quality_state = GameSettingManager::Instance()->GetQualityState();

    m_system_setting_info.effect_level = SystemSettingConfig::Instance()->GetEffectLevel();
    m_system_setting_info.fog = SystemSettingConfig::Instance()->GetFog();
    m_system_setting_info.frame = SystemSettingConfig::Instance()->GetFrame();
}

void SettingManager::OnPlayerSettingChangeReq(void* data)
{
    PLAYER_SETTING_CHANGE_REQ* recvMsg = (PLAYER_SETTING_CHANGE_REQ*)data;

    if (recvMsg->key == PLAYER_SETTING_ENUM_HP)
    {
        m_player_setting_info.hp = recvMsg->value;
    }
    else if (recvMsg->key == PLAYER_SETTING_ENUM_MP)
    {
        m_player_setting_info.mp = recvMsg->value;
    }
}

void SettingManager::OnAutobuySettingChangeReq(void* data)
{
    AUTOBUY_SETTING_CHANGE_REQ* recvMsg = (AUTOBUY_SETTING_CHANGE_REQ*)data;

    // 对象判空
    if (!recvMsg)
        return;

    // 越界检查
    if (recvMsg->key >= m_autobuy_setting_info.autobuyList.size())
        return;

    m_autobuy_setting_info.autobuyList[recvMsg->key] = recvMsg->value;
}

void SettingManager::OnAutobuySettingResetReq()
{
    for (uint32 i = 0; i<m_autobuy_setting_info.autobuyList.size(); i++)
    {
        m_autobuy_setting_info.autobuyList[i] = 0;
    }

    // ack
    AUTOBUY_SETTING_INFO sendMsg = m_autobuy_setting_info;

    m_player->SendToGate(MSG_AUTOBUY_SETTING_RESET_ACK, &sendMsg);
}

void SettingManager::OnVoidFieldSettingInfoReq()
{
    VOID_FIELD_SETTING_INFO sendMsg = m_void_field_setting_info;

    m_player->SendToGate(MSG_VOID_FIELD_SETTING_INFO_ACK, &sendMsg);
}

void SettingManager::OnVoidFieldSettingChangeReq(void* data)
{
    VOID_FIELD_SETTING_CHANGE_REQ* recvMsg = (VOID_FIELD_SETTING_CHANGE_REQ*)data;

    if (recvMsg->key == VOID_FIELD_SETTING_ENUM_USE_REVIVAL)
    {
        m_void_field_setting_info.auto_use_revival = recvMsg->value;
    }
    else if (recvMsg->key == VOID_FIELD_SETTING_ENUM_BUY_REVIVAL)
    {
        m_void_field_setting_info.auto_buy_revival = recvMsg->value;
    }
    else if (recvMsg->key == VOID_FIELD_SETTING_ENUM_CURRENT_REVIVAL)
    {
        m_void_field_setting_info.current_revival = recvMsg->value;
    }
    else if (recvMsg->key == VOID_FIELD_SETTING_ENUM_RECONNECT_ON_LOSS)
    {
        m_void_field_setting_info.reconnect_on_loss = recvMsg->value;
    }
}

void SettingManager::OnVoidFieldSettingResetReq()
{
    m_void_field_setting_info.auto_use_revival = VoidFieldSettingConfig::Instance()->GetAutoUseRevival();
    m_void_field_setting_info.auto_buy_revival = VoidFieldSettingConfig::Instance()->GetAutoBuyRevival();
    m_void_field_setting_info.current_revival = VoidFieldSettingConfig::Instance()->GetCurrentRevival();
    m_void_field_setting_info.reconnect_on_loss = VoidFieldSettingConfig::Instance()->GetReconnectOnLoss();

    VOID_FIELD_SETTING_INFO sendMsg = m_void_field_setting_info;
    m_player->SendToGate(MSG_VOID_FIELD_SETTING_RESET_ACK, &sendMsg);
}

