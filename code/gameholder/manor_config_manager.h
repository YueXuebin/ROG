/*
*   ��������������
*/
#ifndef territory_config_manager_h__
#define territory_config_manager_h__

#include "territory_make_config.h"

class TerritoryConfigManager
{
public:
    TerritoryConfigManager(void);
    ~TerritoryConfigManager(void);

    DECLARE_SINGLETON(TerritoryConfigManager)

    bool LoadConfig(const char* path);

    const std::vector<TERRITORY_MAKE_ROOT_TERRITORY_MAKE_ITEM_STRUCT>* FindManorMakeItem(uint32 id) const;       //���ݹ�ʽid�������������Ҫ���ĵ���Ʒ�б�
    const TERRITORY_MAKE_ROOT_TERRITORY_MAKE_STRUCT* FindManorMakeDstItem(uint32 id) const;                       //���ݹ�ʽid������������ɵ���Ʒ��Ϣ
};

#endif // territory_config_manager_h__
