#include "gl_gameserver_pch.h"
#include "gl_gameserver.h"
#include "game_contact_listener.h"
#include "dungeon_manager.h"
#include "dungeon.h"
#include "creature.h"
#include "action_controller.h"
#include "action_creature_controller.h"
#include "action_player_controller.h"


// AABB��ѯ�ö���
class QueryCallback : public b2QueryCallback
{
public:
    QueryCallback(const b2Vec2& point)
    {
        m_point = point;
        m_fixture = NULL;
    }

    bool ReportFixture(b2Fixture* fixture)
    {
        b2Body* body = fixture->GetBody();
        if (body->GetType() == b2_dynamicBody || body->GetType() == b2_kinematicBody)
        {
            bool inside = fixture->TestPoint(m_point);
            if (inside)
            {
                m_fixture = fixture;
                return false;       // We are done, terminate the query.
            }
        }

        // Continue the query.
        return true;
    }

    b2Vec2 m_point;
    b2Fixture* m_fixture;
};

GlGameServer::GlGameServer()
{
    m_bomb = NULL;
    m_pScene = NULL;
    m_pCreature = NULL;
    m_UnionMapID = 0;

    // ������ײ������
    m_ContactListener = CnNew GameContactListener(this);

    // ����������Ⱦ����
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_jointBit;
    flags += b2Draw::e_aabbBit;
    flags += b2Draw::e_pairBit;
    flags += b2Draw::e_centerOfMassBit;
    m_debugDraw.SetFlags(flags);

    m_lastTime = 0;
    m_deltaTime = 0;
    m_logicTimer = 0;

    m_RenderCounter = 0;
    m_FrameCounter = 0;
    m_LastFrameCount = 0;
    m_FpsTimer = 0;
    m_Fps = 0;
    m_LogicFps = 0;
}

GlGameServer::~GlGameServer()
{
    delete m_ContactListener;
}

bool GlGameServer::Init(uint32 serverID)
{
    // ��ʼ����־
    char logname[30];
    SDSnprintf(logname, 30, "gamesvr-%02u", serverID);
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logname);

    // ������Ϸ������
    m_gameholder = CnNew GameHolder;
    if(!m_gameholder->Init(serverID))
    {   // ��ʼ������
        CnDbg("Server Init failed, exit after 5 second!\n");
        m_gameholder->Uninit();
        CnDelete m_gameholder;
        Crown::SDSleep(2000);
        return false;
    }
    return true;
}

void GlGameServer::UnInit()
{
    // ��������
    m_gameholder->Uninit();
    CnDelete m_gameholder;
    m_gameholder = NULL;
}

void GlGameServer::PreRenderLoop(Settings* settings)
{
    // ����֡ʱ��
    float32 curTime = m_GameTimer.GetMilliseconds();
    m_deltaTime = curTime - m_lastTime;
    if(m_lastTime == 0)
    {
        m_lastTime = curTime;
        return;
    }
    else
        m_lastTime = curTime;

    int32 onceMax = 0;
    m_logicTimer += m_deltaTime;
    while(m_logicTimer >= g_ServerUpdateTime && onceMax <= 5)          // ���һ����Ⱦ֡��n������֡
    {
        m_logicTimer -= g_ServerUpdateTime;

        m_ContactListener->Reset();

        // ִ��һ���߼�֡
        m_gameholder->Update(m_FrameCounter);

        m_FrameCounter++;
        onceMax++;
    }

    // ���Ҫ��Ⱦ�ĳ����Ƿ񻹴���
    if(m_pScene)
    {
        Scene* pScene = DungeonManager::Instance()->GetScene(m_UnionMapID);
        if(m_pScene != pScene)
        {
            m_pScene = NULL;
            m_UnionMapID = 0;
            m_pCreature = NULL;
        }
    }

    // ���Ҷ�Ӧ����������
    Scene* pNewScene = NULL;
    Dungeon* pDungeon = DungeonManager::Instance()->GetDungeon(settings->dungeonID, settings->channelID);
    if(pDungeon)
    {
        pNewScene = pDungeon->GetScene(settings->sceneID);
    }

    if(pNewScene != m_pScene)
    {
        if(m_pScene)
        {
            b2World* oldB2World = m_pScene->GetB2World();
            oldB2World->SetDebugDraw(NULL);
            oldB2World->SetContactListener(NULL);
        }

        // �鿴�³���
        m_pScene = pNewScene;
        if(m_pScene)
        {
            b2World* newB2World = m_pScene->GetB2World();
            newB2World->SetDebugDraw(&m_debugDraw);
            newB2World->SetContactListener(m_ContactListener);

            m_UnionMapID = m_pScene->GetUnionID();
        }
        else
            m_UnionMapID = 0;

        m_pCreature = NULL;
    }

    if(m_pCreature)
    {
        if(m_pCreature->isDestory())
            m_pCreature = NULL;
    }
}

