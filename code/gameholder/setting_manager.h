/*
 *	设置系统
 */
#ifndef SETTING_MANAGER_H
#define SETTING_MANAGER_H

class Player;

class SettingManager
{
public:
    SettingManager(Player* player);
    ~SettingManager();

    void Clear();
    void OnRecv(uint32 msgID, void* data);

    void LoadAutoFightSettingInfo(const AUTO_FIGHT_SETTING_INFO& info);
    const AUTO_FIGHT_SETTING_INFO&    GetAutoFightSettingInfo() {return m_auto_fight_setting_info;}
    void FillAutoFightSettingInfo(AUTO_FIGHT_SETTING_INFO& info);

    void LoadSystemSettingInfo(const SYSTEM_SETTING_INFO& info);
    const SYSTEM_SETTING_INFO&    GetSystemSettingInfo() {return m_system_setting_info;}
    void FillSystemSettingInfo(SYSTEM_SETTING_INFO& info);

    void LoadPlayerSettingInfo(const PLAYER_SETTING_INFO& info);
    //const PLAYER_SETTING_INFO&    GetPlayerSettingInfo() {return m_player_setting_info;}
    PLAYER_SETTING_INFO&    GetPlayerSettingInfo() {return m_player_setting_info;}
    void FillPlayerSettingInfo(PLAYER_SETTING_INFO& info);

    void LoadAutobuySettingInfo(const AUTOBUY_SETTING_INFO& info);
    const AUTOBUY_SETTING_INFO&   GetAutobuySettingInfo() {return m_autobuy_setting_info;}
    void FillAutobuySettingInfo(AUTOBUY_SETTING_INFO& info);

    void LoadVoidFieldSettingInfo(const VOID_FIELD_SETTING_INFO& info);
    VOID_FIELD_SETTING_INFO&   GetVoidFieldSettingInfo() {return m_void_field_setting_info;}
    void FillVoidFieldSettingInfo(VOID_FIELD_SETTING_INFO& info);

    // 取消原地挂机设置
    void CancelCurrentHookSetting();

private:
    void OnAutoFightSettingInfoReq();
    void OnSystemSettingInfoReq();
    void OnPlayerSettingInfoReq();
    void OnAutobuySettingInfoReq();
    void OnAutoFightSettingChangeReq(void* data);
    void OnAutoFightSettingResetReq();
    void OnSystemSettingChangeReq(void* data);
    void OnSystemSettingResetReq();
    void OnPlayerSettingChangeReq(void* data);
    void OnAutobuySettingChangeReq(void* data);
    void OnAutobuySettingResetReq();
    void OnVoidFieldSettingInfoReq();
    void OnVoidFieldSettingChangeReq(void* data);
    void OnVoidFieldSettingResetReq();

private:
    Player*                     m_player;
    AUTO_FIGHT_SETTING_INFO     m_auto_fight_setting_info;
    SYSTEM_SETTING_INFO         m_system_setting_info;
    PLAYER_SETTING_INFO         m_player_setting_info;
    AUTOBUY_SETTING_INFO        m_autobuy_setting_info;
    VOID_FIELD_SETTING_INFO     m_void_field_setting_info;
};

#endif