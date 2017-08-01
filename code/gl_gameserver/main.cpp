#include "gl_gameserver_pch.h"
#include "gl_gameserver.h"


static void Exit(int code);


GlGameServer*   g_GlGameServer;

GLUI *glui = NULL;          // GLUI���ڶ���

b2Timer gGameTimer;
float32 gGameLoad = 0.0f;       // ��Ϸ����

namespace
{
    uint32 gameServerID = 1;    // GameServerID

    // ������ز���
    int mainWindow;             // ���ھ��
    int32 width = 1024;         // ���ڴ�С
    int32 height = 768;         // ���ڴ�С

    // ���ò���
    Settings settings;

    int tx, ty, tw, th;
    float32 viewZoom = 100.0f;    // �Ŵ�/��С

    bool lMouseDown = false;    // ���״̬
    bool mMouseDown = false;
    bool rMouseDown = false;
    b2Vec2 lastp;

}
// ���ڷŴ�/��С
static void Resize(int32 w, int32 h)
{
    width = w;
    height = h;

    GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);      // ��ȡ�����еĿ�������
    glViewport(tx, ty, tw, th);         //  ����ViewPort

    glMatrixMode(GL_PROJECTION);        // ͶӰ����
    glLoadIdentity();

    float32 ratio = float32(tw) / float32(th);

    b2Vec2 extents(ratio * 25.0f, 25.0f);
    extents *= viewZoom;            // view zoom

    b2Vec2 lower = settings.viewCenter - extents;
    b2Vec2 upper = settings.viewCenter + extents;

    // L/R/B/T
    gluOrtho2D(lower.x, upper.x, lower.y, upper.y);     // ����ͶӰ����Ϊ��������
}
// ת����Ļ���굽����ռ�����
static b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
    float32 u = x / float32(tw);
    float32 v = (th - y) / float32(th);

    float32 ratio = float32(tw) / float32(th);
    b2Vec2 extents(ratio * 25.0f, 25.0f);
    extents *= viewZoom;

    b2Vec2 lower = settings.viewCenter - extents;
    b2Vec2 upper = settings.viewCenter + extents;

    b2Vec2 p;
    p.x = (1.0f - u) * lower.x + u * upper.x;
    p.y = (1.0f - v) * lower.y + v * upper.y;
    return p;
}

static void Keyboard(unsigned char key, int x, int y)
{
    B2_NOT_USED(x);
    B2_NOT_USED(y);

    switch (key)
    {
        // esc�˳�
    case 27:
        Exit(0);
        break;
        // z ZoomOut
    case 'z':
        {
            viewZoom = b2Min(1.1f * viewZoom, 20.0f);
            Resize(width, height);
            break;
        }
        // x ZoomIn
    case 'x':
        viewZoom = b2Max(0.9f * viewZoom, 0.02f);
        Resize(width, height);
        break;
        // �ո��
    case ' ':
        {
            b2Vec2 p = ConvertScreenToWorld(x, y);
            g_GlGameServer->AddCircle(p);
        }
        break;
    default:
        {
            break;
        }
    }
}
// ���ⰴ����Ӧ
static void KeyboardSpecial(int key, int x, int y)
{
    B2_NOT_USED(x);
    B2_NOT_USED(y);

    switch (key)
    {
    case GLUT_ACTIVE_SHIFT:
        // ���
    case GLUT_KEY_LEFT:
        settings.viewCenter.x -= 0.5f;
        Resize(width, height);
        break;

        // Press right to pan right.
    case GLUT_KEY_RIGHT:
        settings.viewCenter.x += 0.5f;
        Resize(width, height);
        break;

        // Press down to pan down.
    case GLUT_KEY_DOWN:
        settings.viewCenter.y -= 0.5f;
        Resize(width, height);
        break;

        // Press up to pan up.
    case GLUT_KEY_UP:
        settings.viewCenter.y += 0.5f;
        Resize(width, height);
        break;

        // Press home to reset the view.
    case GLUT_KEY_HOME:
        viewZoom = 1.0f;
        settings.viewCenter.Set(0.0f, 20.0f);
        Resize(width, height);
        break;
    }
}
// ��갴����Ӧ
static void Mouse(int32 button, int32 state, int32 x, int32 y)
{
    B2_NOT_USED(x);
    B2_NOT_USED(y);

    // Use the mouse to move things around.
    if (button == GLUT_LEFT_BUTTON)
    {
        int mod = glutGetModifiers();
        b2Vec2 p = ConvertScreenToWorld(x, y);      // 
        if (state == GLUT_DOWN)
        {
            if (mod == GLUT_ACTIVE_SHIFT)
            {
                g_GlGameServer->ShiftMouseDown(p);
            }
            else
            {
                g_GlGameServer->MouseDown(p);
            }
            lMouseDown = true;
        }
        if (state == GLUT_UP)
        {
            g_GlGameServer->MouseUp(p);
            lMouseDown = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {	
            lastp = ConvertScreenToWorld(x, y);
            rMouseDown = true;
        }

        if (state == GLUT_UP)
        {
            rMouseDown = false;
        }
    }
    else if(button == GLUT_MIDDLE_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            //    lastp = ConvertScreenToWorld(x, y);
            mMouseDown = true;
        }

        if (state == GLUT_UP)
        {
            mMouseDown = false;
        }
    }
}

