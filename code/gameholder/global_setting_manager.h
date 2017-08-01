#ifndef global_setting_manager_h__
#define global_setting_manager_h__

class Player;
class GlobalSettingManager
{
public:
    GlobalSettingManager(void);
    ~GlobalSettingManager(void);

    DECLARE_SINGLETON(GlobalSettingManager)

    void OnServerReady();

    void OnRecv(uint32 msgID, void* data);

    void GlobalSettingLoadReq();                                                   // 请求配置信息信息

    void LoadGlobalSettingInfo(GLOBAL_CONFIG_BAG& info);                           //更新配置信息
    void SendGlobalSettingInfoToCenter();                                          // 发送更新信息给Center
    void FillGlobalSetting(GLOBAL_CONFIG_BAG& info);                               // 填充配置

    bool CheckPackageTime(int32 packageid, int32 itemid, int32 cd);

    bool SetPackageTime(int32 packageid, int32 itemid, int32 cd);

private:

    GLOBAL_CONFIG_BAG m_configInfo;

    typedef std::map<uint32, PropertyList> PackageTiemMap;
    PackageTiemMap m_serverTime;
};

#endif