/*
 *	游戏性能默认配置项"system_setting.xml"
 */
#ifndef system_setting_config_h__
#define system_setting_config_h__

//struct ItemTemplate;

class SystemSettingConfig
{
public:
    SystemSettingConfig();
    virtual ~SystemSettingConfig();

    DECLARE_SINGLETON(SystemSettingConfig)

    void Clear();

    bool LoadConfig(const char* path);

    // 游戏背景音量
    uint32 GetVolume() { return m_volume; }

    // 游戏音效音量
    uint32 GetSeVolume() { return m_se_volume; }

    // 显示玩家
    uint32 GetShowPlayer() { return m_show_player; }

    // 显示宠物
    uint32 GetShowPet() { return m_show_pet; }

    // 显示阴影
    uint32 GetShowShadow() { return m_show_shadow; }

    // 显示粒子
    uint32 GetShowParticle() { return m_show_particle; }

    // MIP map
    uint32 GetMipmap() { return m_mipmap; }

    // 分辨率
    uint32 GetFullscreen() { return m_fullscreen; }

    // 坐骑历史
    uint32 GetRidePet() { return m_ride_pet; }

    //高级灯光是否开启
    int32 GetLight()    {return m_light; }

    //动态光照是否开启
    int32 GetBeam()     {return m_beam;}

    //是高级效果还是低级效果
    int32 GetEffectLevel() { return m_effect_level;}

    //雾是否开启
    int32 GetFog() {return m_fog;}

    //帧率
    int32 GetFrame() {return m_frame;}

private:
    uint32      m_volume;
    uint32      m_se_volume;
    uint32      m_show_player;
    uint32      m_show_pet;
    uint32      m_show_shadow;
    uint32      m_show_particle;
    uint32      m_mipmap;
    uint32      m_fullscreen;
    uint32      m_ride_pet;
    int32       m_beam;
    int32       m_light;
    int32       m_effect_level;
    int32       m_fog;
    int32       m_frame;
};

#endif
