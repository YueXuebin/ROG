/*
*   随机包裹管理器
*/

#ifndef random_package_manager_h__
#define random_package_manager_h__

#include "message_encoder.h"

class Player;

class RandomPackageManager
{
public:
    RandomPackageManager();
    ~RandomPackageManager();

    DECLARE_SINGLETON(RandomPackageManager)

    void OnRecv( uint32 sessionID, uint32 msgID, void* data );       ///< 接收网络消息

    RANDOM_PACKAGE_RECORD_INFO& GetRecordInfo() {return m_RecordInfo;}

    void SendRecordInfo(const uint32 sessionID);
private:
    void FillRecordInfo(void* data);

private:
    RANDOM_PACKAGE_RECORD_INFO  m_RecordInfo;
};


#endif // random_package_manager_h__
