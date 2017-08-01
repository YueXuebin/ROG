#include "centerserver_pch.h"
#include "mail_manager.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "config_loader.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "pusher_360.h"
#include "mail_job.h"

IMPLEMENT_SINGLETON(MailManager)

MailManager::MailManager()
{

}

MailManager::~MailManager()
{

}

// ==================================================
// ����������Ϣ
// ==================================================
void MailManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_MAIL_SEND_REQ:
        RecvSendMailReq(playerID, sessionID, data);
        break;
    case MSG_MAIL_OPT_REQ:
        RecvMailOptReq(playerID, sessionID, data);
        break;
    case MSG_MAIL_GET_ATTACH_REQ:
        RecvGetAttachReq(playerID, sessionID, data);
        break;
    case MSG_MAIL_DEL_EXPIRED_MAIL_NTF:
        RecvDelExpiredMailNtf(playerID, sessionID, data);
        break;
    default:
        break;
    }
}

// ==================================================
// �����ʼ�
// ==================================================
void MailManager::SendMail(uint64 playerID, const std::string& receiverName, const std::string& title, const std::string& message, 
                           uint8 mailType, const std::vector<REWARD_ITEM> list, uint64 tradeOrderID, const std::string& condition)
{
    MAIL_SEND_REQ sendMsg;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    sendMsg.Clear();

    sendMsg.mail_id.Clear();
    sendMsg.mail_type = mailType;
    sendMsg.receiver_id.Clear();
    sendMsg.receiver_name = receiverName;
    sendMsg.sender_id.id_h = U64ID_HIGH(playerID);
    sendMsg.sender_id.id_l = U64ID_LOW(playerID);
    sendMsg.sender_name = "";
    sendMsg.trade_order_id.id_h = U64ID_HIGH(tradeOrderID);
    sendMsg.trade_order_id.id_l = U64ID_LOW(tradeOrderID);
    sendMsg.send_time = now;
    sendMsg.title = title;
    sendMsg.message = message;
    sendMsg.mail_flag = MAIL_FLAG_NEW;
    sendMsg.attachment_flag = 0;
    sendMsg.attachment_list = list;
    sendMsg.receiver_condition = condition;

    MailManager::Instance()->OnRecv(playerID, 0, MSG_MAIL_SEND_REQ, &sendMsg);
}

// ==================================================
// ���շ����ʼ�����
// ==================================================
void MailManager::RecvSendMailReq(uint64 playerID, uint32 sessionID, void* data)
{
    MAIL_SEND_REQ* recvMsg = (MAIL_SEND_REQ*)data;
    uint64 mail_id = 0;

    if(!recvMsg) return;

    mail_id = GenMailID();

    recvMsg->mail_id.id_h = U64ID_HIGH(mail_id);
    recvMsg->mail_id.id_l = U64ID_LOW(mail_id);

    if(recvMsg->mail_type == MAIL_TYPE_PLAYER)
    {
        MailJob::Instance()->SearchMailReceiver(playerID, recvMsg);

        // !!! �ʼ�����Ҫ���ͣ���Ϊ���֮�䲻�ܻ����ʼ�������Ҫ��أ�
        //if(ConfigLoader::Instance()->GetLoginMode() == LOGIN_MODE_360)
        //    Pusher360Executor::Instance()->PushMailContent(recvMsg);
    }
    else if(recvMsg->mail_type == MAIL_TYPE_PLAYER_SYS)
    {
        MailJob::Instance()->SearchMailReceiver(playerID, recvMsg);
    }
    else if(recvMsg->mail_type == MAIL_TYPE_SYSTEM)
    {
        MailJob::Instance()->SendToSystemMailBox(playerID, recvMsg);
    }
    else if(recvMsg->mail_type == MAIL_TYPE_GROUP)
    {
        MailJob::Instance()->SendToGroupMailBox(playerID, recvMsg);
    }
}

