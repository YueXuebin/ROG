//=========================================================================
// CenterServer��ʱ�䴥����
// Ŀǰ��Ҫ������Ͷ�GameServer�㲥
//=========================================================================
#ifndef __TIMING_TRIGGER_MANAGER_H__
#define __TIMING_TRIGGER_MANAGER_H__

class TimingTriggerManager
{
public:
    void OnRecv(uint32 msgID, void* data);

    DECLARE_SINGLETON(TimingTriggerManager)

    TimingTriggerManager();

    ~TimingTriggerManager();

private:
    void OnTimingTriggerStart(void* data);

    void OnTimingTriggerEnd(void* data);

    void OnChangeDay();
};

#endif