#include "robot_pch.h"
#include "chat_action.h"
#include "robot.h"

#include "chat_config.h"

ChatAction::ChatAction(Robot* pRobot, int32 change, int32 dstName, int32 chat) :
ActionBase(pRobot)
{
    m_Name = "chatAction";
    m_channel = change;
    m_chat    = Crown::ToString(chat);
    m_dst_name= Crown::ToString(dstName);
}

ChatAction::~ChatAction(void)
{

}


void ChatAction::OnStart()
{
    ActionBase::OnStart();
    CHAT_MSG req;
    req.channel  = 3;

    const std::map<int32, std::string>& chatList = ChatConfig::Instance()->GetChatList();

    int32 randomNum = Crown::SDRandom(chatList.size()-1);
    std::map<int32, std::string>::const_iterator it = chatList.find(randomNum);
    if (it == chatList.end())
        return;

    req.chat     = it->second;
    req.src_name = m_Robot->GetPropInfo().name;

    m_Robot->Send(MSG_CHAT_REQ,&req);

}


void ChatAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
    case MSG_CHAT_ACK:
        {
           CHAT_MSG* pData = (CHAT_MSG*)data;
           if(pData->errcode == ERR_SUCCEED)
           {
                CnDbg("³É¹¦ %d \n");
           }
        }
        End();
        break;
    }
}

