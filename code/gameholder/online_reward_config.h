/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef online_reward_config_h__
#define online_reward_config_h__

#include <vector>

//struct ItemTemplate;

typedef struct OnlineReward
{
    void Clear()
    {
        id              = 0;
        time            = 0;
        rewardTable.Clear();
    }

    uint32      id;     ///< 在线奖励ID
    uint32      time;   ///< 时间
    REWARD_TABLE    rewardTable;
}OnlineReward;

typedef struct LevelReward
{
    void Clear()
    {
        level              = 0;
        rewardTable.Clear();
    }

    int32      level;
    REWARD_TABLE    rewardTable;
}LevelReward;

class Player;

class OnlineRewardConfig
{
public:
    OnlineRewardConfig();
    virtual ~OnlineRewardConfig();

    DECLARE_SINGLETON(OnlineRewardConfig)

    void Clear();

    bool LoadConfig(const char* path);

    const OnlineReward* GetOnlineReward(uint32 id);

    uint32 GetMaxID() { return m_max_id; }

    REWARD_TABLE* FindIdByLevelReward(int32 level);

    int32 GetSingleProbablityMax() {return m_singleProbablityMax;}
    int32 GetAddGrowPrecent() {return m_addGrowPrecent;}
    int32 GetCritNum() {return m_critNum;}
    int32 GetOnlineSingleProbablity() {return m_online_singleprobablity;}
    int32 GetOnlineCirtProblity() {return  m_online_cirtproblity;}
    int32 GetOnlineDay() {return m_gifts_Intervalday;}
    int32 GetGrowMaxNextLevel() {return m_maxnextlevel;}

    REWARD_ITEM* RandomSingleProbility(int32 probability);

    std::vector<REWARD_ITEM>& GetSingleGrowList() {return m_single_probability;}

    void FindGrowReward(REWARD_TABLE& rewardtable, int32 index);
    void FindFixedReward(REWARD_TABLE& rewardtable, int32 index);

    void FindGrowRewardByOrder(int32 order, int32 index, REWARD_ITEM& curitem);                   //根据填物品的顺序，找到要给的物品

    int32 FindAppointNextIndex(int32 type, int32 index);                            //查找下一个索引

    int32 FindIdByMinxLevelReward();                                                //最小的玩家等级

    int32 FindNextLevelReward(int32 level);                                          //得到下等级的玩家等级

private:
    bool ReadFixedReward(TiXmlElement* xmlEle);             //读取固定奖励
    bool ReadLevelReward(TiXmlElement* xmlEle);
    bool ReadGrowReward(TiXmlElement* xmlEle);
    bool ReadSingleProbability(TiXmlElement* xmlEle);
    void ReadConst(TiXmlElement* int32);

    int32 FindNextIndex(int32 index, std::map<int32, REWARD_TABLE>& rewardMap);
private:
    uint32                          m_max_id;
    std::map<uint32, OnlineReward>  m_online_reward_map;
    std::map<int32, REWARD_TABLE>  m_fixed_reward_map; //天数，奖励
    std::map<int32, REWARD_TABLE>   m_level_reward_map;// 等级，奖励
    std::map<int32, REWARD_TABLE>   m_grow_reward_map;//次数，奖励
    std::vector<REWARD_ITEM>         m_single_probability;//单种奖励的概率列表

    int32                           m_singleProbablityMax;// 单个增加概率最大值
    int32                           m_addGrowPrecent;     //增加成长值的百分比
    int32                           m_critNum;          //毕现的暴击次数
    int32                           m_online_singleprobablity;//在线领奖时单个物品概率
    int32                           m_online_cirtproblity;// 在线领奖暴击所有物品概率

    int32                           m_gifts_Intervalday;              //恩赐奖励的天数多少天，刷新一次
    int32                           m_maxnextlevel;                   //最大的成长等级的下一级

};

#endif
