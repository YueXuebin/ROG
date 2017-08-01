//至尊魔宫
#include "gameholder_pch.h"
#include "babel_manager.h"
#include "player.h"
#include "center_handler.h"
#include "scene.h"
#include "vip_config.h"
#include "dungeon_gain_config.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"
#include "game_util.h"
/*

BabelManager::BabelManager(Player* player) :
m_player(player)
{
    Clear();
}

BabelManager::~BabelManager()
{
}

void BabelManager::Clear()
{

}

void BabelManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_BABEL_INFO_REQ:            // 请求至尊魔宫信息
        {
            OnBabelInfoReq();
        }
        break;
    case MSG_BABEL_START_REQ:
        {
            OnBabelStartReq(data);
        }
        break;
    case MSG_BABEL_RESET_REQ:
        {
            OnBabelResetReq(data);
        }
        break;
    case MSG_BABEL_RAIDS_REQ:
        {
            OnBabelRaidsReq(data);
        }
        break;
    default:
        break;
    }
}

void BabelManager::CalDailyRefresh()
{
    SaveYesterdayData();

    int passTier = m_babel_info.pass_tier;
    int passStage = m_babel_info.pass_stage;

    m_babel_info.Clear();

    m_babel_info.pass_tier = passTier;
    m_babel_info.pass_stage = passStage;
}

void BabelManager::SaveYesterdayData()
{
    int32 count = 0;

    // 当前总共可打层数
    int32 stageTotal = 0;
    for(int32 i = 0; i < m_babel_info.pass_tier; ++i)
    {
        stageTotal += BabelConfig::Instance()->GetBabelStageByTier(i);
    }
    stageTotal += m_babel_info.pass_stage;

    // 当前打过多少层数
    int32 curStageTotal = 0;
    for(int32 i = 0; i < m_babel_info.babel_tier; ++i)
    {
        curStageTotal += BabelConfig::Instance()->GetBabelStageByTier(i);
    }
    curStageTotal += m_babel_info.babel_stage;

    if(m_player->GetGainbackResourceManager()->IsSaveMaxCount())
        count = stageTotal;
    else
        count = stageTotal - curStageTotal;

    m_player->SaveYesterdayCount(OPEN_SYS_BABEL, count);
}

void BabelManager::LoadBabelInfo(const BABEL_INFO& info)
{
    m_babel_info = info;

    CheckBabelNotify();
}

void BabelManager::FillBabelInfo(BABEL_INFO& info)
{
    info = m_babel_info;
}

void BabelManager::OnBabelInfoReq()
{
    //ack
    BABEL_INFO sendMsg = m_babel_info;
    m_player->SendToGate(MSG_BABEL_INFO_ACK, &sendMsg);
}

void BabelManager::OnBabelStartReq(void* data)
{
    BABEL_START_REQ* recvMsg = (BABEL_START_REQ*)data;

    bool bAuto = recvMsg->is_auto;
    uint32 tier = recvMsg->tier;
    uint32 stage = recvMsg->stage;

    if (!bAuto)
    {
        BabelStart(tier, stage);
    }
}

void BabelManager::OnBabelResetReq(void* data)
{
    BabelReset();
}

void BabelManager::OnBabelRaidsReq(void* data)
{
    BabelRaids();
}

void BabelManager::BabelStart(uint32 tier, uint32 stage)
{
    // ack
    BABEL_START_ACK sendMsg;
    sendMsg.Clear();
    sendMsg.errcode = ERR_FAILED;
    sendMsg.is_auto = false;
    sendMsg.tier = tier;
    sendMsg.stage = stage;
    
    if (CanStart(tier, stage))
    {
        const BabelTemplate* babelTemplate = BabelConfig::Instance()->GetBabelTemplate(tier);

        CHANGE_DUNGEON_REQ msg;        
        msg.dungeon_id = babelTemplate->dungeon_start + stage;
        m_player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &msg);

        sendMsg.errcode = ERR_SUCCEED;
    }

    m_player->SendToGate(MSG_BABEL_START_ACK, &sendMsg);
}

void BabelManager::BabelRaids()
{
    BABEL_RAIDS_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;

    // VIP 检查
    if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_52 ,m_player->GetVipLevel()) == 0)
    {
        ack.errcode = ERR_NO_ENOUGH_VIP;
        m_player->SendToGate(MSG_BABEL_RAIDS_ACK, &ack);
        return;
    }

    // 检查体力
    int32 perStageVigor = GameParamConfig::Instance()->GetEnterBabelVigor();
    if (m_player->GetVigor() < perStageVigor)
    {
        ack.errcode = ERR_NO_ENOUGH_VIGOR;
        m_player->SendToGate(MSG_BABEL_RAIDS_ACK, &ack);
        return;
    }

    // 检查魔石
    int32 perStageMoneypoint = GameParamConfig::Instance()->GetBabelRaidsMoneypoint();
    if (m_player->GetMoneyPoint() < perStageMoneypoint)
    {
        ack.errcode = ERR_NO_ENOUGH_MONEYPOINT;
        m_player->SendToGate(MSG_BABEL_RAIDS_ACK, &ack);
        return;
    }

    // 检查有没有历史通关过的且当前还未通关的tier
    if (m_babel_info.babel_tier >= m_babel_info.pass_tier)
    {
        ack.errcode = ERR_BABEL_NO_DUNGEON_CAN_RAIDS;
        m_player->SendToGate(MSG_BABEL_RAIDS_ACK, &ack);
        return;
    }

    // 开始扫荡罗～～～
    REWARD_TABLE reward_table;
    REWARD_TABLE_LIST reward_table_list;
    reward_table_list.Clear();

    bool bVigorOut = false;     // 体力用尽
    bool bMoneyPointOut = false;// 魔石用尽
    int moneyPointSum = 0;      // 记录扫荡消耗的所有魔石

    int beginTier = m_babel_info.babel_tier;

    // 扫荡历史通关过的tier，这里不用检查等级，因为历史通关过的必然是已经开放的
    for (int i = beginTier; i<m_babel_info.pass_tier; i++) // 从当前tier开始
    {
        const BabelTemplate* curTier = BabelConfig::Instance()->GetBabelTemplate(i);

        int stageSumInCurTier = curTier->stage;
        int startDungeonId = curTier->dungeon_start;

        int beginStage = m_babel_info.babel_stage;
        for (int j = beginStage; j<stageSumInCurTier; j++)  // 从当前stage开始
        {
            int curDungeonId = startDungeonId + j;

            // 通一层
            OpenNextBabel(curDungeonId);

            // 任务关联
            m_player->OnTaskSettleDungeon(curDungeonId, true, true);

            // 活跃度
            m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_BABEL, 1, 0, 0);

            // 给奖励
            const DungeonGainTemplate* dungeonTmpl = DungeonGainConfig::Instance()->GetDungeonGainTemplate(curDungeonId);    // 副本模板
            if (dungeonTmpl)
            {
                reward_table.Clear();
                m_player->GetFcmRewardList(dungeonTmpl->gain_items, reward_table.items);
                reward_table.reason = REWARD_REASON_RAIDS_BABEL;
                reward_table.reason_arg = i*100 + j;    // 百位数以上为tier，百位数以下为stage

                reward_table_list.reward_tables.push_back(reward_table);    // 每层一个奖励表
            }

            // 扣体力
            m_player->SubVigor(perStageVigor);
            // 扣魔石
            moneyPointSum += perStageMoneypoint;
            // 检查体力
            if (m_player->GetVigor() < perStageVigor)
            {
                bVigorOut = true;
                break;
            }
            // 检查魔石
            if (m_player->GetMoneyPoint() - moneyPointSum < perStageMoneypoint)
            {
                bMoneyPointOut = true;
                break;
            }
        } // for stages in cur tier

        // 扫荡到体力不够或者魔石不够，本次扫荡就终止了
        if (bVigorOut || bMoneyPointOut)
        {
            break;
        }
    } // for tiers

    // 奖励非空,给奖励
    if (!reward_table_list.reward_tables.empty())
    {
        reward_table_list.reason = REWARD_REASON_RAIDS_BABEL;
        reward_table_list.reason_arg = 0;
        FuseSameReward(reward_table);
        ItemManager::AddRewardTableListWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), reward_table_list);
    }

    // 扣魔石交易流程
    if (moneyPointSum > 0)
        TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_RAIDS_BABEL, PROP_BABEL_RAIDS, 0, moneyPointSum, moneyPointSum);

}

void BabelManager::BabelReset()
{
    if (m_babel_info.left_reset_times < (int32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_26,m_player->GetVipLevel()))
    {
        int32 resetFreeTimes = GameParamConfig::Instance()->GetBabelResetFreeTimes();
        if (m_babel_info.left_reset_times < resetFreeTimes) // 第n次重置免费
        {
            CBResetBabelTrade();
        }
        else
        {
            int32 resetMoneyPoint = GameParamConfig::Instance()->GetBabelResetMoneypoint();
            if (m_player->GetMoneyPoint() >= resetMoneyPoint) //魔石足够
            {
                TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_BABEL_RESET, PROP_BABEL_RESET, 0, 0, resetMoneyPoint);
            }
        }
    }
    else
    {
        BABEL_RESET_ACK sendMsg;
        sendMsg.is_ok = 0;

        // ack
        sendMsg.reset = m_babel_info.left_reset_times;
        m_player->SendToGate(MSG_BABEL_RESET_ACK, &sendMsg);
    }
}

void BabelManager::CBResetBabelTrade()
{
    BABEL_RESET_ACK sendMsg;
    sendMsg.is_ok = 0;

    if (m_babel_info.left_reset_times < (int32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_26,m_player->GetVipLevel()))
    {
        m_babel_info.left_reset_times++;
        m_babel_info.babel_tier = 0;
        m_babel_info.babel_stage = 0;

        CheckBabelNotify();

        sendMsg.is_ok = 1;
    }

    // ack
    sendMsg.reset = m_babel_info.left_reset_times;
    m_player->SendToGate(MSG_BABEL_RESET_ACK, &sendMsg);
}

void BabelManager::CBRaidsBabelTrade()
{
    // 有可扫荡的,即扫荡成功
    BABEL_RAIDS_ACK ack;
    ack.Clear();

    ack.errcode = ERR_SUCCEED;
    ack.tier = m_babel_info.babel_tier;
    ack.stage = m_babel_info.babel_stage;

    m_player->SendToGate(MSG_BABEL_RAIDS_ACK, &ack);
}

int32 BabelManager::GetMaxTier()
{
    return BabelConfig::Instance()->GetMaxTier();
}

void BabelManager::OpenNextBabel(int mDungeonId)
{
    const BabelTemplate* babelTemplate = BabelConfig::Instance()->GetBabelTemplate(m_babel_info.babel_tier);
    if (!babelTemplate) 
        return;

    int32 m_tier = BabelConfig::Instance()->GetBabelTierByDungeonId(mDungeonId);    // 当前的级别
    int32 m_stage = BabelConfig::Instance()->GetBabelStageByDungeonId(mDungeonId);  // 当前的层

    // 必须通关当前级别和层的副本,才能开启下一关卡.
    if (m_tier != m_babel_info.babel_tier)
        return;
    if (m_stage != m_babel_info.babel_stage)
        return;

    if (m_babel_info.babel_stage+1 == babelTemplate->stage) // next tier
    {
        m_babel_info.babel_tier++;
        m_babel_info.babel_stage = 0;

        CheckBabelNotify();
    }
    else    // next stage
    {
        m_babel_info.babel_stage++;
    }

    // 更新通关记录
    if (m_babel_info.babel_tier > m_babel_info.pass_tier)
    {
        m_babel_info.pass_tier = m_babel_info.babel_tier;
        m_babel_info.pass_stage = m_babel_info.babel_stage;
    }
    else if (m_babel_info.babel_tier == m_babel_info.pass_tier)
    {
        if (m_babel_info.babel_stage > m_babel_info.pass_stage)
            m_babel_info.pass_stage = m_babel_info.babel_stage;
    }
}

bool BabelManager::CanStart( uint32 tier, uint32 stage )
{
    // 开启pass_tier内的关卡
    if (tier <= (uint32)m_babel_info.pass_tier)
    {
        const BabelTemplate* babelTemplate = BabelConfig::Instance()->GetBabelTemplate(tier);
        if(!babelTemplate)
            return false;

        // pass_stage+1之内的关卡,且stage在该tier的最大stage内,可开启
        if (
            (stage <= babelTemplate->stage))
        {
            return true;
        }
    }
    // 开启pass_tier+1上的关卡
    else if (tier == (uint32)(m_babel_info.pass_tier+1))
    {
        const BabelTemplate* babelTemplate = BabelConfig::Instance()->GetBabelTemplate(m_babel_info.pass_tier);
        if(!babelTemplate)
            return false;

        // pass_tier的最大stage已经pass,下一tier下的stage=1可开启
        if (((uint32)m_babel_info.pass_stage == babelTemplate->stage) && (stage == 0))
        {
            return true;
        }
    }

    return false;
}

void BabelManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
    if (eventId == EVENT_PLAYER_LEVEL_UP)
    {
        CheckBabelNotify();
    }
}

void BabelManager::CheckBabelNotify()
{
    int32 babelNum = 0;

    for (int32 i = 0; i<BabelConfig::Instance()->GetTierNum(); i++)
    {
        int32 openLevel = BabelConfig::Instance()->GetBabelOpenLevel(i);
        bool bOpen = (m_player->GetLevel() >= openLevel);
        if (bOpen && (m_babel_info.babel_tier <= i)) // 开启并没有Pass的
        {
            babelNum++;
        }
    }

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_BABEL, babelNum, babelNum);
}

bool BabelManager::CardByBabelReset( int32 num )
{
    if (m_babel_info.left_reset_times < (int32)VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_26,m_player->GetVipLevel()))
    {
        CBResetBabelTrade();
        return true;
    }
    else
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return false;
    }
    
}
*/