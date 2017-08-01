#include "centerserver_pch.h"
#include "kunlun_pay.h"
#include "config_loader.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "player.h"
#include "player_manager.h"
#include "player_job.h"
#include "pay_job.h"
#include "dongyi_log.h"
#include "kunlun_pay_http_async.h"
#include "kunlun_pay_event.h"
#include "offline_event_manager.h"

IMPLEMENT_SINGLETON(KlPayExecutor)

// ==============================
// 构造函数
// ==============================
KlPayExecutor::KlPayExecutor()
{
    std::string SemaName = "KunlunPay_" + Crown::ToString(Crown::SDGetCurrentProcessId());
    m_KunlunPaySema = Crown::CreateNamedSemaphore(SemaName.c_str(), 0);
    CnAssert(m_KunlunPaySema);
    CnInfo("create semaphore %s\n", SemaName.c_str());

    m_payAsync = NULL;
}

// ==============================
// 析构函数
// ==============================
KlPayExecutor::~KlPayExecutor()
{
    UnInit();
}

// ==============================
// 初始化
// ==============================
bool KlPayExecutor::Init(const char* addrIP, uint32 addrPort)
{
    if(m_payAsync)
    {
        CnDelete m_payAsync;
        m_payAsync = 0;
    }

    m_payAsync = CnNew KlPayAsync;

    // 初始化异步执行
    m_payAsync->SetSemaphore(m_KunlunPaySema);

    m_payAsync->SetExecutor(this);

    m_payAsync->Init(addrIP, addrPort);

    m_payAsync->Start();

    return true;
}

// ==============================
// 销毁
// ==============================
void KlPayExecutor::UnInit()
{
    if(m_payAsync)
    {
        CnDelete m_payAsync;
        m_payAsync = NULL;
    }

    Crown::DestoryNamedSemaphore(m_KunlunPaySema);
    m_KunlunPaySema = NULL;
}

// ==============================
// 更新
// ==============================
void KlPayExecutor::Update()
{
    Player* player = 0;

    for(KlPayEvent* payEvent = CallbackPopFront(); payEvent; payEvent = CallbackPopFront())
    {
        if(!payEvent)
        {
            CnAssert(false);
            continue;
        }

        std::string server_name = MakeUpServerName(payEvent->m_serverid);

        // 重新加载角色的充值信息
        PlayerManager::Instance()->ReloadPlayerRechargeInfo(payEvent->m_uid, server_name);

        // 如果角色在线，则通知魔石信息
        player = PlayerManager::Instance()->GetPlayerByAccount(payEvent->m_uid, server_name);

        int32 paytype = (payEvent->m_paytype != 0 ? payEvent->m_paytype : PAY_TYPE_KUNLUN);     // 0 表示平台RMB充值

        if(player && (player->GetState() >= Player::kStateInGame))
        {
            player->AddMoneyPoint(payEvent->m_coins, paytype);          

            player->NotifyGamePlayerProp(PROP_MONEYPOINT, player->GetMoneyPoint());

            CnInfo("pay online name=%s pay=%d type=%d\n", player->GetName().c_str(), payEvent->m_coins, paytype);
        }
        else
        {   // 玩家不在线的充值
            CnInfo("pay offline account=%s pay=%d type=%d\n", payEvent->m_uid.c_str(), payEvent->m_coins, paytype);
        }

        std::string eventInfo = Crown::ToString(payEvent->m_coins);
        OfflineEventManager::Instance()->SendOfflineEvent(payEvent->m_uid, server_name, OFF_EVENT_RECHARGE_ONE, eventInfo);

        delete payEvent;
    }
}

KlPayEvent* KlPayExecutor::PayPopFront()
{
    KlPayEvent* pEvent = NULL;
    QueryLock();
    if(!m_queryList.empty())
    {
        pEvent = m_queryList.front();
        m_queryList.pop_front();
    }
    QueryUnLock();
    return pEvent;
}

void KlPayExecutor::PayPushBack(KlPayEvent* pEvent)
{
    QueryLock();
    m_queryList.push_back(pEvent);

    m_KunlunPaySema->Post();

    QueryUnLock();
}

KlPayEvent* KlPayExecutor::CallbackPopFront()
{
    KlPayEvent* pEvent = NULL;
    CallbackLock();
    if(!m_callbackList.empty())
    {
        pEvent = m_callbackList.front();
        m_callbackList.pop_front();
    }
    CallbackUnLock();
    return pEvent;
}

void KlPayExecutor::CallbackPushBack(KlPayEvent* pEvent)
{
    CallbackLock();
    m_callbackList.push_back(pEvent);
    CallbackUnLock();
}

