#ifndef aoi_system_manager_h__
#define aoi_system_manager_h__


class AOIObject;
class SceneObject;
class AOIManager;

class AOISystemManager
{
public:
    AOISystemManager();
    ~AOISystemManager();

    DECLARE_SINGLETON(AOISystemManager)

    bool Init();

    AOIObject* CreateAOIObject(SceneObject* scene_obj, AOIManager* pAOIManager);
    void    DestoryAOIObject(AOIObject* aoiObj);

private:
    Crown::CSDObjectPool<AOIObject>*     m_AOIObjectPool;           // aoiObject 对象池
};


#endif // aoi_system_manager_h__
