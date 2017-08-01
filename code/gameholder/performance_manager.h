#ifndef performance_manager_h__
#define performance_manager_h__

#include "performance_node.h"

class PerformanceManager
{
public:
    PerformanceManager();
    ~PerformanceManager();

    DECLARE_SINGLETON(PerformanceManager)

    void Init();

    void Begin(const std::string& name);
    void End(const std::string& name);

    void Update();

    void RegisterNode(PerformanceNode* pNode);
    void UnRegisterNode(PerformanceNode* pNode);

    uint32  GetTimeIndex() {return m_time_index;}
    PerformanceNode*    GetRoot() {return m_root;}
    PerformanceNode*    GetFrame() {return m_frame;}

    void SetStop(bool stop);
    bool IsStop() {return m_stop;}

private:
    PerformanceNode*    m_root;
    PerformanceNode*    m_frame;            // 统计帧时长
    PerformanceNode*    m_current;

    std::map<std::string, PerformanceNode*>     m_NodeMap;

    uint32  m_time_index;
    bool    m_stop;
};


#endif // performance_manager_h__