void GlGameServer::RenderLoop(Settings* settings)
{
    uint32 textLine = 15;           // ������ʾλ��
    // ��ʾfps�����λ��
    m_debugDraw.DrawString(5, textLine, "Fps:%d Logic:%d Load:%2.0f%% [%d, %d]", m_Fps, m_LogicFps, gGameLoad*100, int32(m_mouseMoveWorld.x), int32(m_mouseMoveWorld.y));
    textLine += 15;

    if(!m_pScene)
    {
        m_debugDraw.DrawString(5, textLine, "****NO SCENE****");
        textLine += 15;
        return;
    }

    b2World* pB2World = m_pScene->GetB2World();

    // �Ƿ�ֹͣ����
    if (settings->pause)
    {
        m_debugDraw.DrawString(5, textLine, "****PAUSED****");
        textLine += 15;
    }

    // ������Ⱦ����
    uint32 flags = 0;
    flags += settings->drawShapes			* b2Draw::e_shapeBit;
    flags += settings->drawJoints			* b2Draw::e_jointBit;
    flags += settings->drawAABBs			* b2Draw::e_aabbBit;
    flags += settings->drawPairs			* b2Draw::e_pairBit;
    flags += settings->drawVelocity         * b2Draw::e_velocityBit;
    flags += settings->drawCOMs				* b2Draw::e_centerOfMassBit;
    flags += settings->drawTree             * b2Draw::e_treeBit;
    m_debugDraw.SetFlags(flags);

    // �����������Ⱦ
    pB2World->DrawDebugData();


    // ��ʾ����ռ�״̬����
    if (settings->drawStats)
    {
        // body��,���ӵ���,joint��
        int32 bodyCount = pB2World->GetBodyCount();
        int32 contactCount = pB2World->GetContactCount();
        int32 jointCount = pB2World->GetJointCount();
        m_debugDraw.DrawString(5, textLine, "bodies=%d contacts=%d joints=%d", bodyCount, contactCount, jointCount);
        textLine += 15;

        // ����������
        int32 proxyCount = pB2World->GetProxyCount();
        int32 height = pB2World->GetTreeHeight();
        int32 balance = pB2World->GetTreeBalance();
        float32 quality = pB2World->GetTreeQuality();
        m_debugDraw.DrawString(5, textLine, "proxies=%d height=%d balance=%d quality=%g", proxyCount, height, balance, quality);
        textLine += 15;
    }

    // ��ʾ���ܷ������
    if (settings->drawProfile)
    {
        const b2Profile& p = pB2World->GetProfile();

        m_debugDraw.DrawString(5, textLine, "step = %5.2f", p.step);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "collide = %5.2f", p.collide);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "solve = %5.2f", p.solve);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "solve init = %5.2f", p.solveInit);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "solve velocity = %5.2f", p.solveVelocity);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "solve position = %5.2f", p.solvePosition);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "solveTOI = %5.2f", p.solveTOI);
        textLine += 15;
        m_debugDraw.DrawString(5, textLine, "broad-phase = %5.2f", p.broadphase);
        textLine += 15;
    }

    // ��mouseJoint
    //if (m_mouseJoint)
    //{
    //    b2Vec2 p1 = m_mouseJoint->GetAnchorB();
    //    b2Vec2 p2 = m_mouseJoint->GetTarget();

    //    b2Color c;
    //    c.Set(0.0f, 1.0f, 0.0f);
    //    m_debugDraw.DrawPoint(p1, 4.0f, c);
    //    m_debugDraw.DrawPoint(p2, 4.0f, c);

    //    c.Set(0.8f, 0.8f, 0.8f);
    //    m_debugDraw.DrawSegment(p1, p2, c);
    //}

    // ����ײ��
    if (settings->drawContactPoints)
    {
        m_ContactListener->DrawContactPoints(settings);
    }

    // ��������ײ
    if(settings->drawTriangles)
    {
        b2Color c;
        c.Set(0.0, 0.7f, 0.9f);

        b2Vec2 b2Points[10];
        const std::vector<CoPolygon*> polygons = m_pScene->GetCoWorld()->GetTriangles();
        for(uint32 i=0; i<polygons.size(); i++)
        {
            const CoPolygon* polygon = polygons[i];
            const std::vector<CoVec2>& points = polygon->Points();

            uint32 vc = 0;
            for(uint32 j=0; j<points.size(); j++)
            {
                if(vc >= 10)
                    break;

                b2Points[j].x = points[j].x;
                b2Points[j].y = points[j].y;
                vc++;
            }
            m_debugDraw.DrawPolygon(b2Points, vc, c);
        }
    }


    // ����ǰ�����·��
    if(settings->drawPath && m_pCreature)
    {
        b2Color c;
        c.Set(0.2f, 0.6f, 0.5f);

        if(m_pCreature->GetObjectType() != OBJ_PLAYER)
        {
            ActionCreatureController* pActionController = (ActionCreatureController*)m_pCreature->GetActionController();
            const std::vector<CoVec2>& path = pActionController->GetPath();

            b2Vec2 p1, p2;
            for(uint32 i=0; i<path.size(); i++)
            {
                if(i == 0)
                {
                    p1.Set((real)m_pCreature->GetX(), (real)m_pCreature->GetY());
                    p2.Set(path[i].x, path[i].y);
                }
                else
                {
                    p1.Set(path[i-1].x, path[i-1].y);
                    p2.Set(path[i].x, path[i].y);
                }
                m_debugDraw.DrawSegment(p1, p2, c);
            }
        }
        else
        {
            ActionPlayerController* pActionPlayerController = (ActionPlayerController*)m_pCreature->GetActionController();
            b2Vec2 center(pActionPlayerController->GetCheckPos().x, pActionPlayerController->GetCheckPos().y);

            m_debugDraw.DrawCircle(center, (float32)m_pCreature->GetRadius(), c);
        }
    }

}