// ==================================================
// �ʼ���������
// ==================================================
void MailManager::RecvMailOptReq(uint64 playerID, uint32 sessionID, void* data)
{
    MAIL_OPT_REQ* recvMsg = (MAIL_OPT_REQ*)data;
    Player* player = 0;

    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    if(recvMsg->mail_flag == MAIL_FLAG_READED)
    {
        player->ReadMail(recvMsg->mail_type, recvMsg->mail_id.id_h, recvMsg->mail_id.id_l, recvMsg->attach_flag);
    }
    else if(recvMsg->mail_flag == MAIL_FLAG_DELETED)
    {
        player->DelMail(recvMsg->mail_type, recvMsg->mail_id.id_h, recvMsg->mail_id.id_l, recvMsg->attach_flag);
    }
    else
    {
        CnAssert(false);
        return;
    }
}

// ==================================================
// ��ȡ�ʼ�����
// ==================================================
void MailManager::RecvGetAttachReq(uint64 playerID, uint32 sessionID, void* data)
{
    MAIL_GET_ATTACH* recvMsg = (MAIL_GET_ATTACH*)data;
    MAIL_INFO* mail_info = NULL;
    Player* player = 0;
    uint64  mail_id = 0;

    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnAssert(false);
        return;
    }

    mail_id = U64ID(recvMsg->mail_id_h, recvMsg->mail_id_l);

    mail_info = player->GetMail(recvMsg->mail_id_h, recvMsg->mail_id_l);
    if(!mail_info)
    {
        CnInfo("Player:%llu attach mail:%llu failed\n", playerID, mail_id);
        //CnAssert(false);
        return;
    }

    mail_info->attachment_flag = MAIL_ATTACH_FLAG_TAKE;

    if((mail_info->mail_type == MAIL_TYPE_PLAYER) || (mail_info->mail_type ==  MAIL_TYPE_PLAYER_SYS))
    {
        MailJob::Instance()->SetPlayerMailAccach(player->GetPlayerID(), mail_id);
    }
    else if(mail_info->mail_type == MAIL_TYPE_SYSTEM)
    {
        MailJob::Instance()->SetSystemMailAttach(player->GetPlayerID(), mail_id);
    }
    else if(mail_info->mail_type == MAIL_TYPE_GROUP)
    {
        MailJob::Instance()->SetGroupMailAttach(player->GetPlayerID(), mail_id);
    }
}

// ==================================================
// ɾ�������ʼ�
// ==================================================
void MailManager::RecvDelExpiredMailNtf(uint64 playerID, uint32 sessionID, void* data)
{
    MAIL_DEL_EXPIRED_MAIL_NTF* recvMsg = (MAIL_DEL_EXPIRED_MAIL_NTF*)data;
    uint64 expiredMailID = 0;

    if(!recvMsg) return;

    // ֪ͨ�ͻ���ɾ�������ʼ�
    GateHandler::Instance()->SendToAll(MSG_MAIL_DEL_EXPIRED_MAIL_NTF, recvMsg, BROADCAST_PLAYER_ID);

    // ֪ͨ GS ɾ�������ʼ�
    GameHandler::Instance()->SendToAll(MSG_MAIL_DEL_EXPIRED_MAIL_NTF, recvMsg);

    // �� CS ��ɾ�������ʼ�
    PlayerManager::Instance()->DelExpiredMail(recvMsg->mail_id_h);
}

// ==================================================
// ���ɶ���ID
// ==================================================
uint64 MailManager::GenMailID()
{
    static uint16 serialNum = 0;
    // ʱ��� 32bit, ���к�16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // ��ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

void MailManager::SendMessageMail(uint64 playerID, const std::string& receiverName, const std::vector<REWARD_ITEM>& list, uint32 message_define, int32 stringNum, ...)
{
    std::string message = "";
    message += Crown::ToString(message_define);     // message_define�����ͻ��ˣ��ɿͻ��˽���Ϊ����
    va_list marker;
    va_start(marker, stringNum);
    for(int i = 0; i < stringNum; ++i)
    {
        message += "|";
        message += va_arg(marker, char*);
    }
    va_end(marker);

    SendMail(playerID, receiverName, "", message, MAIL_TYPE_PLAYER_SYS, list);
}
