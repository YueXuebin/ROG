#include "gameholder_pch.h"
#include "adventure_manager.h"
#include "adventure_config.h"
#include "guild_manager.h"
#include "player.h"


// ==================================================
// 构造函数
// ==================================================
AdventureManager::AdventureManager(Player* player) :
m_player(player)
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
AdventureManager::~AdventureManager()
{

}

// ==================================================
// 清除
// ==================================================
void AdventureManager::Clear()
{
    m_adventure.Clear();
}

// ==================================================
// 加载大冒险数据
// ==================================================
void AdventureManager::LoadAdventureInfo(ADVENTURE_INFO& info)
{
    m_adventure = info;
}

// ==================================================
// 填充大冒险数据
// ==================================================
void AdventureManager::FillAdventureInfo(ADVENTURE_INFO& info)
{
    info = m_adventure;
}

// ==================================================
// 接收消息
// ==================================================
void AdventureManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case MSG_ADVENTURE_INFO_REQ:            // 大冒险信息请求
        {
            OnAdventureInfoReq();
        }
        break;
    case MSG_ADVENTURE_JOIN_REQ:            // 加入大冒险请求
        {
            OnAdventureJoinReq(data);
        }
        break;
    case MSG_ADVENTURE_CMPL_REQ:            // 大冒险完成请求
        {
            OnAdventureCmplReq(data);
        }
        break;
    case MSG_ADVENTURER_LIST_REQ:           // 请求获取冒险者列表
        {
            OnAdventurerListReq(data);
        }
        break;
    case MSG_ADVENTURE_ROB_REQ:             // 打劫大冒险请求
        {
            OnAdventureRobReq(data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 大冒险信息请求
// ==================================================
void AdventureManager::OnAdventureInfoReq()
{
    ADVENTURE_INFO sendmsg;
    uint32 now = 0;
    uint32 gone_time = 0;
    uint32 now_day = 0;
    uint32 refresh_day = 0;

    sendmsg.Clear();

    // 刷新大冒险次数
    now = (uint32)Crown::SDNow().GetTimeValue();

    now_day = now / DAY_SECONDS;                            // 当前距 1970 年的天数
    refresh_day = m_adventure.refresh_time / DAY_SECONDS;   // 上次刷新距 1970 年的天数

    // 检查是否可以刷新
    if(now_day > refresh_day)
    {
        gone_time = now % DAY_SECONDS;

        // 检查是否已达到当天的更新时间（当天的几点）
        if(gone_time >= AdventureConfig::Instance()->GetRefreshTime())
        {
            // 更新刷新时间
            m_adventure.refresh_time = now;
            // 更新冒险次数
            m_adventure.adventure_times = AdventureConfig::Instance()->GetMaxAdventureTimes();
        }
    }

    sendmsg.map_id = m_adventure.map_id;                    // 当前冒险所在地图（0 标示不在冒险）
    sendmsg.adventure_times = m_adventure.adventure_times;  // 当天剩余的冒险次数
    sendmsg.refresh_time = m_adventure.refresh_time;        // 上次的刷新时间（客户端拿去也没用）

    // 如果处在冒险活动中，则需要给客户端倒计时
    if(m_adventure.map_id)
    {
        if(now > m_adventure.end_time)
        {
            sendmsg.end_time = now - m_adventure.end_time;
        }
    }

    m_player->SendToGate(MSG_ADVENTURE_INFO_NTF, &sendmsg);
}

// ==================================================
// 加入大冒险请求
// ==================================================
void AdventureManager::OnAdventureJoinReq(void* data)
{
    ADVENTURE_JOIN_REQ* recvMsg = (ADVENTURE_JOIN_REQ*)data;
    const AdventureTemplate* adventureTmpl = 0;
    uint32 now = 0;

    if(!recvMsg) return;

    // 获取当前时间
    now = (uint32)Crown::SDNow().GetTimeValue();

    // 检查是否已参与了大冒险（通过地图ID非零判断）
    if(m_adventure.map_id)
    {
        return;
    }

    // 检查次数
    if(m_adventure.adventure_times < 1)
    {
        return;
    }

    // 获取冒险模板
    adventureTmpl = AdventureConfig::Instance()->GetAdventureTemplate(recvMsg->map_id);
    if(!adventureTmpl)
    {
        return;
    }

    // 减少一次冒险次数
    --m_adventure.adventure_times;

    // 复制模板信息
    m_adventure.map_id = adventureTmpl->id;                 // 复制ID
    m_adventure.end_time = now + adventureTmpl->take_time;  // 设置结束时间

    // 发送添加冒险信息
    SendAddAdventure(m_player, adventureTmpl, m_adventure.end_time);
}

// ==================================================
// 大冒险完成请求
// ==================================================
void AdventureManager::OnAdventureCmplReq(void* data)
{
    NONE* recvMsg = (NONE*)data;
    const AdventureTemplate* adventureTmpl = 0;
    uint32 now = 0;

    if(!recvMsg) return;

    // 获取当前时间
    now = (uint32)Crown::SDNow().GetTimeValue();

    // 当前未参加大冒险
    if(!m_adventure.map_id)
    {
        return;
    }

    // 检查是否已达到时间
    if(m_adventure.end_time > now)
    {
        return;
    }

    // 获得冒险模板
    adventureTmpl = AdventureConfig::Instance()->GetAdventureTemplate(m_adventure.map_id);
    if(!adventureTmpl)
    {
        return;
    }

    // 完成后将副本ID重置为0
    m_adventure.map_id = 0;

    m_player->AddGamePoint(adventureTmpl->gamepoint, 0);
}

// ==================================================
// 请求获取冒险者列表
// ==================================================
void AdventureManager::OnAdventurerListReq(void* data)
{
    NONE* recvMsg = (NONE*)data;
    if(!recvMsg) return;
    m_player->SendToCenter(MSG_ADVENTURER_LIST_REQ, recvMsg);
}

// ==================================================
// 打劫大冒险请求
// ==================================================
void AdventureManager::OnAdventureRobReq(void* data)
{
    ADVENTURE_ROB_REQ* recvMsg = (ADVENTURE_ROB_REQ*)data;

    if(!recvMsg) return;

    m_player->SendToCenter(MSG_ADVENTURE_ROB_REQ, recvMsg);
}

// ==================================================
// 打劫大冒险应答
// ==================================================
void AdventureManager::OnAdventureRobAck(void* data)
{
    ADVENTURE_ROB_ACK* recvMsg = (ADVENTURE_ROB_ACK*)data;

    if(!recvMsg) return;

    if(recvMsg->errcode == ERR_SUCCEED)
    {
        m_player->AddGamePoint(recvMsg->gamepoint, 0);
    }
    //else
    //{
    //    // 把之前请求时先减去的再加回来
    //    ++m_adventure.adventure_times;
    //}

    m_player->SendToGate(MSG_ADVENTURE_ROB_ACK, recvMsg);
}

// ==================================================
// 添加冒险条目
// ==================================================
void AdventureManager::SendAddAdventure(Player* player, const AdventureTemplate* adventureTmpl, uint32 endtime)
{
    ADVENTURE_ADD_REQ sendMsg;

    if(!player || !adventureTmpl) return;

    sendMsg.Clear();
    sendMsg.player_name = player->GetName();
    sendMsg.guild_name = player->GetGuildName();
    sendMsg.map_id = adventureTmpl->id;
    sendMsg.rob_gamepoint = adventureTmpl->gamepoint;
    sendMsg.rob_times = AdventureConfig::Instance()->GetMaxRobTimes();
    sendMsg.take_time = adventureTmpl->take_time;
    sendMsg.end_time = endtime;
    m_player->SendToCenter(MSG_ADVENTURE_ADD_REQ, &sendMsg);
}