void GlGameServer::PostRenderLoop()
{
    // ����fps
    m_RenderCounter++;
    m_FpsTimer += m_deltaTime;
    if(m_FpsTimer > 1000)
    {
        m_FpsTimer -= 1000;
        // ��Ⱦ֡fps
        m_Fps = m_RenderCounter;
        m_RenderCounter = 0;
        // �߼�֡fps
        m_LogicFps = m_FrameCounter - m_LastFrameCount;
        m_LastFrameCount = m_FrameCounter;
    }
}

void GlGameServer::KeyboardStep()
{
    int32 keyW = ::GetKeyState('W');
    int32 keyA = ::GetKeyState('A');
    int32 keyS = ::GetKeyState('S');
    int32 keyD = ::GetKeyState('D');

    float32 speed = 2;

    b2Vec2 v(0, 0);
    if(keyW < 0)
    {
        v.y += speed;
    }
    if(keyA < 0)
    {
        v.x -= speed;
    }
    if(keyS < 0)
    {
        v.y -= speed;
    }
    if(keyD < 0)
    {
        v.x += speed;
    }

    //if(v.LengthSquared() > 0)
    //    m_MainBody->ApplyLinearVelocity(v);

}

void GlGameServer::PhysicsStep(Settings* settings)
{
    // ��������
    KeyboardStep();

    float32 timeStep = (settings->hz > 0.0f) ? (1.0f / settings->hz) : float32(0.0f);   // ÿ֡ʱ��

    if (settings->pause)
    {
        // ��������
        if (settings->singleStep)
        {
            settings->singleStep = 0;
        }
        else
        {
            timeStep = 0.0f;
        }
    }
    
    //
    m_ContactListener->Reset();
}

void GlGameServer::ShiftMouseDown(const b2Vec2& p)
{

}

void GlGameServer::MouseDown(const b2Vec2& p)
{
    m_mouseWorld = p;

    if(!m_pScene)
        return;

    b2World* pB2World = m_pScene->GetB2World();

    // ���������λ��,����һ��СAABB,������ʰȡ�ж�
    b2AABB aabb;
    b2Vec2 d;
    d.Set(0.001f, 0.001f);
    aabb.lowerBound = p - d;
    aabb.upperBound = p + d;

    // ʰȡ��Χ�ж�
    QueryCallback callback(p);
    pB2World->QueryAABB(&callback, aabb);

    // ����Ƿ��е��������
    if (callback.m_fixture)
    {
        b2Body* body = callback.m_fixture->GetBody();       // ��ñ���������body
        m_pCreature = (Creature*)body->GetUserData();
        body->SetAwake(true);
    }
    // �ƶ�����
    else if(m_pCreature)
    {
        ActionController*  actionController= m_pCreature->GetActionController();
        if(actionController)
        {
            actionController->RunTo((int32)m_mouseWorld.x, (int32)m_mouseWorld.y);
        }
    }
}

void GlGameServer::MouseUp(const b2Vec2& p)
{

}

void GlGameServer::MouseMotion(const b2Vec2& p)
{
    m_mouseWorld = p;


}

void GlGameServer::MouseMove(const b2Vec2& p)
{
    m_mouseMoveWorld = p;
}

void GlGameServer::KeyboardUp(unsigned char key, const b2Vec2& p)
{
    if(key == 'p')
    {
        // ����Ŀ��
        if(m_pCreature)
        {
            m_pCreature->GetActionController()->PushTo((int32)p.x, (int32)p.y, 500, false);
        }
    }
}

void GlGameServer::AddCircle(const b2Vec2& p)
{
    //if (m_bomb)
    //{
    //    m_world->DestroyBody(m_bomb);
    //    m_bomb = NULL;
    //}

    //b2BodyDef bd;
    //bd.type = b2_dynamicBody;
    //bd.position = p;
    //m_bomb = m_world->CreateBody(&bd);


    //b2CircleShape circle;
    //circle.m_radius = 2.0f;

    //b2FixtureDef fd;
    //fd.shape = &circle;
    //fd.density = 1.0f;
    //fd.restitution = 0.0f;

    //b2Vec2 minV = p - b2Vec2(0.3f,0.3f);
    //b2Vec2 maxV = p + b2Vec2(0.3f,0.3f);

    //b2AABB aabb;
    //aabb.lowerBound = minV;
    //aabb.upperBound = maxV;

    //m_bomb->CreateFixture(&fd);


}