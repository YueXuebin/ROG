#include "centerserver_pch.h"
#include "mail_job.h"
#include "dbdriver.h"
#include "player_job.h"
#include "trade_job.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player_config.h"
#include "json_helper.h"

IMPLEMENT_SINGLETON(MailJob)

// ==================================================
// ���캯��
// ==================================================
MailJob::MailJob()
{
    m_dbExecutor = 0;

    InitKeywordMap();
}

// ==================================================
// ��������
// ==================================================
MailJob::~MailJob()
{
    
}

// ==================================================
// ��ʼ���ؼ���ӳ���
// ==================================================
void MailJob::InitKeywordMap()
{
    CondWordMap condWordMap;

    condWordMap.clear();
    condWordMap["player_level"] = "level";
    condWordMap["create_time"] = "create_time";

    m_mailTypeKeywordMap[MAIL_GROUP_CREATE_PLAYER] = condWordMap;

    condWordMap.clear();
    condWordMap["player_level"] = "T1.level";
    condWordMap["create_time"] = "T1.create_time";
    m_mailTypeKeywordMap[MAIL_GROUP_GUILD_MEMBER] = condWordMap;

    // �ʼ���������������
    // ============================================================
    m_mailConditionOpSet.insert(">");
    m_mailConditionOpSet.insert(">=");
    m_mailConditionOpSet.insert("=");
    m_mailConditionOpSet.insert("<");
    m_mailConditionOpSet.insert("<=");
}

// ==================================================
// �����ʼ�����
// ==================================================
//bool MailJob::SaveMailData(Player* player)
//{
//    if(!player) return false;
//
//    std::stringstream ss;
//
//    MAIL_SYSTEM_INFO& mail_info = player->GetMailSysInfo();
//
//    
//
//    return true;
//}

// ==================================================
// ��������ʼ�����
// ==================================================
bool MailJob::SavePlayerMailData(Player* player)
{
    DBRecorder recorder;
    std::vector<uint64> mail_id_list;
    uint64 mail_id = 0;

    if(!player) return false;

    MAIL_SYSTEM_INFO& mail_info = player->GetMailSysInfo();

    std::stringstream ss;
    ss << "UPDATE `player_mail` ";
    ss << "SET `mail_flag` = CASE `mail_id` ";

    for(std::vector<MAIL_INFO>::iterator it = mail_info.player_mail_list.begin();
            it != mail_info.player_mail_list.end(); ++it)
    {
        if(it->mail_flag)
        {
            mail_id = U64ID(it->mail_id.id_h, it->mail_id.id_l);
            mail_id_list.push_back(mail_id);

            ss << "WHEN ";
            ss << mail_id;
            ss << "THEN ";
            ss << it->mail_flag;
        }
    }

    ss << "END WHERE `mail_id` IN (";
    for(uint32 i=0; i < mail_id_list.size(); ++i)
    {
        if(i != 0)
        {
            ss << ",";
        }
        ss << mail_id_list[i];
    }

    recorder = m_dbExecutor->Query(ss.str().c_str());
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}
// ==================================================
// �����ʼ����������ߵĽ�ɫID
// ==================================================
void MailJob::SearchMailReceiver(uint64 playerID, MAIL_SEND_REQ* mail_send_Req)
{
    char name[PLAYER_NAME_LEN*2+1] = {0};
    uint64 receiverID = 0;

    if(!mail_send_Req) return;

    uint32 nameLength = mail_send_Req->receiver_name.length();

    // ���������������쳣,��ֹ��Խ�絼�¶�ջ�쳣,Ӧ�����յ�Э�鴦�ͶԳ����������
    if (nameLength > PLAYER_NAME_LEN)
    {
        CnDbg("player name too long\n");
        return;
    }

    m_dbExecutor->EscapeString(name, mail_send_Req->receiver_name.c_str(), nameLength);

    receiverID = U64ID(mail_send_Req->receiver_id.id_h, mail_send_Req->receiver_id.id_l);

    if(receiverID)
    {
        m_dbExecutor->Query(this, &MailJob::CBSearchMailReceiverName, playerID, *mail_send_Req,
            "SELECT player_name FROM `player` WHERE player_id=%llu", receiverID);
    }
    else
    {
        m_dbExecutor->Query(this, &MailJob::CBSearchMailReceiverID, playerID, *mail_send_Req,
            "SELECT player_id FROM `player` WHERE player_name='%s'", name);
    }
}

