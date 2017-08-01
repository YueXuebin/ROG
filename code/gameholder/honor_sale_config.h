/*
*   荣誉值购买
*/
#ifndef honor_sale_config_h__
#define honor_sale_config_h__

class HonorSaleConfig
{
public:
    HonorSaleConfig();
    virtual ~HonorSaleConfig();

    DECLARE_SINGLETON(HonorSaleConfig)

    bool LoadConfig(const char* path);

//    const struct ItemTemplate* GetSaleItem(uint32 itemID);
//
//private:
//    typedef std::map<uint32, const struct ItemTemplate*>    SaleMap;
//    SaleMap m_sale_map;        // 物品ID
};

#endif
