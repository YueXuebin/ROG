#include "gameholder_pch.h"
#include "passive_skill_manager.h"
#include "passive_skill_config_manager.h"
#include "player.h"
#include "item_manager.h"
#include "event_define.h"
#include "property.h"
#include "formula_manager.h"
#include "property_set_calculater.h"


PassiveSkillManager::PassiveSkillManager(Player* player) :
m_player(player)
{
    m_passiveSkillInfo.Clear();
}

PassiveSkillManager::~PassiveSkillManager()
{

}

void PassiveSkillManager::LoadPassiveSkillInfo(const PASSIVE_SKILL_LIST& info)
{
    m_passiveSkillInfo = info;

    if(m_passiveSkillInfo.passive_skill.size() == 0)
    {
        const PASSIVE_SKILL_ROOT_STRUCT* list = PassiveSkillConfigManager::Instance()->GetPassiveSkillListConfigData();

        m_passiveSkillInfo.available_points = 0;
        for(std::vector<PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT>::const_iterator iter = list->passive_skill_list.begin();
            iter != list->passive_skill_list.end(); iter++)
        {
            PASSIVE_SKILL_INFO info;
            info.id = iter->id;
            info.level = 0;
            info.unlocked = (iter->unlock_points > 0) ? 0 : 1;
            m_passiveSkillInfo.passive_skill.push_back(info);
        }
    }

    CheckUnlockSkills(); // 检查并更新天赋解锁状态
}

void PassiveSkillManager::FillPassiveSkillInfo(PASSIVE_SKILL_LIST& info)
{
    info = m_passiveSkillInfo;
}

void PassiveSkillManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_PASSIVE_SKILL_INFO_REQ:
        SendPassiveSkillInfo();
        break;
    case MSG_PASSIVE_SKILL_UP_REQ:
        OnRecvLevelUp(data);
        break;
    default:
        break;
    }
}

void PassiveSkillManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    
}

// 添加天赋点
void PassiveSkillManager::AddAvailablePoints(uint32 points)
{
    m_passiveSkillInfo.available_points += points;

    SendPassiveSkillInfo();
}

// 洗点
void PassiveSkillManager::ResetPoints()
{
    std::vector<PASSIVE_SKILL_INFO>::iterator iter = m_passiveSkillInfo.passive_skill.begin();
    for(; iter < m_passiveSkillInfo.passive_skill.end(); iter++)
    {
        m_passiveSkillInfo.available_points += iter->level;
        iter->level = 0;
        iter->unlocked = 0;
    }

    CheckUnlockSkills();
    SendPassiveSkillInfo();
    m_player->CalcProperty();
}

// 升级天赋
void PassiveSkillManager::LevelUpSkill(uint32 skillID)
{
    ERROR_CODE sendMsg;

    // 无可用点数
    if(m_passiveSkillInfo.available_points <= 0)
    {
        sendMsg.errcode = ERR_PASSIVE_SKILL_NO_AVAILABLE_POINTS;
        m_player->SendToGate(MSG_PASSIVE_SKILL_UP_ACK, &sendMsg);
        return;
    }

    // 技能未找到
    std::vector<PASSIVE_SKILL_INFO>::iterator skillIt = m_passiveSkillInfo.passive_skill.begin();
    for(; skillIt != m_passiveSkillInfo.passive_skill.end(); ++skillIt)
    {
        if(skillIt->id == skillID)
        {
            break;
        }
    }
    if(skillIt == m_passiveSkillInfo.passive_skill.end())
    {
        sendMsg.errcode = ERR_PASSIVE_SKILL_NOT_EXIST;
        m_player->SendToGate(MSG_PASSIVE_SKILL_UP_ACK, &sendMsg);
        return;
    }

    // 未激活
    if(skillIt->unlocked == 0)
    {
        sendMsg.errcode = ERR_PASSIVE_SKILL_NOT_OPEN;
        m_player->SendToGate(MSG_PASSIVE_SKILL_UP_ACK, &sendMsg);
        return;
    }

    // 已达到最高级
    const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT* nextSkillData = PassiveSkillConfigManager::Instance()->GetPassiveSkillItemConfigData(skillID, skillIt->level + 1);
    if (nextSkillData == NULL)
    {
        sendMsg.errcode = ERR_PASSIVE_SKILL_MAX_LEVEL;
        m_player->SendToGate(MSG_PASSIVE_SKILL_UP_ACK, &sendMsg);
        return;
    }

    m_passiveSkillInfo.available_points--;
    skillIt->level++;
    CheckUnlockSkills();

    SendPassiveSkillInfo();
    m_player->CalcProperty();

    m_player->SendToGate(MSG_PASSIVE_SKILL_UP_ACK, &sendMsg);
}