// ==================================================
// ���͵���ɫ����
// ==================================================
void MailJob::SendToPlayerMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req)
{
    char sender_name[PLAYER_NAME_LEN*2+1] = {0};
    char title[MAX_MAIL_TITLE_LEN*2+1] = {0};
    char message[MAX_MAIL_CONTENT_LEN*2+1] = {0};
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    uint64 order_id = 0;
    Json::FastWriter writer;
    Json::Value item_json;
    Json::Value attach_json;
    std::string attachment_str;

    if(!mail_send_Req) return;

    mail_id = U64ID(mail_send_Req->mail_id.id_h, mail_send_Req->mail_id.id_l);
    receiver_id = U64ID(mail_send_Req->receiver_id.id_h, mail_send_Req->receiver_id.id_l);
    sender_id = U64ID(mail_send_Req->sender_id.id_h, mail_send_Req->sender_id.id_l);
    order_id = U64ID(mail_send_Req->trade_order_id.id_h, mail_send_Req->trade_order_id.id_l);

    // ����ת���
    m_dbExecutor->EscapeString(sender_name, mail_send_Req->sender_name.c_str(), mail_send_Req->sender_name.length());
    m_dbExecutor->EscapeString(title, mail_send_Req->title.c_str(), mail_send_Req->title.length());
    m_dbExecutor->EscapeString(message, mail_send_Req->message.c_str(), mail_send_Req->message.length());

    //��д������Json��Ϣ
    attach_json.clear();
    for(std::vector<REWARD_ITEM>::iterator itemIt = mail_send_Req->attachment_list.begin(); 
            itemIt != mail_send_Req->attachment_list.end(); ++itemIt)
    {
        item_json.clear();
        write_REWARD_ITEM(*itemIt, item_json);
        attach_json["attach"].append(item_json);
    }
    attachment_str = writer.write(attach_json);

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::CBSendToPlayerMailBox, playerID, *mail_send_Req,
        "INSERT INTO `player_mail` SET `mail_id`=%llu, `receiver_id`=%llu, `sender_id`=%llu, `sender_name`='%s', `order_id`=%llu, `mail_type`=%u, "
        "`mail_flag`=%u, `title`='%s', `message`='%s', `attachment_flag`=%u, `attachment`='%s'", 
        mail_id, receiver_id, sender_id, sender_name, order_id, mail_send_Req->mail_type, 
        mail_send_Req->mail_flag, title, message, mail_send_Req->attachment_flag, attachment_str.c_str());
}

// ==================================================
// ���͵�ϵͳ����
// ==================================================
void MailJob::SendToSystemMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req)
{
    char sender_name[PLAYER_NAME_LEN*2+1] = {0};
    char title[MAX_MAIL_TITLE_LEN*2+1] = {0};
    char message[MAX_MAIL_CONTENT_LEN*2+1] = {0};
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    Json::FastWriter writer;
    Json::Value item_json;
    Json::Value attach_json;
    std::string attachment_str;

    if(!mail_send_Req) return;

    mail_id = U64ID(mail_send_Req->mail_id.id_h, mail_send_Req->mail_id.id_l);
    receiver_id = U64ID(mail_send_Req->receiver_id.id_h, mail_send_Req->receiver_id.id_l);
    sender_id = U64ID(mail_send_Req->sender_id.id_h, mail_send_Req->sender_id.id_l);

    // ����ת���
    m_dbExecutor->EscapeString(sender_name, mail_send_Req->sender_name.c_str(), mail_send_Req->sender_name.length());
    m_dbExecutor->EscapeString(title, mail_send_Req->title.c_str(), mail_send_Req->title.length());
    m_dbExecutor->EscapeString(message, mail_send_Req->message.c_str(), mail_send_Req->message.length());

    // ��д������Json��Ϣ
    attach_json.clear();
    for(std::vector<REWARD_ITEM>::iterator itemIt = mail_send_Req->attachment_list.begin(); 
        itemIt != mail_send_Req->attachment_list.end(); ++itemIt)
    {
        item_json.clear();
        write_REWARD_ITEM(*itemIt, item_json);
        attach_json["attach"].append(item_json);
    }
    attachment_str = writer.write(attach_json);

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::CBSendToSystemMailBox, playerID, *mail_send_Req,
        "INSERT INTO `system_mail` SET `mail_id`=%llu, `sender_id`=%llu, `mail_type`=%u, "
        "`title`='%s', `message`='%s', `attachment`='%s'", 
        mail_id, sender_id, mail_send_Req->mail_type, 
        title, message, attachment_str.c_str());
}

