/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载虚空幻境挂机配置信息
******************************************************************************/

#ifndef void_field_setting_config_h__
#define void_field_setting_config_h__

#include <vector>


class VoidFieldSettingConfig
{
public:
    VoidFieldSettingConfig();
    virtual ~VoidFieldSettingConfig();

    DECLARE_SINGLETON(VoidFieldSettingConfig)

    void Clear();

    bool LoadConfig(const char* path);

    // 原地挂机
    uint32 GetCurrentHook() { return m_current_hook; }

    // 自动使用复活纹章
    uint32 GetAutoUseRevival() { return m_auto_use_revival; }

    // 自动购买复活纹章
    uint32 GetAutoBuyRevival() { return m_auto_buy_revival; }

    // 原地复活
    uint32 GetCurrentRevival() { return m_current_revival; }

    // 断线重连进入虚空
    uint32 GetReconnectOnLoss() { return m_reconnect_on_loss; }

private:
    uint32      m_current_hook;
    uint32      m_auto_use_revival;
    uint32      m_auto_buy_revival;
    uint32      m_current_revival;
    uint32      m_reconnect_on_loss;
};

#endif
