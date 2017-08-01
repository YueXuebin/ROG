#ifndef DBDRIVER_H
#define DBDRIVER_H

#include "sdu.h"

#ifdef LINUX
#include <semaphore.h>
#include <fcntl.h>
#endif

#include "message_encoder.h"

#include "mysql.h"
#include "errmsg.h"

class DBRecorder;
class DBExecutor;

//#define QUERY_BODY(fmt, buf)    \
//    if (!fmt) return false;     \
//    char buf [MAX_QUERY_LEN];   \
//{                               \
//    va_list ap;                 \
//    va_start(ap, fmt);          \
//    int m_res = vsnprintf(buf, MAX_QUERY_LEN, fmt, ap); \
//    va_end(ap);                 \
//    if (m_res < 0)                \
//    return false;               \
//}

#define QUERY_BODY  \
    QueryUnit* sqlcall = 0;         \
    int res = 0;                    \
    va_list ap;                     \
    if (!fmt) return;               \
    sqlcall = CnNew QueryUnit();    \
    va_start(ap, fmt);              \
    res = vsnprintf(sqlcall->m_sqlTxt, MAX_SQLTXT_SAVE_LEN, fmt, ap); \
    va_end(ap);                     \
    if (res < 0)                    \
{                               \
    CnDelete sqlcall;           \
    sqlcall = 0;                \
    return;                     \
}

// ================================================================================

typedef struct FieldInfo
{
    enum        enum_field_types mType;     // 字段类型
    uint32      mFlags;
}FieldInfo;

// ================================================================================

typedef struct QueryResult
{
    MYSQL_RES*              mReslut;        // 查詢結果
    uint64                  mRowNum;        // 行数
    uint32                  mColNum;        // 列数
    int64                   mAffNum;        // 影响行数
    FieldInfo*              mFieldInfo;
}QueryResult;

// ================================================================================

class IQueryCallback
{
public:
    IQueryCallback() {}
    virtual ~IQueryCallback() {}
    virtual void Call(DBRecorder& res) = 0;
};

// ================================================================================

class QueryUnit
{
public:
    QueryUnit() :
    m_callback(0), m_execute(FALSE)
    {
        memset(&m_res, 0, sizeof(QueryResult));
    }

    ~QueryUnit()
    {
        Free();
    }

    void Free()
    {
        if(m_callback)
        {
            CnDelete m_callback;
            m_callback = 0;
        }

        if(m_res.mReslut)
        {
            mysql_free_result( m_res.mReslut );
            m_res.mReslut = 0;
        }

        if(m_res.mFieldInfo)
        {
            CnDelete[] m_res.mFieldInfo;
            m_res.mFieldInfo = 0;
        }
    }

    char            m_sqlTxt[MAX_SQLTXT_SAVE_LEN];
    IQueryCallback* m_callback;
    QueryResult     m_res;
    BOOL            m_execute;
};

// ================================================================================
#ifdef DEBUG
const bool DB_CHECK = true;         // 数据库读取验证
#else
const bool DB_CHECK = false;
#endif

class DBRecorder
{
public:
    DBRecorder();
    DBRecorder(QueryUnit* unit);
    virtual                ~DBRecorder();

    DBRecorder&             operator=(QueryUnit* unit);

    void                    Free();

    BOOL                    HasRecord();            // 可以判断 SQL 正常执行，且有结果
    BOOL                    HasExecut();            // 只能判断 SQL 正常执行
    uint64                  GetRowNum();            // 获得 结果集 的行数
    int64                   GetAffNum();            // 获得 受影响 的行数
    
    uint32                  GetCol() {return mCol;}