// ==================================================
// ���͵�������
// ==================================================
void MailJob::SendToGroupMailBox(uint64 playerID, MAIL_SEND_REQ* mail_send_Req)
{
    char sender_name[PLAYER_NAME_LEN*2+1] = {0};
    char title[MAX_MAIL_TITLE_LEN*2+1] = {0};
    char message[MAX_MAIL_CONTENT_LEN*2+1] = {0};
    uint64 mail_id = 0;
    uint64 receiver_id = 0;
    uint64 sender_id = 0;
    Json::FastWriter writer;
    Json::Value item_json;
    Json::Value attach_json;
    std::string attachment_str;

    if(!mail_send_Req) return;

    mail_id = U64ID(mail_send_Req->mail_id.id_h, mail_send_Req->mail_id.id_l);
    receiver_id = U64ID(mail_send_Req->receiver_id.id_h, mail_send_Req->receiver_id.id_l);
    sender_id = U64ID(mail_send_Req->sender_id.id_h, mail_send_Req->sender_id.id_l);

    // ����ת���
    m_dbExecutor->EscapeString(sender_name, mail_send_Req->sender_name.c_str(), mail_send_Req->sender_name.length());
    m_dbExecutor->EscapeString(title, mail_send_Req->title.c_str(), mail_send_Req->title.length());
    m_dbExecutor->EscapeString(message, mail_send_Req->message.c_str(), mail_send_Req->message.length());

    // ��д������Json��Ϣ
    attach_json.clear();
    for(std::vector<REWARD_ITEM>::iterator itemIt = mail_send_Req->attachment_list.begin(); 
        itemIt != mail_send_Req->attachment_list.end(); ++itemIt)
    {
        item_json.clear();
        write_REWARD_ITEM(*itemIt, item_json);
        attach_json["attach"].append(item_json);
    }
    attachment_str = writer.write(attach_json);

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::CBSendToGroupMailBox, playerID, *mail_send_Req,
        "INSERT INTO `group_mail` SET `mail_id`=%llu, `sender_id`=%llu, `mail_type`=%u, "
        "`title`='%s', `message`='%s', `attachment`='%s'", 
        mail_id, sender_id, mail_send_Req->mail_type, 
        title, message, attachment_str.c_str());
}

// ==================================================
// �������ʼ�����ǰ�Ѵ�����ɫ
// ==================================================
void MailJob::SendGroupMailToCreatePlayer(MAIL_SEND_REQ* mail_send_Req, std::string& condition)
{
    uint64 mail_id = 0;
    std::stringstream ss;

    // �ʼ�ID
    mail_id = U64ID(mail_send_Req->mail_id.id_h, mail_send_Req->mail_id.id_l);

    // �������ʼ�
    ss << "INSERT INTO `group_mail_flag` SELECT ";
    ss << mail_id;
    ss << ", player_id, ";
    ss << MAIL_FLAG_NEW;
    ss << ", 0 FROM player";

    // �ж��Ƿ�����ѡ����
    if(condition.length() > 0)
    {
        ss << " WHERE";
        ss << condition;
    }

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::CBSendGroupMailToCreatePlayer, *mail_send_Req, condition, ss.str().c_str());
}

