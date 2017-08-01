#include "centerserver_pch.h"
#include "festival_job.h"
#include "dbdriver.h"
#include "json_helper.h"
#include "festival_manager.h"

IMPLEMENT_SINGLETON(FestivalJob)

FestivalJob::FestivalJob(void)
{
}

FestivalJob::~FestivalJob(void)
{
}


// ==================================================
// 从本地数据库加载活动数据
// ==================================================
void FestivalJob::LoadLocalFestival()
{
    CnDbg("LoadLocalFestival\n");     // 从游戏数据库加载活动数据
    DBRecorder recorder;
    recorder = m_gameExecutor->Query("SELECT `id`, `name`, `type`, `show_pos`, `state`, `desc`,`sort`, `loop_hours`, "
                                    "UNIX_TIMESTAMP(`begin_time`), UNIX_TIMESTAMP(`end_time`), UNIX_TIMESTAMP(`trigger_time`), `task`, `condition`, `rewards`, `flag` FROM festival");
    if(recorder.HasExecut())
    {
        CBLoadFestivalInfo(recorder, false, true);
    }
}

// ==================================================
// 从GMT数据库加载活动数据(此功能不需要了)
// ==================================================
void FestivalJob::LoadGmtFestival()
{
    CnAssert(false);
    return;

    CnDbg("LoadGmtFestival\n");       // 从GMT数据库加载活动数据
    DBRecorder recorder;
    recorder = m_gmtExecutor->Query("SELECT `id`, `name`, `type`, `show_pos`, `state`, `desc`, `sort`, `loop_hours`, "
                                    "UNIX_TIMESTAMP(`begin_time`), UNIX_TIMESTAMP(`end_time`), UNIX_TIMESTAMP(`trigger_time`), `task`, `condition`, `rewards`, `flag` FROM festival");
    if(recorder.HasExecut())
    {
        CBLoadFestivalInfo(recorder, true, true);
    }
}

void FestivalJob::CBLoadFestivalInfo(DBRecorder& res, bool b_update, bool b_notify)
{
    TASK_FESTIVAL_TEMPLATE_LIST festival_list;

    Json::Reader reader;
    Json::Value json_value;

    std::string festival_task_str;
    std::string festival_condition_str;
    std::string festival_reward_str;

    uint64 begin_time = 0;
    uint64 end_time = 0;
    uint64 trigger_time = 0;
    uint32 flag = 0;

    if(!res.HasExecut())
    {
        CnError("Load festival info failed\n");
        return;
    }

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        TASK_FESTIVAL_TEMPLATE festivalTmpl;
        res.GetRecord(festivalTmpl.festival_id);            // 活动 ID
        res.GetRecord(festivalTmpl.name);                   // 名称
        res.GetRecord(festivalTmpl.type);                   // 类型(1:冲值类,2:消费类,3:条件类,4:兑换类,5:文字类, 6:成就类(只会在成就系统内出现))
        res.GetRecord(festivalTmpl.show_pos);               // 显示位置(1:精彩活动,2:独立UI)
        res.GetRecord(festivalTmpl.state);                  // 状态(0:下架,1:上架,2:自动)
        res.GetRecord(festivalTmpl.desc);                   // 描述
        res.GetRecord(festivalTmpl.sort);                   // 排序
        res.GetRecord(festivalTmpl.loop_hours);             // 循环时间
        res.GetRecord(festivalTmpl.begin_time ,false);      // 开始时间
        res.GetRecord(festivalTmpl.end_time, false);        // 结束时间
        res.GetRecord(festivalTmpl.trigger_time, false);    // 触发时间
        res.GetRecord(festival_task_str);                   // 任务的 json 结构
        res.GetRecord(festival_condition_str);              // 节日条件
        res.GetRecord(festival_reward_str);                 // 节日奖励
        res.GetRecord(festivalTmpl.tag);                    // 节日标签

        // 读取任务
        ReadTaskTmpl(festival_task_str, festivalTmpl);      // 解析任务模板

        // 解析任务条件
        json_value.clear();
        if(festival_condition_str == "")
            festival_condition_str = "{}";
        if(!reader.parse(festival_condition_str, json_value, false))
        {
            CnError("Parse festival:%u task faild\n", festivalTmpl.festival_id);
            continue;
        }

        festivalTmpl.minLevel = json_value["minLevel"].asInt();
        festivalTmpl.maxLevel = json_value["maxLevel"].asInt();

        // 解析任务奖励
        json_value.clear();
        if(festival_reward_str == "")
            festival_reward_str = "{}";
        if(!reader.parse(festival_reward_str, json_value, false))
        {
            CnError("Parse festival:%u reward info failed\n", festivalTmpl.festival_id);
            continue;
        }

        festivalTmpl.rewards.reason = REWARD_REASON_FESTIVAL;
        ReadRewardItems(festivalTmpl.rewards, json_value, "items");

        festival_list.list.push_back(festivalTmpl);
    }

    // 重置节日管理器中的节日列表
    FestivalManager::Instance()->SetFestivalList(festival_list);
    
    CnInfo("update local Festival\n");

    // 检查是否因为GMT更新
    if(b_update)
    {
        // 删除当前数据库中的活动，将节日数据插入到本地数据库
        if(DeleteLocalFestival() && InsertLocalFestival(festival_list))
        {
            CnDbg("delete local festival & insert local festival\n");
        }
    }

    if(b_notify)
    {
        CnInfo("notify Festival\n");
        FestivalManager::Instance()->SendToAllGameSvrFestivalList();
        FestivalManager::Instance()->SendToAllGateSvrFestivalList();
    }
}

