#ifndef ride_manager_h__
#define ride_manager_h__

class RideTrain;
typedef std::pair<int32, int32> RideObject;

struct RidePetTemplate;
typedef std::map<uint32, RidePetTemplate> RideTempMap;

typedef std::vector<RIDE_TRAIN> RidetTrainList;

class Player;

class RideManager
{
public:
    RideManager(Player* player);
    ~RideManager(void);

    void LoadRideInfo(const RIDE_INFO& info);                   // 加载骑宠信息
    void FillRideInfo(RIDE_INFO& info);                         // 填充骑宠信息

    void OnRecv(uint32 msgID, void * data);

    typedef std::vector<RIDE_PET_INFO> RidePetList;

    typedef std::vector<RIDE_ALTAS> RideAltasList;

    typedef std::vector<RIDE_QUALITY_INFO> RideQualityList;     // 坐骑刷了多少次的品质列表

    virtual void UseMoneyWay(uint32 itemType, uint32 itemID, uint32 itemNum);   // 花费磨石

    uint32 GetRidePetID() { return m_ride_id; }                 // 获得骑宠ID
    void SetRidePetID(uint32 rideID);                           // 换坐骑
    void ChangeRidePet(uint32 val);                             // 设置骑宠ID

    void RequestDownRide();                                     // 下马请求

    void AddRideBuff(uint32 rideID);                            // 添加骑宠BUFF
    void DelRideBuff(uint32 rideID);                            // 删除骑宠BUFF

    void AddRidePet(uint32 pet_id);                              // 金钱购买哪些跟宠
    void SetUseItemRidePetRecuit(int32 curid);                   // 使用物品招募骑宠

    void SendRidePetListNotify(uint16 opt_flag,RIDE_PET_INFO * pet);                                        // 坐骑初始化列表通知

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 ); // 坐骑的事件处理

    void OnInitRidePet(void* data);
    void RideOrFree(void* data);                                                    // 放生或骑乘骑宠
    void MinMarkRide(void* data);                                                   // 跟宠刻印
    void OpenFiled(void* data);                                                     // 开启骑宠栅栏 
    void ClearMarkRide(void* data);                                                 // 取消所有坐骑刻印

    void RideAbsorb(void* data);                                                    // 坐骑吸收
    void RideAbsorb(const std::string& str);                                        // 人民币坐骑吸收处理
    void CaptureRide(void* data);                                                   // 捕获骑宠 
    void GetUpdateRide(void* data);                                                 // 刷出坐骑
    void UpdateRideByMoney(int32 type,int32 curexe);                                // 人民币消费刷骑宠
    void SendGif(void* data);                                                       // 如果当前品质坐骑已经捕获，那么，赠送礼物方式得到兽耳;

    void Clear();
    void CalRidePro();                                                              // 计算骑宠属性

    void NotifyNum();                                                               // 通知兽耳的个数

    int32 GetMaxRideLevel();                                                        // 获得最大的坐骑等级
    int32 GetRideQualityNum(int32 quality);                                         // 统计坐骑品质的总数

    RIDE_PET_INFO* GetRideInfo(uint32 pet_id);                                      // 获得骑宠信息
    RideTrain* GetRideTrain() {return m_ridetrain;}                                 // 得到训练的类

    RidePetList& GetRideTrainList() {return m_ride_pet_list;}                       // 坐骑训练列表

    void SendMarQuee(uint32 pet_id);                                                // 发送公告

    uint32 GetRideNumByQuality(int32 quailty);                                      // 根据品质获得坐骑数量

    int32 GetRideQuality(int32 pet_id);                                             // 获得骑宠品质

    enum State_Capture
    {
        Capture_Init = 0,
        Capture = 1,
        NoCapture = 2
    };