static void MouseWheel(int wheel, int direction, int x, int y)
{
    B2_NOT_USED(wheel);
    B2_NOT_USED(x);
    B2_NOT_USED(y);

    // ������Zoom In/Out
    if (direction > 0)
    {
        viewZoom /= 1.1f;
    }
    else
    {
        viewZoom *= 1.1f;
    }
    Resize(width, height);
}
// ��갴������ʱ�ƶ�
static void MouseMotion(int32 x, int32 y)
{
    b2Vec2 p = ConvertScreenToWorld(x, y);
    g_GlGameServer->MouseMotion(p);

    if (rMouseDown)
    {
        b2Vec2 diff = p - lastp;
        settings.viewCenter.x -= diff.x;
        settings.viewCenter.y -= diff.y;
        Resize(width, height);
        lastp = ConvertScreenToWorld(x, y);
    }
}
// ����ƶ�
static void MouseMove(int32 x, int32 y)
{
    b2Vec2 p = ConvertScreenToWorld(x, y);
    g_GlGameServer->MouseMove(p);
}
// ����
static void KeyboardUp(unsigned char key, int32 x, int32 y)
{
    B2_NOT_USED(x);
    B2_NOT_USED(y);

    b2Vec2 p = ConvertScreenToWorld(x, y);

    if (g_GlGameServer)
    {
        g_GlGameServer->KeyboardUp(key, p);
    }
}

// ��ȾLoop
float32 loopLastTime = 0;

void SimulationLoop()
{
    float32 loopStartTime = gGameTimer.GetMilliseconds();

    g_GlGameServer->PreRenderLoop(&settings);

    // ���backbuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ����MV����Ϊ��λ����
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    b2Vec2 oldCenter = settings.viewCenter;
    g_GlGameServer->RenderLoop(&settings);

    if (oldCenter.x != settings.viewCenter.x || oldCenter.y != settings.viewCenter.y)
    {
        Resize(width, height);
    }

    // Swap Buffer
    glutSwapBuffers();

    g_GlGameServer->PostRenderLoop();

    float32 loopEndTime = gGameTimer.GetMilliseconds();

    // ���㸺��ʱ��
    float32 loopTime = loopEndTime - loopStartTime;
    float32 frameTime = loopEndTime - loopLastTime;
    loopLastTime = loopEndTime;

    // ����ƽ������
    gGameLoad = loopTime/frameTime * 0.3f + gGameLoad * 0.7f;

}

// ���������60֡��Ⱦ
static void Timer(int)
{
    glutSetWindow(mainWindow);
    glutPostRedisplay();            // �����ػ�
    glutTimerFunc(16, Timer, 0);
}

static void Exit(int code)
{
    GLUI_Master.close_all();
    glutLeaveMainLoop();
}

