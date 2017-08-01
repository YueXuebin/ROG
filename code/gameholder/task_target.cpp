#include "gameholder_pch.h"
#include "task_target.h"
#include "item_manager.h"
#include "task_data.h"
#include "player.h"
#include "online_reward_manager.h"
#include "fashion_manager.h"
#include "equip_manager.h"
#include "wing_manager.h"
#include "skill_info_manager.h"
#include "ride_manager.h"
#include "passive_skill_manager.h"
#include "recharge_manager.h"
#include "vip_statistics_manager.h"
#include "top_up_rank_manager.h"
#include "event_define.h"
#include "task_festive_config.h"
#include "task_manager.h"
#include "task_festive_manager.h"

// 状态触发目标列表
uint32 LevelTriggerTargetTypes[] = {
    TASK_TARGET_8,      // 领取XX等级成长礼包
    TASK_TARGET_10,     // 提升XX护魂到XX等级
    TASK_TARGET_12,     // 获得XX个XX星化身
    TASK_TARGET_23,     // 装备升星（特定）
    TASK_TARGET_24,     // 装备升级（特定）
    TASK_TARGET_25,     // 品质强化（特定）
    TASK_TARGET_39,     // 开启星点(特定)
    TASK_TARGET_42,     // 翅膀提升（特定）
    TASK_TARGET_62,     // 拥有公会
    TASK_TARGET_72,     // 累计充值
    TASK_TARGET_73,     // 每日单笔最大充值金额
    TASK_TARGET_75,     // 每日充值
    TASK_TARGET_76,     // 角色等级
    TASK_TARGET_77,     // 符文升级（特定）
    TASK_TARGET_78,     // 获得坐骑（品质、数量）
    TASK_TARGET_79,     // 累计充值（开服）
    TASK_TARGET_81,     // 全身镶嵌宝石等级
    TASK_TARGET_82,     // 提升某坐骑到特定等级
    TASK_TARGET_83,     // 提升某被动技能到特定等级
    TASK_TARGET_85,     // 宠物护魂激活护魂（特定）
    TASK_TARGET_86,     // 提升某宠物到特定等级
    TASK_TARGET_87,     // 镶嵌特定数量宝石
    TASK_TARGET_89,     // 物品兑换
    TASK_TARGET_97,     // 累计在线时长
    TASK_TARGET_98,     // 微端奖励
    TASK_TARGET_104,    // VIP等级多少级达到多少人
    TASK_TARGET_105,    // VIP等级达到多少级
    TASK_TARGET_109,    // 充值排行发奖区间
    TASK_TARGET_110,    // 充值排行上榜
    TASK_TARGET_112,    // 翅膀排行发奖区间
    TASK_TARGET_113,    // 宠物排行发奖区间
    TASK_TARGET_114,    // 坐骑排行发奖区间
    TASK_TARGET_115,    // 战力排行发奖区间
    TASK_TARGET_116,    // 等级排行发奖区间
    TASK_TARGET_120,    // 累计充值(循环)
    TASK_TARGET_123,    // 重置任务
    TASK_TARGET_130,    // 化身战力排行活动
    TASK_TARGET_124,    // 化身战力排行活动
    TASK_TARGET_131,
    TASK_TARGET_132,
};


RoleTaskTarget::RoleTaskTarget(RoleTask* pTask) :
m_pTask(pTask)
{
    Clear();

    m_player = m_pTask->GetPlayer();
}

RoleTaskTarget::~RoleTaskTarget()
{

}

void RoleTaskTarget::Clear()
{
    m_id        = 0;
    m_num       = 0;
    m_max_num   = 0;
    m_target    = NULL;
}

void RoleTaskTarget::AddTargetValidNum( int32 num )
{
    CnAssert(!IsLevelTriggerType());        // 状态触发型目标，不应该调用此函数
    CnAssert(num>=0);           // 不应该为负数
    if(m_target->type == TASK_TARGET_103)       // 103目标的m_num可以>m_max_num
    {
        m_num += num;
    }
    else
    {
        m_num = Crown::SDMin(m_num + num, m_max_num);
    }

    m_IsChangeFlag = true;
}

void RoleTaskTarget::SetTargetValidNum( int32 num )
{
    m_num = Crown::SDMin(num, m_max_num);
    m_IsChangeFlag = true;
}

