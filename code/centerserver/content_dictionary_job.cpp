#include "centerserver_pch.h"
#include "content_dictionary_job.h"
#include "dbdriver.h"
#include "random_package_manager.h"

IMPLEMENT_SINGLETON(ContentDictionaryJob)

void ContentDictionaryJob::LoadInfo()
{
    DBRecorder res;

    res = m_dbExecutor->Query("SELECT `id`, `content` FROM `content_dictionary`;");
    if(!res.HasExecut())
    {
        CnError("Load Content Dictionary Job failed\n");
        return;
    }

    uint32 id(0);
    std::string content("");
    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(id);
        res.GetRecord(content);

        switch(id)
        {
        case RANDOM_PACKAGE_KEY:
            {
                RANDOM_PACKAGE_RECORD_INFO& info = RandomPackageManager::Instance()->GetRecordInfo();
                JsonToStruct(info, content);
            }
            break;
        }
    }
}

void ContentDictionaryJob::SaveInfo()
{
    DBRecorder res;
    std::string content("");

    RANDOM_PACKAGE_RECORD_INFO& info = RandomPackageManager::Instance()->GetRecordInfo();
    content = StructToJson(info);

    res = m_dbExecutor->Query("INSERT INTO `content_dictionary` VALUES(%u, '%s') ON DUPLICATE KEY UPDATE `content`='%s'",
        RANDOM_PACKAGE_KEY, content.c_str(), content.c_str());

    if(!res.HasExecut())
    {
        CnError("Save Content Dictionary Job failed\n");
        return;
    }
}
