#ifndef ride_config_h__
#define ride_config_h__

typedef struct RidePetLeveUpTempl
{
    int32           experess;           // 最大经验值
    int32           level;              // 级别
    int32           speed;              // 移动速度
    int32           res_mp;             // 回魔
    int32           res_hp;             // 回血

    void Clear()
    {
        experess        = 0;
        level           = 0;
        speed           = 0;
        res_mp          = 0;
        res_hp          = 0;

    }
}RidePetLeveUpTempl;

typedef struct RidePetTemplate
{
    uint32                  id;                 // ID
    int32                   type;               // 类型
    int32                   level;              // 初始等级
    std::string             name;               // 名字
    int32                   fighting;           // 战斗力
    int32                   free_number;        // 放生骑宠总数得到的个数
    int32                   express_price;      // 宠物的经验价值
    int8                    isopen;             // 这个功能是否开启
    int32                   skillID;            // 技能ID

    void Clear()
    {
        id                  = 0;
        type                = 0;
        level               = 0;
        name                = "";
        fighting              = 0;
        free_number         = 0;
        express_price       = 0;
        isopen              = 0;
        skillID             = 0;
    }
}RidePetTemplate;

typedef struct RidePetQuality
{
    int32              m_min;               // 最小范围
    int32              m_max;               // 最大范围

    void Clear()
    {
        m_min              = 0;
        m_max              = 0;
    }
}RidePetQuality;

typedef std::map<uint32, RidePetTemplate> RideTempMap;

//刻印骑宠标记
typedef struct MarkCondit
{
    void Clear()
    {
        vip_level = 0;
        level = 0;
        type = 0;
        deafult = 0;
    }
    int32 type;         // 坐骑的类型
    int32 level;        // 玩家级别限制
    int32 vip_level;    // vip级别限制的id不能为0;
    int32 deafult;      // 是否是默认开启
}MarkCondit;
typedef std::map<uint32, MarkCondit> Map_MarkCondit;

typedef std::vector<REWARD_ITEM>  VecRewardList;

//坐骑图鉴
typedef struct AtlasStruct
{
    void Clear()
    {
        num = 0;
        quality = 0;
        rewardlist.clear();
    }
    int32 num;        // 收集的个数
    int32 quality;    // 品质
    VecRewardList rewardlist; // 奖励的属性值加成
}AtlasStruct;

typedef  std::map<uint32, AtlasStruct> Map_Atlas;          // 坐骑图鉴

// 训练花费的是魔石还是人民币
typedef struct SpendStruct
{
    int32 key;
    int32 num;
    void Clear()
    {
        key = 0;
        num = 0;
    }
}SpendStruct;

// 坐骑训练模式
typedef struct TrainModes
{
    void Clear()
    {
        time = 0;
        exp = 0;
        spend.Clear();
    }
    uint32 time;
    int32 exp;
    SpendStruct spend;
}TrainModes;

// 坐骑训练的栏位开启条件
typedef struct  TrainField
{
    void Clear()
    {
        monyepoint = 0;
       // vip_level = 0;
        deafult = 0;
    }

    int32 monyepoint; // 玩家的魔石
    //int32 vip_level;//vip级别
    int32 deafult; // 是否是默认开启,还是可开启
}TrainField;

typedef std::map<uint32, TrainField> Map_TraiFiles;          // 坐骑训练栏位

class Ride_Config
{
public:
    Ride_Config(void);
    ~Ride_Config(void);

    DECLARE_SINGLETON(Ride_Config)

    bool LoadRidePetConfig(const char* path);
    bool LoadRideLevelConfig(const char* path);             // 骑宠级别的表
    //bool LoadRideMarkConfig(const char* path);
    bool LoadConstConfig(const char* path);
    void LoadPetFosterRange(TiXmlElement* petEle);          // 加载跟宠培养的最大范围
    RidePetTemplate* GetRidePet(uint32 id);                 // 获得骑宠模板