// ==================================================
// �������ʼ��������Ա
// ==================================================
void MailJob::SendGroupMailToGuildMember(MAIL_SEND_REQ* mail_send_Req, std::string& condition, std::vector<std::string>& parameter)
{
    uint64 mail_id = 0;
    std::string guild_name;
    std::stringstream ss;

    // �ʼ�ID
    mail_id = U64ID(mail_send_Req->mail_id.id_h, mail_send_Req->mail_id.id_l);

    if(parameter.size() > 0)
    {
        guild_name = parameter[0];
    }
    else
    {
        return;
    }

    // �������ʼ�
    ss << "INSERT INTO `group_mail_flag` SELECT ";
    ss << mail_id;
    ss << ", T1.player_id, ";
    ss << MAIL_FLAG_NEW;
    ss << ", 0 FROM player AS T1, ";
    ss << "(SELECT guild_id from guild_info WHERE guild_name='";
    ss << guild_name;
    ss << "') AS T2 ";
    ss << "WHERE T1.guild_id = T2.guild_id";

    // �ж��Ƿ�����ѡ����
    if(condition.length() > 0)
    {
        ss << " AND";
        ss << condition;
    }

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::GetGuildMemberForGroupMail, *mail_send_Req, guild_name, condition, ss.str().c_str());
}

// ==================================================
// ���ø����ʼ����
// ==================================================
void MailJob::SetPlayerMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag)
{
    // ���¸����ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetPlayerMailFlag,
        "UPDATE `player_mail` SET `mail_flag`=%u WHERE `mail_id`=%llu AND `receiver_id`=%llu", 
        mailFlag, mailID, playerID);
}

// ==================================================
// ���ø����ʼ���ȡ����
// ==================================================
void MailJob::SetPlayerMailAccach(uint64 playerID, uint64 mailID)
{
    // ���¸����ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetPlayerMailFlag,
        "UPDATE `player_mail` SET `attachment_flag`=%u WHERE `mail_id`=%llu AND `receiver_id`=%llu", 
        MAIL_ATTACH_FLAG_TAKE, mailID, playerID);
}

// ==================================================
// ����ϵͳ�ʼ����
// ==================================================
void MailJob::SetSystemMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag, uint8 attachFlag)
{
    // ����ϵͳ�ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetSystemMailFlag,
        "INSERT INTO `system_mail_flag` VALUES('%llu', '%llu', '%u', '%u') "
        "ON DUPLICATE KEY UPDATE `mail_flag`=%u", 
        mailID, playerID, mailFlag, attachFlag, mailFlag);
}

// ==================================================
// ����ϵͳ�ʼ���ȡ����
// ==================================================
void MailJob::SetSystemMailAttach(uint64 playerID, uint64 mailID)
{
    // ����ϵͳ�ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetSystemMailFlag,
        "INSERT INTO `system_mail_flag` VALUES('%llu', '%llu', '%u', '%u') "
        "ON DUPLICATE KEY UPDATE `mail_flag`=%u, `attachment_flag`=%u", 
        mailID, playerID, MAIL_FLAG_READED, MAIL_ATTACH_FLAG_TAKE, MAIL_FLAG_READED, MAIL_ATTACH_FLAG_TAKE);
}

// ==================================================
// �������ʼ����
// ==================================================
void MailJob::SetGroupMailFlag(uint64 playerID, uint64 mailID, uint8 mailFlag, uint8 attachFlag)
{
    // �������ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetGroupMailFlag,
        "UPDATE `group_mail_flag` SET `mail_flag`=%u, `attachment_flag`=%u WHERE `mail_id`=%llu AND player_id=%llu", 
        mailFlag, attachFlag, mailID, playerID);
}

// ==================================================
// �������ʼ���ȡ����
// ==================================================
void MailJob::SetGroupMailAttach(uint64 playerID, uint64 mailID)
{
    // �������ʼ����
    m_dbExecutor->Query(this, &MailJob::CBSetGroupMailFlag,
        "UPDATE `group_mail_flag` SET `mail_flag`=%u, `attachment_flag`=%u WHERE `mail_id`=%llu AND player_id=%llu", 
        MAIL_FLAG_READED, MAIL_ATTACH_FLAG_TAKE, mailID, playerID);
}

// ==================================================
// �����ʼ����������ߵĽ�ɫ����
// ==================================================
void MailJob::CBSearchMailReceiverName(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req)
{
    Player* player = 0;
    uint64 receiverID = 0;
    std::string receiverName;
    ERROR_CODE sendMsg;

    receiverID = U64ID(mail_send_req.receiver_id.id_h, mail_send_req.receiver_id.id_l);

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    if(res.GetRowNum() < 1)
    {
        sendMsg.errcode = ERR_FAILED;
        if(player)
        {
            player->SendToGate(MSG_MAIL_SEND_ACK, &sendMsg);
        }
        CnDbg("search mail receiver name failed %s\n", mail_send_req.receiver_name.c_str());
        return;
    }
    else
    {
        sendMsg.errcode = ERR_SUCCEED;
        // ��ʱ����������ʱ����Ҫͨ���ʼ����͸��Լ���Ʒ
        if(player && (playerID != receiverID))
        {
            player->SendToGate(MSG_MAIL_SEND_ACK, &sendMsg);
        }
    }

    res.GetRecord(receiverName);

    mail_send_req.receiver_name = receiverName;

    SendToPlayerMailBox(receiverID, &mail_send_req);
}

