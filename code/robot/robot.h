#ifndef robot_h__
#define robot_h__


#include "battle_unit.h"
#include "sceneobject_unit.h"

class CaseBase;
class ActionBase;

class Robot
{
public:
    Robot();
    ~Robot();

    /// 机器人状态
    enum
    {
        kStateInit              = 0,        // 初始状态
        kStateConnectReq        = 1,        // 请求建立网络连接
        kStateNetClosed         = 2,        // 网络网络断开连接
        kStateConnected         = 3,        // 已建立网络连接
        kStateLoginSend         = 4,        // 请求网络登录
        kStateLogin             = 5,        // 已登录
        kStateCreateSend        = 6,        // 请求建立角色
        kStateCreate            = 7,        // 创建成功
        kStateLoadPlayerSend    = 8,        // 请求加载角色进入游戏
        kStateLoadPlayer        = 9,        // 加载角色进入游戏成功
        // ----------------------------------------
        kStateInGame            = 100,      // 在游戏中
        // ----------------------------------------
        kStateClose             = 9997,     // 机器人关闭
        kStateBreakLine         = 9998,     // 机器人断线
        kStateUnint             = 9999      // 机器人跪了
    };

    void Clear();

    std::string& GetAccount() { return m_account; }         // 获得账号
    void SetAccount(std::string val) { m_account = val; }   // 设置账号

    int32 GetState() { return m_state; }                    // 获得机器人状态
    void SetState(int32 val) { m_state = val; }             // 设置机器人状态

    void SetSvrIP(std::string& val) { m_svrIP = val; }
    void SetSvrPort(int32 val) { m_svrPort = val; }

    void Update(uint32 tickcount, uint32 deltacount);                              // 刷新机器人智能决策

    // 接收网络数据
    static void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);
    void Send(uint32 msgID, void* data);                     // 发送网络协议
    bool SendPacket(Net_Packet* packet);                     // 发送网络协议包

    /////////////////////////////////////////////////////////
    // 机器人属性
    PROPERTY_INFO&  GetPropInfo() {return m_prop;}
    uint32  m_DungeonID;
    uint32  m_ChannelID;

    uint32  GetOneEnemy();
    BATTLEUNIT_APPEAR* GetBattleUnit(uint32 objectId);

    uint32 GetSceneObj();
    SCENEOBJECT_APPEAR* GetSceneObjUnit(uint32 touchId);                 //touchId

    void    CloseNet();
    bool    IsClose() {return GetState() == kStateClose;}

    void    UseSkill(int32 objectId, int32 skillId);

    //EQUIP_BAG& GetEquip() {return m_equip;}
    /////////////////////////////////////////////////////////
    int32 GetBagItemNum(uint32 itemid);
private:
    void DoAction(ActionBase* pAction);                    // 执行新的动作

    // 网络-----------------------------------------------------------------
    void CreateNetConnect();                                // 创建网络连接
    void CloseNetConnect();                                 // 关闭网络连接

    // 登陆------------------------------------------------------------------
    void SendEncryptConnectReq();                           // 发送加密请求
    void RecvEncryptPublicKeyNet(void* data);
    void SendLoginReq();                                    // 发送登陆请求
    void RecvLoginAck(void* data);                          // 接收登陆应答

    // 创建角色--------------------------------------------------------------
    void SendCreateReq();                                   // 创建角色信息
    void RecvCreateAck(void* data);                         // 创建角色应答

    void RecvPlayerInfo(void* data);                        // 收取角色信息

    // 请求加载角色进入游戏--------------------------------------------------
    void SendLoadPlayerReq();                               // 加载角色进入游戏
    void RecvLoadPlayerAck(void* data);                     // 加载角色失败应答

    // 接收协议
    void RecvJumpMap(void* data);                           // 接收跳转地图请求

    void RecvSceneObjectAppear(void* data);
    void RecvSceneObjectDisappear(void*data);
    void RecvBattleUnitAppear(void* data);
    void RecvBattleUnitDisappear(void* data);
    void RecvPropertyChange(void* data);
    void RecvOtherMove(void* data);

private:
    TransferClientProxy*    m_clientProxy;  // 网络代理
    int32                   m_state;        // 机器人状态

    std::string             m_svrIP;        // 服务器IP地址
    uint32                  m_svrPort;      // 服务器端口号

    uint32                  m_PublicKey;    // 服务器公有key
    uint32                  m_PrivateKey;   // 客户端私有key
    AfEncrypt               m_Encrptyer;    // 加密器

    std::string             m_account;      // 账号名

    PROPERTY_INFO           m_prop;         // 属性信息
    //EQUIP_BAG               m_equip;        // 装备信息
    //BAG_INFO                m_bag;          // 包裹信息
    TASK_INFO               m_task;         // 任务信息

    typedef std::map<uint32, BattleUnit> BattleUnitMap;     // 所有生物对象<object id, 生物对象>
    BattleUnitMap           m_BattleUnits;

    typedef std::map<uint32, SceneObject_unit> SceneObjectUnitMap;        //所有场景对象<object id,  当前为旗帜>
    SceneObjectUnitMap        m_SceneObjectUnits;


    CaseBase*               m_pCase;        // 行为用例

};

#endif
