/*
*   游戏逻辑模块接口
*/
#ifndef gameholder_h__
#define gameholder_h__

#include "game_define.h"

class GameWorld;

class GameHolder
{
public:
    GameHolder();                                                   // 构造
    ~GameHolder();                                                  // 析构

    static GameHolder* Instance() {return m_instance;}

    bool Init(uint32 server_id);                                    // 初始化
    void Uninit();                                                  // 卸载

    void Update(uint32 frame_count);                                // 更新
    void UpdateNet();                                               // 更新网络

    void OnCrashExit();

    GameWorld* GetGameWorld() {return m_pGameWorld;}
    uint32  GetHeartBeat() {return m_heartBeat;}

    void RequestExit();                                             // 请求安全退出
    bool CanExit();                                                 // 可以安全退出
    bool IsExit() {return m_IsExit;}

    int32 sysLoadPercent;                                           // GameServer的负载(由外部设置)
private:

private:
    bool                                    m_IsExit;               // 是否要安全退出了

    static GameHolder*                      m_instance;             // 实例

    GameWorld*	                            m_pGameWorld;           // 游戏世界

    uint32                                  m_heartBeat;            // 上次心跳的滴答

};


#endif // gameholder_h__