// ==================================================
// �����ʼ����������ߵĽ�ɫID
// ==================================================
void MailJob::CBSearchMailReceiverID(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req)
{
    Player* player = 0;
    uint64  receiver_id = 0;
    ERROR_CODE sendMsg;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    if(res.GetRowNum() < 1)
    {
        sendMsg.errcode = ERR_PLAYER_NOT_EXIT;
        if(player)
        {
            player->SendToGate(MSG_MAIL_SEND_ACK, &sendMsg);
        }
        CnDbg("search mail receiver id failed %s\n", mail_send_req.receiver_name.c_str());
        return;
    }
    else
    {
        sendMsg.errcode = ERR_SUCCEED;
        if(player)
        {
            player->SendToGate(MSG_MAIL_SEND_ACK, &sendMsg);
        }
    }

    res.GetRecord(receiver_id);

    mail_send_req.receiver_id.id_h = U64ID_HIGH(receiver_id);
    mail_send_req.receiver_id.id_l = U64ID_LOW(receiver_id);

    SendToPlayerMailBox(receiver_id, &mail_send_req);
}

// ==================================================
// ���͵���������ص�
// ==================================================
void MailJob::CBSendToPlayerMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req)
{
    MAIL_INFO mail_info;
    uint64 receiver_id = 0;
    Player* player = 0;
    uint64 tradeOrderID = 0;

    receiver_id = U64ID(mail_send_req.receiver_id.id_h, mail_send_req.receiver_id.id_l);

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    FillMailInfo(mail_send_req, mail_info);

    // �� CenterServer ������ʼ�
    player->AddMail(mail_info);

    player->SendToGame(MSG_MAIL_INFO_NTF, &mail_info);
    player->SendToGate(MSG_MAIL_INFO_NTF, &mail_info);

    // ����Ƿ��ж�����
    tradeOrderID = U64ID(mail_send_req.trade_order_id.id_h, mail_send_req.trade_order_id.id_l);
    if(tradeOrderID)
    {
        TradeJob::Instance()->SetTradeInfoDeliverState(tradeOrderID, TRADE_STATE_DELIVER_MAIL);
    }
}

// ==================================================
// ���͵�ϵͳ����ص�
// ==================================================
void MailJob::CBSendToSystemMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req)
{
    MAIL_INFO mail_info;
    Player* player = 0;

    FillMailInfo(mail_send_req, mail_info);

    // �� CenterServer ������ʼ�
    PlayerManager::Instance()->AddSystemMail(mail_info);
    //player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    //if(player)
    //{
    //    player->AddMail(mail_info);
    //}

    GameHandler::Instance()->SendToAll(MSG_MAIL_INFO_NTF, &mail_info);
    GateHandler::Instance()->SendToAll(MSG_MAIL_INFO_NTF, &mail_info, BROADCAST_PLAYER_ID);
}

// ==================================================
// ���͵�������ص�
// ==================================================
void MailJob::CBSendToGroupMailBox(DBRecorder& res, uint64 playerID, MAIL_SEND_REQ mail_send_req)
{
    uint64 mail_id = 0;
    int32 type = 0;
    std::string condition;
    std::vector<std::string> parameter;
    Json::Reader reader;
    Json::Value json_val;

    mail_id = U64ID(mail_send_req.mail_id.id_h, mail_send_req.mail_id.id_l);

    // for test
    //mail_send_req.receiver_condition = "{\"type\":2,\"condition\":[\"player_level|>=|80\", \"create_time|>=|1400166678\"], \"parameter\":[\"Silence\"]}";

    if(!reader.parse(mail_send_req.receiver_condition, json_val, false))
    {
        // ��������ʼ�����������ֱ�ӷ���
        CnError("Parse mail:%llu failed\n", mail_id);
        return;
    }

    type = json_val["type"].asInt();

    switch(type)
    {
    case MAIL_GROUP_CREATE_PLAYER:
        {
            if(ParseGroupMailCondition(type, json_val, condition, parameter))
            {
                SendGroupMailToCreatePlayer(&mail_send_req, condition);
            }
        }
        break;
    case MAIL_GROUP_GUILD_MEMBER:
        {
            if(ParseGroupMailCondition(type, json_val, condition, parameter))
            {
                SendGroupMailToGuildMember(&mail_send_req, condition, parameter);
            }
        }
        break;
    default:
        break;
    }
}