    int32 CaluteRideExpress(uint32 id, int32 level);         // 计算所有骑宠的经验
    int32 GetRidePetLeveupTem(int32 id, int32 level);        // 获得骑宠物所在级别的最大经验值值
    int32 FindRideNextLevel(uint32 id, int32 curexprss);     // 找到骑宠大于经验值得级别

    RidePetLeveUpTempl* GetRideLeveTempl(int32 id, int32 level);        // 得到骑宠的升级模板
    RidePetQuality* GetRidePetQuality(const std::string& quality);      // 得到坐骑的品质范围

    MarkCondit* FindMarkCondit(uint32 id);                  // 查找标识条件
    const Map_MarkCondit& GetMarkCondition();                     // 得到刻印列表

    const Map_Atlas& GetMapAltas();                               // 得到图鉴列表
    AtlasStruct* GetAltasStruct(uint32 atlasid);            // 图鉴

    Map_TraiFiles& GetTrainFields();                        // 得到训练位

    TrainModes* GetTrainMode(uint32 modeid);                // 得到训练模式

    TrainField* FindTrainField(uint32 fieldid);             // 根据栏位id得到对应的条件标识

    int32 GetRideAddExpress() {return m_rideexpress;}
    int32 GetRideStoneNum() {return m_rideStoneNum;}
    int32 GetRideCritNum() {return m_numCrit;}
    int32 GetRideCrit() {return m_crit;}
    uint32 GetRideFreeItem() { return m_ItemID;}
    int32 GetStoneNum() {return m_stonenum;}
    int32 GetAbsorbOrd() {return m_absorb_ord;}
    int32 GetDoubleAbsorb() {return m_doulbeabsorb;}
    const RideTempMap& GetRideTemplMap() { return m_ride_pet;}

    int32 GetMaxField() { return m_maxField;}
    int32 GetInitField() { return m_intField;}

    int32 GetOrdRandnum() {return m_ord_randnum;}
    int32 GetOrdBlue() {return m_ord_blue;}
    int32 GetOrdPureple() {return m_ord_pureple;}
    int32 GetOrdGold() {return m_ord_gold;}
    int32 GetSerRandnum(){return m_ser_randnum;}
    int32 GetSerGold() {return m_ser_gold;}
    int32 GetSerPureple() {return m_ser_pureple;}
    int32 GetSerBule() {return m_ser_blue;}

    int32 GetOrdStoneNum() {return m_ord_stonenum;}
    int32 GetOrdRideBlue() {return m_ord_ride_blue;}
    int32 GetOrdRidePureple() {return m_ord_ride_pureple;}
    int32 GetOrdRideGold() {return m_ord_ride_gold;}
    int32 GetOrdRideGreen() {return  m_ord_ride_gree;}
    int32 GetOrdRideWhile() {return m_ord_ride_while;}

    int32 GetSerStoneNum() {return m_ser_stonenum;}
    int32 GetSerRideBlue() { return m_ser_ride_blue;}
    int32 GetSerRidePureple() {return m_ser_ride_pureple;}
    int32 GetSerRideGold() {return m_ser_ride_gold;}

    int32 GetSerRideGreen() {return  m_ser_ride_gree;}
    int32 GetSerRideWhile() {return m_ser_ride_while;}

    int32 GetblueStone() {return m_bulestone;}
    int32 GetPureStone() {return m_pureplestone;}
    int32 GetGoldStone() { return m_goldstone;}

    int32 GetRideOrdUpdate() {return m_ordupdate;}
    int32 GetRideSerUpdate() {return m_serupdate;}

    int32 GetOrdTimePuple() {return m_ordpurple;}                                           // 骑宠普通刷新多少次紫的
    int32 GetOrdReutrnPurple() {return m_ord_return_purple;}                                // 骑宠普通反紫色点
    int32 GetOrdTimeGold() {return m_ordgold;}                                              // 骑宠普通刷新多少次
    int32 GetOrdReturnGold() {return m_ord_return_gold;}                                    // 骑宠普通反金色点

