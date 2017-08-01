/*
*   强化装备
*/
#ifndef equip_strengthen_manager_h__
#define equip_strengthen_manager_h__

class Player;

class ItemEquip;

class EquipStrengthenManager
{

public:
    friend class ItemManager;
    ItemManager* GetItemMgr();

public:
    EquipStrengthenManager(Player* player);
    ~EquipStrengthenManager();

    void OnRecv(uint32 msgID, void* data);

private:
    void OnQualityUp(void* data);       // 装备升品
    void OnWashUp(void* data);          // 装备洗练
    void OnWashUpUseOld(void* data);    // 装备洗练保留原数值
    void OnWashUpUseNew(void* data);    // 装备洗练确认替换
    void OnStarUp(void* data);          // 装备升星
    void OnEnchant(void* data);         // 装备附魔 

    void SendStrengthenAck(uint32 msgID, ITEM_STRENGTHEN_ACK* sendMsg, ItemEquip* pEquip, bool needCalcProperty); // 发送强化结果给客户端
    void CalcProperty(uint32 equipPlace);   // 重新计算人物属性


private:
    Player*             m_player;
};

#endif // equip_strengthen_manager_h__
