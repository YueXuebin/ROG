/*
*	称号表
*/
#ifndef title_config_h__
#define title_config_h__

typedef std::vector<REWARD_ITEM>  RewardList;

typedef struct ConditionTemp
{
    int32 reason;
    int32 num;
    int32 minRange;
    int32 maxRange;

    void Clear()
    {
        reason = 0;
        minRange = 0;
        maxRange = 0;
        num = 0;
    }

}ConditionTemp;


typedef std::vector<ConditionTemp> Vec_ConditionTemp;

typedef struct TitleStruct
{
    uint32 id;
    int32 type;         // 是世界boss还是答题，还是任务模块类型 
    uint32 time;        // 是永久的还是部分的
    int32 titletype;    //是普通活动还是活动称号

    Vec_ConditionTemp conditemp; // 条件
    RewardList Rewarlist;

    void Clear()
    {
        id = 0;
        type = 0;
        time = 0;
        titletype = 0;
        conditemp.clear();
        Rewarlist.clear();
    }

}TitleStruct;

typedef struct VipNum
{
    int32 num;
    int32 level;
    void Clear()
    {
        num = 0;
        level = 0;
    }
}VipNum;

class Player;

//符合条件的title vec
typedef std::vector<uint32> TitleVec; 
class TitleConfig
{
public:
    TitleConfig(void);
    ~TitleConfig(void);
    DECLARE_SINGLETON(TitleConfig)

    bool LoadTitleConfig(const char* path);


    //得到排名称号id
    TitleVec* GetRankTitle(int32 type,int32 rank);
    //得到符合条件的任务id
    uint32 GetTaskTitle(int32 type,uint32 taskid);
    int32  GetTitleType(uint32 titleid);
    uint32 GetTitleTime(uint32 titleid);

    //得到称号模块类型
    int32 GetTitleModelType(uint32 titleid);
    //得到最大条件列表
    int32 GetGreaterCondition(int32 type,int32 level);
    //符合修罗场条件的称号
    TitleVec* GetAshuraTitle(int32 type, int32 reason, int32 num);

    void AddTitleReward(uint32 titleid, PropertySet& propSet);

    uint32 GetGeneralTitle(int32 type, int32 reason = 0, int32 num = 0);

    //所有玩家
    uint32  DealAllPlayer(int32 type);

    //能加载多少称号
    int32 GetVipNum(int32 viplevel);

    //得到vip条件
    int32 GetVipConditonReason(uint32 titleid);
private:
    void LoaddTitle(TiXmlElement* TitleEle);

    void LoadTileNum(TiXmlElement* TitleEle);

    void LoadCoditionTemp(TitleStruct&titleStruct, TiXmlElement* conditionEle);

    void LoadReward(TiXmlElement * rewardsEles, TitleStruct& titleStruct);

    typedef std::vector<TitleStruct> Vec_Title;
    typedef std::vector<VipNum> Vec_VipNum;

    Vec_Title  vec_title;
    Vec_VipNum m_vec_vipnum;

    TitleVec  m_titleVec; //符合条件的称号id的的个数


};
#endif
