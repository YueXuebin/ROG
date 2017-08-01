/*
*	称号系统
*/
#ifndef title_manager_h__
#define title_manager_h__
class Scene;

class Player;

class TitleManager
{
public:
    typedef std::vector<TITLE_INFO> TitleList;

    TitleManager(Player* player);
    ~TitleManager(void);

    void LoadTitleInfo(TITLE_LIST& info);   ///< 加载称号

    void FillTitleInfo(TITLE_LIST& info);   ///< 填充称号

    void OnRecv(uint32 msgID, void * data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );//接受事件

    void Clear();

    void InitTitle(void* data);         ///<.初始化title

    void EquipTitle(void* data);//装备称号

    void CalDailyRefresh();   ///<.时间刷新

    void CalProp();          ///<.计算属性

    void AddTitle(uint32 titileid);

    void TaskTitle(int32 type,uint32 taskid);

    void AshuarTitle(int32 flag,int32 num);

    void RankTitle(int32 type,int32 rank);

    void MoreThanCondiiton(int32 type,int32 level);

    TitleList& GetTitleList(){return m_titleList;}

    void  onEnterScene( Scene* pScene );

    void OnRankingHourRefresh();           //刷新排行榜，向center申请排行消息

private:
    void SendInitTitleAck(uint16 b_state,TITLE_INFO* titleInfo = 0);
    void SendUpdateTitleAck(TITLE_INFO* titleInfo,bool Instbool);

    int32 GetHaveNum();                      ///<得到拥有称号个数

    void GeneRalTitle(int32 type,int32 reason,int32 num = 0);///判断通用的称号的条件

    void SendOneUpate(uint16 b_state, TITLE_INFO* titleInfo);

    void DealAllPlayer(int32 type);

    void NotifyOtherPlayer();

    TITLE_INFO* GetTitleInfo(uint32 titleid);  ///<.得到称号信息

    void SendRankReqList();                   //发送不同的排行请求

    void OnAskRankReq(int32 ranktype);


    void GetDeleteVipLevel(bool upOrDown,int32 type,int32 titleid, int32& viplevel,int32& maxvipid, std::vector<int32>& titlevec);  ////比较大vip称号，找到小的vip称号
    void JudgeVipTitle(bool upOrDown);                                                                                              ///找到最大的vip称号,还是小于vip等级
    void DeleteTitle(int32 titleid);
    void SetTitleState(uint32 titleid,int32 state);
    int32 JudgeVipUpOrDown(int32 level);                                                                                            ///判断vip是升级，还是降级

    void DeleteOutDateVip();

private:

    TitleList m_titleList;                    ///<.成就列表

    TitleList m_updateList;                   ///<.更新称号

    Player*  m_player;

    enum
    {
        VIP_UP = 1,
        VIP_DOWN = 2,
        VIP_ERROR = 3
    };

};

#endif 
