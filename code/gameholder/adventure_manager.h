#ifndef adventure_manager_h__
#define adventure_manager_h__

class Player;
struct AdventureTemplate;

class AdventureManager
{
public:
    AdventureManager(Player* player);
    ~AdventureManager();

    void Clear();

    void LoadAdventureInfo(ADVENTURE_INFO& info);   // 加载大冒险数据

    void FillAdventureInfo(ADVENTURE_INFO& info);   // 填充大冒险数据

    void OnRecv(uint32 msgID, void * data);

private:

    void OnAdventureInfoReq();              // 大冒险信息请求

    void OnAdventureJoinReq(void* data);    // 加入大冒险请求

    void OnAdventureCmplReq(void* data);    // 大冒险完成请求

    void OnAdventurerListReq(void* data);   // 请求获取冒险者列表

    void OnAdventureRobReq(void* data);     // 打劫大冒险请求

    void OnAdventureRobAck(void* data);     // 打劫大冒险应答

    Player*         m_player;               // 角色指针

    void SendAddAdventure(Player* player, const AdventureTemplate* adventureTmpl, uint32 endtime);

    ADVENTURE_INFO  m_adventure;            // 大冒险数据结构
};

#endif
