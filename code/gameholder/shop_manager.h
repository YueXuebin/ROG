/*
 *	玩家的商城管理器
 */
#ifndef shop_manager_h__
#define shop_manager_h__

typedef std::map<uint32, uint32 > BuyMap;      // 批量购买用的结构<id, buyNum> < <111,1>, <222,3> >
typedef std::map<uint32, int32 >   CostMap;   // 批量查询花费 <type, cost(有可能是负数)>

class Player;
class ShopManager
{
public:
    ShopManager(Player* player);
    ~ShopManager();

    void Clear();

    void LoadMallInfo(const RESTRICTION_LIST& restrictionList);
    void FillMallInfo(RESTRICTION_LIST& restrictionList);

    void OnRecv(uint32 msgID, void* data);

    void GMReloadMallInfo();                                // 通知CenterServer重新加载商城信息
    void GMBuyReq(uint32 msgID, void* data);                // 购买请求
    void GMMallInfoReq();                                   // 商城信息请求
    

    static void OnMallInfoNotify(void* data);                       // 接收商品通知信息

    static void OnMallBuyAck(uint64 playerID, void* data);          // 商城购买应答

    static void OnMallBuyNtf(uint64 playerID, void* data);          // 商城接收购买通知

   
    bool DoMallBuy(uint32 id, uint32 buyNum, uint32 reason);      // 商城下订单((走的商城配置，给外部类调用的,但是不操作到背包内)
    bool DoListMallBuy(const BuyMap* buyItems , uint32 reason);   // 批量下单  map<id,buyNum>
    
    int32 GetCostMoney(uint32 id, uint32 buyNum);                 // 查询总共花费(给外部类调用的, 负数表示内部判断有错误)
    CostMap GetListCostMoney(const BuyMap* buyItems);             // 批量查询费用

    bool IsEnoughMoney(uint32 id, uint32 buyNum);                 // 用户的钱是否够用(金币/钻石都可以判断,)
    bool ListIsEnoughMoney(const BuyMap* buyItems);               // 批量判断钱够不够

    uint32 GetMoneyType(const PRODUCT_INFO* pProduct);            // 获取商品的消耗的货币类型
    const PRODUCT_INFO* GetProduct(uint32 id);                    // 验证商品的有效性

    void SendMallBuyNtf(uint16 errCode);                     // 商城购买错误通知
    uint16 CalProductInfo(const PRODUCT_INFO* tmpl, PRODUCT_INFO& productInfo, uint32 nowTime); //计算商品信息
    bool IsHaveDiscount(const PRODUCT_INFO* product, uint32  nowTime); // 商城是否有折扣

private:

    void OnMallInfoReq(void* data);                         // 请求商城信息

    void OnMallProductReq(void* data);                      // 请求单个商品信息
    void OnMallBuyReq(void* data);                          // 商城购买请求
    void SendRestrictionList();                             // 个人限购列表

private:
    Player*                     m_player;

    RESTRICTION_LIST            m_RestrictionList;          // 限购商品已买数量列表
};

#endif // mall_manager_h__