void FestivalJob::ReadRewardItems(REWARD_TABLE& table, Json::Value festival_info_json, const std::string& str)
{
    uint32 table_count = festival_info_json[str].size();
    for(uint32 i=0; i<table_count; i++)
    {
        REWARD_ITEM item;
        Json::Value table_json = festival_info_json[str][i];
        read_REWARD_ITEM(item, table_json);
        table.items.push_back(item);
    }
}

void FestivalJob::ReadTaskTmpl(std::string& festival_task_str, TASK_FESTIVAL_TEMPLATE& festivalTmpl)
{
    TASK_TEMPLATE taskTmpl;
    uint32 taskID = 0;
    uint32 taskNum = 0;
    Json::Value json_value;
    Json::Reader reader;

    if(!reader.parse(festival_task_str, json_value, false))
    {
        CnError("Parse festival_task info failed\n");
        return;
    }

    taskNum = json_value["task"].size();
    for(uint32 i=0; i<taskNum; ++i)
    {
        taskTmpl.Clear();
        Json::Value& val = json_value["task"][i];
        taskID = i + 1;
        taskTmpl.task_id = CalFestivalTaskID(festivalTmpl.festival_id, taskID);
        taskTmpl.task_type = TASK_TYPE_FESTIVAL;
        taskTmpl.festival_id = festivalTmpl.festival_id;
        taskTmpl.name= val["name"].asString();
        taskTmpl.icon= val["icon"].asString();
        taskTmpl.desc= val["desc"].asString();
        taskTmpl.loop_num = val["loop_num"].asInt();
        ReadTargetsList(taskTmpl, val);
        ReadRewardItems(taskTmpl.rewards, val, "rewards");
        
        if(taskTmpl.targets.size() > 0)
            festivalTmpl.tasks.push_back(taskTmpl);
        else
            CnAssert(false);        // 任务不应该没有目标
    }
}

void FestivalJob::ReadTargetsList(TASK_TEMPLATE& taskTmpl, Json::Value val)
{
    TASK_TARGET taskTarget;

    uint32 targetNum = val["targets"].size();
    for(uint32 i=0; i<targetNum; ++i)
    {
        taskTarget.Clear();
        Json::Value& json_target = val["targets"][i];
        taskTarget.id = i+1;            // 目标ID由顺序决定
        taskTarget.type = json_target["target_type"].asUInt();
        std::string parmavalue = json_target["value"].asString();
        if(ReadTarget(taskTarget, parmavalue))
        {
            taskTmpl.targets.push_back(taskTarget);
        }
        else
        {
            CnError("read festival festival_id=%d task_id=%d failed\n", taskTmpl.festival_id, taskTmpl.task_id);
            CnAssert(false);
        }
    }
}

