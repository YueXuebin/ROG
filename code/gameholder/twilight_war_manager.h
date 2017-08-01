/*
    暮光之战战管理器
 */
#ifndef twilight_war_manager_h__
#define twilight_war_manager_h__

class Player;

class TwilightWarManager
{
public:
    TwilightWarManager();
    ~TwilightWarManager();

    DECLARE_SINGLETON(TwilightWarManager)

    void Clear();

    void Update(uint32 frame_count);

    // 接收信息
    void OnRecv(uint32 msgID, void* data, Player* player=0);

    uint32 GetCurrTime() { return m_currTime; }

    bool IsStart() { return m_isStart; }

    // 开启暮光之战
    void StartTwilightWar();

    // 停止暮光之战
    void StopTwilightWar();

private:

    // 暮光之战开启
    void OnTwilightWarStart(void* data);

    // 暮光之战结束
    void OnTwilightWarEnd(void* data);

    uint32  m_currTime;
    bool    m_isStart;
};

#endif
// twilight_war_manager_h__