void RoleTaskTarget::FillTaskTargetRecord( TASK_TARGET_RECORD& taskRecord ) const
{
    taskRecord.Clear();
    taskRecord.id       = m_id;
    taskRecord.num      = m_num;
    taskRecord.max_num  = m_max_num;
}

void RoleTaskTarget::LoadTaskTargetRecord( const TASK_TARGET_RECORD& taskRecord, const TASK_TEMPLATE* m_template )
{
    Clear();
    m_id        = taskRecord.id;
    m_num       = taskRecord.num;
    m_max_num   = taskRecord.max_num;

    // 根据任务目标的ID,查找目标
    std::vector<TASK_TARGET>::const_iterator findIt = m_template->targets.begin();
    for(;findIt != m_template->targets.end(); ++findIt)
    {
        if(taskRecord.id == findIt->id)
            break;
    }

    // 如果未找到任务目标，则在任务中丢弃这个目标
    if(findIt == m_template->targets.end())
        return;

    m_max_num = findIt->num;     // 修改过任务目标
    m_target = &(*findIt);
}

void RoleTaskTarget::InitByTargetTmpl(const TASK_TARGET& taskTarget)
{
    Clear();
    m_id = taskTarget.id;
    m_num = 0;
    m_max_num = taskTarget.num;
    m_target = &taskTarget;
}

bool RoleTaskTarget::IsLevelTriggerType()
{
    if(!m_target)
        return false;
    uint32 len = sizeof(LevelTriggerTargetTypes)/sizeof(uint32);
    for(uint32 i=0; i<len; i++)
    {
        if(LevelTriggerTargetTypes[i] == GetTargetType())
            return true;
    }
    return false;
}

uint32 RoleTaskTarget::GetTargetType()
{
    return m_target ? m_target->type : 0;
}

uint32 RoleTaskTarget::GetTargetValue() 
{
    return m_target ? m_target->value : 0;
}

uint32 RoleTaskTarget::GetTargetMapID() 
{
    return m_target ? m_target->map_id : 0;
}

uint32 RoleTaskTarget::GetTargetNpcID() 
{
    return m_target ? m_target->npc_id : 0;
}

int32 RoleTaskTarget::GetTargetRate() 
{
    return m_target ? m_target->rate : 0;
}

void RoleTaskTarget::Refrash()
{
    // 重置任务的目标信息不接受重置
    if(m_target->type == TASK_TARGET_123)
        return;

    if(m_target->type == TASK_TARGET_103)
    {
        m_num -= m_max_num;
        if(m_num < 0) m_num = 0;
    }
    else
        m_num = 0;
}

bool RoleTaskTarget::UpdateTargetState()
{
    CnAssert(IsLevelTriggerType());         // 只有状态类目标，可以调用此函数
    return InternalLevelTriggerTaskTarget();
}

bool RoleTaskTarget::OnExchangeItem( uint32 itemID )
{
    switch(GetTargetType())
    {
    case TASK_TARGET_89:
        if(GetTargetValue() == itemID)
            return InternalTriggerTaskTarget(itemID, 0);  
        else
            return false;
    }
    return false;
}

bool RoleTaskTarget::OnKillMonster(uint32 monsterID, int32 num, uint32 mapID)
{
    switch(GetTargetType())
    {
        case TASK_TARGET_1:
        case TASK_TARGET_3:
            return InternalTriggerTaskTarget(monsterID, mapID, num);
            break;
    }
    return false;
}

bool RoleTaskTarget::OnTriggerTaskTarget(uint32 arg1, uint32 arg2)
{
    return InternalTriggerTaskTarget(arg1, arg2);
}

