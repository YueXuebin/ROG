#ifndef game_setting_manager_h__
#define game_setting_manager_h__

class GameSettingManager
{
public:
    GameSettingManager(void);
    ~GameSettingManager(void);

    DECLARE_SINGLETON(GameSettingManager)
    
    void SetGameSettingList(const GAME_SETTING_LIST& list);             ///< 设置节日列表
    void SendToAllGameSvrGameSettingList();                             ///< 通知所有Game设置信息
    void SendGameSettingListToGameSvr(uint32 sessionID);                ///< 通知游戏服务器所有的设置信息

    void NotifyChange();                                                ///< 通知centerserver信息变更
private:
    GAME_SETTING_LIST m_gameSettingInfo;
};
#endif