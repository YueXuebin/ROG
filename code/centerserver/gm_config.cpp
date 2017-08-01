#include "centerserver_pch.h"
#include "config_reader.h"
#include "gm_config.h"


IMPLEMENT_SINGLETON(GMConfig)

bool GMConfig::LoadConfig(const char *fileName)
{
    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(fileName))
    {
        CnError("Load %s file failed\n", fileName);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for (TiXmlElement *pElement = xmlRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        if (Crown::SDStrcmp(pElement->Value(), "white_table") == 0)
        {
            for (TiXmlElement *pWhiteTableItem = pElement->FirstChildElement(); pWhiteTableItem; pWhiteTableItem = pWhiteTableItem->NextSiblingElement())
            {
                std::string ip;
                ConfigReader::Read(pWhiteTableItem, "address", ip);
                m_WhiteTable.insert(ip);
            }
        }
        else
        {
            CnError("gm_config.xml:%s not read", pElement->Value());
        }

    }

    return true;
}

bool GMConfig::IsPermitAddr(const char *ip)
{
    if (m_WhiteTable.find(ip) != m_WhiteTable.end())
        return true;

    return false;
}