#ifndef package_manager_h__
#define package_manager_h__

class Player;

class PackageManager
{
public:
    PackageManager(Player* player);
    ~PackageManager(void);

     void Clear();
     void OnRecv(uint32 msgID, void* data);

     void LoadPackageInfo(const PACKAGE_BAG& info);
     void FillPackageInfo(PACKAGE_BAG& info);
    
     const TREASURE_PACKAGE_ITEM* GetPackageItem(int32 packageid, uint32 type);                                 //获取奖励物品 type 抽奖包类型

     const TREASURE_PACKAGE_ITEM* GetTreasurePackageItem(const TREASURE_PACKAGE_INFO* package, uint32 type);    //获取奖励物品

    void RandomPackageItem(int package_id, std::vector<REWARD_ITEM>* itemList, uint32 type);                    // 随机奖励
private:
    Player*         m_player;                                                                                   ///< 角色指针

    const TREASURE_PACKAGE_ITEM* RandomPackageHandle(const TREASURE_PACKAGE_INFO* package, uint32 type);        /// 获取包裹物品
    bool PackageJudgeTimeHandle(const TREASURE_PACKAGE_ITEM* item, int32 packageId, int32 type);
    bool PackageSetTimeHandle(const TREASURE_PACKAGE_ITEM* item,int32 packageId, int32 type);


    PACKAGE_INFO* GetPackageInfoToType( int32 type );
    PACKAGE_COUNT_INFO* GetPackageInfoCount( uint32 type, uint32 id);
    PACKAGE_TIME_INFO* GetPackageInfoTime( uint32 packageid, uint32 itemid, uint32 type);

    PACKAGE_BAG m_packageMap;
};


#endif