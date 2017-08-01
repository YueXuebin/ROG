#include "gameholder_pch.h"
#include "sign_makeup_config.h"

IMPLEMENT_SINGLETON(SignMakeupConfig)

SignMakeupConfig::SignMakeupConfig()
{
    
}

SignMakeupConfig::~SignMakeupConfig()
{
    
}

bool SignMakeupConfig::LoadConfig(const char* path)
{
    SignMakeupTemplate signMakeup;

    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load sign_makeup file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* costEle = xmlRoot->FirstChildElement(); costEle; costEle = costEle->NextSiblingElement())
    {
        if(strcmp(costEle->Value(), "row") == 0)
        {
            signMakeup.time = SDAtoi(costEle->Attribute("time"));
            signMakeup.money_point = SDAtoi(costEle->Attribute("money_point"));
            m_sign_makeups.push_back(signMakeup);
        }
    }

    return true;
}

int32 SignMakeupConfig::GetMoneyPointByMakeupTime(int32 time)
{
    std::vector<SignMakeupTemplate>::iterator signMakeupIt;
    for (signMakeupIt = m_sign_makeups.begin(); signMakeupIt != m_sign_makeups.end(); signMakeupIt++)
    {

        if(signMakeupIt->time == time)
        {
            return signMakeupIt->money_point;
        }
    }

    uint32 len = (uint32)m_sign_makeups.size();
    if(len > 0)
    {
        return m_sign_makeups[len - 1].money_point;
    }

    return 0;
}

