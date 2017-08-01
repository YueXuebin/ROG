/*
*   宝石
*/
#ifndef gem_manager_h__
#define gem_manager_h__

class Player;
class ItemManager;

class GemManager
{
public:
    friend class ItemManager;
    ItemManager* GetItemMgr();

public:
    GemManager(Player* player);
    ~GemManager();

    void OnRecv(uint32 msgID, void* data);

private:
    void SocketGem(void* data);   // 镶嵌
    void UnsocketGem(void* data); // 拆除
    void FuseGem(void* data);     // 融合
    void EvoluteGem(void* data);  // 幻色
    void AddHole(void* data);     // 打孔

    void CalcProperty(uint32 equipPlace); // 重新计算人物属性

    Player* m_Player;
};

#endif