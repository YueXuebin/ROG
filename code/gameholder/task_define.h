#ifndef task_define_h__
#define task_define_h__


inline bool IsNormalTask(uint8 task_type)
{
    return task_type == TASK_TYPE_MAIN 
        || task_type == TASK_TYPE_BRANCH
        || task_type == TASK_TYPE_DAILY
        || task_type == TASK_TYPE_CRUSADE
        || task_type == TASK_TYPE_GUARD
        || task_type == TASK_TYPE_GUILD
        || task_type == TASK_TYPE_ACHIVE;
}

// ==================================================
/// 任务NPC条件显示
// ==================================================
typedef struct TaskObjShow
{
    void Clear()
    {
        m_obj_id    = 0;
        m_task_id   = 0;
        m_condition = 0;
    }
    uint32      m_obj_id;
    uint32      m_task_id;
    int32       m_condition;
}TaskObjShow;

// ==================================================
// 任务几率模板
// ==================================================
typedef struct ItemProbability
{
    void clear()
    {
        m_item          = 0;
        m_probability   = 0;
        m_multiple      = 0;
    }

    int32       m_item;                 // 类型 可以为品质 也可以指定某个任务
    uint32      m_probability;          // 几率
    uint32      m_multiple;             // 倍数
}ItemProbability;

// ==================================================
// 领取宝箱奖励
// ==================================================
typedef struct BoxReward
{
    void clear()
    {
        m_box_id        = 0;
        m_num           = 0;
        m_rewardlist.Clear();
    }

    uint32                  m_box_id;
    uint32                  m_num;
    REWARD_TABLE            m_rewardlist;
}BoxReward;


class RoleTask;


typedef std::list<RoleTask*>                    RoleTaskList;           // 玩家任务列表

typedef std::vector<FESTIVAL_RECORD>            FestivalRecordList;     // 运营活动任务记录

typedef std::map<uint32, TASK_FESTIVAL_TEMPLATE> TaskFestiveMap;        // 活动任务表<活动id, 活动任务模板>

typedef std::vector<TaskObjShow>                ObjShowList;
typedef std::map<uint32, ObjShowList>           ObjShowMap;         // NpcID 和 Npc任务条件显示
typedef std::map<uint32, TaskObjShow>           TriggerShowMap;

typedef std::map<uint32, BoxReward>             BoxRewardList;

typedef std::map<uint32, REWARD_TABLE>          LevelRewardsList;   // 等级奖励           <等级,奖励>
typedef std::map<uint32, LevelRewardsList>      TypeLevelRewards;   // 类型等级奖励       <任务类型,等级奖励>

typedef std::map<int32, ItemProbability>        ItemProList;        // 类型几率列表
typedef std::map<uint32, ItemProList>           QualityPro;         // 日常和讨伐令的几率 位数 <任务类型, 几率表>


typedef std::map<uint32, const TASK_TEMPLATE*>  TaskTemplateMap;        // <任务ID, 任务模板>
typedef std::vector<const TASK_TEMPLATE*>       TaskTemplateList;       // 任务模板列表




// 随机类型
int32 ItemRandom(const ItemProList& qualityList );
// 根据任务ID,推测任务类型
uint8 QueryTaskTypeById(uint32 taskID);



#endif // task_define_h__
