#include "gameholder_pch.h"
#include "task_game_config_data.h"
#include "task_config_manager.h"
#include "config_reader.h"


IMPLEMENT_SINGLETON(TaskGameConfigData)

TaskGameConfigData::TaskGameConfigData()
{

}

TaskGameConfigData::~TaskGameConfigData()
{
    m_main_task.clear();
    m_daily_task.clear();
    m_all_task.clear();

    for(TaskTemplateList::iterator iter = m_tasklist.begin(); iter != m_tasklist.end(); iter++)
    {
        const TASK_TEMPLATE* pTaskTmpl = *iter;
        CnDelete pTaskTmpl;
    }
    m_tasklist.clear();
}

bool TaskGameConfigData::InitConfigData()
{
    const TASK_GAME_ROOT_STRUCT & RootStruct = TASK_GAME_ENTRY::Instance()->GetStruct();
    std::vector<TASK_GAME_ROOT_TASK_STRUCT>::const_iterator cIter = RootStruct.task_list.begin();
    for(; cIter != RootStruct.task_list.end(); ++cIter) 
    {
        // 任务模板数据
        TASK_TEMPLATE* pTaskTmpl = CnNew TASK_TEMPLATE;

        // 读取任务模板
        ReadTaskConfigData(pTaskTmpl, &(*cIter));

        // 任务模板列表
        m_tasklist.push_back(pTaskTmpl);

        // 各类型任务模板列表
        switch(pTaskTmpl->task_type)
        {
        case TASK_TYPE_MAIN:
            {
                m_main_task[pTaskTmpl->task_id] = pTaskTmpl;

                // 主线任务依赖表
                const TASK_CONDITION* condition = GetTaskPreCondition(pTaskTmpl, TASK_PRECOND_MAIN_TASKID);
                if(condition)
                {
                    CnAssert(condition->type);
                    m_premain_task[condition->value] = pTaskTmpl;
                }
            }
            break;
        case TASK_TYPE_DAILY:
            m_daily_task[pTaskTmpl->task_id] = pTaskTmpl;
            break;
        case TASK_TYPE_CRUSADE:
            m_crusade_task[pTaskTmpl->task_id] = pTaskTmpl;
            break;
        case TASK_TYPE_ACHIVE:
            m_achive_task[pTaskTmpl->task_id] = pTaskTmpl;
            break;
        default:
            CnAssert(false);
            break;
        }

        m_all_task[pTaskTmpl->task_id] = pTaskTmpl;
    }

    return true;
}

void TaskGameConfigData::ReadTaskConfigData(TASK_TEMPLATE* pTaskTmpl, const TASK_GAME_ROOT_TASK_STRUCT* pTaskConfig)
{
    // 任务ID
    pTaskTmpl->task_id = pTaskConfig->task_id;
    // 任务类型
    pTaskTmpl->task_type = pTaskConfig->task_type;
    // 任务子类型
    pTaskTmpl->sub_type = pTaskConfig->sub_type;
    // 任务名字
    pTaskTmpl->name = pTaskConfig->task_name;
    // 任务描述
    pTaskTmpl->desc = "";
    // 任务图标
    pTaskTmpl->icon = "";
    // 任务对应的运营活动ID
    pTaskTmpl->festival_id = 0;
    // 任务可循环次数
    pTaskTmpl->loop_num = 0;
    // 接任务的npc
    pTaskTmpl->take_npc_id = 0;
    // 交任务的npc
    pTaskTmpl->cmpl_npc_id = 0;
    // 是否能够扫荡
    pTaskTmpl->raids = false;
    // 前置条件
    ParsePreCondition(pTaskTmpl, pTaskConfig->pre_condition);
    // 任务目标
    ParseTaskTarget(pTaskTmpl, pTaskConfig->task_target);
    // 任务给予(暂时没有)
    pTaskTmpl->gives.Clear();
    // 任务奖励
    ParseTaskReward(pTaskTmpl, pTaskConfig->task_reward);
    // 任务奖励
    ParseTaskRewardList(&pTaskTmpl->warrior_reward, pTaskConfig->warrior_reward);
    // 任务奖励
    ParseTaskRewardList(&pTaskTmpl->mage_reward, pTaskConfig->mage_reward);
    // 任务奖励
    ParseTaskRewardList(&pTaskTmpl->archer_reward, pTaskConfig->archer_reward);


}
// 解析任务前置条件
void TaskGameConfigData::ParsePreCondition(TASK_TEMPLATE* pTaskTmpl, const std::string& preCondtionStr)
{
    std::vector<Data3> data3_list = ConfigReader::parseData3List(preCondtionStr);

    for(uint32 i=0; i<data3_list.size(); i++)
    {
        const Data3 data = data3_list[i];

        TASK_CONDITION preCond;
        preCond.type = data.value1;

        switch(preCond.type)
        {
        case TASK_PRECOND_MAIN_TASKID:      // 主线任务ID
        case TASK_PRECOND_LEVEL:            // 角色等级大于
        case TASK_PRECOND_BRANCH_TASKID:    // 支线任务ID
        case TASK_PRECOND_LESS_LEVEL:       // 角色等级小于
            break;
        default:
            {
                CnWarn("Unknow task condition type:%u\n", preCond.type);
            }
            break;
        }

        preCond.value = data.value2;

        pTaskTmpl->pre_conditions.push_back(preCond);
    }
}
// 解析任务奖励
void TaskGameConfigData::ParseTaskReward(TASK_TEMPLATE* pTaskTmpl, const std::string& taskRewardStr)
{
    std::vector<REWARD_ITEM> rewartItems = ConfigReader::parseRewardItemList(taskRewardStr);

    for(uint32 i=0; i<rewartItems.size(); i++)
    {
        pTaskTmpl->rewards.items.push_back(rewartItems[i]);
    }
}

