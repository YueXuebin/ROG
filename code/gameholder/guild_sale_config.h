/*
*   ¹«»á¹ºÂò
*/
#ifndef guild_sale_config_h__
#define guild_sale_config_h__

#include <vector>


class GuildSaleConfig
{
public:
    GuildSaleConfig();
    virtual ~GuildSaleConfig();

    DECLARE_SINGLETON(GuildSaleConfig)

    bool LoadConfig(const char* path);
//
//    const struct ItemTemplate* GetSaleItem(uint32 saleID, uint32 itemID);
//
//private:
//    typedef std::vector<const struct ItemTemplate*> SaleList;
//    std::map<uint32, SaleList> m_sale_map; 
};

#endif
