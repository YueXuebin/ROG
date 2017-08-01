#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H


class Player;

// ==================================================
/// 角色的聊天管理系统
// ==================================================
class ChatManager
{
public:
    ChatManager(Player* player);
    ~ChatManager();

    Player* GetPlayer() { return m_player; }                                // 获得角色指针

    void    OnRecv(uint32 msgID, void* data);

private:
    void    OnChatReq(void* data);                                          // 聊天请求

    bool    CanChat(const CHAT_MSG* pChat);                                 // 检查是否能聊天

    bool    IsChatFrequently(const CHAT_MSG* pChat);                        // 检查聊天是否频繁

    void    ParseDebugCommand(const std::string& debugCmd);                 // 调试命令

    void    SendLog(const std::string& strCmd);                             // 发送 Log

    void    OnMarqueeReq(void * data);                                      // marquee

private:
    Player*             m_player;
    uint32              m_chat_time;                                        // 聊天时间

};

#endif
