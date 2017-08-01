#ifndef global_setting_manager_h__
#define global_setting_manager_h__

#include "message_encoder.h"

class GlobalSettingManager
{
public:
    GlobalSettingManager(void);
    ~GlobalSettingManager(void);
    
     DECLARE_SINGLETON(GlobalSettingManager)

     void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

     void SaveGlobalSettingInfo();
     void Init();

     GLOBAL_CONFIG_BAG& GetFashionInfoList() {return m_nextFashionInfoList;}            //  时装信息

private:
    void RecvWorldBossStart(void* data);
    void RecvWorldBossEnd(void*  data);

    void FillGlobalSettingList(void* data);
    void SendGlobalSettingList(uint32 gameserverID = 0);


    GLOBAL_CONFIG_BAG m_nextFashionInfoList;
};

#endif