void ParseCmd(int argc, char* argv[])
{
    while(1) 
    {
        int c = getopt(argc, argv, "-hi:");
        if(c == -1) break;
        switch(c) 
        {
        case 'i': 
            gameServerID = arg_to_int(optarg, 1, "i");
            break;
        }
    }
}

int main(int argc, char** argv)
{
#ifdef WIN 
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);     // check memory leak
    //_CrtSetBreakAlloc(173);
#endif

    // ���������в���
    ParseCmd(argc, argv);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(width, height);          // ���ڴ�С

    // ����
    char title[100];
    sprintf(title, "GameServer id=%d use OpenGL by Box2D(%d.%d.%d)", gameServerID, b2_version.major, b2_version.minor, b2_version.revision);

    // ��������
    mainWindow = glutCreateWindow(title);
    
    // ����GameServer����
    g_GlGameServer = CnNew GlGameServer;
    if(!g_GlGameServer->Init(gameServerID))
    {
        return 1;
    }

    glutDisplayFunc(SimulationLoop);        // ��Ҫ�ػ�ʱ����

    // glut��Ӧ����
    glutMouseWheelFunc(MouseWheel);                     // �������¼�
    glutMotionFunc(MouseMotion);                        // ��갴�º��ƶ��¼�
    glutPassiveMotionFunc(MouseMove);                   // ����ƶ��¼�
    glutKeyboardUpFunc(KeyboardUp);                     // ���̰���up��Ӧ����
    // glui��Ӧ��������(�˴������ڴ�й©,��ʱ�޷����)
    GLUI_Master.set_glutReshapeFunc(Resize);            // ���ڴ�С�����仯
    GLUI_Master.set_glutKeyboardFunc(Keyboard);         // ���̰�����Ӧ����
    GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);   // ���ⰴ����Ӧ����
    GLUI_Master.set_glutMouseFunc(Mouse);               // ��갴����Ӧ


    // ����GLUI�ӿؼ�
    glui = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW_RIGHT );
    glui->set_main_gfx_window(mainWindow);

    // Ƶ��\����\����
    GLUI_EditText* chanelEditText = glui->add_edittext("channel", GLUI_EDITTEXT_INT, &settings.channelID);
    GLUI_EditText* dungeonEditText = glui->add_edittext("dungeon", GLUI_EDITTEXT_INT, &settings.dungeonID);
    GLUI_EditText* sceneEditText = glui->add_edittext("scene", GLUI_EDITTEXT_INT, &settings.sceneID);

    // ��ʾ�������
    GLUI_Panel* drawPanel =	glui->add_panel("Draw");
    glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
    glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
    glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
    glui->add_checkbox_to_panel(drawPanel, "Pairs", &settings.drawPairs);
    glui->add_checkbox_to_panel(drawPanel, "Velocity", &settings.drawVelocity);
    glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
    glui->add_checkbox_to_panel(drawPanel, "Contact Points", &settings.drawContactPoints);
    glui->add_checkbox_to_panel(drawPanel, "Contact Normals", &settings.drawContactNormals);
    glui->add_checkbox_to_panel(drawPanel, "Contact Forces", &settings.drawContactForces);
    glui->add_checkbox_to_panel(drawPanel, "Friction Forces", &settings.drawFrictionForces);
    glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);
    glui->add_checkbox_to_panel(drawPanel, "Profile", &settings.drawProfile);
    glui->add_checkbox_to_panel(drawPanel, "Tree", &settings.drawTree);
    glui->add_checkbox_to_panel(drawPanel, "Path", &settings.drawPath);
    glui->add_checkbox_to_panel(drawPanel, "Triangles", &settings.drawTriangles);
    GLUI_Spinner* gluiSpinner = glui->add_spinner_to_panel(drawPanel, "Tree Depth", GLUI_SPINNER_INT, &b2DynamicTree::drawDepth);
    gluiSpinner->set_speed(0.1f);


    // ֡�ʿ���
    glutTimerFunc(16, Timer, 0);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);       // glut�����󷵻�

    gGameTimer.Reset();

    glutMainLoop();             // ��ѭ��

    g_GlGameServer->UnInit();
    CnDelete g_GlGameServer;

    return 0;
}
