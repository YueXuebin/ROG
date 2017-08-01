#include "gameholder_pch.h"
#include "aoi_object.h"
#include "sceneobject.h"
#include "aoi_manager.h"
#include "scene.h"
#include "player.h"


AOIObject::AOIObject(SceneObject* pSceneObject, AOIManager* pAOIManager) :
m_pSceneObject(pSceneObject),
m_pAOIManager(pAOIManager)
{
    m_CellX = m_pSceneObject->GetCoObject()->GetCellPosX() - m_pAOIManager->GetScene()->GetCoWorld()->GetCellMinX();
    m_CellY = m_pSceneObject->GetCoObject()->GetCellPosY() - m_pAOIManager->GetScene()->GetCoWorld()->GetCellMinY();

    m_LastCellX = m_CellX;
    m_LastCellY = m_CellY;

    m_CellMaxX = m_pAOIManager->GetCellLenX();
    m_CellMaxY = m_pAOIManager->GetCellLenY();

    m_SeeCellRadius = 0;
    if(pSceneObject->IsKindOf(&Player::ms_RTTI))
    {
        // 虚空的视野变大些
        uint32 dungeonID = pAOIManager->GetScene()->GetDungeonID();
        //if(dungeonID == DUNGEON_VOID_FIELD_SOLO_ID || (dungeonID >= DUNGEON_VOID_FIELD_ID_START && dungeonID <= DUNGEON_VOID_FIELD_ID_END))
        //{
        //    SetSeeRange(AOI_VISIT_DISTANCE*2);
        //}
        //else
        {
            SetSeeRange(AOI_VISIT_DISTANCE);            // 默认的玩家的视野范围
        }
    }
}

AOIObject::~AOIObject()
{

}

void AOIObject::UpdateCellPos()
{
    m_LastCellX = m_CellX;
    m_LastCellY = m_CellY;

    m_CellX = m_pSceneObject->GetCoObject()->GetCellPosX() - m_pAOIManager->GetScene()->GetCoWorld()->GetCellMinX();
    m_CellY = m_pSceneObject->GetCoObject()->GetCellPosY() - m_pAOIManager->GetScene()->GetCoWorld()->GetCellMinY();
}

void AOIObject::SetSeeRange(uint32 range)
{
    uint32 cellLength = m_pSceneObject->GetScene()->GetCoWorld()->GetCellLength();
    m_SeeCellRadius = range / cellLength + 1;
}

void AOIObject::EnterSee( SceneObject* pSceneObject )
{
#ifdef DEBUG
    CnAssert(CanSee());
    CnAssert(m_pSceneObject != pSceneObject);
    uint32 id = pSceneObject->GetId();
    CnAssert(m_SeeObjects.find(id) == m_SeeObjects.end());
    m_SeeObjects.insert(std::make_pair(id, pSceneObject));
#endif
}

void AOIObject::LeaveSee( SceneObject* pSceneObject )
{
#ifdef DEBUG
    CnAssert(CanSee());
    CnAssert(m_pSceneObject != pSceneObject);
    uint32 id = pSceneObject->GetId();
    std::map<uint32, SceneObject*>::iterator iter = m_SeeObjects.find(id);
    CnAssert(iter != m_SeeObjects.end());
    m_SeeObjects.erase(id);
#endif
}

