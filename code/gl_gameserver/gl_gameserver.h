#ifndef gl_gameserver_h__
#define gl_gameserver_h__

#include "debug_render.h"

extern float32 gGameLoad;

class GameContactListener;
class Scene;
class Creature;

struct Settings
{
    Settings() :
    viewCenter(0.0f, 0.0f),
    hz(20.0f),                  // һ��20֡
    velocityIterations(2),      // �ٶ�Լ������2�ξͿ�����
    positionIterations(0),      // ����λ��Լ��
    drawShapes(1),
    drawJoints(0),
    drawAABBs(0),
    drawPairs(0),
    drawVelocity(1),
    drawCOMs(0),
    drawContactPoints(0),
    drawContactNormals(0),
    drawContactForces(0),
    drawFrictionForces(0),
    drawTree(0),
    drawPath(1),
    drawTriangles(0),
    drawStats(0),
    drawProfile(0),
    enableWarmStarting(0),
    enableContinuous(0),
    enableSubStepping(0),
    pause(0),
    singleStep(0),
    channelID(1),
    dungeonID(1001),
    sceneID(1)
    {}

    b2Vec2 viewCenter;              // �ӵ�
    float32 hz;                     // Ƶ��
    int32 velocityIterations;       // �ٶ�Լ��������������
    int32 positionIterations;       // λ��Լ��������������
    // ��ʾ���Ʋ���
    int32 drawShapes;               // ��ʾShape
    int32 drawJoints;               // ��ʾjoint
    int32 drawAABBs;                // ��ʾAABB
    int32 drawPairs;                // ��ʾ�Ӵ����
    int32 drawVelocity;             // ��ʾ�ٶ�
    int32 drawCOMs;                 // ��ʾ����
    int32 drawContactPoints;        // ��ʾ�Ӵ���
    int32 drawContactNormals;       // ��ʾ�Ӵ���normal
    int32 drawContactForces;
    int32 drawFrictionForces;
    int32 drawTree;                 // ��ʾ��̬������
    int32 drawPath;                 // ��ʾ�ƶ�·��
    int32 drawTriangles;             // ��ʾ��ײ������
    // ��ʾ���ܷ���
    int32 drawStats;                // �������
    int32 drawProfile;              // ���ܷ���
    // 
    int32 enableWarmStarting;
    int32 enableContinuous;         // ������ײ���for CCD
    int32 enableSubStepping;
    // ����ִ��
    int32 pause;
    int32 singleStep;

    // �鿴�ĳ���
    uint32  channelID;
    uint32  dungeonID;
    uint32  sceneID;
};


class GlGameServer
{
public:
    GlGameServer();
    ~GlGameServer();

    bool Init(uint32 serverID);
    void UnInit();

    // ��Ⱦ&�߼�
    void PreRenderLoop(Settings* settings);
    void RenderLoop(Settings* settings);
    void PostRenderLoop();
    
    // ���&����
    void ShiftMouseDown(const b2Vec2& p);
    void MouseDown(const b2Vec2& p);
    void MouseUp(const b2Vec2& p);
    void MouseMotion(const b2Vec2& p);
    void MouseMove(const b2Vec2& p);
    void KeyboardUp(unsigned char key, const b2Vec2& p);

    void AddCircle(const b2Vec2& p);

    DebugDraw*  GetDebugDraw() {return &m_debugDraw;}

private:
    void KeyboardStep();
    void PhysicsStep(Settings* settings);

private:
    b2Timer m_GameTimer;
    float32 m_lastTime;
    float32 m_deltaTime;
    
    float32 m_logicTimer;

    int32   m_RenderCounter;
    int32   m_FrameCounter;
    int32   m_LastFrameCount;
    float32 m_FpsTimer;
    int32   m_LogicFps;
    int32   m_Fps;


    uint32      m_UnionMapID;       // ��ǰ�鿴�ĳ���ID
    GameHolder* m_gameholder;       // ��Ϸ������
    Scene*      m_pScene;           // ��ǰ�鿴����Ϸ����
    Creature*   m_pCreature;        // ��ǰѡ����������

    b2World* m_pWorld;
    DebugDraw m_debugDraw;          // ���������Ⱦ��

    b2Body* m_emptyBody;            // ���������joint��

    b2Vec2 m_mouseWorld;            // �����������ռ�λ��

    b2Vec2  m_mouseMoveWorld;       // ���λ��

    b2Body* m_bomb;

    GameContactListener* m_ContactListener;  // ��ײ������

};



#endif // gl_gameserver_h__
