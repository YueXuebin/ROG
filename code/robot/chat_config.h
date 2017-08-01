// 读取聊天文件
#ifndef chat_config_h__
#define chat_config_h__

class ChatConfig
{
public:
    ChatConfig();
    ~ChatConfig();

    DECLARE_SINGLETON(ChatConfig)

    bool LoadConfig();

    const std::map<int32, std::string>& GetChatList() {return m_chatList;}
private:
    std::map<int32, std::string> m_chatList;
};


#endif // chat_config_h__