bool FestivalJob::ReadTarget(TASK_TARGET& target, const std::string& parma)
{
    // 参照任务类型表
    bool ret = false;
    std::vector<std::string> values = Crown::SDSplitStrings(parma, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        switch(target.type)
        {
        case TASK_TARGET_1:         // 杀怪物收集道具
            ret = ReadTarget_1(target, parma);
            break;
        case TASK_TARGET_3:         // 杀怪物
            ret = ReadTarget_3(target, parma);
            break;
        case TASK_TARGET_4:         // 通关副本
            ret = ReadTarget_NpcValueNum(target, parma);
            break;
        case TASK_TARGET_6:
            ret = ReadTarget_6(target, parma);
            break;
        case TASK_TARGET_7:
        case TASK_TARGET_8:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_9:
        case TASK_TARGET_10:
            ret = ReadTarget_ValueNum(target, parma);     // value, num
            break;
        case TASK_TARGET_11:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_12:
        case TASK_TARGET_13:
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_15:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_17:
            ret = ReadTarget_17(target, parma);
            break;
        case TASK_TARGET_20:
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_23:
        case TASK_TARGET_24:
        case TASK_TARGET_25:
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_26:
        case TASK_TARGET_27:
        case TASK_TARGET_28:
        case TASK_TARGET_36:
        case TASK_TARGET_38:
        case TASK_TARGET_39:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_41:
        
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_42:
        case TASK_TARGET_43:
        case TASK_TARGET_45:
        case TASK_TARGET_46:
        case TASK_TARGET_47:
        case TASK_TARGET_49:
        case TASK_TARGET_50:
        case TASK_TARGET_52:
        case TASK_TARGET_54:
        case TASK_TARGET_55:
        case TASK_TARGET_56:
        case TASK_TARGET_57:
        case TASK_TARGET_58:
        case TASK_TARGET_59:
        case TASK_TARGET_62:
        case TASK_TARGET_63:
        case TASK_TARGET_64:    //完成多少次公会任务
        case TASK_TARGET_65:    //多少次宠物一件激发
        case TASK_TARGET_66:    //圣剑升星多少次
        case TASK_TARGET_69:    //装备升级（次数）
        case TASK_TARGET_70:    //装备升品（次数）
        case TASK_TARGET_71:
        case TASK_TARGET_72:
        case TASK_TARGET_73:
        case TASK_TARGET_74:
        case TASK_TARGET_75:
        case TASK_TARGET_76:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_77:
        case TASK_TARGET_78:
        case TASK_TARGET_79:
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_80:
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_81:
        case TASK_TARGET_82:
        case TASK_TARGET_83:
        case TASK_TARGET_85:
        case TASK_TARGET_86:
        case TASK_TARGET_87:
        case TASK_TARGET_89:
            ret = ReadTarget_ValueNum(target, parma);     // value, num
            break;
        case TASK_TARGET_95:
        case TASK_TARGET_97:
        case TASK_TARGET_98:
        case TASK_TARGET_99:
        case TASK_TARGET_100:    // 所有使用宝藏卡和魔石的抽取行为抽XX次
        case TASK_TARGET_101:    // 铂金抽取XX次
        case TASK_TARGET_102:    // 至尊抽取XX次
        case TASK_TARGET_103:    // 单笔充值
        case TASK_TARGET_117:    // 每日登陆(重置)
            ret = ReadTarget_Num(target, parma);
            break;
        case TASK_TARGET_104:
            ret = ReadTarget_ValueNum(target, parma);     // value, num
            break;
        case TASK_TARGET_105:    // vip等级
        case TASK_TARGET_106:    // 珍兽挑战 参与次数
        case TASK_TARGET_107:    // 月黑时刻 参与次数
        case TASK_TARGET_108:    // 珍兽克星 击杀次数
            ret = ReadTarget_Num(target, parma);
            break;
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
        case TASK_TARGET_132:   // 任意指定XX数量的XX星级以上的化身，升级到XX等级
            ret = ReadTarget_ValueNum(target, parma);
            break;
        case TASK_TARGET_110:   // 充值排行上榜
        case TASK_TARGET_119:   // 累计消费(循环)
        case TASK_TARGET_120:   // 累计充值(循环)
            ret = ReadTarget_Num(target, parma);
            break;
        default:
            CnAssert(false);
            break;
        }
    }

    if(!ret)
    {
        CnError("target id=%d type=%d condition error: %s\n", target.id, target.type, parma.c_str());       // 任务条件填写错误
    }
    return ret;
}

