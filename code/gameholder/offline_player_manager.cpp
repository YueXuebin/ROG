#include "gameholder_pch.h"
#include "offline_player_manager.h"
#include "center_handler.h"
#include "dungeon_manager.h"
#include "event_define.h"
#include "game_define.h"
#include "player_manager.h"

const int32 FreshMinuteTime = 5;      // ���ݱ�����ʱ��(����)

IMPLEMENT_SINGLETON(OfflinePlayerManager)

OfflinePlayerManager::OfflinePlayerManager()
{
    m_MinuteTimer = 0;
}

OfflinePlayerManager::~OfflinePlayerManager()
{

}

const OFFLINE_PLAYER_INFO* OfflinePlayerManager::GetOfflinePlayerRecord(uint64 offlinePlayerID)
{
    // ����������в�������
    Player* onLinePlayer = PlayerManager::Instance()->GetPlayerByPlayerID(offlinePlayerID);
    if(onLinePlayer && onLinePlayer->GetLevel() > 1)
    {
        OFFLINE_PLAYER_RECORD record;
        onLinePlayer->FillOffilePlayerInfo(record.record);
        record.recordTime = FreshMinuteTime;
        m_OfflinePlayerRecords[offlinePlayerID] = record;           // ���������������
        return &m_OfflinePlayerRecords[offlinePlayerID].record;
    }

    // ���б��в�ѯ
    if(m_OfflinePlayerRecords.find(offlinePlayerID) != m_OfflinePlayerRecords.end())
    {
        return &m_OfflinePlayerRecords[offlinePlayerID].record;
    }

    return NULL;
}

const OFFLINE_PLAYER_INFO* OfflinePlayerManager::GetOfflinePlayerRecordByName(const std::string& offlinePlayerName)
{
    Player* onLinePlayer = PlayerManager::Instance()->GetPlayerByName(offlinePlayerName);
    if(onLinePlayer && onLinePlayer->GetLevel() > 1)
    {
        OFFLINE_PLAYER_RECORD record;
        onLinePlayer->FillOffilePlayerInfo(record.record);
        record.recordTime = FreshMinuteTime;
        m_OfflinePlayerRecords[onLinePlayer->GetPlayerID()] = record;           // ���������������
        return &m_OfflinePlayerRecords[onLinePlayer->GetPlayerID()].record;
    }

    // ���б��в�ѯ
    for(std::map<uint64, OFFLINE_PLAYER_RECORD>::iterator iter = m_OfflinePlayerRecords.begin(); iter != m_OfflinePlayerRecords.end(); iter++)
    {
        OFFLINE_PLAYER_RECORD& record = iter->second;
        if(record.record.name == offlinePlayerName)
        {
            return &record.record;
        }
    }

    return NULL;
}
// ��CenterServer�����������ҵ�����
void OfflinePlayerManager::AskCSOfflinePlayerInfo(uint64 offlinePlayerID, uint8 reason)
{
    OFFLINE_PLAYER_INFO_REQ req;
    req.reason = reason;
    
    req.player_id_h = U64ID_HIGH(offlinePlayerID);
    req.player_id_l = U64ID_LOW(offlinePlayerID);

    CenterHandler::Instance()->SendToMasterCenter(MSG_OFFLINE_PLAYER_INFO_REQ, &req);
}

void OfflinePlayerManager::AskCSOfflinePlayerInfoByName(const std::string& offlinePlayerName, uint8 reason)
{
    OFFLINE_PLAYER_INFO_REQ req;
    req.reason = reason;
    req.player_name = offlinePlayerName;

    CenterHandler::Instance()->SendToMasterCenter(MSG_OFFLINE_PLAYER_INFO_REQ, &req);
}

void OfflinePlayerManager::CBRequestOfflinePlayerInfo(const OFFLINE_PLAYER_ACK& OfflinePlayerAck)
{
    uint64 offlinePlayerID = U64ID(OfflinePlayerAck.offline_player_info.player_id_h, OfflinePlayerAck.offline_player_info.player_id_l);

    OFFLINE_PLAYER_RECORD _record;
    _record.record = OfflinePlayerAck.offline_player_info;
    _record.recordTime = FreshMinuteTime;

    m_OfflinePlayerRecords[offlinePlayerID] = _record;

    OnSendToRequestPlayer(offlinePlayerID, OfflinePlayerAck.reason);         // ����δ��ɵ�����
    OnLoadOfflinePlayerInfo(offlinePlayerID);
}

