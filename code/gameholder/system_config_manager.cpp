#include "gameholder_pch.h"
#include "system_config_manager.h"



IMPLEMENT_SINGLETON(SystemConfigManager)

SystemConfigManager::SystemConfigManager()
{
    GAME_PARAM_ENTRY::CreateInstance();
    DECOMPOSE_ENTRY::CreateInstance();
    RANDOM_PACKAGE_ENTRY::CreateInstance();
}

SystemConfigManager::~SystemConfigManager()
{
    GAME_PARAM_ENTRY::DestroyInstance();
    DECOMPOSE_ENTRY::DestroyInstance();
    RANDOM_PACKAGE_ENTRY::DestroyInstance();
}

uint32 SystemConfigManager::ParseStringCD(const std::string& str_cd)
{
    if (str_cd.empty())
    {
        return 0;
    }

    uint32 d(0);
    uint32 h(0);
    uint32 m(0);

    std::string temp("");

    for (size_t i=0; i<str_cd.length(); ++i)
    {
        if (str_cd[i] >= '0' && str_cd[i] <= '9')
        {
            temp += str_cd[i];
        }
        else if (!temp.empty())
        {
            switch (str_cd[i])
            {
            case 'd':
            case 'D':
                {
                    d = Crown::SDAtou(temp.c_str());
                }
                break;
            case 'h':
            case 'H':
                {
                    h = Crown::SDAtou(temp.c_str());
                }
                break;
            case 'm':
            case 'M':
                {
                    m = Crown::SDAtou(temp.c_str());
                }
                break;
            }

            temp = "";
        }
    }

    return d * 24 * 60 *60 + h * 60 * 60 + m * 60;
}

bool SystemConfigManager::IsRecord(const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pStruct)
{
    if (!pStruct)
    {
        return false;
    }

    for (size_t i=0; i<pStruct->item_list.size(); ++i)
    {
        if (pStruct->item_list[i].get_times > 0)
        {
            return true;
        }
    }

    return false;
}

bool SystemConfigManager::LoadConfig( const char* path )
{
    if (GAME_PARAM_ENTRY::Instance()->LoadConfig(path))
    {
        for(std::vector<GAME_PARAM_ROOT_SYSTEM_STRUCT>::const_iterator iter = GAME_PARAM_ENTRY::Instance()->GetStruct().system_list.begin();
            iter != GAME_PARAM_ENTRY::Instance()->GetStruct().system_list.end(); iter++)
        {
            m_GameParamConfigMap[iter->name] = &(*iter);
        }
    }

    if (DECOMPOSE_ENTRY::Instance()->LoadConfig(path))
    {
        const DECOMPOSE_ROOT_FORMULA_STRUCT* pStruct(NULL);
        DecomposeConfigMap::iterator mIter = m_DecomposeConfigMap.end();

        const DECOMPOSE_ROOT_STRUCT & RootStruct = DECOMPOSE_ENTRY::Instance()->GetStruct();
        for (std::vector<DECOMPOSE_ROOT_FORMULA_STRUCT>::const_iterator cIter = RootStruct.formula_list.begin();
            cIter != RootStruct.formula_list.end(); ++cIter)
        {
            pStruct = &(*cIter);

            mIter = m_DecomposeConfigMap.find(pStruct->item_type);
            if (m_DecomposeConfigMap.end() == mIter)
            {
                QualityFormulaMap FormulaMap;
                FormulaMap[pStruct->item_quality] = pStruct;
                m_DecomposeConfigMap[pStruct->item_type] = FormulaMap;
            }
            else
            {
                QualityFormulaMap & FormulaMap = mIter->second;
                FormulaMap[pStruct->item_quality] = pStruct;
            }
        }
    }

    if (RANDOM_PACKAGE_ENTRY::Instance()->LoadConfig(path))
    {
        const RANDOM_PACKAGE_ROOT_STRUCT & RootStruct = RANDOM_PACKAGE_ENTRY::Instance()->GetStruct();
        for (std::vector<RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT>::const_iterator cIter = RootStruct.package_list.begin();
            cIter != RootStruct.package_list.end(); ++cIter)
        {
            m_RandomPackageMap[cIter->id] = &(*cIter);
        }
    }

    return true;
}

int32 SystemConfigManager::GetMaxLevel()
{
    GameParamConfigMap::const_iterator cIter = m_GameParamConfigMap.find("max_level");
    if (m_GameParamConfigMap.end() == cIter)
    {
        return 0x7f;
    }

    return cIter->second->value;
}

int32 SystemConfigManager::GetGameConfigValue(const std::string& name)
{
    GameParamConfigMap::const_iterator cIter = m_GameParamConfigMap.find(name);
    if (m_GameParamConfigMap.end() == cIter)
    {
        return 0;
    }

    return cIter->second->value;
}

const DECOMPOSE_ROOT_FORMULA_STRUCT* SystemConfigManager::GetDecomposeFormula(const uint32 item_type, const uint32 item_quality) const
{
    DecomposeConfigMap::const_iterator cIter = m_DecomposeConfigMap.find(item_type);
    if (m_DecomposeConfigMap.end() == cIter)
    {
        return NULL;
    }

    QualityFormulaMap::const_iterator ccIter = cIter->second.find(item_quality);
    if (cIter->second.end() == ccIter)
    {
        return NULL;
    }

    return ccIter->second;
}

const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* SystemConfigManager::GetRandomPackage(const uint32 id) const
{
    RandomPackageMap::const_iterator cIter = m_RandomPackageMap.find(id);
    if (m_RandomPackageMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}
