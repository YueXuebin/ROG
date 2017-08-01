#ifndef game_setting_job_h__
#define game_setting_job_h__

class DBExecutor;
class DBRecorder;
class GameSettingJob
{
public:
    GameSettingJob(void);
    ~GameSettingJob(void);

    DECLARE_SINGLETON(GameSettingJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* gamedb)
    {
        m_gameExecutor = gamedb;
    }

    void InitGameSettingInfo();                                                 // 初始化游戏数据
    void LoadLocalGameSetting(bool initFlag);                                   // 从本地数据库加载游戏设置信息
    void HandleGmtGameSetting();
private:
    void CBLoadGameSettingInfo(DBRecorder& res, bool b_update, bool b_notify);  

    void ParseGameSettingInfo();                                                // 解析初始数据
    bool UpdateGameSettingInfo(GAME_SETTING_INFO& gameSettingInfo);             // 更改游戏设置

    void SaveGameSettingInfo(GAME_SETTING_INFO& gameSettingInfo);               // 保存活动数据

    void LoadSystemVersion(uint32 system_id, uint32& version);  ///< 加载数据库中的系统版本

    bool UpdateSystemVersion(uint32 system_id, uint32 version); ///< 更新数据库中的系统版本

private:
    DBExecutor*     m_gameExecutor;

    GAME_SETTING_LIST m_gameSettingInfo;
};
#endif 