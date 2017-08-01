#ifndef performance_node_h__
#define performance_node_h__

const uint32 RECORD_MAX = 200;


class PerformanceNode
{
public:
    PerformanceNode(const std::string& name, PerformanceNode* parent);
    ~PerformanceNode();

    const std::string& GetName() {return m_name;}
    const std::string& GetFullName() {return m_fullname;}
    
    uint32 DeltaTime() {return delta_time;}

    PerformanceNode*    GetParent() {return m_parent;}
    PerformanceNode*    GetChildByName(const std::string& name);
    PerformanceNode*    GetChildByIndex(uint32 index);
    uint32              GetChildNum() {return m_Children.size();}

    uint32              GetEntryNum() {return m_reEntry;}

    void Start();
    void End();
    void Update();

    uint32* GetDeltaTimes() {return delta_times;}

private:
    void    AddChild(PerformanceNode* pChild);

private:
    PerformanceNode*    m_parent;
    std::vector<PerformanceNode*>   m_Children;

    std::string m_name;
    std::string m_fullname;

    uint32  start_time;
    uint32  end_time;
    uint32  delta_time;

    uint32  reEntry;
    uint32  m_reEntry;

    uint32  delta_times[RECORD_MAX];
};


#endif // performance_node_h__
