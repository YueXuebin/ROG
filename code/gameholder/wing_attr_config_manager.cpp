#include "gameholder_pch.h"
#include "wing_attr_config_manager.h"
#include "wing_attr_config.h"

IMPLEMENT_SINGLETON(WingAttrConfigManager)

WingAttrConfigManager::WingAttrConfigManager(void)
{
     WING_ATTR_ENTRY::CreateInstance();
}

WingAttrConfigManager::~WingAttrConfigManager(void)
{
    WING_ATTR_ENTRY::DestroyInstance();
}

bool WingAttrConfigManager::LoadConfig(const char* path)
{
    // ¼ÓÔØ³á°òÊôÐÔÅäÖÃ
    if (WING_ATTR_ENTRY::Instance()->LoadConfig(path))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const WING_ATTR_ROOT_WING_STRUCT* WingAttrConfigManager::FindWing(uint32 id) const
{

    const WING_ATTR_ROOT_STRUCT& RootStruct = WING_ATTR_ENTRY::Instance()->GetStruct();

    for (std::vector<WING_ATTR_ROOT_WING_STRUCT>::const_iterator cIter = RootStruct.wing_list.begin(); cIter != RootStruct.wing_list.end(); ++cIter)
    {

        if (cIter->id == id)
        {
            return &(*cIter);
        }
    }

    return NULL;
}

const WING_ATTR_ROOT_WING_ITEM_STRUCT* WingAttrConfigManager::FindWingItem (uint32 id, uint32 level) const
{
    const WING_ATTR_ROOT_STRUCT& RootStruct = WING_ATTR_ENTRY::Instance()->GetStruct();

    for (std::vector<WING_ATTR_ROOT_WING_STRUCT>::const_iterator cIter = RootStruct.wing_list.begin(); cIter != RootStruct.wing_list.end(); ++cIter)
    {

        if (cIter->id == id)
        {
            for (std::vector<WING_ATTR_ROOT_WING_ITEM_STRUCT> ::const_iterator cIter2 = cIter->item_list.begin(); cIter2 != cIter->item_list.end(); ++cIter2)
            {
                if(cIter2->level == level)
                {
                    return &(*cIter2);
                }
            }
        }
    }

    return NULL;
}