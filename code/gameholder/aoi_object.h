#ifndef aoi_object_h__
#define aoi_object_h__

#include "aoi_define.h"

class SceneObject;
class AOIManager;

/*
 *	场景对象的，AOI系统代理体
 */
class AOIObject
{
public:
    AOIObject(SceneObject* pSceneObject, AOIManager* pAOIManager);
    ~AOIObject();

    SceneObject* GetSceneObject() {return m_pSceneObject;}

    int32   GetCellX() {return m_CellX;}
    int32   GetCellY() {return m_CellY;}
    int32   GetLastCellX() {return m_LastCellX;}
    int32   GetLastCellY() {return m_LastCellY;}

    void    UpdateCellPos();

    // 当前视野
    int32   GetMinSeeX() {return Crown::SDMax(m_CellX - m_SeeCellRadius, 0);}
    int32   GetMaxSeeX() {return Crown::SDMin(m_CellX + m_SeeCellRadius, m_CellMaxX-1);}
    int32   GetMinSeeY() {return Crown::SDMax(m_CellY - m_SeeCellRadius, 0);}
    int32   GetMaxSeeY() {return Crown::SDMin(m_CellY + m_SeeCellRadius, m_CellMaxY-1);}
    // 旧视野
    int32   GetLastMinSeeX() {return Crown::SDMax(m_LastCellX - m_SeeCellRadius, 0);}
    int32   GetLastMaxSeeX() {return Crown::SDMin(m_LastCellX + m_SeeCellRadius, m_CellMaxX-1);}
    int32   GetLastMinSeeY() {return Crown::SDMax(m_LastCellY - m_SeeCellRadius, 0);}
    int32   GetLastMaxSeeY() {return Crown::SDMin(m_LastCellY + m_SeeCellRadius, m_CellMaxY-1);}

    bool    CanSee() {return m_SeeCellRadius > 0;}      // 其实只有player才有视野

    void    EnterSee(SceneObject* pSceneObject);
    void    LeaveSee(SceneObject* pSceneObject);

#ifdef DEBUG
    std::map<uint32, SceneObject*>  m_SeeObjects;       // 可见的所有对象(调试验证用)
#endif

private:
    void    SetSeeRange(uint32 range);      // 设置视野(至可在构建时设置)

private:
    int32       m_LastCellX;
    int32       m_LastCellY;
    int32       m_CellX;
    int32       m_CellY;
    int32       m_CellMaxX;
    int32       m_CellMaxY;
    
    int32      m_SeeCellRadius;            // 可视范围(以Cell为单位)

    SceneObject*    m_pSceneObject;
    AOIManager*     m_pAOIManager;
    
};

#endif // aoi_object_h__
