#ifndef touch_h__
#define touch_h__
#include "sceneobject.h"

// 采集点
class BattleUnit;

class Touch : public SceneObject
{
    CnDeclareRTTI
public:
    Touch();
    virtual ~Touch();

    virtual void Update(uint32 frame_count);
    virtual void OnDestory() {}

    uint32  GetTouchID() {return m_touchID;}
    void    SetTouchID(uint32 patchID) {m_touchID = patchID;}

    void    SetTime(uint32 time) {m_time = time;}

    const std::string& GetObsName() const {return m_ObsName;}
    void SetObsName(const std::string& obs_name) {m_ObsName = obs_name;}

    enum
    {
        kInit   = 0,            // 没有被采集
        kStart  = 1,            // 采集中
        kEnd    = 2,            // 采集完成
    };

    uint8   GetToucherID() {return m_ToucherID;}

    int16   TouchStart(BattleUnit* battleUnit, int32& remainingTime);
    void    TouchSuccess();
    void    TouchBreak();


    void SetShowID(uint32 showID) {m_ShowID = showID;}
    uint32 GetShowID() {return m_ShowID;}

    VECTOR3D_INT m_ObsPos;              // 物件集位置
    VECTOR3D_INT m_PickSize;            // 拾取范围 
    VECTOR3D_INT m_PickPos;             // 拾取范围位置
private:
    uint32  m_touchID;              // 采集点ID
    int32   m_time;                 // 总耗时(毫秒)

    uint32  m_ToucherID;                // 使用者单位ID
    
    int32   m_touchingTime;         // 已经采集的时间
    uint8   m_state;                // 状态

    uint32  m_ShowID;               // 外形ID
    std::string m_ObsName;              // 物件集名
};

#endif // touch_h__
