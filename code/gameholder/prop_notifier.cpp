#include "gameholder_pch.h"
#include "prop_notifier.h"
#include "battle_unit.h"
#include "scene.h"


PropNotifier::PropNotifier(BattleUnit* owner) :
m_Owner(owner)
{
    m_StartNotify = false;
}

PropNotifier::~PropNotifier()
{

}

void PropNotifier::Update()
{
    if(!m_UpdateDirty)
        return;

    if (!m_Owner->GetScene())
        return;

    if(m_StartNotify)
    {
        m_UpdateDirty = false;

        PROPERTY_LIST_NTF list_ntf;

        int32 ntfCount = 0;
        for(PropMap::iterator iter = m_PropMap.begin(); iter != m_PropMap.end(); iter++)
        {
            if(iter->second.m_Dirty)
            {
                iter->second.m_Dirty = false;
                if(m_StartNotify)
                {
                    PROPERTY_NTF ntf;
                    ntf.property_id = iter->second.m_PropertyId;
                    ntf.value = iter->second.m_Value;
                    ntf.str = iter->second.m_StrValue;
                    list_ntf.list.push_back(ntf);
                    ntfCount++;
                }
            }
        }

        if(ntfCount > 0)
        {
            list_ntf.object_id = m_Owner->GetId();
            m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_PROPERTY_LIST_NTF, &list_ntf);
        }
    }

    m_StartNotify = true;       // 从第二个Update开始，发送通知
}

void PropNotifier::NotifyProperty( uint32 property_id, int32 value, const std::string& strValue)
{
    if( m_PropMap.find(property_id) == m_PropMap.end() )
    {
        PropData propData;
        propData.m_PropertyId = property_id;
        propData.m_Value = value;
        propData.m_StrValue = strValue;
        propData.m_Dirty = true;

        m_PropMap[property_id] = propData;
    }
    else if(m_PropMap[property_id].m_Value != value)
    {
        m_PropMap[property_id].m_Value = value;
        m_PropMap[property_id].m_StrValue = strValue;
        m_PropMap[property_id].m_Dirty = true;
    }
    else if (m_PropMap[property_id].m_StrValue != strValue)
    {
        m_PropMap[property_id].m_Value = value;
        m_PropMap[property_id].m_StrValue = strValue;
        m_PropMap[property_id].m_Dirty = true;
    }

    m_UpdateDirty = true;
}
