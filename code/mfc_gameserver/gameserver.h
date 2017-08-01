/*
 *	GameServerÄ£¿é
 */
#ifndef gameserver_h__
#define gameserver_h__

class GameHolder;

class GameServer
{
public:
    GameServer();
    ~GameServer();

    bool Init();
    void Uninit();

    bool IsInit() {return m_Init;}

    void Tick();

private:
    bool    m_Init;
    GameHolder* m_gameholder;

    uint32 m_lastTime;
};

extern GameServer g_GameServer;

extern uint32  g_FrameCount;

#endif // gameserver_h__
