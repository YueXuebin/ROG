#ifndef kunlun_pay_event_h__
#define kunlun_pay_event_h__

#include "../net_manager/pool_allocator.h"

#define KUNLUN_PAY_EVENT_POOL_COUNT 10000

// ==================================================
// 昆仑登陆事件
// ==================================================
class KlPayEvent
{
public:
    KlPayEvent() { }
    ~KlPayEvent() { }

    DECLARE_ALLOCATOR(KlPayEvent, KUNLUN_PAY_EVENT_POOL_COUNT)

    void Clear()
    {
        m_oid       = "";
        m_uid       = "";
        m_amount    = "";
        m_coins     = 0;
        m_dtime     = 0;
        m_sign      = "";
        m_ext       = "";
        m_partner   = "";
        m_serverid  = "";
        m_paytype   = 0;
    }

    std::string     m_oid;          ///< 昆仑平台订单号
    std::string     m_uid;          ///< 昆仑平台订单用户ID64位整数 1-2^63-1注意数据库字段长度
    std::string     m_amount;       ///< 运营商平台订单金额，需要支持0金币加游戏币，注意需要支持到小数点后两位，如 1.00
    int32           m_coins;        ///< 运营商平台订单游戏币，游戏币数目需要以此字段为准
    uint32          m_dtime;        ///< 订单完成时间戳，例如：1293840000 表示 2011-01-01 00:00:00
    std::string     m_sign;         ///< 通信签名（注意为小写），参与生成签名的KEY由昆仑分配
    std::string     m_ext;          ///< 回调扩展字段 暂时为空
    std::string     m_partner;      ///< 第三方平台标志
    std::string     m_serverid;     ///< 服务器ID信息（仅开发模式二时需要）
    int32           m_paytype;     ///<.是否是gmt 审批的魔石, 审批的类型
};

#endif // kunlun_pay_event_h__
