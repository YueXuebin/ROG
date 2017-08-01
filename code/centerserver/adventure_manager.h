#ifndef ADVENTURE_MANAGER_H
#define ADVENTURE_MANAGER_H

class Player;

// ==================================================
/// 好友管理系统
// ==================================================
class AdventureManager
{
public:
    AdventureManager();
    ~AdventureManager();

    DECLARE_SINGLETON(AdventureManager)

    void Clear();

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

private:

    void OnAdventureListReq(uint64 playerID, void* data);   ///< 请求冒险者列表

    void OnAdventureRobReq(uint64 playerID, void* data);    ///< 打劫大冒险请求

    void OnAdventureAddReq(uint64 playerID, void* data);    ///< 添加大冒险信息
};

#endif