bool FestivalJob::ReadTarget_1(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(6 == key_value.size())
    {
        target.npc_id = Crown::SDAtou(key_value[0].c_str());
        target.map_id = Crown::SDAtou(key_value[1].c_str());
        target.value = Crown::SDAtou(key_value[2].c_str());
        target.item_id = Crown::SDAtou(key_value[3].c_str());
        target.rate = SDAtoi(key_value[4].c_str());
        target.num = Crown::SDAtou(key_value[5].c_str());

        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_3(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 4)
    {
        target.npc_id = Crown::SDAtou(key_value[0].c_str());
        target.map_id = Crown::SDAtou(key_value[1].c_str());
        target.value = Crown::SDAtou(key_value[2].c_str());
        target.num = Crown::SDAtou(key_value[3].c_str());

        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_NpcValueNum(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 3)
    {
        target.npc_id = Crown::SDAtou(key_value[0].c_str());
        target.value = Crown::SDAtou(key_value[1].c_str());
        target.num = Crown::SDAtou(key_value[2].c_str());

        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_6(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 1)
    {
        target.value = Crown::SDAtou(key_value[0].c_str());
        target.num = 1;

        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_17(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 3)
    {
        target.npc_id = Crown::SDAtou(key_value[0].c_str());
        target.map_id = Crown::SDAtou(key_value[1].c_str());
        target.value = Crown::SDAtou(key_value[2].c_str());
        target.num = 1;

        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_ValueNum(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 2)
    {
        target.value = Crown::SDAtou(key_value[0].c_str());
        target.num = Crown::SDAtou(key_value[1].c_str());
        return true;
    }

    return false;
}

bool FestivalJob::ReadTarget_Num(TASK_TARGET& target, const std::string& param)
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(param, '|');
    if(key_value.size() == 1)
    {
        target.num = Crown::SDAtou(key_value[0].c_str());
        return true;
    }

    return false;
}

//删除本地
bool FestivalJob::DeleteLocalFestival()
{
    DBRecorder recorder;

    recorder = m_gameExecutor->Query("DELETE FROM festival");

    if(!recorder.HasExecut()) 
        return false;

    return true;
}

// 添加本地
bool FestivalJob::InsertLocalFestival(const TASK_FESTIVAL_TEMPLATE_LIST& festival_list)
{
    uint32 recordCount = 0;
    uint32 num = 0;
    std::string str_begin_time = "";
    std::string str_end_time = "";
    std::string str_trigger_time = "";
    std::string str_sql = "";
    int32 index = 0;
    DBRecorder recorder;
    static char name[1024] = {0};
    static char desc[1024] = {0};
    static char sql_buff[MAX_SQLTXT_LEN] = {0};

    for (std::vector<TASK_FESTIVAL_TEMPLATE>::const_iterator festivalIt = festival_list.list.begin();
            festivalIt != festival_list.list.end(); ++festivalIt)
    {
        static char taskTxt[MAX_SQLTXT_LEN] = {0};
        WriteTaskTemplate(taskTxt, festivalIt->tasks);

        //解析条件
        static char conditionTxt[MAX_SQLTXT_LEN] = {0};
        WriteConditon(conditionTxt, *festivalIt);

        //读取其他奖励
        static char rewardTxt[MAX_SQLTXT_LEN] = {0};
        WriteReward(rewardTxt, festivalIt->rewards);

        std::string server = "{}";

        m_gameExecutor->EscapeString(name, festivalIt->name.c_str(), festivalIt->name.length());
        m_gameExecutor->EscapeString(desc, festivalIt->desc.c_str(), festivalIt->desc.length());

        if(festivalIt->begin_time)
            str_begin_time = "FROM_UNIXTIME(" + Crown::ToString(festivalIt->begin_time) + ")" ;
        else
            str_begin_time = "0" ;

        if(festivalIt->end_time)
            str_end_time = "FROM_UNIXTIME(" + Crown::ToString(festivalIt->end_time) + ")";
        else
            str_end_time = "0";

        if (festivalIt->trigger_time)
        {
            str_trigger_time = "FROM_UNIXTIME(" + Crown::ToString(festivalIt->trigger_time) + ")";
        }
        else
        {
            str_trigger_time = "0";
        }

        // 拼装 SQL 插入头
        if(recordCount == 0)
        {
            index += SDSnprintf(&sql_buff[index], MAX_SQLTXT_LEN - index, 
                    "INSERT `festival`(`id`, `name`, `type`, `show_pos`, `state`, `sort`, `loop_hours`, "
                    "`begin_time`, `end_time`, `trigger_time`, `desc`, `task`, `condition`, `rewards`, `servers`, `flag`) VALUES\n");
        }

        // 拼装 SQL 数值
        index += SDSnprintf(&sql_buff[index], MAX_SQLTXT_LEN - index, "(%u, '%s', %u, %u, %u, "
                                                                      "%u, %u, %s, %s, '%s', "
                                                                      "'%s', '%s', '%s', '%s', '%s', %u)", 
                            festivalIt->festival_id, name, festivalIt->type, festivalIt->show_pos, festivalIt->state,
                            festivalIt->sort, festivalIt->loop_hours, str_begin_time.c_str(), str_end_time.c_str(), str_trigger_time.c_str(),
                            desc, taskTxt, conditionTxt, rewardTxt, server.c_str(), festivalIt->tag);

        // 计数
        ++recordCount;
        ++num;

        // 记录间逗号分隔符
        if((recordCount < MAX_FESTIVEAL_JOB_SIZE) && (num < festival_list.list.size()))
        {
            index += SDSnprintf(&sql_buff[index], MAX_SQLTXT_LEN, ",");
        }

        if((recordCount >= MAX_FESTIVEAL_JOB_SIZE) || (recordCount >= festival_list.list.size()))
        {
            recorder = m_gameExecutor->QueryMem(sql_buff);
            // 如果发生错误还要继续执行后续的数据录入
            if(!recorder.HasExecut())
            {
                CnError("Insert Ferstival list\n");
            }
            recordCount = 0;
            index = 0;
        }
    }

    return true;
}

void FestivalJob::WriteReward(char* rewardsqlTxt, REWARD_TABLE table)
{
    Json::Value Json_reward_table;
    Json::FastWriter reward_writer;

    if(!rewardsqlTxt)
        return;

    for(uint32 j = 0; j < table.items.size(); j++)
    {
        const REWARD_ITEM& item = table.items[j];
        Json::Value js_item;
        write_REWARD_ITEM(item, js_item);
        Json_reward_table["items"].append(js_item);
    }

    std::string rewardstr = reward_writer.write(Json_reward_table);
    m_gameExecutor->EscapeString(rewardsqlTxt, rewardstr.c_str(), rewardstr.length());
    if(rewardstr == "" || table.items.size() <= 0)
    {
        rewardstr = "{}";
        sprintf(rewardsqlTxt, "%s", rewardstr.c_str());
    }
}

void FestivalJob::WriteConditon(char* conditionTxt, const TASK_FESTIVAL_TEMPLATE& festivaltemp)
{
    if(!conditionTxt)
        return;

    std::string conditionstr = ""; // 解析任务
    Json::FastWriter writer;
    Json::Value info_templ_json;

    info_templ_json["minLevel"] = festivaltemp.minLevel;
    info_templ_json["maxLevel"] = festivaltemp.maxLevel;
    conditionstr = writer.write(info_templ_json);
    m_gameExecutor->EscapeString(conditionTxt, conditionstr.c_str(), conditionstr.length());
    if(conditionstr == "")
    {
        conditionstr = "{}";
        sprintf(conditionTxt, "%s", conditionstr.c_str());
    }
}

void FestivalJob::WriteTaskTemplate(char* tasksqlTxt, std::vector<TASK_TEMPLATE> tasklist)
{
    std::string taskstr = ""; // 解析任务
    Json::FastWriter writer;
    Json::Value task_list_json;
    Json::Value task_json;

    if(!tasksqlTxt)
        return;

    for(std::vector<TASK_TEMPLATE>::iterator taskIt = tasklist.begin();
        taskIt != tasklist.end(); ++taskIt)
    {
        task_json.clear();
        //task_json["type"] = taskIt->task_type;
        task_json["id"] = CalOriginFestivalTaskID(taskIt->task_id);
        task_json["loop_num"] = taskIt->loop_num;
        task_json["icon"] = taskIt->icon;
        //task_json["name"] = taskIt->name;
        task_json["desc"] = taskIt->desc;

        //加载reward,和taraget
        write_TaskTarget(&(*taskIt), task_json);

        //写入任务奖励
        for(uint32 j = 0; j < taskIt->rewards.items.size(); j++)
        {
            const REWARD_ITEM& item = taskIt->rewards.items[j];
            Json::Value json_item;
            write_REWARD_ITEM(item, json_item);
            task_json["rewards"].append(json_item);
        }

        task_list_json["task"].append(task_json);
    }

    taskstr = writer.write(task_list_json);
    if((taskstr == "") || (tasklist.size() < 1))
    {
        taskstr = "{}";
    }

    m_gameExecutor->EscapeString(tasksqlTxt, taskstr.c_str(), taskstr.length());
}

// GMT请求同步活动数据
void FestivalJob::HandleGmtFestivalList()
{
    LoadLocalFestival();        // 从游戏数据库上重新加载活动数据
}

bool FestivalJob::InsertGmtSyncRecorder(int32 type, uint32 time)
{
    DBRecorder recorder;
    recorder = m_gameExecutor->Query("INSERT INTO gmt_sync_recorder(`type`, `time`) VALUES ('%d', FROM_UNIXTIME('%u'))", type, time);
    if(!recorder.HasExecut())
    {
        CnError("Insert insertActivytyUpate failed\n");
        return false;
    }
    return true;
}
