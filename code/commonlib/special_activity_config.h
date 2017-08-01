/*
 *	特殊活动配置信息
 */

#ifndef special_activity_config_h__
#define special_activity_config_h__

// 不同活动参数说明 [12/27/2013 ZhangHM]

//活动id 在 message_defines.xml 特殊活动定义下
//reward_id 对应 activity_reward.xml里的编号
//param为活动参数,不同活动意义不同
//param1,param2为每项扩展参数,不同活动意义不同

//首次充值-------------------------------------------------------
// 0: SPECIAL_ACTIVITY_FIRST_TIME_RECHARGE 
// param:无效,param1:无效,从0开始,param2:无效

//全名冲级得好礼-------------------------------------------------------
// 1:SPECIAL_ACTIVITY_PLAYER_LEVEL"
//param:无效,param1:索引,从0开始,param2:玩家等级

//天天战力大比拼-------------------------------------------------------
// 2: SPECIAL_ACTIVITY_BATTLE_POWER_RANKING 
// param:取前n名,param1:高排名,param2:低排名

//每日充值-------------------------------------------------------
// 3:SPECIAL_ACTIVITY_DAILY_RECHARGE"
//param:无效,param1:索引,从0开始,param2:充值数

//翅膀提升-------------------------------------------------------
// 4:SPECIAL_ACTIVITY_UPGRADE_WING"
//param:无效,param1:索引,从0开始,param2:翅膀阶数

//-------------------------------------------------------
typedef struct SpecialActivityInfo
{
    void Clear()
    {
        param1 = 0;
        param2 = 0;
        rewardId = 0;
    }

    int32 param1;
    int32 param2;
    uint32 rewardId;

}SpecialActivityInfo;

typedef std::vector<SpecialActivityInfo> SpecialActivityInfoList;

typedef struct SpecialActivity
{
    void Clear()
    {
        id = 0;
        param = 0;
        start_time = 0;
        end_time = 0;
        infoList.clear();
    }

    int32 id;
    int32 param;
    uint32 start_time;
    uint32 end_time;
    SpecialActivityInfoList infoList;

}SpecialActivity;

typedef std::map<uint32, SpecialActivity> SpecialActivityMap;

class SpecialActivityConfig
{
public:
    SpecialActivityConfig();
    ~SpecialActivityConfig();

    DECLARE_SINGLETON(SpecialActivityConfig)

    void Clear();

    bool LoadConfig(const char* path);

    const SpecialActivity* GetActivity(int32 activityId);
    const SpecialActivityInfo* GetActivityInfoByIdAndParam1(int32 activityId, int32 param1);
    const SpecialActivityInfo* GetActivityInfoByIdAndParam2(int32 activityId, int32 param2);

    int32 GetRewardByActivityIdAndIndex(int32 activityId, uint32 index, REWARD_TABLE& rewardTable);
    int32 GetRewardByActivityId(int32 activityId, REWARD_TABLE_LIST& rewardTableList);
    int32 GetInfoByActivityId( int32 activityId, SPECIAL_ACTIVITY_INFO_LIST& infoList );
    int32 GetActivityTime( int32 activityId, uint32& start_time, uint32& end_time );

    int32 CheckActivityTime( int32 activityId );

    void UpdateStatus(SPECIAL_ACTIVITY_INFO_LIST& infoList, int32 param, int32 status);

private:
    SpecialActivityMap m_specialActivityMap;

};

#endif