bool RoleTaskTarget::InternalTriggerTaskTarget( uint32 arg1, uint32 arg2, int32 arg3 )
{
    m_IsChangeFlag = false;

    switch(GetTargetType())
    {
        // 杀怪物收集道具
    case TASK_TARGET_1:
        {
            if(GetTargetValue() == arg1)
            {
                if(GetTargetMapID() && (GetTargetMapID() != arg2))
                {
                    return false;
                }

                // 计算获得物品的概率
                int32 rand_num = Crown::SDRandom(BASE_RATE_NUM);
                if(rand_num >= GetTargetRate())
                {
                    return false;
                }

                // 累加击杀怪物数
                AddTargetValidNum(arg3);
            }
        }
        break;
        // 杀怪物
    case TASK_TARGET_3:
        {
            if(GetTargetValue() == arg1)
            {
                // 如果需要指定地图击杀，则要判断地图是否相同
                if(GetTargetMapID() && (GetTargetMapID() != arg2))
                {
                    return false;
                }

                // 累加击杀怪物数
                AddTargetValidNum(arg3);
            }
        }
        break;
        // 通关副本
    case TASK_TARGET_4:
        if(arg1 == GetTargetMapID() && arg2 == 1)   // 副本ID一致 且扫荡时任务目标也为扫荡
        {
            AddTargetValidNum(1);
        }
        break;
        // 对话
    case TASK_TARGET_6:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(1);
        }
        break;
        // 领取勇者争霸奖励xx次数
    case TASK_TARGET_7:
        {
            AddTargetValidNum(arg1);
        }
        break;
        //领取XX等级成长礼包
    case TASK_TARGET_8:
        InternalLevelTriggerTaskTarget();
        break;
        // 合成xx颗xx级宝石
    case TASK_TARGET_9:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(arg2);
        }
        break;
        // XX的护魂升级到xx级
    case TASK_TARGET_10:
        InternalLevelTriggerTaskTarget();
        break;
        // 获得xx点公会贡献点
    case TASK_TARGET_11:
        {
            AddTargetValidNum(arg1);
        }
        break;
        // 激活xx个xx化身
    case TASK_TARGET_12: 
        {
            InternalLevelTriggerTaskTarget();
        }
        break;
        // 化身系统购买xx宝箱xx次
    case TASK_TARGET_13:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(arg2);
        }
        break;
        // 领取多xx次僵尸生存礼
    case TASK_TARGET_15:
        {
            AddTargetValidNum(arg1);
        }
        break;
        // 跳转场景
    case TASK_TARGET_17:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(1);
        }
        break;
        // 镶嵌xx级宝石
    case TASK_TARGET_20:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(arg2);
        }
        break;
        // 装备升星（特定）
    case TASK_TARGET_23:
        // 装备升级（特定）
    case TASK_TARGET_24:
        // 品质强化（特定）
    case TASK_TARGET_25:
        InternalLevelTriggerTaskTarget();
        break;
        // 宝石镶嵌
    case TASK_TARGET_26:
        // 宝石合成
    case TASK_TARGET_27:
        // 宠物激发
    case TASK_TARGET_28:
        // 装备骑宠
    case TASK_TARGET_36:
        // 添加好友
    case TASK_TARGET_38:
        {
            AddTargetValidNum(1);
        }
        break;
        // 开启星点(特定)
    case TASK_TARGET_39:
        InternalLevelTriggerTaskTarget();
        break;
        // 参加活动
    case TASK_TARGET_41:
        {
            if(GetTargetValue())
            {
                if(GetTargetValue() == arg1)
                    AddTargetValidNum(1);
            }
            else
            {
                AddTargetValidNum(1);
            }
        }
        break;
        // 翅膀提升（特定）
    case TASK_TARGET_42:
       InternalLevelTriggerTaskTarget();
        break;
        // npc购买
    case TASK_TARGET_43:
        {
            if(GetTargetValue())
            {
                if(GetTargetValue() == arg1)
                    AddTargetValidNum(1);
            }
            else
            {
                AddTargetValidNum(1);
            }
        }
        break;
        // 勇者争霸
    case TASK_TARGET_45:
        // 日常任务（一轮）
    case TASK_TARGET_46:
        // 讨伐令
    case TASK_TARGET_47:
        // 砸金像
    case TASK_TARGET_49:
        // 藏宝库
    case TASK_TARGET_50:
        // 切磋 改为组队一次
    case TASK_TARGET_52:
        // 护卫
    case TASK_TARGET_54:
        // 召唤坐骑
    case TASK_TARGET_55:
        // 装备升星（次数）
    case TASK_TARGET_56:
        // 开启星点（次数）
    case TASK_TARGET_57:
        // 翅膀提升（次数）
    case TASK_TARGET_58:
        // 升级符文（次数）
    case TASK_TARGET_59:
        {
            AddTargetValidNum(1);
        }
        break;
    // 拥有公会
    case TASK_TARGET_62:
        InternalLevelTriggerTaskTarget();
        break;
        // 公会捐赠
    case TASK_TARGET_63:
        // 完成公会任务
    case TASK_TARGET_64:
        // 使用宠物一键激发
    case TASK_TARGET_65:
        // 圣剑升星
    case TASK_TARGET_66:        // 装备升级（次数）
    case TASK_TARGET_69:
        // 装备升品（次数）
    case TASK_TARGET_70:
        // 每日累计消费
    case TASK_TARGET_71:
        {
            AddTargetValidNum(arg1);
        }
        break;
        // 累计充值
    case TASK_TARGET_72:
        InternalLevelTriggerTaskTarget();
        break;
        // 每日单笔最大充值金额
    case TASK_TARGET_73:
        InternalLevelTriggerTaskTarget();
        break;
        // 每日充值，当天的充值金额
    case TASK_TARGET_75:
        InternalLevelTriggerTaskTarget();
        break;
        // 角色升级
    case TASK_TARGET_76:
        InternalLevelTriggerTaskTarget();
        break;
        // 符文升级（特定）
    case TASK_TARGET_77:
        // 获得坐骑（品质、数量）
    case TASK_TARGET_78:
        // 累计充值(从开服开始算)
    case TASK_TARGET_79:
        // 累计充值(循环)
    case TASK_TARGET_120:
        InternalLevelTriggerTaskTarget();
        break;
        // 累计消费
    case TASK_TARGET_80:
        {
            AddTargetValidNum(arg1);
        }
        break;
        // 全身镶嵌宝石等级
    case TASK_TARGET_81:
        // 提升某坐骑到特定等级
    case TASK_TARGET_82:
        // 提升某被动技能到特定等级
    case TASK_TARGET_83:
        // 宠物护魂激活护魂（特定）
    case TASK_TARGET_85:
        // 提升某宠物到特定等级
    case TASK_TARGET_86:
        // 镶嵌特定数量宝石
    case TASK_TARGET_87:
        InternalLevelTriggerTaskTarget();
        break;
        // 物品兑换
    case TASK_TARGET_89:
        InternalLevelTriggerTaskTarget();
        break;
    // 每日登陆
    case TASK_TARGET_95:
        {
            AddTargetValidNum(arg2);
        }
        break;
        // 累计在线时常
    case TASK_TARGET_97:
        InternalLevelTriggerTaskTarget();
        break;
        // 微端奖励
    case TASK_TARGET_98:
        InternalLevelTriggerTaskTarget();
        break;
        // 微端登陆F
    case TASK_TARGET_99:
        {
            AddTargetValidNum(1);
        }
        break;
        // 所有使用宝藏卡和魔石的抽取行为抽XX次
    case TASK_TARGET_100:
        // 铂金抽取XX次
    case TASK_TARGET_101:
        // 至尊抽取XX次
    case TASK_TARGET_102:
        {
            AddTargetValidNum(arg2);
        }
        break;
        // 单笔充值多少魔石，获得道具
    case TASK_TARGET_103:
        {
            if((int32)arg1 >= GetMaxNum())
            {
                int32 count = arg1 / GetMaxNum();
                AddTargetValidNum(GetMaxNum() * count);
            }
        }
        break;
        // vip等级value达到人数num
    case TASK_TARGET_104:
        // VIP等级达到多少级
    case TASK_TARGET_105:
        InternalLevelTriggerTaskTarget();
        break;
        // 珍兽挑战 参与次数
    case TASK_TARGET_106:
        // 月黑时刻 参与次数
    case TASK_TARGET_107:
        // 珍兽克星 击杀次数
    case TASK_TARGET_108:
        {
            AddTargetValidNum(arg1);
        }
        break;
    case TASK_TARGET_109:   // 充值排行发奖区间
    case TASK_TARGET_110:   // 充值排行上榜
    case TASK_TARGET_112:   // 翅膀排行发奖区间
    case TASK_TARGET_113:
    case TASK_TARGET_114:
    case TASK_TARGET_115:
    case TASK_TARGET_116:
    case TASK_TARGET_123:
    case TASK_TARGET_124:
    case TASK_TARGET_130:
    case TASK_TARGET_131:
    case TASK_TARGET_132:

        InternalLevelTriggerTaskTarget();
        break;
    case TASK_TARGET_111:   // 单笔充值奖励
        {
            // 判断充值金额,直接发送奖励
            if(m_target->num == 1 && (int32)arg1 >= m_target->value)
            {
                AddTargetValidNum(GetMaxNum());
            }
            else if((int32)arg1 >= m_target->value && (int32)arg1 < m_target->num)
            {
                AddTargetValidNum(GetMaxNum());
            }
        }
        break;
    // 每日登陆(重置)
    case TASK_TARGET_117:
        {
            //AddTargetValidNum(arg2);
            m_player->SendMyselfEvent(EVENT_TASK_NUM_DAILY_LOGIN, m_pTask->GetFestivalID(), 1, 0);
        }
        break;
    // 砸金蛋次数
    case TASK_TARGET_118:
        {
            if(GetTargetValue() == arg1)
                AddTargetValidNum(arg2);
        }
        break;
    // 累计消费(循环)
    case TASK_TARGET_119:
        {
            if(int32(m_num + arg1) >= m_max_num)
            {
                int32 count = int32((m_num + arg1) / m_max_num);
                int32 num = (m_num + arg1) % m_max_num;
                SetTargetValidNum(num);
                m_player->SendMyselfEvent(EVENT_TASK_NUM_TOTAL_CONSUMPTION, m_pTask->GetFestivalID(), count, 0);
                CnDbg("EVENT_TASK_NUM_TOTAL_CONSUMPTION event %d\n", count);
            }
            else
            {
                AddTargetValidNum(arg1);
            }
        }
        break;
        // 翅膀升阶
    case TASK_TARGET_121:
        // 圣剑升星
    case TASK_TARGET_122:
        {
            if(GetTargetValue() == 0)
            {
                if(arg1 > (uint32)GetNum())
                    SetTargetValidNum(arg1);
            }
            else
            {
                if(GetTargetValue() == arg2 && arg1 > (uint32)GetNum())
                    SetTargetValidNum(arg1);
            }
        }
        break;
    case TASK_TARGET_125:   // 化身战力
    case TASK_TARGET_126:   // 宠物战力
    case TASK_TARGET_127:   // 翅膀战力
    case TASK_TARGET_128:   // 坐骑战力
    case TASK_TARGET_129:   // 总战力
        {
            if(GetTargetValue() == arg2 && arg1 >= (uint32)GetMaxNum())
            {
                SetTargetValidNum(GetMaxNum());
            }
        }
        break;
    default:
        CnAssert(false);
        CnError("OnTriggerTask unexpected task type:%u\n", GetTargetType());
        break;
    }
    return m_IsChangeFlag;
}


