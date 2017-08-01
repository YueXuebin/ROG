/*
*   消耗管理
*/
#ifndef expend_manager_h__
#define expend_manager_h__

class Player;

class ExpendManager
{
    friend class Player;
public:
    ExpendManager(Player* player);
    ~ExpendManager();

    uint16  ExpendItem(const REWARD_TABLE& expendTable);                        // 消耗物品 返回错误码

    REWARD_TABLE MergeRewardTable(const REWARD_TABLE& expendTable);             // 合并同类物品

    uint16  CheckExpendItem(const REWARD_TABLE& expendTable);                   // 检查消耗
    uint16  CheckExpend(int32 type, int32 num, uint32 itemID = 0);              // 检查消耗类型
    uint16  CheckExpendItemByType(const REWARD_ITEM& expendItem);               // 根据类型检查不同消耗

    uint16  CheckGamePoint(int32 number);
    uint16  CheckSoulPoint(int32 number);
    uint16  CheckBagItem(uint32 id, int32 number);
    uint16  CheckHonorPoint(int32 number);
    uint16  CheckVigor(int32 number);
    uint16  CheckGuildPoint(int32 number);
    uint16  CheckBattleSoul(int32 number);
    uint16  CheckStarSoul(int32 number);
    uint16  CheckBindMoneyPoint(int32 number);
    uint16  CheckIncarnationPoint(int32 number);
    uint16  CheckMoneypoint(int32 num);

    void    SubItem(const REWARD_TABLE& expendTable);                           // 消耗物品
    void    SubExpend(int32 type, int32 num, int32 reason, uint32 itemID = 0);  // 删除消耗
    void    SubItemByType(const REWARD_ITEM& expendIt, int32 reason);           // 根据类型消耗不同物品

    void    SubGamePoint(int32 number, int32 reason);
    void    SubSoulPoint(int32 number, int32 reason);
    void    SubStarPoint(int32 number, int32 reason);
    void    SubBagItem(uint32 id, int32 number, int32 reason);
    void    SubHonorPoint(int32 number, int32 reason);
    void    SubVigor(int32 number);
    void    SubGuildPoint(int32 number, int32 reason);
    void    SubBattleSoulPoint(int32 number);
    void    SubBindMoneyPoint(int32 number, int32 reason);
    void    SubIncarnationPoint( int32 number, int32 reason );
private:
    Player* m_player;
};

#endif // expend_manager_h__