    int32 GetSerTimePurple() {return m_serpurple;}                                          // 骑宠高级刷新多少次紫的
    int32 GetSerReturnPurple() {return m_ser_return_purple;}                                // 骑宠高级反紫色点
    int32 GetSerTimeGold() { return m_sergold;}                                             // 骑宠高级刷新多少次金色的
    int32 GetSerReturnGold() {return m_ser_return_gold;}                                    // 骑宠高级反多少点金色的

    int32 GetSerSwaPer() {return m_rideSeniorSwa_Per;}
    int32 GetOrdSwarper()   {return m_rideOrdiSwa_Per;}

    int32 GetMaxGrass() {return m_maxgrass;}
    int32 GetMaxBug() {return m_maxbug;}
    int32 GetMaxHoney() {return m_maxhoney;}
    int32 GetGrassRange() {return m_grass_range;}
    int32 GetBugRange() {return m_bug_range;}
    int32 GetHoneyRange() {return m_honey_range;}
    int32 GetMoneyToScecond() {return m_moneytoscend;}

    int32 GetSeniorUpdateItemNum() {return m_senionrItemnum;}
    int32 GetOrdinaryUpdateItemNum() {return m_ordItemnum;}

    int32 GetSpendSupermeItemNum() {return m_supermeStoneNum;}

    int32 GetSuperBlueNum() {return m_superblueNum;}
private:
    RideTempMap   m_ride_pet;                                           // 骑宠

    typedef std::vector<RidePetLeveUpTempl> Vec_Ride_levelUp;            // 骑宠升级结构体

    typedef  std::map<uint32, Vec_Ride_levelUp> Map_RidePet_LevelUp;    // 骑宠升级经验LevelUpExp

    typedef std::map<std::string, RidePetQuality> Vec_RidePetQuality;  // 骑宠的品质范围

    Vec_RidePetQuality                      m_vecridequility;           // 品质

    Map_Atlas                               m_mapAltas;                 // 坐骑图鉴

    typedef std::map<uint32,TrainModes>     Map_TrainModes;             // 坐骑的训练模式
    Map_TrainModes                          m_trainModes;               // 坐骑的训练模式
    Map_TraiFiles                           m_trainFields;              // 坐骑的训练栏位
    Map_MarkCondit                          m_MarkCondit;               // 骑宠刻印开启条件
    Map_RidePet_LevelUp                     m_rideMapLevelUp;           // 骑宠升级表;

    int32    m_rideexpress;                             // 骑宠宠每次增加的经验值
    int32    m_rideStoneNum;                            // 骑宠升级魔石需要的个数;
    int32    m_numCrit;                                 // 几次暴击增加一次经验值(暴击率)
    int32    m_crit;                                    // 暴击倍数
    uint32   m_ItemID;                                  // 放生后得到的物品兽耳id;
    int32    m_stonenum;                                // 开启栏位要花的魔石数;
    int32    m_absorb_ord;                              // 骑宠普通吸收的游戏币 
    int32    m_doulbeabsorb;                            // 双倍吸收的人民币
    int32    m_maxField;                                // 最大的栅栏数
    int32    m_intField;                                // 初始化的栅栏数

    int32    m_ord_randnum;                             // 普通刷新的随机物品的个数
    int32    m_ord_blue;                                // 普通刷新生成蓝色物品的概率
    int32    m_ord_pureple;                             // 普通刷新生成紫色物品的概率
    int32    m_ord_gold;                                // 普通刷新生成金色物品的概率；
    int32    m_ser_randnum;                             // 高级刷新的随机物品的个数;
    int32    m_ser_gold;                                // 高级刷新生成金色物品的概率;
    int32    m_ser_pureple;                             // 高级刷新生成紫色物品的概率;
    int32    m_ser_blue;                                // 高级刷新生成蓝色物品的概率;

