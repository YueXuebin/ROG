#ifndef fashion_manager_h__
#define fashion_manager_h__

typedef struct FashionQualityNum
{
    int32       level;             // 品质，星级数
    int32       num;               // 个数

    // 清空
    void Clear()
    {
        level          = 0;
        num            = 0;
    }
} FashionQualityNum;

typedef std::vector<FashionQualityNum> QualityNumList;
struct FashionTemplate;
class Player;

class FashionManager
{
public:
    typedef std::vector<FASHION_INFO> FashionList;
    typedef std::vector<uint32> TreasureRandomList;
    typedef std::map<uint32, uint32> PackageCountMap;
    typedef std::map<uint32, PropertyList> PackageTiemMap;
    typedef std::vector<uint32> FashionSkillIdList;

    FashionManager(Player* player);
    ~FashionManager();

    void Clear();

    void OnRecv(uint32 msgID, void * data);

    void LoadFashionInfo(const FASHION_BAG& info);                                                  // 加载装备信息
    void FillFashionInfo(FASHION_BAG& info);                                                        // 填充装备信息

    void CalFashionProp();                                                                          // 计算时装属性
    void AddFashion(uint32 fashion_id);                                                             // 添加时装（脸，翅膀，时装）
    void SendFashionListNotify();                                                                   // 发送时装列表通知
    void FashionCompose(void* data);                                                                // 时装合成

    void OnSetFashionReq(void* data);                                                               // 设置时装

    uint32  GetFaceID() { return m_face_id; }                                                       // 获得脸谱ID
    uint32  GetFashionID() { return m_fashion_id; }                                                 // 获得时装ID
    void    SetFashionID(uint32 fid) { m_fashion_id = fid; }

    void SetUseItemFashionRecuit(int32 curid);                                                      // 使用物品换装

    void OnActivateFashion(void* data);                                                             // 时装激活
    void OnFashionLevelUp(void* data);                                                              // 时装升级
    void OnGetTreasureReq(void* data);                                                              // 时装宝藏抽取

    void onFashionSmeltReq(void* data);                                                             // 时装碎片熔炼

    void onIncarnationBuyReq(void* data);                                                           // 化身点购买
    const QualityNumList& GetQualityNumList() { return qualityNumList;}                                   // 激活统计品质类表个数

    void SetFashionSkillID();                                                                       // 设置化身技能

    int32 GetTaskFashionLevel(int32 id);                                                            // 获取化身等级

    int32 GetTaskFashionStarLevel(int32 star, int32 level);                                                        // 获取化身等级

    int32 GetRSkillId(){ return m_RSkillId; }                                                       // R技能

    void GmSkillFashion();                                                                          // gm开启化身技能
private:
    void FashionAlatsReq(void* data);                                                               // 时装图鉴
    bool CheckHaveFashion(uint32 fashion_id);                                                       // 检查fashion列表里是否有这个时装

    uint16 SubFashionCompose(const FashionTemplate* fashionTmpl);                                   // 时装合成的物品

    void SendFashionComposeAck(uint16 error, uint32 fashionid);                                     // 发送装备合成

    void SendActivateFashionAck(uint16 error);                                                      // 时装激活通知
    void SendFashionLevelUpAck(uint16 error);                                                       // 时装升级通知
    void SendIncarnationBuyAck(uint16 error);                                                       // 化身商店购买

    void SendFashionGetTREasureAck(uint16 error, int32 type, int32 lasttime);                       // 抽奖回应

    bool CheckRandomIncarnationCard(uint32 fashionid, bool& have);                                  // 判断幻化卡是否可以抽取
    bool CheckHaveIncarnationCard(uint32 fashionid);                                                // 判断幻化卡是否可以抽取

    void RandomTreasureHandle(int type, int goodf, std::vector<REWARD_ITEM>* itemList, bool& have); // 随机奖励
    void GetTreasureFirstHandle(int type, int itemID, std::vector<REWARD_ITEM>* itemList);          // 获取指定奖励

    void FashionMarquee(int32 id, int32 quality);                                                   // 推送全服公告
    void GetFashionCardMarquee(int32 id);                                                           // 推送全服公告

    void InitQualityNum();                                                                          // 统计xx星化身的个数;
    void AddQualityNum(int32 quality);                                                              // 添加xx化身的个数
    void SendTaskEvent();

    void InitFashionSkill(bool flag);                                                                        // 初始技能列表
    std::vector<uint32> GetFashionIdList();                                                         // 化身id列表
    FASHION_INFO* GetFashionById(uint32 id);                                                        // 获取化身信息
    bool HasFashionSkillID(uint32 fashionSkillId);

    void OnSetFashionSkillId(void* data);                                                           // 设置化身技能
    void OnGetFashionSkillInfo(void* data);                                                         // 请求化身技能数据

    int32 JudgeFashionTotalNum();                                                                   // 激活化身总数
    int32 JudgeFashionTotalLevel();                                                                 // 激活化身等级


private:
    Player*                 m_player;                                                               // 角色指针

    uint32                  m_face_id;                                                              // 脸ID
    uint32                  m_fashion_id;                                                           // 时装ID

    FashionList             m_face_list;                                                            // 脸谱列表
    FashionList             m_fashion_list;                                                         // 时装列表

    uint32                  m_last_finish_time;                                                     // 最后一次免费抽取时间

    TreasureRandomList      m_randomList;                                                           // 抽奖随机参数

    QualityNumList          qualityNumList;                                                         // 激活统计品质个数

    PackageCountMap         m_packageCountMap;                                                      // 包次数计数
    PackageTiemMap          m_packageItemTime;                                                      // 物品时间计时

    FashionSkillIdList      m_skillIdList;                                                          // 开启技能列表
    uint32                  m_fashionSkillId;                                                       // 当前使用的化身技能id
    uint32                  m_RSkillId;                                                             // 当前使用的R技能id

};

#endif