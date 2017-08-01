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


    uint32 GetVersion() { return m_version; }                               // 获得版本号
    const MallInfoMap&  GetXmlMallInfoMap() { return m_xmlMallMap; }
    const MallInfoMap&  GetDBMallInfoMap() { return m_dbMallMap; }

    void                AddDBProductInfo(const PRODUCT_INFO& productInfo);  // 添加商品信息
    void                DelDBProductInfo(uint32 productID);                 // 删除商品信息
    void                ClearDBProductInfo();                               // 清空商品信息

    const PRODUCT_INFO* GetDBInfoByProductID(uint32 productID);             // 根据商品ID查询商品
    const PRODUCT_INFO* GetDBInfoByItemID(uint32 itemID);                   // 根据道具ID查询商品

    const SERVICE_ROOT_SERVICE_STRUCT*  FindServiceTplById(uint32 serviceId);

private:
    int32               m_version;                                          // 版本号
    MallInfoMap         m_xmlMallMap;                                       // XML表中的商城基本信息
    MallInfoMap         m_dbMallMap;                                        // 数据库中的商城基本信息
};

#endif // mall_config_manager_h__
