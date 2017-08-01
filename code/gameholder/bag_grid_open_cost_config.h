//背包格开启花费
#ifndef __BAG_GRID_OPEN_COST_CONFIG_H__
#define __BAG_GRID_OPEN_COST_CONFIG_H__

class BagGridOpenCostConfig
{
public:
    DECLARE_SINGLETON(BagGridOpenCostConfig)

    bool isValidGridIdx(uint32 idx) const;

    uint32 GetGridMaxIdx() const;

    //range:[beginIdx, endIdx]
    uint32 GetGridOpenCost(uint32 beginIdx, uint32 endIdx) const;

    void Clear();

    bool LoadConfig(const char* path);

    BagGridOpenCostConfig();

    virtual ~BagGridOpenCostConfig();

private:
    typedef struct _GridCostData
    {
        uint32 idx;
        uint32 moneyPointCost;
    } GridCostData;

    typedef std::vector<GridCostData> GridCostDataList;

    GridCostDataList mGridCostDataList;
};

#endif