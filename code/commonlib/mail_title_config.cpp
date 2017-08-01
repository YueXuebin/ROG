#include "commonlib_pch.h"
#include "mail_title_config.h"

IMPLEMENT_SINGLETON(MailTitleConfig)

MailTitleConfig::MailTitleConfig()
{
    m_mailTitleData.clear();
}

MailTitleConfig::~MailTitleConfig()
{

}

bool MailTitleConfig::LoadMailTitleInfo(const char* path)
{
    MailTitleData mailTmpl;
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load mail title file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* titleEle = xmlRoot->FirstChildElement(); titleEle; titleEle = titleEle->NextSiblingElement())
    {
        if(strcmp(titleEle->Value(), "mail") == 0)
        {
            mailTmpl.Clear();

            mailTmpl.title = titleEle->Attribute("title");
            mailTmpl.message = titleEle->Attribute("message");
            titleEle->QueryUnsignedAttribute("id", &mailTmpl.id);

            if(m_mailTitleData.find(mailTmpl.id) != m_mailTitleData.end())
            {
                CnError("mail title id:%d is exist!\n", mailTmpl.id);
                continue;
            }

            m_mailTitleData[mailTmpl.id] = mailTmpl;
        }
    }

    return true;
}

MailTitleData* MailTitleConfig::GetMailData(uint32 id)
{
    std::map<uint32, MailTitleData>::iterator mailIt = m_mailTitleData.find(id);
    if(mailIt == m_mailTitleData.end())
    {
        return NULL;
    }
    return &mailIt->second;
}

std::string MailTitleConfig::GetMailTitle(uint32 id, ...)
{
    char titleBuff[MAX_MAIL_TITLE_LEN] = {0};
    MailTitleData* mailTitle = GetMailData(id);
    unsigned int nLen = 0;
    va_list arglist;

    if(!mailTitle) return titleBuff;

    va_start(arglist, id);

    vsnprintf(titleBuff, MAX_MAIL_TITLE_LEN, mailTitle->title.c_str(), arglist);

    va_end(arglist);

    return titleBuff;
}

std::string MailTitleConfig::GetMailMessage(uint32 id, ...)
{
    char messageBuff[MAX_MAIL_CONTENT_LEN] = {0};
    MailTitleData* mailTitle = GetMailData(id);
    unsigned int nLen = 0;
    va_list arglist;

    if(!mailTitle) return messageBuff;

    va_start(arglist, id);

    vsnprintf(messageBuff, MAX_MAIL_TITLE_LEN, mailTitle->message.c_str(), arglist);

    va_end(arglist);

    return messageBuff;
}
