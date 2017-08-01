/*
 *	场景中的对象的基类
 */
#ifndef sceneobject_h__
#define sceneobject_h__

#include "object.h"

class Scene;
class b2Body;


class SceneObject : public Object, public CoObjectOwner, public GirdObjectOwner
{
    CnDeclareRTTI
public:
    SceneObject(uint32 objType, bool IsPhysics = true);
    virtual ~SceneObject();

    virtual void onEnterScene(Scene* pScene);
    virtual void onLeaveScene(Scene* pScene);

    virtual void Update(uint32 frame_count);
    virtual void OnDestory() = 0;

    virtual void UpdateB2PhysicsCategoryAndMask() {CnAssert(!m_pB2Body);}       // 非生物对象不应该有物理

    void SetDestory();                                   // 销毁对象
    bool isDestory() {return m_destory;}

    int32 GetAngle() { return m_angle; }                    // 获得角度
    void SetAngle(int val);                                 // 设置角度

    int32 GetX() { return m_x; }                            // 获得 x 坐标
    void SetX(int32 val) {SetPosition(val, GetY());}        // 设置 x 坐标

    int32 GetY() { return m_y; }                            // 获得 y 坐标
    void SetY(int32 val) {SetPosition(GetX(), val);}        // 设置 y 坐标

    void SetPosition(const CoVec2& pos) {SetPosition(int32(pos.x), int32(pos.y));}
    void SetPosition(int32 x, int32 y);
    CoVec2 GetPosition() {return CoVec2(GetX(), GetY());}

    void SetPhysicsVelocity(const CoVec2& v);
    CoVec2 GetPhysicsVelocity();
    CoVec2 GetFinalPhysicsVelocity();

    void SetPhysicsCategory(uint32 category);        // 设置物理碰撞filter
    uint32 GetPhysicsCategory() {return m_B2Category;}
    void SetPhysicsMask(uint32 mask);
    uint32 GetPhysicsMask() {return m_B2Mask;}

    void SetPhysicsBullet(bool val);                // 暂时无效(物理中的CCD关掉了)

    void SetRadius(int32 radius);
    int32 GetRadius() {return m_radius;}                  // 得到半径

    void SetHide(bool hide) {m_hide = hide;}
    bool GetHide() {return m_hide;}

    void SetCantMove(bool cantmove);
    bool GetCantMove() {return m_CantMove;}

    void SetCantAngle(bool cantangle) {m_CantAngle = cantangle;}
    bool GetCantAngle() {return m_CantAngle;}

    void SetCantBuff(bool cantbuff) {m_CantBuff = cantbuff;}
    bool GetCantBuff() {return m_CantBuff;}

    void SetCantHeal(bool cantheal) {m_CantHeal = cantheal;}
    bool GetCantHeal() {return m_CantHeal;}

    void SetCantTaunt(bool canttaunt) {m_CantTaunt = canttaunt;}
    bool GetCantTaunt() {return m_CantTaunt;}

    void SetCantPush(bool cantpush) {m_CantPush = cantpush;}
    bool GetCantPush() {return m_CantPush;}

    Scene* GetScene() {return m_pScene;}
    
    CoDynamicObject* GetCoObject() {return m_pCoObject;}
    b2Body* GetB2Object() {return m_pB2Body;}

    virtual bool IsAppearNotifierAll() {return false;}        // 出现/消失是否要全场景通知

private:
    void SetPositionByPhy(int32 x, int32 y);
    void UpdateB2Body();

protected:
    // box2d物理相关
    bool                m_HasBox2D;             // 是否要创建box2d物理对象
    b2Body*             m_pB2Body;
    uint32              m_B2Category;
    uint32              m_B2Mask;

private:
    CoDynamicObject*    m_pCoObject;
    GridObject*         m_pGridObject;

    bool m_destory;
    int32 m_angle;          // 移动角度
    int32 m_x;              // X坐标
    int32 m_y;              // Y坐标
    int32 m_radius;         // 半径
    bool m_CantMove;        // 不会移动
    bool m_CantAngle;       // 不会转向
    bool m_CantBuff;        // 不受buff
    bool m_CantHeal;        // 不受加血 (不受技能等加血, 但可以自动回血)
    bool m_CantTaunt;       // 不受嘲讽
    bool m_CantPush;        // 不被推动
    bool m_hide;            // 隐身
    bool m_dungeonAssociate;  // 副本结束关联
    Scene* m_pScene;

};

#endif // sceneobject_h__
