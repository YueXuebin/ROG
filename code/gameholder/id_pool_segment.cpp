#include "gameholder_pch.h"
#include "id_pool_segment.h"
#include "id_pool_manager.h"


IdPoolSegment*  CreateIdPoolSegment(uint32 type)
{
    return IdPoolManager::Instance()->GetSegmentPool()->Alloc(type);
}

void DestoryIdPoolSegment(IdPoolSegment* pPoolSegment)
{
    IdPoolManager::Instance()->GetSegmentPool()->Free(pPoolSegment);
}

IdPoolOctet*  CreateIdPoolOctet()
{
    return IdPoolManager::Instance()->GetOctetPool()->Alloc();
}

void DestoryIdPoolOctet(IdPoolOctet* pPoolOctet)
{
    IdPoolManager::Instance()->GetOctetPool()->Free(pPoolOctet);
}

//==================================================================================
IdPoolSegment::IdPoolSegment(uint32 type) :
m_type(type)
{
    memset(m_child, 0, sizeof(m_child));
    m_Octets = NULL;
}

IdPoolSegment::~IdPoolSegment()
{
    // 释放
    if(m_Octets)
    {
        DestoryIdPoolOctet(m_Octets);
        m_Octets = NULL;
    }

    for(int32 i=0; i<256; i++)
    {
        if(m_child[i])
        {
            DestoryIdPoolSegment(m_child[i]);
            m_child[i] = NULL;
        }
    }
}

bool IdPoolSegment::RegisterID(uint32 id)
{
    if(m_type == SEGMENT_32)
    {
        uint8 mod_id = MOD_31_24(id);
        if(m_child[mod_id] == NULL)
        {
            m_child[mod_id] = CreateIdPoolSegment(SEGMENT_24);
        }
        return m_child[mod_id]->RegisterID(id);
    }
    else if(m_type == SEGMENT_24)
    {
        uint8 mod_id = MOD_23_16(id);
        if(m_child[mod_id] == NULL)
        {
            m_child[mod_id] = CreateIdPoolSegment(SEGMENT_16);
        }
        return m_child[mod_id]->RegisterID(id);
    }
    else if(m_type == SEGMENT_16)
    {
        uint8 mod_id = MOD_15_8(id);
        if(m_child[mod_id] == NULL)
        {
            m_child[mod_id] = CreateIdPoolSegment(SEGMENT_8);
        }
        return m_child[mod_id]->RegisterID(id);
    }
    else if(m_type == SEGMENT_8)
    {
        if(m_Octets == NULL)
        {
            m_Octets = CreateIdPoolOctet();
        }
        return m_Octets->RegisterID(id);
    }
    else
    {
        CnAssert(false);
        return false;
    }
}

bool IdPoolSegment::UnregisterID(uint32 id)
{
    if(m_type == SEGMENT_32)
    {
        uint8 mod_id = MOD_31_24(id);
        return m_child[mod_id]->UnregisterID(id);
    }
    else if(m_type == SEGMENT_24)
    {
        uint8 mod_id = MOD_23_16(id);
        return m_child[mod_id]->UnregisterID(id);
    }
    else if(m_type == SEGMENT_16)
    {
        uint8 mod_id = MOD_15_8(id);
        return m_child[mod_id]->UnregisterID(id);
    }
    else if(m_type == SEGMENT_8)
    {
        return m_Octets->UnregisterID(id);
    }
    else
    {
        CnAssert(false);
        return false;
    }
}

void IdPoolSegment::CleanZero()
{
    // 深度优先的，空段删除
    if(m_type == SEGMENT_32 || m_type == SEGMENT_24 || m_type == SEGMENT_16)
    {
        for(int32 i=0; i<256; i++)
        {
            if(m_child[i])
            {
                m_child[i]->CleanZero();

                if(m_child[i]->IsAllNull())
                {
                    DestoryIdPoolSegment(m_child[i]);
                    m_child[i] = NULL;
                }

            }
        }
    }
    else if(m_type == SEGMENT_8)
    {
        if(m_Octets && m_Octets->IsZero())
        {
            DestoryIdPoolOctet(m_Octets);
            m_Octets = NULL;
        }
    }
    else
    {
        CnAssert(false);
    }
}

bool IdPoolSegment::IsAllNull()
{
    if(m_type == SEGMENT_32 || m_type == SEGMENT_24 || m_type == SEGMENT_16)
    {
        for(int32 i=0; i<256; i++)
        {
            if(m_child[i])
            {
                return false;
            }
        }
        return true;
    }
    else if(m_type == SEGMENT_8)
    {
        if(m_Octets)
            return m_Octets->IsZero();
        else
            return true;
    }
    else
    {
        CnAssert(false);
        return false;
    }
    
}

//==================================================================================
IdPoolOctet::IdPoolOctet()
{
    memset(m_bits, 0, sizeof(m_bits));
}

IdPoolOctet::~IdPoolOctet()
{

}

bool IdPoolOctet::RegisterID(uint32 id)
{
    uint8 mod_id = MOD_7_0(id);
    uint32 index = mod_id/32;
    uint32 bit_index = mod_id%32;
    uint32 bit_mask = 1 << bit_index;

    if( (m_bits[index] & bit_mask) != 0 )
        return false;           // 该id已经注册

    m_bits[index] |= bit_mask;
    return true;
}

bool IdPoolOctet::UnregisterID(uint32 id)
{
    uint8 mod_id = MOD_7_0(id);
    uint32 index = mod_id/32;
    uint32 bit_index = mod_id%32;
    uint32 bit_mask = 1 << bit_index;

    if( (m_bits[index] & bit_mask) == 0 )
        return false;           // 该id已经注销了

    m_bits[index] &= ~bit_mask;
    return true;
}

bool IdPoolOctet::IsZero()
{
    for(int32 i=0; i<8; i++)
    {
        if(m_bits[i] != 0)
        {
            return false;
        }
    }
    return true;;
}
