#ifndef capture_flag_manager_h__
#define capture_flag_manager_h__

// 夺旗管理器

class CaptureFlagManager
{
public:
    CaptureFlagManager();
    ~CaptureFlagManager();

    DECLARE_SINGLETON(CaptureFlagManager)

    void Clear();

    bool    CanEnter();

    void    Update(uint32 curr_tick);
    void    OnRecv(uint32 msgID, void* data);

    uint32  GetCurrTime() {return m_currTime;}                  // 毫秒
    bool    IsOpen() {return m_IsOpen;}
private:
    // 活动开启
    void    OnActivityStart(void* data);
    void    OnActivityEnd();
private:
    uint32  m_currTime;
    bool    m_IsOpen;
};

#endif // capture_flag_manager_h__
