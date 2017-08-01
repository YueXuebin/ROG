#include "gameholder_pch.h"
#include "sceneobject.h"
#include "scene.h"
#include "creature.h"
#include "box2d_define.h"


CnImplementRTTI(SceneObject, Object);

SceneObject::SceneObject(uint32 objType, bool IsPhysics) :
Object(objType)
{
    m_destory = false;

    m_x = m_y = 0;
    m_angle = 0;

    m_pScene = NULL;

    m_radius = PHYSICAL_RADIUS;

    m_hide = false;

    m_dungeonAssociate = false;

    m_CantMove = false;
    m_CantAngle = false;
    m_CantBuff = false;
    m_CantHeal = false;
    m_CantPush = false;

    // 创建物理对象
    if(IsPhysics)
        m_pCoObject = CreateCoObject((real)GetX(), (real)GetY(), (real)GetRadius());
    else
        m_pCoObject = NULL;

    // 仅掉落物品使用网格化场景管理
    if(objType == OBJ_DROP_OBJECT)
        m_pGridObject = CnNew GridObject(this);
    else
        m_pGridObject = NULL;

    // 仅生物对象才拥有box2d物理对象
    if(objType == OBJ_PLAYER || objType == OBJ_MONSTER || objType == OBJ_PET)
        m_HasBox2D = true;
    else
        m_HasBox2D = false;
    m_pB2Body = NULL;
    m_B2Category = PHY_CATEGORY_CREATURE;
    m_B2Mask = PHY_CATEGORY_ALL;
}

SceneObject::~SceneObject()
{
    if(m_pGridObject)
        CnDelete m_pGridObject;

    if(m_pCoObject)
        CnDelete m_pCoObject;

    if(m_pB2Body)
    {
        CnAssert(m_HasBox2D);
        GetScene()->GetB2World()->DestroyBody(m_pB2Body);
        m_pB2Body = NULL;
    }
}

void SceneObject::SetDestory()
{
    if(m_destory)
        return;

    m_destory = true;
    OnDestory();
}

void SceneObject::onEnterScene(Scene* pScene)
{
    CnAssert(!m_pScene);
    m_pScene = pScene;

    // 加入物理对象到物理空间中
    if(m_pCoObject)
    {
        pScene->GetCoWorld()->AddObject(m_pCoObject);
    }
    // 加入对象到网格空间中
    if(m_pGridObject)
    {
        m_pScene->GetGridWorld()->AddGridObject(m_pGridObject);
    }
    // 加入对象到Box2d物理空间中
    if(m_HasBox2D)
    {
        b2BodyDef bodyDef;
        if(GetObjectType() == OBJ_PLAYER)
            bodyDef.type = b2_kinematicBody;
        else
            bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((real)GetX(), (real)GetY());
        m_pB2Body = m_pScene->GetB2World()->CreateBody(&bodyDef);
        m_pB2Body->SetUserData(this);

        b2CircleShape circle;
        circle.m_radius = (real)GetRadius();

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circle;
        if(bodyDef.type == b2_dynamicBody)
            fixtureDef.density = 1.0f;          // 动态物品必须有密度
        fixtureDef.filter.categoryBits = m_B2Category;
        fixtureDef.filter.maskBits = m_B2Mask;

        m_pB2Body->CreateFixture(&fixtureDef);
        UpdateB2Body();
    }
}

void SceneObject::onLeaveScene(Scene* pScene)
{
    CnAssert(m_pScene == pScene);

    // 从网格空间中移除
    if(m_pGridObject)
        pScene->GetGridWorld()->RemoveGridObject(m_pGridObject);

    // 从物理空间中移除物理对象
    if(m_pCoObject)
        pScene->GetCoWorld()->RemoveObject(m_pCoObject);

    // 从box2d物理空间中移除物理对象
    if(m_pB2Body)
    {
        CnAssert(m_HasBox2D);
        m_pScene->GetB2World()->DestroyBody(m_pB2Body);
        m_pB2Body = NULL;
    }

    m_pScene = NULL;
}

