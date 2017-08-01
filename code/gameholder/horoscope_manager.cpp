#include "gameholder_pch.h"
#include "horoscope_manager.h"
#include "horoscope_config.h"
#include "player.h"
#include "event_define.h"
#include "property_set_calculater.h"
#include "game_util.h"


HoroscopeManager::HoroscopeManager(Player* player) :
m_player(player)
{
    Clear();
}

HoroscopeManager::~HoroscopeManager()
{

}

void HoroscopeManager::Clear()
{
    m_horoscope_level   = 0;
    m_levelup_id        = 0;
    m_levelup_time      = 0;
}

void HoroscopeManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_HOROSCOPE_INFO_REQ:
        OnHoroscopeInfoReq();
        break;
    case MSG_HOROSCOPE_LEVELUP_REQ:
        OnHoroscopeLevelUpReq(data);
        break;
    default:
        break;
    }
}

void HoroscopeManager::FillInfo(HOROSCOPE_INFO& info)
{
    info.horoscope_level = m_horoscope_level;
    info.levelup_id = m_levelup_id;
    info.levelup_time = m_levelup_time;
}

void HoroscopeManager::LoadInfo(const HOROSCOPE_INFO& info)
{
    m_horoscope_level = info.horoscope_level;
    m_levelup_id = info.levelup_id;
    m_levelup_time = info.levelup_time;
}

void HoroscopeManager::CalProp()
{
    std::vector<const ConstellationTemplate*> list;
    PropertySet propSet;

    //计算星空图所达到的级别的属性
    HoroscopeConfig::Instance()->GetConstellationProps(m_horoscope_level, list);

    for(std::vector<const ConstellationTemplate*>::const_iterator scopeIt = list.begin(); scopeIt != list.end(); ++scopeIt)
    {
        for(std::vector<Property>::const_iterator propIt = (*scopeIt)->props.begin(); propIt != (*scopeIt)->props.end(); ++propIt)
        {
            propSet.Add(propIt->type, propIt->value);
        }
    }

    //m_player->GetPropertySetCalculater()->CalcBattlePowerByHoroscope(propSet);
}

void HoroscopeManager::OnHoroscopeInfoReq()
{
    SendHoroscopeInfonAck();
}

void HoroscopeManager::OnHoroscopeLevelUpReq(void* data)
{
    NONE* recvMsg = (NONE*)data;

    uint16 errcode = ERR_FAILED;
    uint32 left_time = 0;

    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_HOROSCOPE, m_player) != ERR_SUCCEED)
    //{
    //    return;
    //}

    // 检查是否已达到升级的最大等级
    if(Levelup()) 
    {
        errcode = ERR_SUCCEED;
    }

    if(errcode == ERR_FAILED)
    {
        goto _END;
    }
    // 去掉 CD 时间
    // m_levelup_time = (uint32)Crown::SDNow().GetTimeValue() + consTmpl->time;
    // left_time = consTmpl->time;

    m_player->SendTaskEvent(TASK_EVENT_UP_HOROSCOPE, m_horoscope_level, 0);
    m_player->SendTaskEvent(TASK_EVENT_USE_HOROSCOPE, 0, 0);

    m_player->SendPlaySystemLog(PLAYSYSTEM_HOROSCOPE_UP, m_horoscope_level);

    // 重新计算角色属性
    m_player->CalcProperty();

_END:
    SendHoroscopeLevelUpAck(errcode, m_horoscope_level, left_time);
}

//升级
bool HoroscopeManager::Levelup()
{
    const ConstellationTemplate* consTmpl  = HoroscopeConfig::Instance()->GetConstellation(m_horoscope_level + 1);
    if(!consTmpl)
    {
        CnWarn("horoscope tmp level:%d is not exist!\n", m_horoscope_level + 1);
        return false;
    }

    if( consTmpl->player_level <= m_player->GetLevel())
    {
        if(!consTmpl)
        {
            return false;
        }

        if(m_horoscope_level >= HoroscopeConfig::Instance()->GetMaxHoroscopeLevel())
        {
            return false;
        }
        // 查找是否已存在此星座的升级信息
        // 检查星座的升级信息
        if(SubLeveupObj(consTmpl))
        {
            m_horoscope_level = m_horoscope_level+ 1;
            m_levelup_time = (uint32)Crown::SDNow().GetTimeValue();

            //多少级开启黄金白银
            SendMarQuee();

            return true;
        }

    }

    return false;
}
//一键升级

