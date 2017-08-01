#ifndef content_dictionary_job_h__
#define content_dictionary_job_h__

class DBExecutor;
class DBRecorder;

class ContentDictionaryJob
{
public:
    enum ContentKey
    {
        RANDOM_PACKAGE_KEY = 1
    };

public:
    ContentDictionaryJob(){m_dbExecutor = NULL;}
    ~ContentDictionaryJob(){}

    DECLARE_SINGLETON(ContentDictionaryJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadInfo();

    void SaveInfo();

private:
    DBExecutor* m_dbExecutor;

};

#endif