#ifndef tower_defence_config_h__
#define tower_defence_config_h__

class TowerDefenceConfig
{
public:
    TowerDefenceConfig();
    ~TowerDefenceConfig();

    DECLARE_SINGLETON(TowerDefenceConfig)

    bool LoadConfig(const char* path);

private:
};


#endif // tower_defence_config_h__
