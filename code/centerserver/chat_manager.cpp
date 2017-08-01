#include "centerserver_pch.h"
#include "chat_manager.h"


IMPLEMENT_SINGLETON(ChatManager)


ChatManager::ChatManager()
{

}

ChatManager::~ChatManager()
{

}


bool ChatManager::IsChatBlockIP( uint32 ip )
{
    for(uint32 i=0; i<m_ChatBlockIPs.size(); i++)
    {
        if(ip == m_ChatBlockIPs[i])
            return true;
    }
    return false;
}

void ChatManager::AddChatBlockIP( uint32 ip )
{
    if(IsChatBlockIP(ip))
        return;

    m_ChatBlockIPs.push_back(ip);
}

void ChatManager::RemoveChatBlockIP( uint32 ip )
{
    if(!IsChatBlockIP(ip))
        return;

    for(std::vector<uint32>::iterator iter = m_ChatBlockIPs.begin(); iter != m_ChatBlockIPs.end(); iter++)
    {
        if(ip == *iter)
        {
            m_ChatBlockIPs.erase(iter);
            return;
        }
    }
}

