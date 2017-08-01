#include "gameholder_pch.h"
#include "aoi_system_manager.h"
#include "aoi_object.h"

IMPLEMENT_SINGLETON(AOISystemManager)

AOISystemManager::AOISystemManager()
{

}

AOISystemManager::~AOISystemManager()
{
    CnDelete m_AOIObjectPool;
}

bool AOISystemManager::Init()
{
    m_AOIObjectPool = CnNew Crown::CSDObjectPool<AOIObject>(200, 100);

    return true;
}

AOIObject* AOISystemManager::CreateAOIObject(SceneObject* scene_obj, AOIManager* pAOIManager)
{
    return m_AOIObjectPool->Alloc(scene_obj, pAOIManager);
}

void AOISystemManager::DestoryAOIObject( AOIObject* aoiObj )
{
    m_AOIObjectPool->Free(aoiObj);
}
