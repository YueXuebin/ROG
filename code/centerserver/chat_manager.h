#ifndef chat_manager_h__
#define chat_manager_h__

class ChatManager
{
public:
    ChatManager();
    ~ChatManager();

    DECLARE_SINGLETON(ChatManager)

    // IP禁言
    bool    IsChatBlockIP(uint32 ip);
    void    AddChatBlockIP(uint32 ip);
    void    RemoveChatBlockIP(uint32 ip);
    std::vector<uint32> GetChatBlockIPs() {return m_ChatBlockIPs;}

private:
    std::vector<uint32>     m_ChatBlockIPs;         // IP禁言列表
};


#endif // chat_manager_h__
