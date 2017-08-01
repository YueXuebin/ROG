#include "gameholder_pch.h"
#include "shorcut_manager.h"
#include "shorcut_config.h"
#include "player.h"
#include "item_manager.h"
#include "scene.h"
#include "event_define.h"

ShorcutManager::ShorcutManager(Player* player):
m_player(player)
{
    Clear();
}

ShorcutManager::~ShorcutManager(void)
{
}

// ==================================================
// 清除
// ==================================================
void ShorcutManager::Clear()
{
   m_shorcut_info.Clear();
}


// ==================================================
// 接收消息
// ==================================================
void ShorcutManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case MSG_PLAYER_SHORCUT_REQ:
        {
            OnSetShorcutReq(data);          // 设置时装
        }
        break;
    case MSG_PLAYER_TASTE_VIP_REQ:
        {
            OnTasteVipReq(data);          // vip体验卡
        }
        break;
    case MSG_PLAYER_LIKE_REQ:
        {
            OnSetLikeReq(data);          // fb点赞奖励
        }
        break;
    case MSG_PLAYER_DAILY_FASHION_REQ:
        {
            OnDailyFashionReq(data);    // 每日化身
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 加载游戏收藏信息
// ==================================================
void ShorcutManager::LoadShorcutInfo(SHORCUT_BAG& info)
{
     m_shorcut_info = info;
}

// ==================================================
// 填充游戏收藏信息
// ==================================================
void ShorcutManager::FillShorcutInfo(SHORCUT_BAG& info)
{
    info = m_shorcut_info;
}

void ShorcutManager::OnSetShorcutReq(void* data)
{
    if(m_shorcut_info.shorcut_flag == 0)
    {
        const REWARD_TABLE* rList = ShorcutConfig::Instance()->GetRewardList(SHORCUT_TYPE_SHORCUT);
        
        // 检查包裹
        //if(!m_player->GetItemManager()->CanAddRewardTable(*rList))
        //{
        //     m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        //    return;
        //}

        //for (std::vector<REWARD_ITEM>::const_iterator re = rList->items.begin(); re != rList->items.end(); re++)
        //{
        //    
        //    m_player->GetItemManager()->AddItemByType(re->type, re->id, re->num, REWARD_REASON_SHORCUT_REWARD);
        //}

        m_shorcut_info.shorcut_flag = 1;
        
        m_player->SendToGate(MSG_PLAYER_SHORCUT_ACK, &m_shorcut_info);
    }
    else
    {
    	return;
    }
}

void ShorcutManager::OnTasteVipReq( void* data )
{
    if(m_shorcut_info.taste_vip == 0)
    {
        const REWARD_TABLE* rList = ShorcutConfig::Instance()->GetRewardList(SHORCUT_TYPE_TASTE_VIP);

        // 检查包裹
        //if(!m_player->GetItemManager()->CanAddRewardTable(*rList))
        //{
        //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        //    return;
        //}

        //for (std::vector<REWARD_ITEM>::const_iterator re = rList->items.begin(); re != rList->items.end(); re++)
        //{
        //    m_player->GetItemManager()->AddItemByType(re->type, re->id, re->num, REWARD_REASON_SHORCUT_REWARD);
        //}

        m_shorcut_info.taste_vip = 1;

        SHORCUT_BAG sendMsg;
        sendMsg.taste_vip = m_shorcut_info.taste_vip;
        m_player->SendToGate(MSG_PLAYER_TASTE_VIP_ACK, &sendMsg);
    }
    else
    {
        return;
    }
}

void ShorcutManager::OnSetLikeReq( void* data )
{
    if(m_shorcut_info.like_flag < 1)
    {
        const REWARD_TABLE* rList = ShorcutConfig::Instance()->GetRewardList(SHORCUT_TYPE_LIKE);
        if(rList == NULL)
        {
            CnDbg("Get Like SHORCUT_TYPE_LIKE Reward Failed\n");
            return;
        }

        // 检查包裹
        //if(!m_player->GetItemManager()->CanAddRewardTable(*rList))
        //{
        //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        //    CnDbg("Get Like SHORCUT_TYPE_LIKE Reward ERR_NO_ENOUGH_BAGPOS\n");
        //    return;
        //}

        //for (std::vector<REWARD_ITEM>::const_iterator re = rList->items.begin(); re != rList->items.end(); re++)
        //{
        //    m_player->GetItemManager()->AddItemByType(re->type, re->id, re->num, REWARD_REASON_FACEBOOK_LIKE);
        //}

        m_shorcut_info.like_flag = 1;
       
        m_player->SendToGate(MSG_PLAYER_SHORCUT_ACK, &m_shorcut_info);
        m_player->GetScene()->SendEvent(0, EVENT_FB_LIKE_TITLE, m_shorcut_info.like_flag, 0, 0);
        CnDbg("Get Like SHORCUT_TYPE_LIKE Reward success\n");
    }
    else
    {
        CnDbg("Get Like flag Failed\n");
        return;
    }
}

void ShorcutManager::OnGmSetLike()
{
    if(m_shorcut_info.like_flag < 1 )
    {
        const REWARD_TABLE* rList = ShorcutConfig::Instance()->GetRewardList(SHORCUT_TYPE_LIKE);
        if(rList == NULL)
            return;

        // 检查包裹
        //if(!m_player->GetItemManager()->CanAddRewardTable(*rList))
        //{
        //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
        //    return;
        //}

        //for (std::vector<REWARD_ITEM>::const_iterator re = rList->items.begin(); re != rList->items.end(); re++)
        //{

        //    m_player->GetItemManager()->AddItemByType(re->type, re->id, re->num, REWARD_REASON_FACEBOOK_LIKE);
        //}

        m_shorcut_info.like_flag = 1;

        m_player->SendToGate(MSG_PLAYER_SHORCUT_ACK, &m_shorcut_info);
        m_player->GetScene()->SendEvent(0, EVENT_FB_LIKE_TITLE, m_shorcut_info.like_flag, 0, 0);
    }
    else
    {
        return;
    }
}

bool ShorcutManager::JudgeDailyFashionFreeTime()
{
    if(m_shorcut_info.dftime == 0)
    {
        return true;
    }

    Crown::CSDDateTime now = Crown::SDNow();
    now.SetTime(0,0,0);
    if(m_shorcut_info.dftime > (uint32)now.GetTimeValue())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void ShorcutManager::SetDailyFashionFreeTime()
{
    m_shorcut_info.dftime = (uint32)Crown::SDNow().GetTimeValue();
    OnDailyFashionReq(NULL);
}

void ShorcutManager::OnDailyFashionReq( void* data )
{
    SHORCUT_BAG sendMsg;

    if(m_shorcut_info.dftime == 0)
    {
        sendMsg.dftime = 2;
    }
    else
    {
        Crown::CSDDateTime now = Crown::SDNow();
        now.SetTime(0,0,0);
        if(m_shorcut_info.dftime > (uint32)now.GetTimeValue())
        {
            sendMsg.dftime = 1;
        }
        else
        {
            sendMsg.dftime = 2;
        }
    }

    m_player->SendToGate(MSG_PLAYER_DAILY_FASHION_ACK, &sendMsg);

}
