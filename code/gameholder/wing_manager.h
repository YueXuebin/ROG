/*
 *	翅膀
 */
#ifndef wing_manager_h__
#define wing_manager_h__
#include "wing_attr_config.h"

class Player;
struct WingTemplate;
struct LockSpend;
class WingManager
{
public:
    WingManager(Player* player);
    ~WingManager();

    typedef std::vector<PRO_LOCK> VEC_PRO_LOCK;

    void Clear();

    void LoadInfo(const WING_INFO& info);
    void FillInfo(WING_INFO& info);

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void CalDailyRefresh();                                 // 每日刷新

    void onWingInfoReq();                                   // 翅膀信息请求
    void onAdvanceReq(void* data);                          // 升阶请求
    void onActivateReq(void* data);                         // 翅膀激活请求
    void onTransmogrificationReq(uint32 msgID, void* data); // 幻化请求

    void onLockSpendCondition(void* data);

    bool ActivateWing(uint32 id);                           // 激活

    void CalWingProp();                                     // 计算翅膀属性


    int32 GetWingLevel();                                   // 获取最高翅膀阶数
    
    WING_ONE_INFO* GetWingOneInfo(uint32 id);               // 单个翅膀信息
    WING_INFO& GetWingInfo() {return m_wing_info;}          // 翅膀信息

private:
    void SendProlLockAck(PRO_LOCK* prolock,uint16 curerror);
    void sendPro();                                         // 发送翅膀属性

    void SendWingAdvanck(uint16 error_code, uint32 wingID);                                   // 进阶应答

    void WingMarquee(int32 level);                                                            // 发送公告
    bool AdvanceOneWing(uint32 id, const WING_ATTR_ROOT_WING_ITEM_STRUCT* wingItemTpl);       // 把指定的翅膀，升级到wingItemTpl里面的等级

private:
    Player* m_player;

    WING_INFO m_wing_info;
    bool            m_PropertyDirty;                        // 属性发生改变
};

#endif // wing_manager_h__