// 解析任务奖励
void TaskGameConfigData::ParseTaskRewardList(REWARD_TABLE* pReward, const std::string& taskRewardStr)
{
    std::vector<REWARD_ITEM> rewartItems = ConfigReader::parseRewardItemList(taskRewardStr);

    for(uint32 i=0; i<rewartItems.size(); i++)
    {
        pReward->items.push_back(rewartItems[i]);
    }
}

// 加载任务目标
void TaskGameConfigData::ParseTaskTarget(TASK_TEMPLATE* pTaskTmpl, const std::string& taskTargetStr)
{
    std::vector<int32> ini_list = ConfigReader::parseIntList(taskTargetStr);
    if(ini_list.size() <= 0)
    {
        CnAssert(false);
        CnFatal("task %d no target\n", pTaskTmpl->task_id);
        return;
    }


    TASK_TARGET taskTarget;
    int32 errCount = 0;

    taskTarget.type = ini_list[0];


    // 根据任务目标读取配置(请参照《任务_任务类型表》了解各目标的参数意义)
    switch(taskTarget.type)
    {
    case TASK_TARGET_1:     // 杀怪物收集道具
        {
            CnAssert(ini_list.size() == 7);
            taskTarget.npc_id = ini_list[1];
            taskTarget.map_id = ini_list[2];
            taskTarget.value = ini_list[3];
            taskTarget.item_id = ini_list[4];
            taskTarget.rate = ini_list[5];
            taskTarget.num = ini_list[6];
        }
        break;
    case TASK_TARGET_3:     // 杀怪物
        {
            CnAssert(ini_list.size() == 5);
            taskTarget.npc_id = ini_list[1];
            taskTarget.map_id = ini_list[2];
            taskTarget.value = ini_list[3];
            taskTarget.num = ini_list[4];
        }
        break;
    case TASK_TARGET_4:     // 通关副本
        {
            CnAssert(ini_list.size() == 4);
            taskTarget.npc_id = ini_list[1];
            taskTarget.map_id = ini_list[2];
            taskTarget.num = ini_list[3];
        }
        break;
    case TASK_TARGET_6:     // 对话
        {
            CnAssert(ini_list.size() == 2);
            taskTarget.value = ini_list[1];                     // npc id
            taskTarget.num = 1;
        }
        break;
    case TASK_TARGET_17:    // 跳转场景
        {
            CnAssert(ini_list.size() == 4);
            taskTarget.npc_id = ini_list[1];                    // 要摸的trigger
            taskTarget.map_id = ini_list[2];                    // trigger触发的跳副本id
            taskTarget.value = ini_list[3];                     // 去哪个副本完成任务
            taskTarget.num = 1;
        }
        break;
    case TASK_TARGET_9:     // 合成xx颗xx级宝石
    case TASK_TARGET_10:    // 提升XX护魂到XX等级
    case TASK_TARGET_12:    // 获得XX个XX星化身
    case TASK_TARGET_13:    // 化身系统购买XX宝箱XX次
    case TASK_TARGET_20:    // 镶嵌XX颗XX级宝石
    case TASK_TARGET_23:    // 装备升星（特定）
    case TASK_TARGET_24:    // 装备升级（特定）
    case TASK_TARGET_25:    // 品质强化（特定）
    case TASK_TARGET_41:    // 参加活动
    case TASK_TARGET_77:    // 符文等级（特定）
    case TASK_TARGET_78:    // 获得坐骑
    case TASK_TARGET_79:    // 累计充值（开服）
    case TASK_TARGET_81:    // 全身镶嵌宝石等级
    case TASK_TARGET_82:    // 坐骑升级
    case TASK_TARGET_83:    // 被动技能升级
    case TASK_TARGET_85:    // 激活护魂迷宫
    case TASK_TARGET_86:    // 提升宠物等级
    case TASK_TARGET_87:    // 镶嵌特定宝石数
    case TASK_TARGET_89:    // 物品兑换
    case TASK_TARGET_104:   // vip等级value达到num人
    case TASK_TARGET_109:   // 充值排行发奖区间
    case TASK_TARGET_111:   // 单笔充值无限奖励
    case TASK_TARGET_112:   // 翅膀排行发奖区间
    case TASK_TARGET_113:   // 宠物排行发奖区间
    case TASK_TARGET_114:   // 坐骑排行发奖区间
    case TASK_TARGET_115:   // 战力排行发奖区间
    case TASK_TARGET_116:   // 等级排行发奖区间
    case TASK_TARGET_118:   // 砸金蛋次数
    case TASK_TARGET_121:   // 翅膀升阶
    case TASK_TARGET_122:   // 圣剑升星
    case TASK_TARGET_123:   // 重置活动
    case TASK_TARGET_130:   // 化身战力排行活动
    case TASK_TARGET_124:   // 勇者争霸排行活动
    case TASK_TARGET_125:   // 化身战力
    case TASK_TARGET_126:   // 宠物战力
    case TASK_TARGET_127:   // 翅膀战力
    case TASK_TARGET_128:   // 坐骑战力
    case TASK_TARGET_129:   // 总战力
    case TASK_TARGET_131:   // 升级任意化身，多少级
    case TASK_TARGET_132:   // 化身相关(圣剑功能)
        {
            CnAssert(ini_list.size() == 3);
            taskTarget.value = ini_list[1];
            taskTarget.num = ini_list[2];
        }
        break;
    case TASK_TARGET_7:     // 领取勇者争霸奖励次数
    case TASK_TARGET_8:     // 领取XX等级成长礼包
    case TASK_TARGET_11:    // 获得XX点公会贡献点
    case TASK_TARGET_15:    // 领取僵尸生存宝箱XX次
    case TASK_TARGET_26:    // 镶嵌装备宝石
    case TASK_TARGET_27:    // 装备合成宝石
    case TASK_TARGET_28:    // 宠物激发
    case TASK_TARGET_36:    // 设置骑宠
    case TASK_TARGET_38:    // 添加好友
    case TASK_TARGET_39:    // 开启星点
    case TASK_TARGET_42:    // 翅膀升阶
    case TASK_TARGET_43:    // NPC购买物品
    case TASK_TARGET_45:    // 勇者争霸
    case TASK_TARGET_46:    // 完成一轮日常任务
    case TASK_TARGET_47:    // 讨伐令
    case TASK_TARGET_49:    // 砸金像
    case TASK_TARGET_50:    // 进入宝库
    case TASK_TARGET_52:    // 切磋 改为组队一次
    case TASK_TARGET_54:    // 护卫
    case TASK_TARGET_55:    // 召唤坐骑
    case TASK_TARGET_56:    // 装备升星次数
    case TASK_TARGET_57:    // 开启星点
    case TASK_TARGET_58:    // 翅膀提升
    case TASK_TARGET_59:    // 技能符文升级
    case TASK_TARGET_62:    // 加入公会
    case TASK_TARGET_63:    // 捐赠公会贡献点
    case TASK_TARGET_64:    // 完成多少次公会任务
    case TASK_TARGET_65:    // 多少次宠物一件激发
    case TASK_TARGET_66:    // 圣剑升星多少次
    case TASK_TARGET_69:    // 装备升级（次数）
    case TASK_TARGET_70:    // 装备升品（次数）
    case TASK_TARGET_71:    // 每日消费
    case TASK_TARGET_72:    // 累计充值
    case TASK_TARGET_73:    // 单笔充值
    case TASK_TARGET_74:    // 每日战力比拼
    case TASK_TARGET_75:    // 每日充值
    case TASK_TARGET_76:    // 角色等级
    case TASK_TARGET_80:    // 累计消费
    case TASK_TARGET_88:    // 微端登陆
    case TASK_TARGET_97:    // 累计在线时长
    case TASK_TARGET_98:    // 微端奖励
    case TASK_TARGET_99:    // 微端登陆
    case TASK_TARGET_95:    // 每日登陆
    case TASK_TARGET_100:   // 所有使用宝藏卡和魔石的抽取行为抽XX次
    case TASK_TARGET_101:   // 铂金抽取XX次
    case TASK_TARGET_102:   // 至尊抽取XX次
    case TASK_TARGET_103:   // 单笔充值
    case TASK_TARGET_105:   // vip等级
    case TASK_TARGET_106:   // 珍兽挑战 参与次数
    case TASK_TARGET_107:   // 月黑时刻 参与次数
    case TASK_TARGET_108:   // 珍兽克星 击杀次数
    case TASK_TARGET_110:   // 充值排行上榜
    case TASK_TARGET_117:   // 每日登陆(重置)
    case TASK_TARGET_119:   // 累计消费(循环)
    case TASK_TARGET_120:   // 累计充值(循环)
        {
            CnAssert(ini_list.size() == 2);
            taskTarget.num = ini_list[1];
        }
        break;
    default:
        {
            ++errCount;
            //CnAssert(false);
            CnWarn("Error task:%u target type:%d\n", pTaskTmpl->task_id, taskTarget.type);
        }
        break;
    }

    if(errCount > 0)
    {
        CnWarn("Task target id:%u\n", taskTarget.id);
    }

    pTaskTmpl->targets.push_back(taskTarget);
}