    int32    m_ord_stonenum;                            // 普通刷新的花费的魔石数
    int32    m_ord_ride_blue;                           // 普通刷新生成蓝色坐骑的概率
    int32    m_ord_ride_pureple;                        // 普通刷新生成紫色坐骑的概率
    int32    m_ord_ride_gold;                           // 普通刷新生成金色坐骑的概率；
    int32    m_ord_ride_gree;                           // 普通刷新生成绿色坐骑的概率；
    int32    m_ord_ride_while;                          // 普通刷新生成白色坐骑的概率；

    int32    m_ser_stonenum;                            // 高级刷新的花费的魔石数
    int32    m_ser_ride_blue;                           // 高级刷新生成蓝色坐骑的概万分比率;
    int32    m_ser_ride_pureple;                        // 高级刷新生成紫色坐骑的概率;
    int32    m_ser_ride_gold;                           // 高级刷新生成金色坐骑的概率;
    int32    m_ser_ride_gree;                           // 高级刷新生成绿色坐骑的概率；
    int32    m_ser_ride_while;                          // 高级刷新生成白色坐骑的概率；

    int32    m_maxgrass;                                // 青草的最大数量
    int32    m_maxbug;                                  // 虫的最大数量
    int32    m_maxhoney;                                // /蜂蜜的最大数量

    int32    m_grass_range;                             // 大于这个值得话，那么随机生成四个蓝色性质的宠物，
    int32    m_bug_range;                               // 大于这个值得话，那么随机生成四个紫色性质的宠物
    int32    m_honey_range;                             // 大于这个值得话，那么随机生成四个金色性质的宠物

    int32    m_bulestone;                               // 刷出蓝色品质以及以上坐骑不进行任何契约签到操作得到的幻兽石个数
    int32    m_pureplestone;                            // 刷出蓝色品质以及以上坐骑不进行任何契约签到操作得到的幻兽石个
    int32    m_goldstone;                               // 刷出蓝色品质以及以上坐骑不进行任何契约签到操作得到的幻兽石个
    int32    m_moneytoscend;                            // 骑宠训练秒cd1魔石等于多少秒

    int32    m_ordupdate;                               // 骑宠普通刷新多少蓝的返点数
    int32    m_serupdate;                               // 骑宠高级刷新多少点蓝点的返点数

    int32    m_ordpurple;                               // 骑宠普通刷新多少次紫的
    int32    m_ord_return_purple;                       // 骑宠普通反紫色点
    int32    m_ordgold;                                 // 骑宠普通刷新多少次
    int32    m_ord_return_gold;                         // 骑宠普通反金色点


    int32    m_serpurple;                               // 骑宠高级刷新多少次紫的
    int32    m_ser_return_purple;                       // 骑宠高级反紫色点
    int32    m_sergold;                                 // 骑宠高级刷新多少次金色的
    int32    m_ser_return_gold;                         // 骑宠高级反多少点金色的


    int32    m_rideSeniorSwa_Per;                       // 骑宠高级吞噬的百分比
    int32    m_rideOrdiSwa_Per;                         // 骑宠普通吞噬的百分比

    int32    m_senionrItemnum;                          // 高级刷新的的幻兽石的个数
    int32    m_ordItemnum;                              // 普通刷新的的幻兽石的个数
    int32    m_supermeStoneNum;                         // 至尊幻兽石花费的的个数
    int32    m_superblueNum;                            // 至尊返点的个数;

private:
    void LoadRidePet(TiXmlElement* petEle);

    void LoadRideAltas(TiXmlElement* xmlRoot);          // 加载坐骑图鉴

    void LoadTrainFields(TiXmlElement* xmlRoot);        // 加载训练栏位

    void LoadTrainModels(TiXmlElement* xmlRoot);        // 加载训练模式

    void LoadRidePetLevel(TiXmlElement* petEle);

    void LoadMarks(TiXmlElement* xmlRoot);              // 加载骑宠makr标记

    void LoadConst(TiXmlElement* xmlRoot);              // 加载骑宠的常量

};
#endif
