#include "gameholder_pch.h"
#include "id_manager.h"
#include "object_with_id.h"

ObjectWithId::ObjectWithId(uint32 type) :
    m_Id(IdManager::Instance()->GenerateId(type)),
    m_Type(type)
{
}

ObjectWithId::~ObjectWithId()
{
    IdManager::Instance()->ReleaseId(GetId());
}

