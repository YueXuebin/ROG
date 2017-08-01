/*
*   物品分解
*/
#ifndef decompose_manager_h__
#define decompose_manager_h__

class Player;
class ItemManager;
class BagManager;
class ItemBase;
class ItemMaterial;

class DecomposeManager
{
public:
    ItemManager* GetItemMgr();

public:
    DecomposeManager(Player* player);
    ~DecomposeManager();

    void OnRecv(uint32 msgID, void* data);

    static void MergeMaterial(std::vector<ITEM_MATERIAL>& itemList, const ITEM_MATERIAL& item);

private:
    void OnDecomposeReq(void* data);
    void OnDecomposeAck(uint16 errCode, std::set<ItemBase*>& items, REWARD_TABLE& rewardTable);

private:
     Player*             m_player;
};


#endif // decompose_manager_h__
