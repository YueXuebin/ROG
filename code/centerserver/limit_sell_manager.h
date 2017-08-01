/*
*   限购管理器
*/
#ifndef limit_sell_manager_h__
#define limit_sell_manager_h__

class Player;

// 限购商品信息
typedef struct LimitSellProductInfo
{
    void Clear()
    {
        product_id = 0;
        num = 0;
        time_stamp = 0;
    }
    uint32 product_id;  // 商品id
    int32  num;         // 已购数量
    uint32  time_stamp; // 购买时间戳
}LimitSellProductInfo;


class LimitSellManager
{
public:
    LimitSellManager(Player* player);
    ~LimitSellManager();

    void Clear();

    bool CheckIfCanBuy(uint32 productID, uint32 productNum);    // 检查是否可买
    void AddSellNum(uint32 productID, uint32 productNum);       // 增加销售数量

private:
    Player* m_player;
    std::map<uint32, LimitSellProductInfo> m_limitSellList;          // 销售列表 [商品号, 商品信息]

};

#endif // limit_sell_manager_h__