// 检查并更新未解锁的天赋
void PassiveSkillManager::CheckUnlockSkills()
{
    // 计算总升级点数
    int32 totalPoints(0); 
    std::vector<PASSIVE_SKILL_INFO>::iterator iter = m_passiveSkillInfo.passive_skill.begin();
    for(; iter < m_passiveSkillInfo.passive_skill.end(); iter++)
    {
        const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT* skillInfo = PassiveSkillConfigManager::Instance()->GetPassiveSkillConfigData(iter->id);
        totalPoints += iter->level;
    }

    // 判断解锁状态
    for(iter = m_passiveSkillInfo.passive_skill.begin(); iter < m_passiveSkillInfo.passive_skill.end(); iter++)
    {
        const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT* skillInfo = PassiveSkillConfigManager::Instance()->GetPassiveSkillConfigData(iter->id);
        if (skillInfo->unlock_points > 0)
        {
            if (totalPoints >= skillInfo->unlock_points)
            {
                iter->unlocked = 1; // 解锁
            }
        }
        else
        {
            iter->unlocked = 1; // 解锁
        }
    }
}

// 计算天赋属性
void PassiveSkillManager::CalcPassiveSkillProp()
{
    PropertySet& propSet = m_player->GetPropertySetCalculater()->GetTalentProp();
    propSet.Clear();

    for(std::vector<PASSIVE_SKILL_INFO>::iterator iter = m_passiveSkillInfo.passive_skill.begin();
        iter != m_passiveSkillInfo.passive_skill.end(); iter++)
    {
        if(iter->level == 0)
            continue;

        const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT* item = PassiveSkillConfigManager::Instance()->GetPassiveSkillItemConfigData(iter->id, iter->level);
        if(!item)
        {
            CnAssert(false);
            continue;
        }

        if(item->prop1_id)
        {
            propSet.Add(item->prop1_id, item->prop1_value);
        }

        if(item->prop2_id)
        {
            propSet.Add(item->prop2_id, item->prop2_value);
        } 
    }
}

// 升级天赋
void PassiveSkillManager::OnRecvLevelUp(void* data)
{
    SKILL_ID* recvMsg = (SKILL_ID*)data;
    LevelUpSkill(recvMsg->skill_id);
}

// 发送天赋数据响应
void PassiveSkillManager::SendPassiveSkillInfo()
{
    PASSIVE_SKILL_LIST sendMsg;
    sendMsg = m_passiveSkillInfo;
    m_player->SendToGate(MSG_PASSIVE_SKILL_INFO_ACK, &sendMsg);
}


// 查看所有天赋中的最高等级
int32 PassiveSkillManager::GetMaxPassiveSkillLevel()
{
    int32 level = 0;

    for(std::vector<PASSIVE_SKILL_INFO>::iterator passiveIt = m_passiveSkillInfo.passive_skill.begin(); 
        passiveIt != m_passiveSkillInfo.passive_skill.end(); ++passiveIt)
    {
        if(passiveIt == m_passiveSkillInfo.passive_skill.begin())
        {
            level = passiveIt->level;
        }
        else if(passiveIt->level > level)
        {
            level = passiveIt->level;
        }
    }

    return level;
}

// 获取一个天赋信息
PASSIVE_SKILL_INFO* PassiveSkillManager::GetOneSkillInfo(uint32 id)
{
    std::vector<PASSIVE_SKILL_INFO>::iterator passiveIt = m_passiveSkillInfo.passive_skill.begin();
    for(; passiveIt != m_passiveSkillInfo.passive_skill.end(); ++passiveIt)
    {
        if(passiveIt->id == id)
        {
            break;
        }
    }

    if(passiveIt == m_passiveSkillInfo.passive_skill.end())
    {
        return NULL;
    }

    return &(*passiveIt);
}