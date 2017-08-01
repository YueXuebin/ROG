/*
    定时触发管理器
 */
#ifndef timing_trigger_manager_h__
#define timing_trigger_manager_h__

class TimingTriggerManager
{
public:
    TimingTriggerManager();
    ~TimingTriggerManager();

    DECLARE_SINGLETON(TimingTriggerManager)

    void Clear();
    void OnRecv(uint32 msgID, void* data);

private:
    void OnTimingTriggerStart(void* data);
    void OnTimingTriggerEnd(void* data);

    void OnChangeDay();

    void OnTimeFresh();
};

#endif // timing_trigger_manager_h__