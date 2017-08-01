#ifndef QTGAMESERVER_H
#define QTGAMESERVER_H

#include <QtGui/QMainWindow>
#include "ui_qtgameserver.h"
#include <QTime>

class QtGameView;

extern float g_Scale;
extern uint32 gServerID;

class qtgameserver : public QMainWindow
{
    Q_OBJECT

public:
    qtgameserver(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~qtgameserver();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void createActions();
    void createToolBars();

private slots:
    void SacleInfo();
    void CellInfo();
    void TriangleInfo();
    void BoundingBoxInfo();
    void ChangeDungeonID(const QString &);

private:
    Ui::qtgameserverClass ui;
    QToolBar *infoToolBar;
    QAction *MapScaleAct;
    QAction *CellInfoAct;
    QAction *TriangleAct;
    QAction *BoundingBoxAct;

    QComboBox* ChannelIDCbx;
    QComboBox* DungeonIDCbx;
    QComboBox* SceneIDCbx;

    GameHolder* m_gameholder;

    QtGameView* m_pView;
    QTime   m_Time;

    int32   m_lastTime;
    int32   m_InfoTime;
};

#endif // QTGAMESERVER_H
