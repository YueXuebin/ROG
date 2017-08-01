#include "gameholder_pch.h"
#include "off_line_vigor_config.h"
IMPLEMENT_SINGLETON(OffLineVigorConfig)
OffLineVigorConfig::OffLineVigorConfig(void)
{
}

OffLineVigorConfig::~OffLineVigorConfig(void)
{
}
bool OffLineVigorConfig::LoadConfig(const char* path)
{
     TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load offline_vigor file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    OffLineVigor offlinevigor;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        offlinevigor.clear();
        uint32 id = 0;

       xmlEle->QueryUnsignedAttribute("id", &id);
       xmlEle->QueryIntAttribute("vip_level", &offlinevigor.vip_level);
       xmlEle->QueryIntAttribute("money_point", &offlinevigor.money_point);
       xmlEle->QueryUnsignedAttribute("recover_percent", &offlinevigor.recover_percent);

       if(id == 0) continue;

       if(m_offlineVigormap.find(id) == m_offlineVigormap.end())
       {
           m_offlineVigormap[id] = offlinevigor;
       }
       else
       {
           CnWarn("Duplicate offlineVigro id  %u\n",id);
       }
    }

   return true;
}
const OffLineVigor* OffLineVigorConfig::FindOffLineVigorById(uint32 id)
{
    std::map<uint32, OffLineVigor>::iterator it = m_offlineVigormap.find(id);
    if(it != m_offlineVigormap.end())
        return &(it->second);

    return NULL;
}