bool HoroscopeManager::OneLevelup()
{
    const ConstellationTemplate* consTmpl  = HoroscopeConfig::Instance()->GetConstellation(m_horoscope_level + 1);
    while(m_player->GetLevel()<= consTmpl->player_level)
    {
        if(!consTmpl)
        {
            return false;
        }

        if(m_horoscope_level >= HoroscopeConfig::Instance()->GetMaxHoroscopeLevel())
        {
            return false;
        }
        // 查找是否已存在此星座的升级信息
        // 检查星座的升级信息
        if(SubLeveupObj(consTmpl))
        {
            m_horoscope_level = m_horoscope_level+ 1;
            m_levelup_time = (uint32)Crown::SDNow().GetTimeValue();

            //多少级开启黄金白银
            SendMarQuee();
        }
        consTmpl  = HoroscopeConfig::Instance()->GetConstellation(m_horoscope_level + 1);

        return true;
    }
    return false;
}


bool HoroscopeManager::SubLeveupObj(const ConstellationTemplate* consTmpl)
{
    if(!consTmpl)
        return false;
    uint16 errcode = ERR_FAILED;

    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;


    rewardTable.reason = REWARD_REASON_HOROSCOPE;

    rewardItem.Clear();
    rewardItem.id = PROP_GAMEPOINT;
    rewardItem.num = consTmpl->game_point;
    rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id= PROP_SOULPOINT;
    rewardItem.num = consTmpl->soul_point;
    rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id = HOROSEO_ITEM_STAR;
    rewardItem.num = consTmpl->start_point;
    rewardTable.items.push_back(rewardItem);

    errcode = m_player->SubItemTable(rewardTable);
    if(errcode != ERR_SUCCEED)
    {
        return false;
    }

    return true;
}

void HoroscopeManager::SendMarQuee()
{
    SERVER_MARQUEE sendMsg;
    sendMsg.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    if(m_horoscope_level == HoroscopeConfig::Instance()->GetMaxBronzeLevel())
    {
        sendMsg.id = MARQUEE_BRONZE_HOROSCOPE;
    }
    else if(m_horoscope_level == HoroscopeConfig::Instance()->GetMaxSilverLevel())
    {
        sendMsg.id = MARQUEE_SILVER_HOROSCOPE;
    }
    else if(m_horoscope_level == HoroscopeConfig::Instance()->GetMaxGoldLevel())
    {
        sendMsg.id = MARQUEE_GOLD_HOROSCOPE;
    }
    else
    {
        return;
    }

    Player::OnMarQuee(sendMsg);
}

void HoroscopeManager::SendHoroscopeInfonAck()
{
    HOROSCOPE_INFO sendMsg;
    sendMsg.Clear();

    sendMsg.horoscope_level = m_horoscope_level;
    sendMsg.levelup_id = m_levelup_id;
    sendMsg.levelup_time = m_levelup_time;

    m_player->SendToGate(MSG_HOROSCOPE_INFO_ACK, &sendMsg);
}

void HoroscopeManager::SendHoroscopeLevelUpAck(uint16 errcode, int32 horoscope_level, uint32 time)
{
    HOROSCOPE_LEVELUP_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.horoscope_level = horoscope_level;
    sendMsg.time = time;
    m_player->SendToGate(MSG_HOROSCOPE_LEVELUP_ACK, &sendMsg);
}

void HoroscopeManager::SendHoroscopeLevelUpCmplAck(uint16 errcode, uint8 id, uint8 level, uint32 time)
{
    HOROSCOPE_LEVELUP_CMPL_ACK sendMsg;
    sendMsg.errcode = errcode;
    sendMsg.id = id;
    sendMsg.level = level;
    sendMsg.time = time;
    m_player->SendToGate(MSG_HOROSCOPE_LEVELUP_CMPL_ACK, &sendMsg);
}
