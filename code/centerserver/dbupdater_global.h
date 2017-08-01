#ifndef dbupdater_global_h__
#define dbupdater_global_h__


class DBExecutor;
class DBRecorder;

#define DB_VERSION(major, minor, revision) ( (((uint64)major & 0xFFFFFF)<<40) | (((uint64)minor & 0xFFFFF)<<20) | ((uint64)revision & 0xFFFFF) )
#define DB_MAJOR(version) ((uint32)(((uint64)version >> 40) & 0xFFFFF))
#define DB_MINOR(version) ((uint32)(((uint64)version >> 20) & 0xFFFFF))
#define DB_REVISION(version) ((uint32)(version & 0xFFFFF ))


/*
 *	跨平台数据库的更新
 */
class DBUpdaterGlobal
{

public:
    DBUpdaterGlobal();
    virtual     ~DBUpdaterGlobal();

    DECLARE_SINGLETON(DBUpdaterGlobal)

    bool        Init(DBExecutor* executor);
    void        UnInit();

    bool        CheckVersionTable();                            // 检查是否存在版本表

    typedef bool (DBUpdaterGlobal::*UpdateFunc)();
    typedef struct UpdateContent
    {
        uint64      m_version;
        UpdateFunc  m_func;
        const char*       m_comment;
    } UpdateContent;

private:
    DBUpdaterGlobal( const DBUpdaterGlobal& );
    DBUpdaterGlobal& operator=( const DBUpdaterGlobal& );
    DBUpdaterGlobal* operator&();
    const DBUpdaterGlobal* operator&() const;

    bool    HasVersionTable();                              // 检查是否存在版本表
    void    AddAllUpdateVersion();                          // 将所有版本都添加到更新列表中
    void    PickOutUpdateVersion();                         // 挑选出更新版本
    bool    InsertUpdateVersion(UpdateContent* content);    // 插入新的版本信息

    bool    V_1();


    bool                                m_running;
    DBExecutor*                         m_executor;
    std::vector<UpdateContent*>         m_sys_version_list;
    std::vector<UpdateContent*>         m_update_version_list;
    std::vector<uint64>                 m_db_version_list;
    static UpdateContent                m_update_list[];
};


#endif // dbupdater_global_h__