    BOOL                    GetRecord(int8& val, bool check = DB_CHECK);
    BOOL                    GetRecord(uint8& val, bool check = DB_CHECK);
    BOOL                    GetRecord(int16& val, bool check = DB_CHECK);
    BOOL                    GetRecord(uint16& val, bool check = DB_CHECK);
    BOOL                    GetRecord(int32& val, bool check = DB_CHECK);
    BOOL                    GetRecord(uint32& val, bool check = DB_CHECK);
    BOOL                    GetRecord(int64& val, bool check = DB_CHECK);
    BOOL                    GetRecord(uint64& val, bool check = DB_CHECK);
    BOOL                    GetRecord(float& val, bool check = DB_CHECK);
    BOOL                    GetRecord(double& val, bool check = DB_CHECK);
    BOOL                    GetRecord(std::string& val, bool check = DB_CHECK);
    BOOL                    GetRecord(void* buf, uint32 bufLen, bool check = DB_CHECK);

    BOOL                    GetRecord(uint64 row, uint32 col, int8& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, uint8& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, int16& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, uint16& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, int32& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, uint32& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, int64& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, uint64& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, float& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, double& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, std::string& val, bool check);
    BOOL                    GetRecord(uint64 row, uint32 col, void* buf, uint32 bufLen, bool check);

private:
    DBRecorder( const DBRecorder& );
    DBRecorder& operator=( const DBRecorder& );
    DBRecorder* operator&();
    const DBRecorder* operator&() const;

    BOOL                    IsValidParam( uint64 row, uint32 col );
    void                    Next();
    void                    OnError();

    uint64                  mRow;
    uint32                  mCol;

    QueryUnit*              mUnit;
    QueryResult*            mResult;
};

// ================================================================================

class IDBDriver
{
public:
    IDBDriver() {}
    virtual ~IDBDriver() { UnInit(); }

    static BOOL     Init()
    {
        if(mCount < 1)
        {
            if (mysql_library_init (0, NULL, NULL))
            {
                return FALSE;
            }
        }
        ++mCount;
        return TRUE;
    }

    static void     UnInit()
    {
        --mCount;
        if(mCount < 1)
        {
            mysql_library_end();
        }
    }

    BOOL            IsConnected();
    BOOL            Connect( const char* db, const char* server, const char* user, const char* password, uint32 port, const char* charset);
    void            DisConnect();

    BOOL            StartTransaction();
    BOOL            Rollback();
    BOOL            Commit();
    BOOL            EndTransaction();

    uint64          EscapeString(char *to, const char *from, uint64 length);

    bool            Query( QueryUnit* unit );          ///< 查询函数

    MYSQL*          GetMySQL();

protected:
    static int      mCount;
    BOOL            mIsConnected;
    MYSQL*          mMySQL;
};

// ================================================================================

// DBDriverA 用作对数据库的异步查询操作
class DBDriverAsync : public IDBDriver, public Crown::CSDThread
{
public:
    friend class    DBRecorder;

    DBDriverAsync();
    virtual         ~DBDriverAsync();

    void            SetSemaphore(Crown::CSDNamedSemaphore* sema) {m_WaitForHandle = sema;}
    void             SetExecutor(DBExecutor* executor); ///< 设置数据库执行器

    void             Run();                             ///< 线程执行函数
    void             Stop() {mIsRun = false;}           ///< 停止线程

    void SDAPI ThrdProc()
    {
        Run();
    }

private:
    DBDriverAsync( const DBDriverAsync& );
    DBDriverAsync& operator=( const DBDriverAsync& );
    DBDriverAsync* operator&();
    const DBDriverAsync* operator&() const;

    Crown::CSDNamedSemaphore*  m_WaitForHandle;

    bool            mIsRun;
    DBExecutor*     mExecutor;
};

// ================================================================================

// DBDriverB 用作对数据库的阻塞查询操作
class DBDriverBlock : public IDBDriver
{
public:
    friend class    DBRecorder;

    DBDriverBlock();
    virtual         ~DBDriverBlock();

private:
    DBDriverBlock( const DBDriverBlock& );
    DBDriverBlock& operator=( const DBDriverBlock& );
    DBDriverBlock* operator&();
    const DBDriverBlock* operator&() const;
};

