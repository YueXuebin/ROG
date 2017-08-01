#include "gameholder_pch.h"
#include "head_portrait_manager.h"
#include "player.h"
#include "team_manager.h"

HeadPortraitManager::HeadPortraitManager(Player* player):
m_player(player)
{
    Clear();
}

HeadPortraitManager::~HeadPortraitManager(void)
{
}

void HeadPortraitManager::Clear()
{
    m_head_portrait_info.Clear();
}

void HeadPortraitManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_HEAD_PORTRAIT_INFO_REQ:           // 设置头像
        OnSetHeadReq(data);
        break;
    case MSG_HEAD_PORTRAIT_INIT_REQ:           // 头像开启信息
        OnInitOpenInfoReq(data);
        break;
    default:
        break;
    }
}

void HeadPortraitManager::LoadInfo(const HEAD_PORTRAIT_INFO& info)
{
    m_head_portrait_info = info;
    // 由于有个CreateHeadPortrait() 会在创角的时候，自动分配默认头像到数据库，所以info一直都存在，所以，这里就给注释掉了
    if(m_head_portrait_info.cur == 0)
    {
       m_head_portrait_info.cur = 19000 + m_player->GetSex();
       m_head_portrait_info.open_list.push_back(m_head_portrait_info.cur);
       m_head_portrait_info.open_list.push_back(19003);
       m_head_portrait_info.open_list.push_back(19004);
       m_head_portrait_info.open_list.push_back(19005);
       m_head_portrait_info.open_list.push_back(19006);
    }
}

void HeadPortraitManager::FillInfo(HEAD_PORTRAIT_INFO& info) const
{
    info = m_head_portrait_info;
}

void HeadPortraitManager::OnSetHeadReq(void* data)
{
    uint16 error = ERR_FAILED;
    ERROR_CODE* headReq =(ERROR_CODE*) data;
    if(!headReq)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    ERROR_CODE sendMsg;
    sendMsg.errcode = 0;
    for (std::vector<unsigned int>::iterator it = m_head_portrait_info.open_list.begin(); it != m_head_portrait_info.open_list.end(); ++it)
    {
        if(headReq->errcode == *it)
        {
            m_head_portrait_info.cur = headReq->errcode;
            sendMsg.errcode = headReq->errcode;
            break;
        }
    }
    m_player->SendToGate(MSG_HEAD_PORTRAIT_INFO_ACK, &sendMsg);

    // 通知更新队伍信息
    if(sendMsg.errcode != 0)
        TeamManager::Instance()->UpdateTeamMemberInfo(m_player);

}

void HeadPortraitManager::OnInitOpenInfoReq(void* data)
{
    m_player->SendToGate(MSG_HEAD_PORTRAIT_INIT_ACK, &m_head_portrait_info);
}