/*
*   NPC购买
*/
#ifndef sale_config_h__
#define sale_config_h__

#include <vector>

typedef struct SaleCondition
{
	int32 min_vip_level;   //最低vip等级限制
	uint32 regular_type;	//regular类型限制

	void Clear()
	{
		min_vip_level = 0;
		regular_type = 0;
	}
}SaleCondition;

class SaleConfig
{
public:
    SaleConfig();
    virtual ~SaleConfig();

    DECLARE_SINGLETON(SaleConfig)

    bool LoadConfig(const char* path);

    //const struct ItemTemplate* GetSaleItem(uint32 saleID, uint32 itemID);

    const struct SaleCondition* GetSaleCondition(uint32 saleID);

private:
    //typedef std::vector<const struct ItemTemplate*> SaleList;
    //std::map<uint32, SaleList> m_sale_map;

    typedef std::map<uint32, struct SaleCondition> SaleConditionMap;
    SaleConditionMap m_sale_condition_map;
};

#endif