void OfflinePlayerManager::RequestOfflinePlayerInfoByID(uint64 offlinePlayerID, uint8 reason, uint32 msgID, Player* player)
{
    const OFFLINE_PLAYER_INFO* offlinePlayer = GetOfflinePlayerRecord(offlinePlayerID);
    // ������ұ���û�ҵ�����������
    if(offlinePlayer == NULL)
    {
        REQUEST_RECORD request_record;
        request_record.AskPlayerID = player->GetPlayerID();
        request_record.offlinePlayerID = offlinePlayerID;
        request_record.Ask_MessageID = msgID;
        m_offlineReqList.push_back(request_record);
        AskCSOfflinePlayerInfo(offlinePlayerID, reason);
    }
    else
    {
        OFFLINE_PLAYER_ACK sendMsg;
        sendMsg.reason = reason;
        sendMsg.offline_player_info = *offlinePlayer;
        player->SendToGate(MSG_OFFLINE_PLAYER_CLIENT_ACK, &sendMsg);
    }
}

void OfflinePlayerManager::RequestOfflinePlayerInfoByName(const std::string& offlinePlayerName, uint8 reason, uint32 msgID, Player* player)
{
    const OFFLINE_PLAYER_INFO* offlinePlayer = GetOfflinePlayerRecordByName(offlinePlayerName);
    // ������ұ���û�ҵ�����������
    if(offlinePlayer == NULL)
    {
        REQUEST_RECORD request_record;
        request_record.AskPlayerID = player->GetPlayerID();
        request_record.offlinePlayerName = offlinePlayerName;
        request_record.Ask_MessageID = msgID;
        m_offlineReqList.push_back(request_record);
        AskCSOfflinePlayerInfoByName(offlinePlayerName, reason);
    }
    else
    {
        OFFLINE_PLAYER_ACK sendMsg;
        sendMsg.reason = reason;
        sendMsg.offline_player_info = *offlinePlayer;
        player->SendToGate(MSG_OFFLINE_PLAYER_CLIENT_ACK, &sendMsg);
    }
}

void OfflinePlayerManager::OnLoadOfflinePlayerInfo( uint64 offlinePlayerID )
{
    // �����г������¼�
    DungeonManager::Instance()->SendEvent(SYSTEM_EVENT_LOAD_OFFLINE_PLAYER, U64ID_HIGH(offlinePlayerID), U64ID_LOW(offlinePlayerID));
}

void OfflinePlayerManager::OnSendToRequestPlayer(uint64 offlinePlayerID, uint8 reason)
{
    // ���͸���Ҫ�Ŀͻ���
    OFFLINE_PLAYER_ACK offline_player_ack;
    offline_player_ack.reason = reason;
    const OFFLINE_PLAYER_INFO* offlinePlayer = GetOfflinePlayerRecord(offlinePlayerID);
    CnAssert(offlinePlayer);

    for(std::vector<REQUEST_RECORD>::iterator iter = m_offlineReqList.begin(); iter != m_offlineReqList.end();)
    {
        REQUEST_RECORD& reqRecord = *iter;

        bool isfind = false;

        CnAssert(offlinePlayerID);
        if(reqRecord.offlinePlayerID == offlinePlayerID)
        {
            CnAssert(reqRecord.offlinePlayerName.length() == 0);
            isfind = true;
        }
        else if(reqRecord.offlinePlayerName == offlinePlayer->name)
        {
            CnAssert(reqRecord.offlinePlayerID == 0);
            isfind = true;
        }

        if(isfind)
        {
            CnAssert(reqRecord.AskPlayerID != 0);

            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(reqRecord.AskPlayerID);
            if(player)
            {
                if(iter->Ask_MessageID == MSG_OFFLINE_PLAYER_CLIENT_REQ)
                {
                    offline_player_ack.offline_player_info = *offlinePlayer;
                    player->SendToGate(MSG_OFFLINE_PLAYER_CLIENT_ACK, &offline_player_ack); 
                }
                else
                    CnWarn("unvalid request offline player by msgId = %d\n", iter->Ask_MessageID);
            }

            iter = m_offlineReqList.erase(iter);
        }
        else
            iter++;
    }
}

void OfflinePlayerManager::Update()
{
    m_MinuteTimer += g_ServerUpdateTime;
    if(m_MinuteTimer >= 60000)
    {   // ÿ���Ӹ���һ��
        m_MinuteTimer = 0;
        RefreshPlayerRecord();
    }
}

void OfflinePlayerManager::RefreshPlayerRecord()
{
    for(std::map<uint64, OFFLINE_PLAYER_RECORD>::iterator iter = m_OfflinePlayerRecords.begin(); iter != m_OfflinePlayerRecords.end();)
    {
        iter->second.recordTime--;

        if(iter->second.recordTime <= 0)
        {
            std::map<uint64, OFFLINE_PLAYER_RECORD>::iterator delIter = iter;
            iter++;
            m_OfflinePlayerRecords.erase(delIter);
        }
        else
            iter++;
    }
}




