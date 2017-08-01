/*
 *	玩家的商城服务管理器
 */
#ifndef shop_service_manager_h__
#define shop_service_manager_h__

class Player;

class ShopServiceManager
{
public:
    ShopServiceManager(Player* player);
    ~ShopServiceManager();

    uint32 GetEquipWashUpLockServiceId(uint32 lockCount);
    uint32 GetEquipGemHoleChangeColorLockServiceId(uint32 lockCount);
    uint32 GetFusionAutoFillSkillStoneServiceId(uint32 quality, uint32 buyNum);

private:
    Player*                     m_player;
};

#endif // shop_service_manager_h__
