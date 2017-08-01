/*
 *	转生链，数据结构
 */
#ifndef reincarnated_data_h__
#define reincarnated_data_h__

const int32 MAX_REINCARNATED_COUNT = 10;         // 最大转生10次

class ReincarnatedData
{
public:
    uint32  m_BossId;       // 转生boss id
    int32   m_Num;          // 转生数
    std::string m_AIName;   // 转生AI

    ReincarnatedData()
    {
        Clear();
    }

    void Clear()
    {
        m_BossId = 0;
        m_Num = 0;
        m_AIName.clear();
    }

    bool IsClear()
    {
        return (m_BossId == 0 && m_Num == 0);
    }
};

class ReincarnatedList
{
public:
    ReincarnatedList()
    {
        m_CurrentMax = 0;
    }

    void    Clear();
    void    Add(const ReincarnatedData& data);
  
    ReincarnatedData    Pop();


private:
    ReincarnatedData    list[MAX_REINCARNATED_COUNT];
    int32   m_CurrentMax;
};

#endif // reincarnated_data_h__
