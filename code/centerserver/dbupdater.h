#ifndef DBUPDATER_H
#define DBUPDATER_H



class DBExecutor;
class DBRecorder;

#define DB_VERSION(major, minor, revision) ( (((uint64)major & 0xFFFFFF)<<40) | (((uint64)minor & 0xFFFFF)<<20) | ((uint64)revision & 0xFFFFF) )
#define DB_MAJOR(version) ((uint32)(((uint64)version >> 40) & 0xFFFFF))
#define DB_MINOR(version) ((uint32)(((uint64)version >> 20) & 0xFFFFF))
#define DB_REVISION(version) ((uint32)(version & 0xFFFFF ))

class DBUpdater
{
public:
    DBUpdater();
    virtual     ~DBUpdater();

    DECLARE_SINGLETON(DBUpdater)

    bool        Init(DBExecutor* executor);
    void        UnInit();

    bool        CheckVersionTable();                            // 检查是否存在版本表

    typedef bool (DBUpdater::*UpdateFunc)();
    typedef struct UpdateContent
    {
        uint64      m_version;
        UpdateFunc  m_func;
        const char*       m_comment;
    } UpdateContent;

private:
    DBUpdater( const DBUpdater& );
    DBUpdater& operator=( const DBUpdater& );
    DBUpdater* operator&();
    const DBUpdater* operator&() const;

    bool    HasVersionTable();                              // 检查是否存在版本表
    void    AddAllUpdateVersion();                          // 将所有版本都添加到更新列表中
    void    PickOutUpdateVersion();                         // 挑选出更新版本
    bool    InsertUpdateVersion(UpdateContent* content);    // 插入新的版本信息

    bool    V_1(), V_2(), V_3(), V_4(), V_5(), V_6(), V_7(), V_8(), V_9(), V_10(), V_11(), V_12(), V_13(), V_14(), V_15(), V_16(), V_17(), V_18(), V_19(), V_20(),
        V_21(), V_22(), V_23(), V_24(), V_25(), V_26(), V_27(), V_28(),V_29(),V_30(), V_31();

    bool                                m_running;
    DBExecutor*                         m_executor;
    std::vector<UpdateContent*>         m_sys_version_list;
    std::vector<UpdateContent*>         m_update_version_list;
    std::vector<uint64>                 m_db_version_list;
    static UpdateContent                m_update_list[];
};

#endif // STOREAUTOUPADTE_H