// ==================================================
// �������ʼ�����ǰ��ɫ
// ==================================================
void MailJob::CBSendGroupMailToCreatePlayer(DBRecorder& res, MAIL_SEND_REQ mail_send_req, std::string condition)
{
    MAIL_INFO mail_info;
    Player* player = 0;
    std::string sqlTxt = "SELECT player_id FROM player WHERE" + condition;

    FillMailInfo(mail_send_req, mail_info);

    if(condition == "")
    {
        // �� CenterServer ������ʼ�
        PlayerManager::Instance()->AddSystemMail(mail_info);

        // ֪ͨGameServer�ʼ�
        PlayerManager::Instance()->SendGameMail(mail_info);

        // ֪ͨ�ͻ����ʼ�
        GateHandler::Instance()->SendToAll(MSG_MAIL_INFO_NTF, &mail_info, BROADCAST_PLAYER_ID);

        return;
    }

    // ��ѯ��ɫ�б���Ϣ
    m_dbExecutor->Query(this, &MailJob::CBSendGroupMailToCreatePlayerByCondition, mail_info, sqlTxt.c_str());
}

// ==================================================
// �������ʼ�����ɫ�б�
// ==================================================
void MailJob::CBSendGroupMailToCreatePlayerByCondition(DBRecorder& res, MAIL_INFO mail_info)
{
    std::vector<uint64> playerList;
    uint64 playerID = 0;
    Player* player = 0;

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(playerID);
        playerList.push_back(playerID);
    }

    for(std::vector<uint64>::iterator idIt = playerList.begin(); idIt != playerList.end(); ++idIt)
    {
        player = PlayerManager::Instance()->GetPlayerByPlayerID(*idIt);

        if(!player) continue;

        player->AddMail(mail_info);
        player->SendToGame(MSG_MAIL_INFO_NTF, &mail_info);
        player->SendToGate(MSG_MAIL_INFO_NTF, &mail_info);
    }
}

// ==================================================
// ������ż������ǰ�����Ա
// ==================================================
void MailJob::GetGuildMemberForGroupMail(DBRecorder& res, MAIL_SEND_REQ mail_send_req, std::string guildName, std::string condition)
{
    MAIL_INFO mail_info;
    std::stringstream ss;

    FillMailInfo(mail_send_req, mail_info);

    // ����
    ss << "SELECT T1.player_id FROM player AS T1, ";
    ss << "(SELECT guild_id from guild_info WHERE guild_name='";
    ss << guildName;
    ss << "') AS T2 ";
    ss << "WHERE T1.guild_id = T2.guild_id";

    // �ж��Ƿ�����ѡ����
    if(condition.length() > 0)
    {
        ss << " AND";
        ss << condition;
    }

    // д���ʼ���Ϣ
    m_dbExecutor->Query(this, &MailJob::NotifySendGroupMailToGuildMember, mail_info, ss.str().c_str());
}

// ==================================================
// ֪ͨ�����Ա���ʼ�
// ==================================================
void MailJob::NotifySendGroupMailToGuildMember(DBRecorder& res, MAIL_INFO mail_info)
{
    std::vector<uint64> playerList;
    uint64 playerID = 0;
    Player* player = 0;

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(playerID);
        playerList.push_back(playerID);
    }

    for(std::vector<uint64>::iterator idIt = playerList.begin(); idIt != playerList.end(); ++idIt)
    {
        player = PlayerManager::Instance()->GetPlayerByPlayerID(*idIt);

        if(!player) continue;

        player->AddMail(mail_info);
        player->SendToGame(MSG_MAIL_INFO_NTF, &mail_info);
        player->SendToGate(MSG_MAIL_INFO_NTF, &mail_info);
    }
}

