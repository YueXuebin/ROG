#include "gl_gameserver_pch.h"
#include "game_contact_listener.h"
#include "debug_render.h"
#include "gl_gameserver.h"

GameContactListener::GameContactListener(GlGameServer* gs) :
m_GameServer(gs)
{
    m_pointCount = 0;
}

void GameContactListener::BeginContact(b2Contact* contact) 
{
    B2_NOT_USED(contact); 
}

void GameContactListener::EndContact(b2Contact* contact) 
{
    B2_NOT_USED(contact); 
}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    const b2Manifold* manifold = contact->GetManifold();

    if (manifold->pointCount == 0)
    {
        return;         // 没有碰撞点
    }

    // 碰撞对应两个fixture
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // 获取碰撞点状态
    b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
    b2GetPointStates(state1, state2, oldManifold, manifold);

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
    {
        ContactPoint* cp = m_points + m_pointCount;
        cp->fixtureA = fixtureA;
        cp->fixtureB = fixtureB;
        cp->position = worldManifold.points[i];
        cp->normal = worldManifold.normal;
        cp->state = state2[i];
        ++m_pointCount;
    }
}

void GameContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    B2_NOT_USED(contact);
    B2_NOT_USED(impulse);
}

void GameContactListener::Reset()
{
    m_pointCount = 0;
}

void GameContactListener::DrawContactPoints(Settings* settings)
{
    DebugDraw* pDraw = m_GameServer->GetDebugDraw();
    if(!pDraw)
        return;

    const float32 k_axisScale = 0.3f;

    for (int32 i = 0; i < m_pointCount; ++i)
    {
        ContactPoint* point = m_points + i;

        if (point->state == b2_addState)
        {
            // Add
            pDraw->DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));       // 新增的碰撞点
        }
        else if (point->state == b2_persistState)
        {
            // Persist
            pDraw->DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));        // 持续显示的碰撞点
        }

        // 显示碰撞点的normal方向
        if (settings->drawContactNormals == 1)
        {
            b2Vec2 p1 = point->position;
            b2Vec2 p2 = p1 + k_axisScale * point->normal;
            pDraw->DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
        }
        else if (settings->drawContactForces == 1)
        {
            //b2Vec2 p1 = point->position;
            //b2Vec2 p2 = p1 + k_forceScale * point->normalForce * point->normal;
            //m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
        }

        if (settings->drawFrictionForces == 1)
        {
            //b2Vec2 tangent = b2Cross(point->normal, 1.0f);
            //b2Vec2 p1 = point->position;
            //b2Vec2 p2 = p1 + k_forceScale * point->tangentForce * tangent;
            //m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
        }
    }
}