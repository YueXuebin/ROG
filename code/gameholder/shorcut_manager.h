#ifndef shorcut_manager_h__
#define shorcut_manager_h__

class Player;

class ShorcutManager
{
public:
    ShorcutManager(Player* player);
    ~ShorcutManager(void);

    void Clear();

    void OnRecv(uint32 msgID, void * data);

    void LoadShorcutInfo(SHORCUT_BAG& info);                                ///< 加载游戏收藏信息
    void FillShorcutInfo(SHORCUT_BAG& info);                                ///< 填充游戏收藏信息
    void OnGmSetLike();                                                     // fb点赞设置
    bool JudgeDailyFashionFreeTime();
    void SetDailyFashionFreeTime();
private:
    Player*         m_player;                                               ///< 角色指针
    
    void OnSetShorcutReq(void* data);                                       // 收藏有礼状态设置
    void OnTasteVipReq(void* data);                                         // vip体验卡状态设置
    void OnSetLikeReq(void* data);                                          // fb点赞设置
    void OnDailyFashionReq(void* data);                                          // fb点赞设置

    SHORCUT_BAG m_shorcut_info;                                             ///< 游戏收藏信息
};

#endif