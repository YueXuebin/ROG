class DBExecutor;
class DBRecorder;

class Player;

#ifndef mall_job_h__
#define mall_job_h__

class MallJob
{
    MallJob();
    ~MallJob();

    DECLARE_SINGLETON(MallJob)

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val) { m_dbExecutor = val; } 

    void LoadSystemVersion(uint32 system_id, uint32& version);  ///< 加载数据库中的系统版本

    bool UpdateSystemVersion(uint32 system_id, uint32 version); ///< 更新数据库中的系统版本

    bool DelModifyMallInfo();                                   ///< 删除商城表中可删除的数据

    bool InsertNewMallInfo();                                   ///< 向DB中加入新的商城数据

    void LoadDBMallInfo();                                      ///< 从数据库中读取商品信息

private:
    enum
    {
        MAX_MALLINFO_SIZE = 20,
    };

    MallJob( const MallJob& );
    MallJob& operator=( const MallJob& );
    MallJob* operator&();
    const MallJob* operator&() const;

    DBExecutor*     m_dbExecutor;                               ///< SQL执行器
};

#endif
