#include "gameholder_pch.h"
#include "protect_config.h"

IMPLEMENT_SINGLETON(ProtectConfig)

ProtectConfig::ProtectConfig(void)
{
}

ProtectConfig::~ProtectConfig(void)
{
}

bool ProtectConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    ProtectMoney listItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* saleEle = xmlRoot->FirstChildElement(); saleEle; saleEle = saleEle->NextSiblingElement())
    {
        listItem.Clear();
        listItem.count = SDAtoi(saleEle->Attribute("count"));
        listItem.money = SDAtoi(saleEle->Attribute("money"));

        m_protectMoney.push_back(listItem);

    }

    return true;
}

int32 ProtectConfig::GetProtectMoney( int32 count )
{
    for (ProtectMoneyList::iterator item = m_protectMoney.begin(); item != m_protectMoney.end(); item++)
    {
        if(count == item->count)
        {
            return item->money;
        }
    }

    ProtectMoney endit = m_protectMoney.back();

    return endit.money;
}