// ==================================================
// ����ʼ���Ϣ
// ==================================================
void MailJob::FillMailInfo(MAIL_SEND_REQ& mail_send_req, MAIL_INFO& mail_info)
{
    mail_info.Clear();
    mail_info.mail_id.id_h = mail_send_req.mail_id.id_h;
    mail_info.mail_id.id_l = mail_send_req.mail_id.id_l;
    mail_info.receiver_id.id_h = mail_send_req.receiver_id.id_h;
    mail_info.receiver_id.id_l = mail_send_req.receiver_id.id_l;
    mail_info.sender_id.id_h = mail_send_req.sender_id.id_h;
    mail_info.sender_id.id_l = mail_send_req.sender_id.id_l;
    mail_info.sender_name = mail_send_req.sender_name;
    mail_info.send_time = mail_send_req.send_time;
    mail_info.mail_type = mail_send_req.mail_type;
    mail_info.mail_flag = mail_send_req.mail_flag;
    mail_info.title = mail_send_req.title;
    mail_info.message = mail_send_req.message;
    mail_info.attachment_flag = mail_send_req.attachment_flag;
    mail_info.attachment_list = mail_send_req.attachment_list;
}

// ==================================================
// ���ø����ʼ����
// ==================================================
void MailJob::CBSetPlayerMailFlag(DBRecorder& res)
{

}

// ==================================================
// ����ϵͳ�ʼ����
// ==================================================
void MailJob::CBSetSystemMailFlag(DBRecorder& res)
{

}

// ==================================================
// �������ʼ����
// ==================================================
void MailJob::CBSetGroupMailFlag(DBRecorder& res)
{

}

// ==================================================
// �������ʼ����ж�����
// ==================================================
bool MailJob::ParseGroupMailCondition(int32 type, Json::Value& json_val, std::string& condition, std::vector<std::string>& parameter)
{
    MailTypeKeywordMap::iterator typeIt;
    CondWordMap::iterator condIt;
    std::string one_condition;
    std::string condKey;
    std::string condOp;
    std::string condValue;
    std::stringstream ss;
    int32 count = 0;

    condition = "";

    typeIt = m_mailTypeKeywordMap.find(type);
    if(typeIt == m_mailTypeKeywordMap.end())
    {
        return false;
    }

    uint32 condition_count = json_val["condition"].size();
    for(uint32 i=0; i<condition_count; ++i)
    {
        std::string one_condition = json_val["condition"][i].asString();
        std::vector<std::string> values = Crown::SDSplitStrings(one_condition, '|');
        if(values.size() == 3)
        {
            condKey = values[0];
            condOp = values[1];
            condValue = values[2];
        }
        else
        {
            CnError("parse group mail failed\n");
            CnAssert(false);
            return false;
        }

        // ����ѯ�Ĺؼ���
        condIt = typeIt->second.find(condKey);
        if(condIt == typeIt->second.end())
        {
            CnError("parse group mail failed\n");
            CnAssert(false);
            return false;
        }

        // ����жϷ�
        if(!CheckConditionOp(condOp))
        {
            CnError("parse group mail failed\n");
            CnAssert(false);
            return false;
        }

        if(count)
        {
            ss << " AND";
        }

        ss << " ";
        ss << condIt->second;
        ss << " ";
        ss << condOp;
        ss << " ";

        if(condKey == "create_time")
        {
            ss << "FROM_UNIXTIME(";
            ss << condValue;
            ss << ")";
        }
        else
        {
            ss << condValue;
        }

        ++count;
    }

    condition = ss.str();

    uint32 parameter_count = json_val["parameter"].size();
    for(uint32 i=0; i<parameter_count; ++i)
    {
        std::string one_parameter = json_val["parameter"][i].asString();
        parameter.push_back(one_parameter);
    }

    return true;
}

// ==================================================
// �����������
// ==================================================
bool MailJob::CheckConditionOp(std::string op)
{
    std::set<std::string>::iterator opIt;

    opIt = m_mailConditionOpSet.find(op);
    if(opIt == m_mailConditionOpSet.end())
    {
        return false;
    }

    return true;
}
