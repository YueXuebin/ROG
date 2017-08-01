#include "gameholder_pch.h"
#include "skill_info.h"

SkillInfo::SkillInfo()
{
    Clear();
}

SkillInfo::~SkillInfo()
{

}

void SkillInfo::Clear()
{
    m_finalInfo.Clear();
    m_IsTriggered = false;
    m_Damage = 0;
}

void SkillInfo::FillPropertySet(PropertySet& propertySet) const
{
    for(uint32 i=0; i<m_finalInfo.props.size(); i++)
    {
        const KEY_VALUE& prop = m_finalInfo.props[i];
        propertySet.Add(prop.key, prop.value);
    }
}

void SkillInfo::AddProp(uint32 key, int32 value)
{
    KEY_VALUE prop;
    prop.key = key;
    prop.value = value;
    AddProp(prop);
}

int32 SkillInfo::GetProp(uint32 key)
{
    for(uint32 i=0; i<m_finalInfo.props.size(); i++)
    {
        const KEY_VALUE& prop = m_finalInfo.props[i];
        if(prop.key == key)
            return prop.value;
    }
    return 0;
}

