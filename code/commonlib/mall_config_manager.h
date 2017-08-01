#ifndef mall_config_manager_h__
#define mall_config_manager_h__
#include "mall_config.h"
#include "service_config.h"

typedef std::map<uint32, PRODUCT_INFO> MallInfoMap;

class MallConfigManager
{
public:
    MallConfigManager(void);
    virtual ~MallConfigManager(void);

    DECLARE_SINGLETON(MallConfigManager);

    bool LoadConfig(const char* path);


    uint32 GetVersion() { return m_version; }                               // ��ð汾��
    const MallInfoMap&  GetXmlMallInfoMap() { return m_xmlMallMap; }
    const MallInfoMap&  GetDBMallInfoMap() { return m_dbMallMap; }

    void                AddDBProductInfo(const PRODUCT_INFO& productInfo);  // �����Ʒ��Ϣ
    void                DelDBProductInfo(uint32 productID);                 // ɾ����Ʒ��Ϣ
    void                ClearDBProductInfo();                               // �����Ʒ��Ϣ

    const PRODUCT_INFO* GetDBInfoByProductID(uint32 productID);             // ������ƷID��ѯ��Ʒ
    const PRODUCT_INFO* GetDBInfoByItemID(uint32 itemID);                   // ���ݵ���ID��ѯ��Ʒ

    const SERVICE_ROOT_SERVICE_STRUCT*  FindServiceTplById(uint32 serviceId);

private:
    int32               m_version;                                          // �汾��
    MallInfoMap         m_xmlMallMap;                                       // XML���е��̳ǻ�����Ϣ
    MallInfoMap         m_dbMallMap;                                        // ���ݿ��е��̳ǻ�����Ϣ
};

#endif // mall_config_manager_h__