private:

    void SetRiddeInfo(uint32 petid,int32 exp,int32 level = 0);
    RIDE_PET_INFO* GetRandomInfo(uint32 pet_id);                                    // 得到随机坐骑

    void SendRideOrFreeAck(int32 _type, const RIDE_PET_INFO& ride_pet);             // 发送的骑乘或者放生
    void SendOpenFieldAck();                                                        // 发送栅栏开启;
    void SendUpdateRideAck();                                                       // 发送刷出骑宠请求 
    void SendRandomAck();                                                           // 发送随机分配物品列表
    void SetNoMark(uint32 petid);                                                   // 设置刻印
    void SendMarkAck();                                                             // 发送刻印标识

    void RideMarkCondi();                                                           // 刻印条件
    void AddMarkPos(uint32 markid);
    void SetMarkRide(uint32 markid,uint32 pet_id);
    RIDE_MARKT* GetMarkID(uint32 markid);                                           // 查找刻印标识

    bool JudgeByRandom(int32 m_stonemoney, int32 itemid,int32 itemNum ,int32 type);                 // 判读是否可以随机物品和宠物
    void RandomRideAndOjbect(int32 _grassper,int32 _bugper,int32 _honeyper,int32 type);             // 分配的骑宠和物品

    void ActiveAltas(uint32 petid);                                              // 是否激活图鉴
    void AddAtlas(uint32 altasid);                                               // 添加图鉴
    void AddAtlasPro(PropertySet& propSet);                                      // 计算图鉴属性

    void RandomTypeOject(int32 type);                                           // 生成生成不同种类的诱捕 //高级和普通诱捕
    bool HasOjbect(uint32 ojbectid);                                            // 物品列表里面是否有这个物品

    void RandomObject(int32 blue,int32 purple,int32 gold,int32 numobject,int32 defaultnum);// 根据不同概率,生成的个数，生成不同种类的物品
    bool RandomRide(int32 quality1,int32 quality2);                                      // 随机出对应质量的骑宠

    void RadnomRideByStone(int32 _grassper,int32 _bugper,int32 _honeyper ,int32 type);   // 根据兽饵越多能随到的坐骑的概率越大
    void PushProbability(int32 num, int32 maxvalue , int32 probability,
        int32 pointvalue , int32 type 
        , std::vector<RideObject>& vec_obj);                                     // 物品个数越多，生成坐骑的概率越大

    void ClearRandomObjNum (uint32 ride_id);                                        // 清空随机物品个数为0
    void SetAddRandomObject(uint32 ojbectid,int32 addNum);                          // 设置物品添加的个数
    void SetPointNum(uint32 ride_id, int num);                                      // 根据品质计算还有多少点
    void GetPointByQuality(int32& hownum ,int32 maxvalue);

    void AddObjectType(int32 type);                                                 // 添加指定类型的物品

    void AbsorbLeveup(uint32 petid);                                                // 吸收一键升级
    void NotifyUpdateNum();                                                         // 通知刷坐骑个数

    void UpdateSupermeRide();                                                       // 高级刷新坐骑
    void UpdateRideByQualityNum();                                                  // 是否到达最大数，那么给出相应坐骑品质

    //计算骑宠吸收经验
    void CalRideAbsorb(int32 exper,RIDE_PET_INFO* rideInfo,RIDE_PET_INFO* rideInfo1 = 0,RIDE_PET_INFO* rideInfo2 = 0,RIDE_PET_INFO* rideInfo3 = 0);

    //根据刷新次数，返回紫点和金点
    void OnUpdateReturnPoint(int32 type);
    //根据刷新次数，高级级返点
    void SerReturnPoint(RIDE_QUALITY_INFO& ridequality);
    //根据刷新普通返点
    void OrdReturnPoint(RIDE_QUALITY_INFO& ridequality);

    //没有捕获在次刷新得到的物品
    void NoCatchRandomObject(int32 type);

    //没有抓捕，返点
    void NoCatchReturnPoint(int32 quality);

    //根据战斗力，计算现在跟宠的品质
    int32 SetRideQuality(int32 pet_id);

    //根据类型判读骑宠应该随机对应品质坐骑// int32 num还剩多少返点
    void JudgeTypeRandom(int32 type, int32 num = 0);

    //大于最大数一定生成对应品质的坐骑坐骑
    bool CanRandomRide(int32 num, int32 maxvalue, int32 type);

    //查找对应打开骑宠的品质
    bool FindOpenQuality(RideTempMap& curRideTemp, int32 quality1, int32 quality2);

    int32 FindQualityByItem(int32 type);
    //生成坐骑的概率
    void RideByProbability(std::vector<RideObject>& vec_obj, int32 type);

    //列表里是否有对应质量的坐骑
    bool FindRideQuality(std::vector<RideObject>& vec_obj, int32 quality);

    //根据vipid判断当前的
    bool OpenMarkByvip(int32 id);

    //发布收集品质公告
    void CollectQuality(int32 atlaID);
    //删除骑宠
    void DeleteRide(uint32 pet_id);

    void SendCollectTask(uint32 pet_id);

    //栏位上是否有这个坐骑
    bool FieldIsHaveRide(uint32 markid, uint32 pet_id);

private:

    Player*         m_player;                               // 角色指针

    RidePetList     m_ride_pet_list;                        // 骑宠信息
    uint32          m_ride_id;                              // 坐骑ID
    int32           m_startTime;                            // 记录一天的开始时间
    int32           m_level;                                // 当前宠物级别
    int32           m_fieldnum;                             // 当前栅栏的个数

    State_Capture   m_enum_capture;                         // 刷出了宠物，但没有进行捕获操作，再次刷新得到兽耳
    uint32          m_lastRideid;                           // 上次没有捕获的宠物的id;
    uint32          m_fieldride_id;                         // 记录栏位已经满的时候，骑宠id;

    int8            b_usefree;
    uint32          m_free_time;                            // 免费提升的时间

    RidePetList     m_randomRide_pet;                       // 随机分配的宠物信息

    typedef  std::vector<REWARD_ITEM>  VecRewardItem;
    VecRewardItem m_randomObject;

    typedef std::vector<RIDE_MARKT>  VecRideMarkList;
    VecRideMarkList m_markList;                             // 骑宠刻印列表
    VecRideMarkList m_updatemarkList;                       // 骑宠跟新的刻印列表


    RideAltasList    m_rideAltasList;                       // 坐骑图鉴

    RideTrain*       m_ridetrain;                           // 坐骑训练

    RideQualityList m_ridequalitylist;                      // 坐骑返点的列表

    RidePetList     m_updateRideList;                       // 跟新坐骑列表
};

#endif
