#ifndef RECHARGE_MANAGER_H
#define RECHARGE_MANAGER_H

#include "task_define.h"

class Player;
class TaskManager;

// ==================================================
/// 角色的充值管理系统
// ==================================================
class RechargeManager
{
public:
    RechargeManager(Player* player);
    ~RechargeManager();

    void LoadRechargeInfo(const PLAYER_RECHARGE_INFO& info);                // 加载充值信息

    int32 GetTotalRecharge(RoleTask* pTask = NULL);                         // 获得节日充值总数，或充值总数
    int32 GetTodayRecharge(bool isSingle = true);                           // 获得当日充值数

    void SetRechargeCount(int32 money);                                     // 设置充值总数，仅用于测试，其他不能用
    int32 GetRechargeCount();                                               // 返回从开服到现在的充值总值

    uint32 GetTotalRechargeTime();                                          // 获得最后一次充值时间

private:
    Player*                 m_player;                                       // 玩家
    PLAYER_RECHARGE_INFO    m_recharge_info;
    bool b_exe;
};

#endif
