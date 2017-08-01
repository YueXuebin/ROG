#ifndef potion_use_manager_h__
#define potion_use_manager_h__

class Player;

/*
 *	药剂使用管理器
 */
class PotionManager
{
public:
    PotionManager(Player* player);
    ~PotionManager();

    void LoadPotionInfo(const POTION_INFO& info);                     // 加载药水设置信息
    void FillPotionInfo(POTION_INFO& info) const;                     // 填充药水设置信息

    void OnRecv(uint32 msgID, void* data);

    uint32 GetPotionIDBySlotID(uint32 slotID);

private:
    void OnSetPotion(void* data);

private:
    Player*             m_player;
    POTION_INFO         m_PotionInfo;

};


#endif // potion_use_manager_h__