void SceneObject::Update(uint32 frame_count)
{
    if (!GetScene())
    {
        return;
    }

    // 物理对象驱动对象位置
    if(m_pB2Body)
    {
        const b2Vec2& phyPosition = m_pB2Body->GetPosition();
        SetPositionByPhy((int32)phyPosition.x, (int32)phyPosition.y);
    }
}

void SceneObject::SetPositionByPhy(int32 x, int32 y)
{
    m_x = x;
    m_y = y;

    if(m_pCoObject)
        m_pCoObject->SetPosition((real)m_x, (real)m_y);

    if(m_pGridObject)
        m_pGridObject->SetPosition((real)m_x, (real)m_y);
}

void SceneObject::SetPosition(int32 x, int32 y)
{
    CnAssert(!GetCantMove());       // 设置为不可移动后，逻辑上不应该再执行到此处
    
    SetPositionByPhy(x, y);

    if(m_pB2Body)
    {
        CnAssert(m_HasBox2D);
        m_pB2Body->SetTransform(b2Vec2((real)m_x, (real)m_y), ANGLE_TO_RADIAN(m_angle));
        m_pB2Body->SetAwake(true);
    }
}

void SceneObject::SetAngle(int val)
{
    if(GetCantAngle())
    {
        CnWarn("cant angle set angle\n");       // 一定是设置了不可转向，却可以移动
        return;
    }

    m_angle = val;
    if(m_pCoObject)
        m_pCoObject->SetAngle(ANGLE_TO_RADIAN(-val));
}

void SceneObject::SetRadius(int32 radius)
{
    m_radius = radius;
    if(m_pCoObject)
        m_pCoObject->SetRadius((real)m_radius);
    UpdateB2Body();
}

void SceneObject::SetCantMove(bool cantmove)
{
    m_CantMove = cantmove;
    UpdateB2Body();
}

void SceneObject::UpdateB2Body()
{
    if(!m_pB2Body)
        return;
    
    // 不能移动的
    if(m_CantMove || GetObjectType() == OBJ_PLAYER || GetObjectType() == OBJ_PET)
        m_pB2Body->SetType(b2_kinematicBody);
    else
        m_pB2Body->SetType(b2_dynamicBody);

    // 半径
    if(m_radius <= 0)
        m_pB2Body->SetActive(false);
    else
    {
        m_pB2Body->SetActive(true);
        m_pB2Body->GetFixtureList()->GetShape()->m_radius = (real)m_radius;
    }

}

void SceneObject::SetPhysicsVelocity(const CoVec2& v)
{
    if(m_pB2Body)
        m_pB2Body->ApplyLinearVelocity(b2Vec2(v.x, v.y));
}

CoVec2 SceneObject::GetPhysicsVelocity()
{
    CoVec2 v;
    if(m_pB2Body)
    {
        v.Set(m_pB2Body->GetApplyLinearVelocity().x, m_pB2Body->GetApplyLinearVelocity().y);
    }
    else
    {
        v.SetZero();
    }
    return v;
}

CoVec2 SceneObject::GetFinalPhysicsVelocity()
{
    CoVec2 v;
    if(m_pB2Body)
    {
        v.Set(m_pB2Body->GetLinearVelocity().x, m_pB2Body->GetLinearVelocity().y);
    }
    else
    {
        v.SetZero();
    }
    return v;
}

void SceneObject::SetPhysicsCategory(uint32 category)
{
    m_B2Category = category;
    UpdateB2PhysicsCategoryAndMask();
}

void SceneObject::SetPhysicsMask(uint32 mask)
{
    m_B2Mask = mask;
    UpdateB2PhysicsCategoryAndMask();
}

void SceneObject::SetPhysicsBullet(bool val)
{
    if(m_pB2Body)
    {
        m_pB2Body->SetBullet(val);
    }
}