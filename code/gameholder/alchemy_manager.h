/*
*   ���������Ʒ
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
    void OnMakeItem(void* data);                  // ������Ʒ
    bool MakeEquip(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                       // ����װ��
    bool MakeMaterial(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                    // ���ɲ���
    bool MakeConsume(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                     // ��������Ʒ
    bool MakeStone(const ALCHEMY_ROOT_ALCHEMY_STRUCT* alchemyStruct, uint32 pNum, ALCHEMY_MAKE_INFO_ACK* sendMsg);                       // ����ʯͷ

    bool checkItemNum(int32 itemId, int32 allNum);                       // ����������
    bool removeItemForMake(int32 itemId, int32 allNum);                       //ɾ������

private:
     Player*             m_player;
};


#endif // manor_make_manager_h__
