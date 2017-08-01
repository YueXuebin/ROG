#include "gameholder_pch.h"
#include "r_skill_cofnig.h"

IMPLEMENT_SINGLETON(RSkillCofnig)

RSkillCofnig::RSkillCofnig(void)
{
}

RSkillCofnig::~RSkillCofnig(void)
{
}

bool RSkillCofnig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    std::string  strvalue = "";
    RSkillInfo info;
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    { 
        //RSkillInfo info;
        //for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
        {
            info.Clear();

            info.id = SDAtoi(tableEle->Attribute("id"));
            info.percentage = SDAtoi(tableEle->Attribute("percentage"));
            info.attack = SDAtoi(tableEle->Attribute("attack"));
            strvalue = tableEle->Attribute("pro");
            info.pro = GetPropListByString(strvalue);
            m_skillList.push_back(info);
        }
        
    }

    return true;
}

const RSkillInfo* RSkillCofnig::GetSkill( uint32 id )
{
    for (RSkillInfoList::iterator item = m_skillList.begin(); item != m_skillList.end(); item++)
    {
        if(item->id == id)
            return &(*item);
    }
    return NULL;
}