bool RoleTaskTarget::InternalLevelTriggerTaskTarget()
{
    CnAssert(IsLevelTriggerType());

    int32 old_num = m_num;

    switch(GetTargetType())
    {
    // 领取XX等级成长礼包
    case TASK_TARGET_8:
        {
            OnlineRewardManager* onlinereward = m_player->GetOnlineRewardManager();
            if(onlinereward)
            {
                ONLINE_REWARD_INFO info = onlinereward->GetInfo();
                SetTargetValidNum(info.level);
            }
        }
        break;
    // 提升XX护魂到XX等级
    case TASK_TARGET_10:
        {
            //std::vector<PROTECT_SOUL> list;
            //m_player->GetPetSoulManager()->GetActivieProtectSoulList(list);
            //for( std::vector<PROTECT_SOUL>::iterator it = list.begin(); it != list.end(); ++it)
            //{
            //    if(it->id == GetTargetValue())
            //    {
            //        SetTargetValidNum(it->level);
            //    }
            //}
        }
        break;
    // 获得XX个XX星化身
    case TASK_TARGET_12:
        {
            QualityNumList fashionQuality = m_player->GetFashionManager()->GetQualityNumList();
            QualityNumList::iterator quality_iter;
            for (quality_iter = fashionQuality.begin(); quality_iter != fashionQuality.end(); quality_iter++)
            {
                if(quality_iter->level == (int32)GetTargetValue())
                {
                    SetTargetValidNum(quality_iter->num);
                }
            }
        }
        break;
    // 装备升星（特定）
    case TASK_TARGET_23:
        {
//             EQUIP_INFO*     equipInfo = NULL;
//             EquipManager*   equipMgr = m_player->GetEquipManager();
//             if(GetTargetValue())
//             {
//                 if(GetTargetValue() == EQUIP_WHOLE_BODY)
//                     equipInfo = equipMgr->GetEquipByMinStar();
//                 else
//                     equipInfo = equipMgr->GetEquipByType(GetTargetValue());
//             }
//             else
//                 equipInfo = equipMgr->GetEquipByMaxStar();
// 
//             if(equipInfo)
//                 SetTargetValidNum(equipInfo->star);
        }
        break;
    // 装备升级（特定）
    case TASK_TARGET_24:
        {
//             EQUIP_INFO*     equipInfo = NULL;
//             EquipManager*   equipMgr = m_player->GetEquipManager();
//             if(GetTargetValue())
//             {
//                 if(GetTargetValue() == EQUIP_WHOLE_BODY)
//                     equipInfo = equipMgr->GetEquipByMinLevel();
//                 else
//                     equipInfo = equipMgr->GetEquipByType(GetTargetValue());
//             }
//             else
//                 equipInfo = equipMgr->GetEquipByMaxLevel();
// 
//             if(equipInfo)
//                 SetTargetValidNum(equipInfo->level);
        }
        break;
    // 品质强化（特定）
    case TASK_TARGET_25:
        {
//             EQUIP_INFO*     equipInfo = NULL;
//             EquipManager*   equipMgr = m_player->GetEquipManager();
//             if(GetTargetValue())
//             {
//                 if(GetTargetValue() == EQUIP_WHOLE_BODY)
//                     equipInfo = equipMgr->GetEquipByMinQuality();
//                 else
//                     equipInfo = equipMgr->GetEquipByType(GetTargetValue());
//             }
//             else
//                 equipInfo = equipMgr->GetEquipByMaxQuality();
// 
//             if(equipInfo)
//                 SetTargetValidNum(equipInfo->quality);
        }
        break;
    // 开启星点(特定)
    case TASK_TARGET_39:
        SetTargetValidNum(m_player->GetHoroscopeLevel());
        break;
    // 翅膀提升（特定）
    case TASK_TARGET_42:
        SetTargetValidNum(m_player->GetWingManager()->GetWingLevel());
        break;
    // 拥有公会
    case TASK_TARGET_62:
        {
            SetTargetValidNum( m_player->GetGuildID() ? 1 : 0 );
        }
        break;
    // 累计充值
    case TASK_TARGET_72:
        {
            int32 num = m_player->GetRechargeManager()->GetTotalRecharge(m_pTask);
            SetTargetValidNum(num);
        }
        break;
    // 每日单笔最大充值金额
    case TASK_TARGET_73:
        {
            int32 num = m_player->GetRechargeManager()->GetTodayRecharge();
            SetTargetValidNum(num);
        }
        break;
    // 每日充值
    case TASK_TARGET_75:
        {
            int32 num = m_player->GetRechargeManager()->GetTodayRecharge(false);
            SetTargetValidNum(num);
        }
        break;
    // 角色等级
    case TASK_TARGET_76:
        {
            SetTargetValidNum(m_player->GetLevel());
        }
        break;
    // 符文升级（特定）
    case TASK_TARGET_77:
        {
            SkillInfoManager* skillMgr = m_player->GetSkillInfoMgr();
            if(skillMgr)
            {
                //if(GetTargetValue() == SKILL_RUNE_FIRE)
                //    SetTargetValidNum(skillMgr->GetRuneLevelByRuneType(SKILL_RUNE_FIRE));
                //else if(GetTargetValue() == SKILL_RUNE_ICE)
                //    SetTargetValidNum(skillMgr->GetRuneLevelByRuneType(SKILL_RUNE_ICE));
                //else if(GetTargetValue() == SKILL_RUNE_THUNDER)
                //    SetTargetValidNum(skillMgr->GetRuneLevelByRuneType(SKILL_RUNE_THUNDER));
                //else if(GetTargetValue() == 0)
                //    SetTargetValidNum(skillMgr->GetMaxSkillLevel());
                //else if(GetTargetValue() == EQUIP_WHOLE_BODY)
                //    SetTargetValidNum(skillMgr->GetMinSkillLevel());
            }
        }
        break;
    // 获得坐骑（品质、数量）
    case TASK_TARGET_78:
        {
            RideManager* rideMgr = m_player->GetRideManager();
            if(rideMgr)
            {
                SetTargetValidNum( rideMgr->GetRideNumByQuality(GetTargetValue()) );
            }
        }
        break;
    // 累计充值（开服）
    case TASK_TARGET_79:
        {
            int32 num = m_player->GetRechargeManager()->GetTotalRecharge();
            uint32 rechargeTime = m_player->GetRechargeManager()->GetTotalRechargeTime();
            CnAssert(GetTargetValue() != 0);           // 充值时间的时间戳不应该为0
            if(rechargeTime > GetTargetValue())        // 充值时间大于在任务目标时间时 才算首充成功
                SetTargetValidNum(num);
        }
        break;
    // 全身镶嵌宝石等级
    case TASK_TARGET_81:
        {
            //SetTargetValidNum(m_player->GetEquipManager()->GetEquipMinJewelLevel());
        }
        break;
    // 提升某坐骑到特定等级
    case TASK_TARGET_82:
        {
            RideManager* rideMgr = m_player->GetRideManager();

            if(rideMgr)
            {
                if(GetTargetValue())
                {
                    RIDE_PET_INFO* petInfo = rideMgr->GetRideInfo(GetTargetValue());
                    if(petInfo)
                    {
                        SetTargetValidNum(petInfo->level);
                    }
                }
                else
                {
                    SetTargetValidNum(rideMgr->GetMaxRideLevel());
                }
            }
        }
        break;
    // 提升某被动技能到特定等级
    case TASK_TARGET_83:
        {
            PassiveSkillManager* passiveSkillMgr = m_player->GetPassiveSkillManager();
            PASSIVE_SKILL_INFO* passiveSkill = 0;

            if(passiveSkillMgr)
            {
                if(GetTargetValue())
                {
                    passiveSkill = passiveSkillMgr->GetOneSkillInfo(GetTargetValue());
                    if(passiveSkill)
                    {
                        SetTargetValidNum(passiveSkill->level);
                    }
                }
                else
                {
                    int32 level = passiveSkillMgr->GetMaxPassiveSkillLevel();
                    SetTargetValidNum(level);
                }
            }
        }
        break;
    // 宠物护魂激活护魂（特定）
    case TASK_TARGET_85:
        {
            //std::vector<uint32> list;
            //m_player->GetPetSoulManager()->GetActivieProtectSoulIDList(list);
            //for(std::vector<uint32>::iterator it = list.begin(); it != list.end(); ++it)
            //{
            //    if(*it == GetTargetValue())
            //    {
            //        SetTargetValidNum(1);
            //        break;
            //    }
            //}
        }
        break;
    // 提升某宠物到特定等级
    case TASK_TARGET_86:
        {
            //FollowPetManager* followPetMgr = m_player->GetFollowPetManager();

            //if(followPetMgr)
            //{
            //    if(GetTargetValue())
            //    {
            //        FOLLOW_PET_INFO* petInfo = followPetMgr->GetFollowPetInfo(GetTargetValue());
            //        if(petInfo)
            //        {
            //            SetTargetValidNum(petInfo->cur_level);
            //        }
            //    }
            //    else
            //    {
            //        int32 level = followPetMgr->GetMaxPetLevel();
            //        SetTargetValidNum(level);
            //    }
            //}
        }
        break;
    // 镶嵌特定数量宝石
    case TASK_TARGET_87:
        {
            //int32 num = m_player->GetEquipManager()->GetWholeBodyJewelNum();
            //SetTargetValidNum(num);
        }
        break;
    // 物品兑换
    case TASK_TARGET_89:
        {
            if(GetTargetValue())
            {
                //ItemManager* itemMgr  = m_pTask->GetPlayer()->GetItemManager();
                //int32 itemNum = itemMgr->GetItemNum(GetTargetValue());
                //SetTargetValidNum(itemNum);
            }
        }
        break;
    // 累计在线时长
    case TASK_TARGET_97:
        {
            //总在线时长
            //SetTargetValidNum(m_player->GetOnLineTime());
            //每日在线时长
            SetTargetValidNum(m_player->GetOnlineRewardManager()->GetOnlineRewardTime());
        }
        break;
    // 微端奖励
    case TASK_TARGET_98:
        {
            int32 num = 1;
            SetTargetValidNum(num);
        }
        break;
    // vip等级value达到人数num
    case TASK_TARGET_104:
        {
            int32 playerNum = VipStatisticsManager::Instance()->GetPlayerNumByVipLevel(GetTargetValue());
            SetTargetValidNum(playerNum);
        }
        break;
    // VIP等级达到多少级
    case TASK_TARGET_105:
        {
            SetTargetValidNum(m_player->GetVipLevel());
        }
        break;
    // 充值排行发奖区间
    case TASK_TARGET_109:
        {
            uint32 num = 0;
            if (TopUpRankManager::Instance()->IsInReceiveRewardTime((uint32)Crown::SDNow().GetTimeValue(), m_pTask->GetFestivalID()))
            {
                uint32 indexID = m_pTask->GetFestivalID() * 1000 + TASK_TARGET_109;
                uint32 rank = TopUpRankManager::Instance()->GetTopUpRank(m_player->GetPlayerID(), indexID);
                if (rank >= GetTargetValue() && int32(rank) <= m_max_num)
                {
                    num = m_max_num;
                }
            }
            SetTargetValidNum(num);
        }
        break;
    // 充值排行上榜
    case TASK_TARGET_110:
        {
            
            uint32 num = 0;
            if (TopUpRankManager::Instance()->IsInReceiveRewardTime((uint32)Crown::SDNow().GetTimeValue(), m_pTask->GetFestivalID()))
            {
                uint32 indexID = m_pTask->GetFestivalID() * 1000 + TASK_TARGET_109;
                uint32 rank = TopUpRankManager::Instance()->GetTopUpRank(m_player->GetPlayerID(), indexID);
                num = rank != 0 ? 1 : 0;
            }
            SetTargetValidNum(num);
        }
        break;
    // 翅膀排行发奖区间
    case TASK_TARGET_112:
    case TASK_TARGET_113:
    case TASK_TARGET_114:
    case TASK_TARGET_115:
    case TASK_TARGET_116:
    case TASK_TARGET_130:
    case TASK_TARGET_124:
        {
            uint32 num = 0;
            if (TopUpRankManager::Instance()->IsInReceiveRewardTime((uint32)Crown::SDNow().GetTimeValue(), m_pTask->GetFestivalID()))
            {
                uint32 indexID = m_pTask->GetFestivalID() * 1000 + GetTargetType();
                uint32 rank = TopUpRankManager::Instance()->GetTopUpRank(m_player->GetPlayerID(), indexID);
                if (rank >= GetTargetValue() && int32(rank) <= m_max_num)
                {
                    num = m_max_num;
                }
            }
            SetTargetValidNum(num);
        }
        break;
    // 累计充值(循环)
    case TASK_TARGET_120:
        {
            int32 num = m_player->GetRechargeManager()->GetTotalRecharge(m_pTask);
            int32 tCount = int32(num / m_max_num);
            if(tCount > m_num)
            {
                int32 count = tCount - m_num;
                SetTargetValidNum(tCount);
                m_player->SendMyselfEvent(EVENT_TASK_NUM_CUMULATIVE_RECHARGE, m_pTask->GetFestivalID(), count, 0);
                CnDbg("EVENT_TASK_NUM_CUMULATIVE_RECHARGE event %d\n", count);
            }
        }
        break;
    case TASK_TARGET_123:
        {
            // 重置任务
            bool status= TaskFestiveConfig::Instance()->IsInActivityValidTime(m_player->GetCenterSvrID(), m_pTask->GetFestivalID(), m_num);
            if(!status)
            {
                //SetTargetValidNum(curt);
                m_num = TaskFestiveConfig::Instance()->GetActivityBeginTime(m_player->GetCenterSvrID(), m_pTask->GetFestivalID());
                m_player->GetTaskManager()->ResetTaskFestival(m_player->GetCenterSvrID(), m_pTask->GetFestivalID());
            }
        }
        break;
    // 升级任意化身，多少级
    case TASK_TARGET_131:
        {
            int32 level = m_player->GetFashionManager()->GetTaskFashionLevel(GetTargetValue());
            if(level > GetNum())
                SetTargetValidNum(level);
        }
        break;
    // 任意指定XX数量的XX星级以上的化身，升级到XX等级
    case TASK_TARGET_132:
        {
            // 化身等级
            uint32 star = GetTargetValue() / 1000;
            // 化身星级
            uint32 level = GetTargetValue() % 1000;

            int32 num = m_player->GetFashionManager()->GetTaskFashionStarLevel(star, level);
            if(num > GetNum())
                SetTargetValidNum(num);
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    return old_num != m_num;
}