// ================================================================================
// 增加参数时注意
// 第一个为模板定义,要=void
// 后面的模板特例化
// class QueryCallback<<T, T1, T2, T3, T4, T5>
// ================================================================================
template<typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void>
class QueryCallback : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1, T2, T3, T4, T5, T6);

    QueryCallback(T* obj, CBFunc func, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) :
    m_obj(obj), m_cbfunc(func), m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;
    T2      m_t2;
    T3      m_t3;
    T4      m_t4;
    T5      m_t5;
    T6      m_t6;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1, m_t2, m_t3, m_t4, m_t5, m_t6);
    }
};

template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
class QueryCallback<T, T1, T2, T3, T4, T5> : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1, T2, T3, T4, T5);

    QueryCallback(T* obj, CBFunc func, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) :
    m_obj(obj), m_cbfunc(func), m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;
    T2      m_t2;
    T3      m_t3;
    T4      m_t4;
    T5      m_t5;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1, m_t2, m_t3, m_t4, m_t5);
    }
};

template<typename T, typename T1, typename T2, typename T3, typename T4>
class QueryCallback<T, T1, T2, T3, T4> : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1, T2, T3, T4);

    QueryCallback(T* obj, CBFunc func, T1 t1, T2 t2, T3 t3, T4 t4) :
    m_obj(obj), m_cbfunc(func), m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;
    T2      m_t2;
    T3      m_t3;
    T4      m_t4;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1, m_t2, m_t3, m_t4);
    }
};

template<typename T, typename T1, typename T2, typename T3>
class QueryCallback<T, T1, T2, T3> : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1, T2, T3);

    QueryCallback(T* obj, CBFunc func, T1 t1, T2 t2, T3 t3) :
    m_obj(obj), m_cbfunc(func), m_t1(t1), m_t2(t2), m_t3(t3)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;
    T2      m_t2;
    T3      m_t3;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1, m_t2, m_t3);
    }
};

template<typename T, typename T1, typename T2>
class QueryCallback<T, T1, T2> : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1, T2);

    QueryCallback(T* obj, CBFunc func, T1 t1, T2 t2) :
    m_obj(obj), m_cbfunc(func), m_t1(t1), m_t2(t2)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;
    T2      m_t2;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1, m_t2);
    }
};

template<typename T, typename T1>
class QueryCallback<T, T1> : public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&, T1);

    QueryCallback(T* obj, CBFunc func, T1 t1) :
    m_obj(obj), m_cbfunc(func), m_t1(t1)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;
    T1      m_t1;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res, m_t1);
    }
};

template<typename T>
class QueryCallback<T>: public IQueryCallback
{
public:
    typedef void (T::*CBFunc)(DBRecorder&);

    QueryCallback(T* obj, CBFunc func) :
    m_obj(obj), m_cbfunc(func)
    {

    }
private:
    T*      m_obj;
    CBFunc  m_cbfunc;

    void Call(DBRecorder& res)
    {
        (m_obj->*m_cbfunc)(res);
    }
};

// ================================================================================

class DBExecutor
{
public:
    DBExecutor();
    ~DBExecutor();

    BOOL            Init(const char* signalname, uint8 connNum, const char* database, const char* ip, 
        const char* user, const char* password, uint32 port, const char* charset);
    void            UnInit();

    const char*     GetSignalName() { return mSignalName; }

    void            Update()
    {
        for(QueryUnit* sqlcall = CallbackPopFront(); sqlcall; sqlcall = CallbackPopFront())
        {
            DBRecorder recorder(sqlcall);
            sqlcall->m_callback->Call(recorder);
            //CnDelete sqlcall;
        }
    }

    uint64          EscapeString(char *to, const char *from, uint64 length)
    {
        if(!mDBDriverAsyncList)
            return 0;
        return mDBDriverAsyncList[0].EscapeString(to, from, length);
    }

    QueryUnit*        QueryPopFront()
    {
        QueryUnit* sqlcall = NULL;
        QueryLock();
        if(!mSQLList.empty())
        {
            sqlcall = mSQLList.front();
            mSQLList.pop_front();
        }
        QueryUnLock();
        return sqlcall;
    }

    bool StartTransaction()
    {
        if (mysql_autocommit(mDBDriverBlock->GetMySQL(), 0) != 0)
        {
            return false;
        }
        return true;
    }

