#ifndef announce_manager_h__
#define announce_manager_h__

/*
*	循环公告
*/
typedef struct AnnouceStruct
{

    int32 looptime;             // 循环周期(秒)
    int32 isloop;               // 是否循环
    std::string chat;           // 公告内容
    uint32 nextlooptime;        // 下次播放时间
    uint32 starttime;           // 下次播放时间
    uint32 endtime;             // 下次播放时间

    void clear()
    {
        looptime = 0;
        isloop = 0;
        chat = "";
        nextlooptime = 0;
    }
} AnnouceStruct;


class AnnouceManager
{
public:
    AnnouceManager();
    ~AnnouceManager();

    DECLARE_SINGLETON(AnnouceManager)

    void Update();

    void OnInsert(CHAT_MSG* recvMsg);

    bool GetAnnouceList(Json::Value& cmdjson);
    bool DeleteAnnouceId(int32 id);
private:

    std::map<int32, AnnouceStruct> m_annouceList;							    // 循环公告列表

    int32							m_id;								        // 公告唯一id
};
#endif 