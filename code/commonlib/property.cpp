#include "commonlib_pch.h"
#include "property.h"

PropertySet::PropertySet()
{

}

PropertySet::PropertySet(const PropertyList& list)
{
    m_list = list;
}

PropertySet::~PropertySet()
{

}

void PropertySet::Clear()
{
    m_list.clear();
}

void PropertySet::Add(const PropertySet& propSet)
{
    Add(propSet.m_list);
}

void PropertySet::Add(const PropertyList& list)
{
    for(PropertyList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
        Add(*it);
    }
}

void PropertySet::Add(const Property& prop)
{
    Add(prop.type, prop.value);
}

void PropertySet::Add( int32 type, int32 value )
{
    PropertyList::iterator it;

    for(it = m_list.begin(); it != m_list.end(); ++it)
    {
        if(it->type == type)
        {
            break;
        }
    }

    if(it != m_list.end())
    {
        it->value += value;
        return;
    }

    Property prop(type, value);
    m_list.push_back(prop);
}

void PropertySet::Set(int32 type, int32 value)
{
    PropertyList::iterator it;

    for(it = m_list.begin(); it != m_list.end(); ++it)
    {
        if(it->type == type)
        {
            break;
        }
    }

    if(it != m_list.end())
    {
        it->value = value;
        return;
    }

    Property prop(type, value);
    m_list.push_back(prop);
}

void PropertySet::Remove(int32 type)
{
    PropertyList::iterator it;

    for(it = m_list.begin(); it != m_list.end(); ++it)
    {
        if(it->type == type)
        {
            m_list.erase(it);
            break;
        }
    }
}

int32 PropertySet::GetValue( int32 type ) const
{
    for(PropertyList::const_iterator proiter = m_list.begin(); proiter != m_list.end(); proiter++)
    {
        if(type == proiter->type)
            return proiter->value;
    }
    return 0;
}

bool PropertySet::HasType(int32 type) const
{
    for(PropertyList::const_iterator proiter = m_list.begin(); proiter != m_list.end(); proiter++)
    {
        if(type == proiter->type)
            return true;
    }
    return false;
}

PropertySet& PropertySet::operator=(const PropertySet& c)
{
    m_list.clear();

    for (PropertyList::const_iterator cIter = c.m_list.begin();
        cIter != c.m_list.end(); ++cIter)
    {
        m_list.push_back(*cIter);
    }

    return *this;
}

void FuseSameProp(PropertyList& propList)
{
    PropertyList oldProList = propList;
    propList.clear();

    for(PropertyList::iterator oldPropIt = oldProList.begin(); oldPropIt != oldProList.end(); ++oldPropIt)
    {
        PropertyList::iterator newPropIt = propList.begin();
        for(; newPropIt != propList.end(); ++newPropIt)
        {
            if(newPropIt->type == oldPropIt->type)
                break;
        }
        if(newPropIt == propList.end())
            propList.push_back(*oldPropIt);
        else
            newPropIt->value += oldPropIt->value;
    }
}

PropertyList GetPropListByString(const std::string& propStr)
{
    PropertyList propList;
    propList.clear();
    std::vector<std::string> values = Crown::SDSplitStrings(propStr, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

        Property curproperty;
        curproperty.Clear();
        if(key_value.size() == 2)
        {
            curproperty.type = SDAtoi(key_value[0].c_str());
            curproperty.value= SDAtoi(key_value[1].c_str());
        }
        else
            CnAssert(false);

        propList.push_back(curproperty);
    }

    return propList;
}

bool operator == (const PropertySet& a, const PropertySet& b)
{
    for(PropertyList::const_iterator aIter = a.m_list.begin(); aIter != a.m_list.end(); aIter++)
    {
        uint32 propAType = aIter->type;
        int32  propAValue = aIter->value;
        int32  propBValue = 0;

        bool isTypeExist = false;
        for(PropertyList::const_iterator bIter = b.m_list.begin(); bIter != b.m_list.end(); bIter++)
        {
            if(bIter->type == propAType)
            {
                isTypeExist = true;
                propBValue = bIter->value;
                break;
            }
        }

        if(isTypeExist)
        {
            if(propAValue != propBValue)
                return false;
        }
        else
        {
            if(propAValue != 0)
                return false;
        }
    }
    
    return true;
}