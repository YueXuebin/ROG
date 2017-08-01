#ifndef aoi_manager_h__
#define aoi_manager_h__

#include "aoi_define.h"

class SceneObject;
class AOIObject;
class Scene;
class CellVisitor;
class SceneNotifierAOI;
class Player;

/*
 *	Area Of Interest
 *  所有的通知，排除有关系的对象
 */
class AOIManager
{
public:
    AOIManager(Scene *scene, SceneNotifierAOI* pSceneNotifier);
    ~AOIManager();

    void Enter(SceneObject* sceneObj);
    void Leave(SceneObject* sceneObj);

    void NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender);

    void Update();

    Scene* GetScene() {return m_pScene;}

    int32   GetCellLenX() {return m_CellLenX;}
    int32   GetCellLenY() {return m_CellLenY;}

    CellVisitor* GetCellVisitor(int32 x, int32 y);

    bool IsPlayerSee(SceneObject* pSceneObject);
    bool IsPlayerCanSee(Player* player, SceneObject* pSceneObject);

private:
    AOIObject* GetAOIObject(uint32 id);

    void Update_AOIObject(AOIObject* aoiObject);
    void Update_Player(AOIObject* aoiObject);

    void EnterAction(AOIObject *obj);
    void LeaveAction(AOIObject *obj);

    void EnterCells(AOIObject *obj);
    void LeaveCells(AOIObject* obj);

private:
    Scene*                  m_pScene;
    SceneNotifierAOI*       m_pSceneNotifier;

    // Cell Visitor,用来表达每个Cell中有多少个角色能看到该Cell
    CellVisitor**           m_CellVisitors;     // 二维数组,一一对应于场景中的剖分格
    uint32                  m_CellLenX;
    uint32                  m_CellLenY;

    std::map<uint32, AOIObject*>  m_AOIObjectMap;   // 所有的AOI对象
};


#endif //aoi_manager_h__