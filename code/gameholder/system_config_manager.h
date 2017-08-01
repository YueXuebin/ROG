#ifndef system_config_manager_h__
#define system_config_manager_h__

#include "game_param_config.h"
#include "decompose_config.h"
#include "random_package_config.h"
/*
 *	系统相关配置
 */
typedef std::map<std::string, const GAME_PARAM_ROOT_SYSTEM_STRUCT*> GameParamConfigMap;
typedef std::map<uint32, const DECOMPOSE_ROOT_FORMULA_STRUCT*> QualityFormulaMap;
typedef std::map<uint32, QualityFormulaMap> DecomposeConfigMap;
typedef std::map<uint32, const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT*> RandomPackageMap;

class SystemConfigManager
{
public:
    SystemConfigManager();
    virtual ~SystemConfigManager();

    DECLARE_SINGLETON(SystemConfigManager)

public:
    static uint32 ParseStringCD(const std::string& str_cd);
    static bool IsRecord(const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pStruct);

public:
    bool LoadConfig(const char* path);

    // 玩家最大等级
    int32 GetMaxLevel();

    const DecomposeConfigMap& GetDecomposeConfigMap() const {return m_DecomposeConfigMap;}

    const DECOMPOSE_ROOT_FORMULA_STRUCT* GetDecomposeFormula(const uint32 item_type, const uint32 item_quality) const;

    const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* GetRandomPackage(const uint32 id) const;

    int32 GetGameConfigValue(const std::string& name);

private:
    GameParamConfigMap  m_GameParamConfigMap;       // <name, data>
    DecomposeConfigMap  m_DecomposeConfigMap;
    RandomPackageMap    m_RandomPackageMap;
};


#endif // system_config_manager_h__