const TASK_CONDITION* TaskGameConfigData::GetTaskPreCondition(TASK_TEMPLATE* pTaskTmpl, uint32 conditionType)
{
    for(uint32 i=0; i<pTaskTmpl->pre_conditions.size(); i++)
    {
        const TASK_CONDITION& condition = pTaskTmpl->pre_conditions[i];
        if(condition.type == conditionType)
            return &condition;
    }
    return NULL;
}

const TASK_TEMPLATE* TaskGameConfigData::GetMainTaskByPreID(uint32 pre_id)
{
    TaskTemplateMap::iterator it = m_premain_task.find(pre_id);
    if(it != m_premain_task.end())
    {
        return it->second;
    }

    // 如果没有以pre_id为前置的任务，则寻找id大于pre_id且相差最小的任务
    uint32 test_id = pre_id+1;
    TaskTemplateMap::const_iterator test_it = m_main_task.find(test_id);
    while(test_it == m_main_task.end() && (test_id - pre_id)<100)     // 尝试n次
    {
        test_id++;
        test_it = m_main_task.find(test_id);
    }

    if(test_it != m_main_task.end())
    {
        CnDbg("task %d no pre, replace to %d\n", pre_id, test_id);
        return test_it->second;
    }

    // 实在是找不到任务了
    return NULL;
}

const TASK_TEMPLATE* TaskGameConfigData::GetTaskTemplate(uint32 taskID)
{
    TaskTemplateMap::const_iterator iter = m_all_task.find(taskID);
    if(iter != m_all_task.end())
        return iter->second;
    else
        return NULL;
}

const TASK_TEMPLATE* TaskGameConfigData::GetCrusadeTemplate(uint32 taskID)
{
    TaskTemplateMap::const_iterator iter = m_crusade_task.find(taskID);
    if(iter != m_all_task.end())
        return iter->second;
    else
        return NULL;
}
