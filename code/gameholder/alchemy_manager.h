/*
*   领地制作物品
*/
#ifndef alchemy_manager_h__
#define alchemy_manager_h__

#include "item_base.h"
#include "alchemy_config_manager.h"
#include "item_equip.h"

class Player;
class ItemManager;
class BagManager;

class AlchemyManager
{
public:
    ItemManager* GetItemMgr();
    BagManager*  GetBagMgr();
public:
    AlchemyManager(Player* player);
    ~AlchemyManager();

    void OnRecv(uint32 msgID, void* data);

private:
    void OnMakeItem(void* data);                  // 制作物品
    bool MakeEquip(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                       // 生成装备
    bool MakeMaterial(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                    // 生成材料
    bool MakeConsume(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                     // 生成消耗品
    bool MakeStone(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                       // 生成石头

    bool checkItemNum(int32 itemId, int32 allNum);                       // 检查材料数量
    bool removeItemForMake(int32 itemId, int32 allNum);                       //删除材料

private:
     Player*             m_player;
};


#endif // manor_make_manager_h__
