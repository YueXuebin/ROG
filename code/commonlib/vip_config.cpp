#include "commonlib_pch.h"
#include "vip_config.h"
IMPLEMENT_SINGLETON(VipConfig)


VipConfig::VipConfig()
{

}

VipConfig::~VipConfig()
{

}

bool VipConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load vip file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    VipTmplVert listviptemp;
    for(TiXmlElement* vipEle = xmlRoot->FirstChildElement(); vipEle; vipEle = vipEle->NextSiblingElement())
    {
        uint32 id = 0;
        vipEle->QueryUnsignedAttribute("ID", &id);

        listviptemp.clear();

        for (int32 i=0; i<=10 ;i++)
        {
            LoadVipTmpl(i ,vipEle, listviptemp);
        }

        if( m_vipMap.find(id) == m_vipMap.end())
        {
            m_vipMap[id] = listviptemp;
        }
        else
        {
            CnWarn("Duplicate vip id: %u\n", id);
        }
    }

    return true;
}

void VipConfig::LoadVipTmpl(int32 num, TiXmlElement* vipEle, VipTmplVert& listviptemp)
{
    VipTemplate vipTmpl;
    char vipTxt[20] = {0};

    if(!vipEle) return;

    vipTmpl.Clear();
    vipTmpl.level = num;

    sprintf(vipTxt, "VIP%d", num);
    vipEle->QueryIntAttribute(vipTxt, &vipTmpl.value);

    listviptemp.push_back(vipTmpl);
}

int32 VipConfig::GetVipValue(int32 id, int32 vipLevel)
{
    VipMap::iterator vipIter;

    vipIter = m_vipMap.find(id);
    if (vipIter == m_vipMap.end())
    {
        CnAssert(false);
        return 0;
    }

    if((vipLevel < 0) || (vipIter->second.size() <= (uint32)vipLevel))
    {
        CnAssert(false);
        return 0;
    }

    return vipIter->second[vipLevel].value;
}



int32 VipConfig::GetVipValueSum (int32 id)
{
    int32 sumvalue = 0;
    VipMap::iterator vipIter;
    vipIter = m_vipMap.find(id);
    if (vipIter != m_vipMap.end())
    {
        VipTmplVert::iterator itervip;
        for (itervip = vipIter->second.begin(); itervip != vipIter->second.end(); itervip++)
        {

            sumvalue =  sumvalue + itervip->value;
        }
    }

    return sumvalue;
}
int32 VipConfig::GetVipLevel(int32 growthPoint)
{
    VipMap::iterator vipIter;
    vipIter = m_vipMap.find(VIP_PRIVILEGE_999);
    if (vipIter == m_vipMap.end())
    {
        CnAssert(false);
        return 0;
    }

    VipTmplVert::iterator itervip;
    for (itervip = vipIter->second.begin(); itervip != vipIter->second.end(); itervip++)
    {
        if(growthPoint < itervip->value)
        {
            if(itervip->level >= 1)
                return (itervip->level - 1);
            else
                return itervip->level;
        }
        else if(growthPoint == itervip->value)
        {
            return  itervip->level;
        }
        else if(itervip->level == VIP_MAX_VIP_LEVEL)
        {
            if(growthPoint > itervip->value)
                return itervip->level;
        }
    }

    return 0;
}
