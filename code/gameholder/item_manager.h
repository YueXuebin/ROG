/*
 *	背包管理器
 */
#ifndef item_manager_h__
#define item_manager_h__

#include "item_base.h"
#include "config_reader.h"

class Player;
class BagManager;
class StoreManager;
class EquipManager;
class SkillInfoManager;
class SoldierTalentManager;
class ItemController;

class ItemManager
{
public:
    friend class BagManager;
    friend class StoreManager;
    friend class EquipManager;
    friend class SkillInfoManager;
    friend class SoldierTalentManager;
    friend class GemManager;
    friend class EquipStrengthenManager;
    friend class SkillStrengthenManager;
    friend class ItemController;

    BagManager* GetBagMgr();
    StoreManager* GetStoreMgr();
    EquipManager* GetEquipMgr();
    SkillInfoManager* GetSkillInfoMgr();
    EquipManager* GetSoldierEquipMgr();
    SkillInfoManager* GetSoldierSkillInfoMgr();
    SoldierTalentManager* GetSoldierTalentMgr();
    ItemController* GetItemController();     

public:
    ItemManager(Player* player);
    ~ItemManager();

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void Update();
    void ForceUpdate();

    bool IsDirty() const { return m_bIsDirty; }
    void SetDirty( bool b) { m_bIsDirty = b; }

    ItemBase* GetPlaceItem(uint8 place, int32 pos);
    ItemBase* GetPlaceItemById(uint8 place, uint32 itemId);                                     // 根据物品id获取背包里面的物品，可能返回null
    void GetPlaceItemListById(uint8 place, uint32 itemId, std::vector<ItemBase*>& outList);     // 根据物品id获取背包里面的物品列表,可能返回Null

    uint16 CheckDstPlace(ItemBase* pSrc, uint8 dstPlace, int32 dstPos, int32 num = -1);
    uint32 GetItemNum(int32 itemId);
    bool CheckItemNum(int32 itemId, int32 allNum);                                              // 检查材料数量

    void FindDstPlace(ItemBase* pSrc, uint8 dstPlace, int32& dstPos);

    void RemovePlaceItem(uint8 place, int32 pos, ItemBase* pItem);              // 内部调用，安装拆除用
    void RemoveItemNum(uint32 reason, int32 num, uint8 place, uint32 pos);      // 根据物品位置删除指定数量的物品，目前只有从背包删除
    bool RemoveItemNumByItemId(uint32 reason, int32 itemId, int32 allNum);      // 根据物品ID从背包里面删除指定的数量

    uint16 SellPlaceItem(uint8 place, int32 pos);

    uint16 MoveItem(ItemBase* pSrc, uint8 dstPlace, int32 dstPos);

    uint16 SwapItem(ItemBase* pSrc, ItemBase* pDst);

    uint16 UninstallToBag(uint8 srcPlace, int32 srcPos, int32 dstPos);

    void DestoryItem(ItemBase* pItem);
public:
    uint16 AddToBag(uint32 reason, ItemBase* pItem);

    // RewardTable相关
	ERROR_CODE AddRewardTable(const REWARD_TABLE* table);
    bool CanAddRewardTable(const REWARD_TABLE* table, int32* needBagNum = NULL);
    bool CanAddToBag(const std::map<uint32, uint32>* items, int32* needBagNum);

private:
    void OnOptItemReq(void* data);

    uint16 OnOptItemMove(OPT_ITEM_INFO& info);
    uint16 OnOptItemSell(OPT_ITEM_INFO& info);
    uint16 OnOptItemUse(OPT_ITEM_INFO& info);

private:
    void NtfOptItem(const OPT_ITEM_INFO& info);
    void NtfOptItemAdd(ItemBase* pItem, int32 num = -1);
    void NtfOptItemUpdate(ItemBase* pItem, int32 num = -1);
    void NtfOptItemSub(ItemBase* pItem, int32 num = -1);

    void Notify(OPT_ITEM_INFO_NTF& msg);

private:
    void AddToItemList(ItemBase * pItem);

    void AddRewardItemByType(int32 type, int32 value);    // 添加非物品类奖励
    void AddRewardItemToBag(const REWARD_ITEM* rewardItem);     // 添加奖励物品到背包

private:
    Player*             m_player;

    ItemList            m_AllItemList;                 // 栏位的映射

    bool                m_bIsDirty;
    OPT_ITEM_INFO_NTF   m_OptItemInfoNtf;
};

#endif
