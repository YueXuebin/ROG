#ifndef player_config_manager_h__
#define player_config_manager_h__

#include "levelup_config.h"
/*
 *	ÕÊº“œ‡πÿ≈‰÷√
 */
class PlayerConfigManager
{
public:
    PlayerConfigManager();
    virtual ~PlayerConfigManager();

    DECLARE_SINGLETON(PlayerConfigManager)

public:
    bool LoadConfig(const char* path);

    const LEVELUP_ROOT_LEVEL_STRUCT* GetPropConfigByLevel(int32 level) const;
    int32   GetPropByLevel(int32 level, int32 propertyId) const;

    int32   GetMaxExp(int32 level) const;

private:
     std::map<uint32, const LEVELUP_ROOT_LEVEL_STRUCT*>    m_LevelupConfigMap;       // <level, level data>


};


#endif // player_config_manager_h__
