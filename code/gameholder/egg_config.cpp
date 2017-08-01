#include "gameholder_pch.h"
#include "egg_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(EggConfig)

EggConfig::EggConfig(void)
{
}

EggConfig::~EggConfig(void)
{
    Clear();
}

bool EggConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load egg file: %s failed\n", path);
        return false;
    }

    TiXmlElement *xmlRoot = xmlDoc.RootElement();
    if (!xmlRoot)
    {
        return false;
    }

    for (TiXmlElement *elem = xmlRoot->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
        EGG_INFO_CONFI item;
        ConfigReader::Read(elem, "type", item.type);
        ConfigReader::Read(elem, "item_id", item.item_id);
        ConfigReader::Read(elem, "need_num", item.need_num);
        ConfigReader::Read(elem, "package_id", item.package_id);
        m_eggList.push_back(item);
    }
    return true;
}  
 
void EggConfig::Clear()
{
    m_eggList.clear();
}

const EGG_INFO_CONFI* EggConfig::GetEggItemByType( uint32 type )
{
    for (EGG_ITEM_LIST::const_iterator el = m_eggList.begin(); el != m_eggList.end(); el++)
    {
        if(el->type == type)
        {
            return &(*el);
        }
    }
    return NULL;
}
