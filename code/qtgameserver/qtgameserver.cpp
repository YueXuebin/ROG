#include "qtgameserver_pch.h"
#include "qtgameserver.h"
#include "qtgameview.h"

uint32  g_FrameCount = 0;
uint32  g_FrameUsedTime = 0;

qtgameserver::qtgameserver(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    
    createActions();
    createToolBars();

    char logname[20];
    SDSprintf(logname, "qtgamesvr-%02u", gServerID);
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logname);
    Crown::ColorLog::Instance()->SetLogTime(true);

    m_gameholder = CnNew GameHolder;
    
    if(!m_gameholder->Init(gServerID, RES_DIR"sdb/"))
    {	// ³õÊ¼»¯´íÎó
        CnAssert(false);
        return;
    }
    
    startTimer(g_ServerUpdateTime);

    m_pView = new QtGameView();

    setCentralWidget(m_pView);

    char title[100];
    SDSnprintf(title, 100, "gameserver %d", gServerID);

    setWindowTitle(tr(title));

    m_lastTime = 0;
    m_InfoTime = 0;
    m_Time.start();
}

qtgameserver::~qtgameserver()
{
    m_gameholder->Uninit();
    CnDelete m_gameholder;

    Crown::ColorLog::DestroyInstance();
}

void qtgameserver::createActions()
{
    MapScaleAct = new QAction(tr("Scale"), this);
    MapScaleAct->setStatusTip(tr("map scale"));
    connect(MapScaleAct, SIGNAL(triggered()), this, SLOT(SacleInfo()));

    CellInfoAct = new QAction(tr("CellInfo"), this);
    CellInfoAct->setStatusTip(tr("cell info"));
    connect(CellInfoAct, SIGNAL(triggered()), this, SLOT(CellInfo()));

    TriangleAct = new QAction(tr("Triangle"), this);
    TriangleAct->setStatusTip(tr("triangle"));
    connect(TriangleAct, SIGNAL(triggered()), this, SLOT(TriangleInfo()));

    BoundingBoxAct = new QAction(tr("bound"), this);
    BoundingBoxAct->setStatusTip(tr("bound"));
    connect(BoundingBoxAct, SIGNAL(triggered()), this, SLOT(BoundingBoxInfo()));


    ChannelIDCbx = new QComboBox;
    ChannelIDCbx->setFixedWidth(70);
    ChannelIDCbx->setEditable(true);
    for (int i = 1; i < 10; i++)
        ChannelIDCbx->addItem(QString().setNum(i));
    connect(ChannelIDCbx, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeDungeonID(QString)));

    DungeonIDCbx = new QComboBox;
    DungeonIDCbx->setEditable(true);
    DungeonIDCbx->setFixedWidth(70);
    for (int i = 1; i < 10; i++)
        DungeonIDCbx->addItem(QString().setNum(i));
    connect(DungeonIDCbx, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeDungeonID(QString)));

    SceneIDCbx = new QComboBox;
    SceneIDCbx->setEditable(true);
    SceneIDCbx->setFixedWidth(70);
    for (int i = 1; i < 10; i++)
        SceneIDCbx->addItem(QString().setNum(i));
    connect(SceneIDCbx, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeDungeonID(QString)));
}

void qtgameserver::createToolBars()
{
    infoToolBar = addToolBar(tr("File"));
    infoToolBar->addAction(MapScaleAct);
    infoToolBar->addAction(CellInfoAct);
    infoToolBar->addAction(TriangleAct);
    infoToolBar->addAction(BoundingBoxAct);

    infoToolBar->addWidget(ChannelIDCbx);
    infoToolBar->addWidget(DungeonIDCbx);
    infoToolBar->addWidget(SceneIDCbx);

}

void qtgameserver::SacleInfo()
{
    if(g_Scale > 0.09) 
        g_Scale = 0.05;
    else
        g_Scale = 0.1;

    m_pView->resetCachedContent();
}

void qtgameserver::CellInfo()
{
    m_pView->m_CellInfoType++;
    if(m_pView->m_CellInfoType > 2)
        m_pView->m_CellInfoType = 0;

    m_pView->resetCachedContent();
}

void qtgameserver::TriangleInfo()
{
    m_pView->m_TriangleInfoType++;
    if(m_pView->m_TriangleInfoType > 2)
        m_pView->m_TriangleInfoType = 0;

    m_pView->resetCachedContent();
}

void qtgameserver::BoundingBoxInfo()
{
    m_pView->m_BoundingBoxType++;
    if(m_pView->m_BoundingBoxType > 1)
        m_pView->m_BoundingBoxType = 0;

    m_pView->resetCachedContent();
}

void qtgameserver::ChangeDungeonID(const QString &)
{
    int channelId = ChannelIDCbx->currentText().toInt();
    int dungeonId = DungeonIDCbx->currentText().toInt();
    int sceneId = SceneIDCbx->currentText().toInt();

    m_pView->m_ChannelId = channelId;
    m_pView->m_DungeonId = dungeonId;
    m_pView->m_SceneId = sceneId;
    
}

void qtgameserver::timerEvent(QTimerEvent *event)
{
    Crown::ColorLog::Instance()->Update();

    bool isUpdate = false;
    int32 currentTime = m_Time.elapsed();
    while(currentTime - m_lastTime > g_ServerUpdateTime)
    {
        m_gameholder->Update(currentTime);
        m_lastTime += g_ServerUpdateTime;
        isUpdate = true;

        m_InfoTime += g_ServerUpdateTime;

        g_FrameCount++;
    }

    g_FrameUsedTime = m_Time.elapsed() - currentTime;
    
    if(isUpdate)
        m_pView->Update();

    if(m_InfoTime >= 1000)
        m_pView->resetCachedContent();

}