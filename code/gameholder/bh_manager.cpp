#include "gameholder_pch.h"
#include "bh_manager.h"
#include "player.h"
#include "brave_heart_config.h"
#include "property_set_calculater.h"
#include "event_define.h"
#include "item_manager.h"
#include "trade_manager.h"

BhManager::BhManager(Player* player):
m_player(player)
{
    Clear();
}

BhManager::~BhManager(void)
{
}

void BhManager::CalDailyRefresh()
{
    m_bh_info.today_count = 0;
    m_bh_info.cur_type = 0;
    m_bh_info.pass_stage = 0;
    m_bh_info.heart_point = 0;
    m_bh_info.heart_point_sum = 0;
    m_bh_info.is_selected = 0;
}

void BhManager::Clear()
{
}

void BhManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_BH_INFO_REQ:
        {
            OnBhInfoReq();
        }
        break;
    case MSG_BH_SELECT_TYPE_REQ:
        {
            OnBhSelectTypeReq(data);
        }
        break;
    case MSG_BH_START_REQ:
        {
            OnBhStartReq(data);
        }
        break;
    case MSG_BH_REDEEM_REQ:
        {
            OnBhRedeemReq(data);
        }
        break;
    case MSG_BH_REWARD_REQ:
        {
            OnBhRewardReq(data);
        }
        break;
    default:
        break;
    }
}

void BhManager::LoadBhInfo(const BH_INFO& info)
{
    m_bh_info = info;

    if (m_bh_info.addition_list.size() == 0) // init data
    {
        m_bh_info.today_count = 0;
        m_bh_info.cur_type = 0;
        m_bh_info.pass_type = 0;
        m_bh_info.pass_stage = 0;
        m_bh_info.heart_point = 0;
        m_bh_info.heart_point_sum = 0;
        m_bh_info.is_selected = 0;
/*
0 攻击力    9 PROP_ATTACK
1 防御力    10 PROP_DEFENCE
2 命中      12 PROP_HIT_RATE
3 暴击      11 PROP_CRITICAL_ATTACK
4 生命      7 PROP_MAXHP
5 暴击抵抗  100 PROP_CRIT_DAMAGE_RED
6 暴击伤害  97 PROP_CRITICAL_DAMAGE_ADDITION
7 闪避      13 PROP_DODGE
8 韧性      99 PROP_CRIT_RED
*/
        for (int i = 0; i<9; i++)
        {
            BH_ADDITION add;

            if (i == 0)
                add.add_prop = PROP_ATTACK;
            else if (i == 1)
                add.add_prop = PROP_ARMOR;
            else if (i == 2)
                add.add_prop = PROP_HIT_RATE;
            else if (i == 3)
                add.add_prop = PROP_CRITICAL_ATTACK;
            else if (i == 4)
                add.add_prop = PROP_MAXHP;
            else if (i == 6)
                add.add_prop = PROP_CRITICAL_DAMAGE_ADDITION;
            else if (i == 7)
                add.add_prop = PROP_DODGE;

            add.add_value = 0;
            m_bh_info.addition_list.push_back(add);
        }
    }
}

void BhManager::FillBhInfo(BH_INFO& info)
{
    info = m_bh_info;
}

// ==================================================
// 计算勇敢的心属性
// ==================================================
void BhManager::CalBhProp()
{
//     PropertySet propSet;
//     for(std::vector<BH_ADDITION>::iterator attr = m_bh_info.addition_list.begin(); attr != m_bh_info.addition_list.end(); ++attr)
//     {
//         propSet.Add(attr->add_prop, attr->add_value);
//     }
// 
//     m_player->GetPropertySetCalculater()->CalcBattlePowerByBraveHeart(propSet);
}

void BhManager::OnBhInfoReq()
{
    //ack
    BH_INFO sendMsg = m_bh_info;
    m_player->SendToGate(MSG_BH_INFO_ACK, &sendMsg);
}

void BhManager::OnBhSelectTypeReq(void* data)
{
    BH_SELECT_TYPE_ACK sendMsg;

    uint32 dailyMaxTimes = 0;
    
    if(m_bh_info.today_count < dailyMaxTimes)// check count
    {
        const CountInfoTemplate* countInfo = BraveHeartConfig::Instance()->GetCountInfo(m_bh_info.today_count + 1);
        if(countInfo == NULL)
        {
            OnBhSelectTypeCB(ERR_UNFOUND_CONFIG, 0);
            return;
        }

        BH_SELECT_TYPE_REQ* recvMsg = (BH_SELECT_TYPE_REQ*)data;
        if (m_bh_info.pass_type >= recvMsg->bh_type) // check pass type
        {
            if(m_player->SubBagItem(BRAVE_HEART_CARD_ID, countInfo->card))
            {
                //m_player->GetBag()->DelItemByID(BRAVE_HEART_CARD_ID, countInfo->card);
                OnBhSelectTypeCB(ERR_SUCCEED, recvMsg->bh_type);
            }
            else if(m_player->GetMoneyPoint() >= countInfo->money)
            {
                std::string str = "";
                str += Crown::ToString(recvMsg->bh_type);
                TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_BUY_BRAVE_HEART_COUNT, PROP_BRAVE_HEART_COUNT, 0, 0 , countInfo->money, str);
                OnBhSelectTypeCB(ERR_SUCCEED, recvMsg->bh_type);
            }
        }
        else
        {
            OnBhSelectTypeCB(ERR_FAILED, 0);
        }
    }
    else
    {
        OnBhSelectTypeCB(ERR_FAILED, 0);
    }
}

