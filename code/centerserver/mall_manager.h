/*
*   商城管理器
*/
#ifndef mall_manager_h__
#define mall_manager_h__

class MallManager
{
public:
    MallManager();
    ~MallManager();

    DECLARE_SINGLETON(MallManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);       // 接收网络消息

    bool UpdateDBVersion();                                     // 从数据库中加载商城数据

    void LoadDBMallInfo();                                      // 从数据库中加载商品信息

    void SendMallInfoToGameSvr(uint32 sessionID);               // 通知GS商城的商品信息

    void RecvReloadMallInfo();                                  // 重新加载限购商城数据

    void RecvReloadMallDBInfo();                                // 重新加载商DB数据

private:
    void RecvMallInfoReq(uint64 playerID);                      // 请求商城数据
    void RecvMallProductReq(uint64 playerID, void* data);       // 单个商品信息请求
    void RecvMallBuyReq(uint64 playerID, void* data);           // 请求商城购买

    uint16 CalProductInfo(const PRODUCT_INFO* tmplD, PRODUCT_INFO& productInfo, uint32 nowTime);    // 获得产品信息

    void PlayerBuyProductSuccess(uint64 playerID, uint32 productID, uint32 productNum);

    bool IsHaveDiscount(const PRODUCT_INFO* product, uint32 nowTime);

    void CalMallPrice(const PRODUCT_INFO* product, PRODUCT_INFO& productInfo, uint32 nowTime);    // 商城限购时间段的值限购价钱

private:
    std::map<uint32, int32> m_globalLimit;                      // 抢购 [商品号, 被抢数量]
};

#endif // mall_manager_h__
