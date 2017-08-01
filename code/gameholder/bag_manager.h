/*
 *	背包管理器
 */
#ifndef bag_manager_h__
#define bag_manager_h__

#include "item_factory.h"

class Player;
class ItemManager;

class BagManager
{
public:
    friend class ItemManager;
    ItemManager* GetItemMgr();

public:
    BagManager(Player* player);
    ~BagManager();

    void    Clear();

    void    LoadBagInfo(const ITEM_INFO& info);
    void    FillBagInfo(ITEM_INFO& info);

    void    OnRecv(uint32 msgID, void* data);

    // 其他模块进入背包
    uint16  AddItem(uint32 reason, ItemBase* pItem, int32 pos = -1);
    void    SubItem(uint32 reason, int32 pos, int32 num = 1);

    int32   GetEmptyGridNum() const; 
    int32   FindEmptyGrid() const;
    bool    IsEmptyGrid(int32 pos) const;
    bool    IsValidGrid(int32 pos) const;
    bool    IsCanStack(ItemBase * pSrc, ItemBase * pDst) const;

    const ItemGrid& GetItemGrid() const { return m_ItemGrid; }

    ItemBase* GetItemByPos(int32 pos);
    ItemBase* GetItemById(int32 id);
    void GetItemListById(int32 id, std::vector<ItemBase*>& outList);

protected:
    ItemGrid& GetItemGrid() { return m_ItemGrid; }

private:
    void    AddToGrid(ItemBase * pItem, bool bSwap = false);
    void    StackItem(ItemBase * pSrc, ItemBase * pDst);
    uint16  SwapItem( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos );
    uint16  SellItem(int32 pos);

    bool SortItem(ItemBase* pItemA, ItemBase* pItemB);

private:
    void OnItemInfoReq(void* data);
    void OnItemSortReq(void* data);

    void SendItemInfo();

private:
    ItemGrid        m_ItemGrid;                 // 栏位的映射

    Player*         m_player;
};

#endif
