/*
*   绑定魔石商店
*/
#ifndef bind_money_point_sale_config_h__
#define bind_money_point_sale_config_h__

#include <vector>



class BindMoneyPointSaleConfig
{
public:
    BindMoneyPointSaleConfig();
    virtual ~BindMoneyPointSaleConfig();

    DECLARE_SINGLETON(BindMoneyPointSaleConfig)

    bool LoadConfig(const char* path);

    //const struct ItemTemplate* GetSaleItem(uint32 itemID);

private:
    //typedef std::map<uint32, const struct ItemTemplate*>    SaleMap;
    //SaleMap m_sale_map;        // 物品ID

};

#endif
