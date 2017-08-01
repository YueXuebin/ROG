#ifndef mail_title_config_h__
#define mail_title_config_h__

typedef struct MailTitleData
{
    void Clear()
    {
        id = 0;
        title = "";
        message = "";
    }

    uint32      id;
    std::string title;
    std::string message;
}MailTitleData;

class MailTitleConfig
{
public:
    MailTitleConfig();
    ~MailTitleConfig();

    DECLARE_SINGLETON(MailTitleConfig)

    bool LoadMailTitleInfo(const char* path);

    MailTitleData* GetMailData(uint32 id);

    std::string GetMailTitle(uint32 id, ...);

    std::string GetMailMessage(uint32 id, ...);

private:
    std::map<uint32, MailTitleData> m_mailTitleData;
};

#endif // mail_title_config_h__
