/*
 *	ID池管理器(32位ID负责的维护与回收,确保ID不会重复)
 */
#ifndef id_pool_manager_h__
#define id_pool_manager_h__

class IdPoolSegment;
class IdPoolOctet;

class IdPoolManager
{
public:
    IdPoolManager();
    ~IdPoolManager();

    DECLARE_SINGLETON(IdPoolManager)

    bool    Init();

    bool    RegisterID(uint32 id);      // 注册ID，如果已经注册了该ID，返回false
    bool    UnregisterID(uint32 id);    // 注销ID，如果已经注销了该ID，返回false

    void    CleanZero();                // 删除没有注册的ID段

    Crown::CSDObjectPool<IdPoolSegment>*    GetSegmentPool() {return m_SegmentPool;}
    Crown::CSDObjectPool<IdPoolOctet>*    GetOctetPool() {return m_OctetPool;}

private:
    IdPoolSegment*   m_RootSegment;

    Crown::CSDObjectPool<IdPoolSegment>*     m_SegmentPool;
    Crown::CSDObjectPool<IdPoolOctet>*       m_OctetPool;

};


#endif // id_pool_manager_h__
