// 圣剑系统
#ifndef sword_manager_h__
#define sword_manager_h__

#include "sword_config.h"

class Player;

class SwordManager
{
public:
    SwordManager(Player* player);
    ~SwordManager(void);

    void Clear();

    void LoadInfo(const SWORD_NEW_INFO& info);
    void FillInfo(SWORD_NEW_INFO& info);

    void OnRecv(uint32 msgID, void* data);

    void onSwordInfoReq();                      // 信息请求
    void onUpgradeReq(void* data);              // 升星请求 (支持自由配星 每一阶的星数可以自由配置)
    void onAdvanceReq(void* data);              // 升阶请求 (支持自由配阶 阶数可以自由配置)
    void onActivateReq(void* data);             // 激活圣剑请求
    void onUnlockReq(void* data);               // 解锁请求
    void onEquipReq(void* data);                // 装备请求

    void CalSwordProp();

    void onUnlock(int32 swordID, int32 lockID);

    SWORD_ONE_INFO* GetSwordOneInfo(uint32 swordID);

    int32 GetEquipSwordID(){return m_sword_info.equip_id;}
private:
    void InitProList();                                         // 初始化

    bool ActivitySword(uint32 swordID);                         // 激活圣剑
    bool UpStar(SWORD_ONE_INFO* swordOneInfo, int starNum);     // 升星

    void SendSwordUpgradeAck(uint16 error_code, SWORD_ONE_INFO* swordOneInfo);
    void SendSwordAdvanceAck(uint16 error_code, SWORD_ONE_INFO* swordOneInfo);
    void SendSwordActivateAck(uint16 error_code, uint32 swordID);
    void SendSwordUnlock(int32 errCode, SWORD_ONE_INFO* pSwordInfo);
private:
    Player * m_player;

    SWORD_NEW_INFO m_sword_info;
};
#endif // sword_manager_h__