    bool Rollback()
    {
        if (mysql_rollback(mDBDriverBlock->GetMySQL()) != 0)
        {
            return false;
        }
        return true;
    }

    bool Commit()
    {
        if (mysql_commit(mDBDriverBlock->GetMySQL()) != 0)
        {
            return false;
        }
        return true;
    }

    bool EndTransaction()
    {
        if (mysql_autocommit(mDBDriverBlock->GetMySQL(), 1) != 0)
        {
            return false;
        }
        return true;
    }

    void            QueryPushBack(QueryUnit* sqlcall)
    {
        QueryLock();

        mSQLList.push_back(sqlcall);
        m_Semaphore->Post();

        QueryUnLock();
    }

    QueryUnit*        CallbackPopFront()
    {
        QueryUnit* sqlcall = NULL;
        CallbackLock();
        if(!mCallList.empty())
        {
            sqlcall = mCallList.front();
            mCallList.pop_front();
        }
        CallbackUnLock();
        return sqlcall;
    }

    void            CallbackPushBack(QueryUnit* sqlcall)
    {
        CallbackLock();
        mCallList.push_back(sqlcall);
        CallbackUnLock();
    }

    QueryUnit* Query(const char *fmt, ...);
    QueryUnit* QueryMem(const char *fmt);

    template<typename T>
    void Query(T* obj, void (T::*method)(DBRecorder&), const char *fmt, ...);
    template<typename T, typename T1>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1), T1 t1, const char *fmt, ...);
    template<typename T, typename T1, typename T2>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1, T2), T1 t1, T2 t2, const char *fmt, ...);
    template<typename T, typename T1, typename T2, typename T3>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3), T1 t1, T2 t2, T3 t3, const char *fmt, ...);
    template<typename T, typename T1, typename T2, typename T3, typename T4>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4), T1 t1, T2 t2, T3 t3, T4 t4, const char *fmt, ...);
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4, T5), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, const char *fmt, ...);
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    void Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4, T5, T6), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, const char *fmt, ...);
private:
    // 查询列表锁
    void            QueryLock() { mQueryLock.Lock(); }
    void            QueryUnLock() { mQueryLock.Unlock(); }
    // 结果列表锁
    void            CallbackLock() { mCallbackLock.Lock(); }
    void            CallbackUnLock() { mCallbackLock.Unlock(); }

    enum
    {
        MAX_SEMAPHORE_NUM = 65535
    };

    Crown::CSDMutex         mQueryLock;
    Crown::CSDMutex         mCallbackLock;

    Crown::CSDNamedSemaphore*      m_Semaphore;

    char                    mSignalName[128];

    int32                   mConnNum;
    std::list<QueryUnit*>   mSQLList;
    std::list<QueryUnit*>   mCallList;
    DBDriverAsync*          mDBDriverAsyncList;
    DBDriverBlock*          mDBDriverBlock;
    uint8                   mDBNum;
};

// --------------------------------------------------------------------------------

template<typename T>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&), const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T>(obj, method);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1), T1 t1, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1>(obj, method, t1);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1, typename T2>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1, T2), T1 t1, T2 t2, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1, T2>(obj, method, t1, t2);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1, typename T2, typename T3>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3), T1 t1, T2 t2, T3 t3, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1, T2, T3>(obj, method, t1, t2, t3);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1, typename T2, typename T3, typename T4>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4), T1 t1, T2 t2, T3 t3, T4 t4, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1, T2, T3, T4>(obj, method, t1, t2, t3, t4);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4, T5), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1, T2, T3, T4, T5>(obj, method, t1, t2, t3, t4, t5);

    QueryPushBack(sqlcall);
}

template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void DBExecutor::Query(T* obj, void (T::*method)(DBRecorder&, T1, T2, T3, T4, T5, T6), T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, const char *fmt, ...)
{
    QUERY_BODY;

    sqlcall->m_callback = CnNew QueryCallback<T, T1, T2, T3, T4, T5, T6>(obj, method, t1, t2, t3, t4, t5, t6);

    QueryPushBack(sqlcall);
}

// ================================================================================

#endif // DBDriver_H
