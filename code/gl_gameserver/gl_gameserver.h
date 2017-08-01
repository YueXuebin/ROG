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
    hz(20.0f),                  // 一秒20帧
    velocityIterations(2),      // 速度约束迭代2次就可以了
    positionIterations(0),      // 不做位置约束
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

    b2Vec2 viewCenter;              // 视点
    float32 hz;                     // 频率
    int32 velocityIterations;       // 速度约束调整迭代次数
    int32 positionIterations;       // 位置约束调整迭代次数
    // 显示绘制参数
    int32 drawShapes;               // 显示Shape
    int32 drawJoints;               // 显示joint
    int32 drawAABBs;                // 显示AABB
    int32 drawPairs;                // 显示接触点对
    int32 drawVelocity;             // 显示速度
    int32 drawCOMs;                 // 显示质心
    int32 drawContactPoints;        // 显示接触点
    int32 drawContactNormals;       // 显示接触点normal
    int32 drawContactForces;
    int32 drawFrictionForces;
    int32 drawTree;                 // 显示动态二叉树
    int32 drawPath;                 // 显示移动路径
    int32 drawTriangles;             // 显示碰撞三角形
    // 显示性能分析
    int32 drawStats;                // 对象计数
    int32 drawProfile;              // 性能分析
    // 
    int32 enableWarmStarting;
    int32 enableContinuous;         // 连续碰撞检测for CCD
    int32 enableSubStepping;
    // 单步执行
    int32 pause;
    int32 singleStep;

    // 查看的场景
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

    // 渲染&逻辑
    void PreRenderLoop(Settings* settings);
    void RenderLoop(Settings* settings);
    void PostRenderLoop();
    
    // 鼠标&键盘
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


    uint32      m_UnionMapID;       // 当前查看的场景ID
    GameHolder* m_gameholder;       // 游戏服务器
    Scene*      m_pScene;           // 当前查看的游戏场景
    Creature*   m_pCreature;        // 当前选择的生物对象

    b2World* m_pWorld;
    DebugDraw m_debugDraw;          // 物理对象渲染器

    b2Body* m_emptyBody;            // 鼠标点击产生joint用

    b2Vec2 m_mouseWorld;            // 鼠标点击的物理空间位置

    b2Vec2  m_mouseMoveWorld;       // 鼠标位置

    b2Body* m_bomb;

    GameContactListener* m_ContactListener;  // 碰撞监听器

};



#endif // gl_gameserver_h__