void BhManager::OnBhStartReq(void* data)
{
    BH_START_ACK sendMsg;

    uint32 dailyMaxTimes = 0;
    if(m_bh_info.today_count < dailyMaxTimes)// check count
    {
        // jump dungeon
        BH_START_REQ* recvMsg = (BH_START_REQ*)data;
        m_bh_info.cur_diff = recvMsg->bh_diff;

        CHANGE_DUNGEON_REQ msg;
        msg.Clear();
        msg.dungeon_id = BraveHeartConfig::Instance()->GetDungeonId(m_bh_info.cur_type, m_bh_info.pass_stage+1, m_bh_info.cur_diff);
        m_player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &msg);

        sendMsg.errcode = ERR_SUCCEED;
        sendMsg.bh_diff = recvMsg->bh_diff;
    }
    else
    {
        sendMsg.errcode = ERR_FAILED;
    }

    //ack
    m_player->SendToGate(MSG_BH_START_ACK, &sendMsg);
}

void BhManager::OnBhRedeemReq(void* data)
{
    BH_REDEEM_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;

    BH_REDEEM_REQ* recvMsg = (BH_REDEEM_REQ*)data;

    int32 idx = recvMsg->idx; 
    int32 attrId = recvMsg->attr;
    int32 heartPointCost = recvMsg->heart_point_cost;

    if (m_bh_info.heart_point >= heartPointCost) // 心点够不够
    {
        int32 stage = m_bh_info.pass_stage;
        if (stage == 0) // 第九层的时候，副本结束后pass stage已经设置成0
            stage = 9;

        int32 redeemAttrVal = BraveHeartConfig::Instance()->GetRedeemAttrValue(m_bh_info.cur_type, stage, attrId, heartPointCost);
        if (redeemAttrVal > 0)
        {
            m_bh_info.heart_point -= heartPointCost;    // 减心点
            // 增加属性
            for (int i = 0; i<9; i++)
            {
                if (m_bh_info.addition_list[i].add_prop == attrId)
                {
                    m_bh_info.addition_list[i].add_value += redeemAttrVal;
                    break;
                }
            }
            m_player->CalcProperty();  // 重新计算角色属性

            sendMsg.errcode = ERR_SUCCEED;
            sendMsg.idx = recvMsg->idx;
            sendMsg.attr = attrId;
            sendMsg.attr_val = redeemAttrVal;
            sendMsg.heart_point_cost = heartPointCost;
        }
    }

    //ack
    m_player->SendToGate(MSG_BH_REDEEM_ACK, &sendMsg);
}

void BhManager::OnBhRewardReq(void* data)
{
    BH_REWARD_ACK sendMsg;
    sendMsg.errcode = ERR_SUCCEED;

    // get reward items by heart_point_sum in current tier
    REWARD_TABLE rewardTable;
    BraveHeartConfig::Instance()->GetRewardTable(m_bh_info.cur_type, m_bh_info.heart_point_sum, rewardTable);
    if (rewardTable.items.size() == 0)
    {
        sendMsg.errcode = ERR_FAILED;
    }
    else
    {
        m_player->TakeInRewardTable(&rewardTable);
    }

    // clear heart point
    m_bh_info.heart_point = 0;
    m_bh_info.heart_point_sum = 0;

    //ack
    m_player->SendToGate(MSG_BH_REWARD_ACK, &sendMsg);
}

int BhManager::DungeonEnd(bool bWin)
{
    int32 heartPoint = 0;
    if (bWin == true)
    {
        // 心点奖励
        heartPoint = BraveHeartConfig::Instance()->GetHeartPoint(m_bh_info.cur_type, m_bh_info.pass_stage+1, m_bh_info.cur_diff);
        m_bh_info.heart_point += heartPoint;
        m_bh_info.heart_point_sum += heartPoint;

        //
        m_bh_info.pass_stage++;
        if (m_bh_info.pass_stage == BH_STAGE_MAX)
        {
            m_bh_info.pass_stage = 0;
            if (m_bh_info.pass_type <= m_bh_info.cur_type)
                m_bh_info.pass_type++;
            m_bh_info.today_count++;
            m_bh_info.is_selected = 0;
        }
    }
    else if (bWin == false)
    {
        m_bh_info.pass_stage = 0;
        m_bh_info.today_count++;
        m_bh_info.is_selected = 0;
    }

    return heartPoint;
}

int32 BhManager::GetEnterTimes()
{
    return 0;
}

void BhManager::OnBhSelectTypeCB( int32 err, int32 type )
{
    BH_SELECT_TYPE_ACK sendMsg;
    if(err == ERR_SUCCEED)
    {
        m_bh_info.cur_type = type;
        m_bh_info.is_selected = 1;

        sendMsg.errcode = ERR_SUCCEED;
        sendMsg.bh_type = m_bh_info.cur_type;
    }
    else
    {
        sendMsg.errcode = ERR_FAILED;
    }
    m_player->SendToGate(MSG_BH_SELECT_TYPE_ACK, &sendMsg);

}
