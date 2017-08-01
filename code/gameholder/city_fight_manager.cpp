#include "gameholder_pch.h"
#include "city_fight_manager.h"
#include "player_manager.h"
#include "guild_manager.h"
#include "guild_config.h"
#include "activity_manager.h"
#include "player.h"
#include "dungeon_manager.h"
#include "dungeon_set.h"
#include "item_manager.h"
#include "center_handler.h"
#include "server_config.h"
#include "game_util.h"
#include "mail_manager.h"

IMPLEMENT_SINGLETON(CityFightManager)

// ==================================================
// ���캯��
// ==================================================
CityFightManager::CityFightManager()
{
    Clear();
}

// ==================================================
// ��������
// ==================================================
CityFightManager::~CityFightManager()
{
}

// ==================================================
// �������
// ==================================================
void CityFightManager::Clear()
{
    m_score_recorder.clear();
    ResetCityState();
    m_cur_time = 0;
    m_ntf_time = 0;
    m_time_score_time = 0;
    m_city_info_modify = false;
    m_city_info_need_save = false;
    m_open = false;
    m_activity_server = false;
}

// ==================================================
// Э�����client
// ==================================================
void CityFightManager::OnRecv(uint32 msgID, void* data, Player* player)
{
    switch (msgID)
    {
        case MSG_CITY_FIGHT_INFO_REQ:
            {
                OnCityFightInfoReq(data, player);
                break;
            }
        case MSG_CITY_FIGHT_ENTER_REQ:
            {
               OnCityFightEnterReq(data, player);
                break;
            }
        case MSG_CITY_FIGHT_REWARD_REQ:
            {
                OnCityFightRewardReq(data, player);
                break;
            }
        default:
            break;
    }
}

// ==================================================
// Э�����centerserver
// ==================================================
void CityFightManager::OnCSRecv( uint64 playerID, uint32 msgID, void* data )
{
    switch (msgID)
    {
    case MSG_CITY_FIGHT_INFO_NTF_ACK:
        {
            OnCityFightInfoNtfAck(data);
            break;
        }
    case MSG_CITY_FIGHT_START_NTF:
        {
            OnCityFightStart(data);
            break;
        }
    case MSG_CITY_FIGHT_END_NTF:
        {
            OnCityFightEnd(data);
            break;
        }
    default:
        break;
    }

}

// ==================================================
// ��cs���ͳ���״̬�㲥
// ==================================================
void CityFightManager::SendCityFightInfoNtf()
{
    CITY_FIGHT_GUILD_SCORE guildScore;
    CITY_FIGHT_ONE_CITY_NTF oneCityNtf;
    CITY_FIGHT_INFO_NTF ntf;

    // �������
    ntf.open = m_open;

    // �浵���
    ntf.save_in_db = m_city_info_need_save;
    m_city_info_need_save = false;

    CityOccupyInfoMap::const_iterator cityOccupyIt = m_city_occupy_info.begin();
    CityScoreRecorderMap::iterator cityIt;
    for (; cityOccupyIt != m_city_occupy_info.end(); cityOccupyIt++)
    {
        oneCityNtf.Clear();
        oneCityNtf.city_id = cityOccupyIt->second.cityID;
        oneCityNtf.guild_name = cityOccupyIt->second.guildName;
        oneCityNtf.flag = cityOccupyIt->second.state;
        oneCityNtf.timestamp = cityOccupyIt->second.timeStamp;

        // ��ó��е÷����
        cityIt = m_score_recorder.find(cityOccupyIt->second.cityID);
        if (cityIt != m_score_recorder.end())
        {
            // ȡ�������
            std::list<GuildScoreRecorder*>::const_iterator scoreIt = cityIt->second.scoreList.begin();
            for (; scoreIt != cityIt->second.scoreList.end(); ++scoreIt)
            {
                guildScore.Clear();
                guildScore.guild_name = (*scoreIt)->guildName;
                guildScore.score = (*scoreIt)->score;
                oneCityNtf.guild_list.push_back(guildScore);
            }
        }

        ntf.city_list.push_back(oneCityNtf);
    }

    CenterHandler::Instance()->SendToMasterCenter(MSG_CITY_FIGHT_INFO_NTF, &ntf);
}

// ==================================================
// ��cs�������״̬�㲥
// ==================================================
void CityFightManager::SendCityFightInfoNtfReq()
{
    // ������������Ϻ�,��cs������
    NONE sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_CITY_FIGHT_INFO_NTF_REQ, &sendMsg);
}

// ==================================================
// csͬ�������ĳ���״̬
// ==================================================
void CityFightManager::OnCityFightInfoNtfAck( void* data )
{
    CITY_FIGHT_INFO_NTF* info = (CITY_FIGHT_INFO_NTF*) data;
    if (!info)
        return;

    // ����״̬�ı�
    if ((bool)info->open != m_open)
    {
        NONE ntf;
        m_open = (bool)info->open;
        if (m_open)
        {
            PlayerManager::Instance()->SendToAll(MSG_CITY_FIGHT_REALLY_START_NTF, &ntf);
        }
        else
        {
            PlayerManager::Instance()->SendToAll(MSG_CITY_FIGHT_REALLY_END_NTF, &ntf);
        }
    }
    
    CITY_FIGHT_ONE_CITY_NTF* cityNtf = NULL;
    CITY_FIGHT_GUILD_SCORE* guildScore = NULL;
    CityOccupyInfoMap::iterator cityOccupyIter;
    CityOccupyInfo cityOccupyInfo;
    CityScoreRecorderMap::iterator cityIt;
    GuildScoreRecorder guildScoreRecorder;
    for (uint32 i = 0; i < info->city_list.size(); ++i)
    {
        cityNtf = &info->city_list[i];
        if (!cityNtf)
            continue;

        // ����ռ����Ϣ
        cityOccupyIter = m_city_occupy_info.find(cityNtf->city_id);
        if (cityOccupyIter != m_city_occupy_info.end())
        {
            cityOccupyIter->second.guildName = cityNtf->guild_name;
            cityOccupyIter->second.state = cityNtf->flag;
            cityOccupyIter->second.timeStamp = cityNtf->timestamp;
        }
        else
        {
            cityOccupyInfo.Clear();
            cityOccupyInfo.cityID = cityNtf->city_id;
            cityOccupyInfo.state = cityNtf->flag;
            cityOccupyInfo.guildName = cityNtf->guild_name;
            cityOccupyInfo.timeStamp = cityNtf->timestamp;

            m_city_occupy_info.insert(std::make_pair(cityOccupyInfo.cityID, cityOccupyInfo));
        }

        // ���������Ϣ
        cityIt = m_score_recorder.find(cityNtf->city_id);
        if (cityIt == m_score_recorder.end())
        {
            CityScoreRecorder cityScoreRecorder;
            cityScoreRecorder.Clear();
            cityScoreRecorder.cityID = cityNtf->city_id;
            m_score_recorder[cityNtf->city_id] = cityScoreRecorder;

            cityIt = m_score_recorder.find(cityNtf->city_id);
        }

        cityIt->second.Clear();
        for (uint32 i = 0; i < cityNtf->guild_list.size(); ++i)
        {
            guildScore = &cityNtf->guild_list[i];
            GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildScore->guild_name);
            if (!guildInfo)
                continue;

            guildScoreRecorder.Clear();
            guildScoreRecorder.guildName = guildScore->guild_name;
            guildScoreRecorder.guildID = U64ID(guildInfo->guild_id_h, guildInfo->guild_id_l);
            guildScoreRecorder.score = guildScore->score;

            cityIt->second.scoreMap[guildScoreRecorder.guildID] = guildScoreRecorder;
            cityIt->second.scoreList.push_back(&cityIt->second.scoreMap[guildScoreRecorder.guildID]);
        }
    }
}

// ==================================================
// ��ʼ
// ==================================================
void CityFightManager::OnCityFightStart( void* data )
{
    // �����֮ǰ����
    Clear();

    // �����Ƿ����ڷ�����
    m_activity_server = IsActivityServer();
    if (m_activity_server)
    {
        // ���ó��п�����
        CityOccupyInfoMap::iterator it = m_city_occupy_info.begin();
        for (; it != m_city_occupy_info.end(); it ++)
        {
            it->second.state = CITY_FIGHT_FLAG_COMBAT;
        }

        // ֪ͨcs��ռ����Ϣ�浵
        m_city_info_need_save = true;
        // �����б��
        m_city_info_modify = true;
        // ��0,��֤��һupdateͬ����Ϣ
        m_ntf_time = 0;
        // �ǻ���ڷ�����,���������
        // �ǻ���ڷ�����,������ݵ�һ�ι㲥�������ٿ���
        m_open = true;
        
        NONE ntf;
        PlayerManager::Instance()->SendToAll(MSG_CITY_FIGHT_REALLY_START_NTF, &ntf);
    }
}

// ==================================================
// ����
// ==================================================
void CityFightManager::OnCityFightEnd( void* data )
{
    // ֻ�л���ڵ�gs�Ž��н���,����gsͨ��csͬ����Ϣ
    if (!m_activity_server)
        return;

    // ��������CityScoreRecorderMap
    CityOccupyInfoMap::iterator occupyIter = m_city_occupy_info.begin();
    for (; occupyIter != m_city_occupy_info.end(); occupyIter++)
    {
        // ս����,����δ��ռ��
        if (occupyIter->second.state == CITY_FIGHT_FLAG_COMBAT)
        {
            // ����Ϊδռ��
            occupyIter->second.state = CITY_FIGHT_FLAG_UNOCCUPY;

            CityScoreRecorderMap::iterator cityIt = m_score_recorder.find(occupyIter->second.cityID);
            if (cityIt == m_score_recorder.end())
                continue;

            if (cityIt->second.scoreList.size() == 1)
            {
                // ֻ��һ������
                std::list<GuildScoreRecorder*>::iterator firstScoreIt = cityIt->second.scoreList.begin();
                occupyIter->second.state = CITY_FIGHT_FLAG_OCCUPY;
                occupyIter->second.guildName = (*firstScoreIt)->guildName;
                occupyIter->second.timeStamp = (uint32)Crown::SDNow().GetTimeValue();

                // ����ȫ������
                SendMarquee(occupyIter->second.guildName, occupyIter->second.cityID);

                // ���͹����ʼ�
                SendMailToGuild(occupyIter->second.guildName, occupyIter->second.cityID);
            }
            else if (cityIt->second.scoreList.size() > 1)
            {
                // �������
                cityIt->second.scoreList.sort(ScoreCompareRules);
                std::list<GuildScoreRecorder*>::iterator firstScoreIt = cityIt->second.scoreList.begin();
                std::list<GuildScoreRecorder*>::iterator secondScoreIt = firstScoreIt;
                secondScoreIt++;
                if ((*firstScoreIt)->score > (*secondScoreIt)->score)
                {
                    occupyIter->second.state = CITY_FIGHT_FLAG_OCCUPY;
                    occupyIter->second.guildName = (*firstScoreIt)->guildName;
                    occupyIter->second.timeStamp = (uint32)Crown::SDNow().GetTimeValue();

                    // ����ȫ������
                    SendMarquee(occupyIter->second.guildName, occupyIter->second.cityID);

                    // ���͹����ʼ�
                    SendMailToGuild(occupyIter->second.guildName, occupyIter->second.cityID);
                }
            }

            // �رճ���
            CloseCity(occupyIter->second.cityID);
        }
    }

    // ֪ͨcs��ռ����Ϣ�浵
    m_city_info_need_save = true;
    // �����б��
    m_city_info_modify = true;
    // ��0,��֤��һupdateͬ����Ϣ
    m_ntf_time = 0;
    // �ر�
    m_open = false;

    NONE ntf;
    PlayerManager::Instance()->SendToAll(MSG_CITY_FIGHT_REALLY_END_NTF, &ntf);
}

// ==================================================
// ������Ϣ����
// ==================================================
void CityFightManager::OnCityFightInfoReq(void* data, Player* player)
{
    if (!player)
        return;

    CITY_FIGHT_ALL_CITY_INFO sendMsg;
    sendMsg.open = m_open;

    CITY_FIGHT_CITY_INFO fightCityInfo;
    CITY_FIGHT_GUILD_SCORE guildScore;

    CityOccupyInfoMap::const_iterator cityOccupyIt = m_city_occupy_info.begin();
    CityScoreRecorderMap::iterator cityIt;
    for (; cityOccupyIt != m_city_occupy_info.end(); cityOccupyIt++)
    {
        fightCityInfo.Clear();
        fightCityInfo.city_id = cityOccupyIt->second.cityID;
        fightCityInfo.guild_name = cityOccupyIt->second.guildName;
        fightCityInfo.flag = cityOccupyIt->second.state;
        fightCityInfo.reward_flag = NO_REWARD;

        // �����콱���
        if (CheckRewardCondition(player, cityOccupyIt->second.cityID) == ERR_SUCCEED)
        {
            fightCityInfo.reward_flag = CAN_ACCEPT_REWARD;
        }
        else if ((cityOccupyIt->second.timeStamp > 0) && (player->GetCityFightRewardTimeStamp(cityOccupyIt->second.cityID) >= cityOccupyIt->second.timeStamp))
        {
            fightCityInfo.reward_flag = ACCEPTED_REWARD;
        }

        // ��ó��е÷����
        cityIt = m_score_recorder.find(cityOccupyIt->second.cityID);
        if (cityIt == m_score_recorder.end())
        {
            // �ó��л�û�е÷����
            sendMsg.city_list.push_back(fightCityInfo);
            continue;
        }

        fightCityInfo.total_guild_num = cityIt->second.scoreList.size();

        // ȡtop10����
        std::string playerGuildName = player->GetGuildName();
        bool playerGuildInTopTen = false;
        cityIt->second.scoreList.sort(ScoreCompareRules);
        std::list<GuildScoreRecorder*>::const_iterator scoreIt = cityIt->second.scoreList.begin();
        for (uint32 i = 0; (i < 10) && (scoreIt != cityIt->second.scoreList.end()); ++scoreIt, ++i)
        {
            guildScore.Clear();
            guildScore.guild_name = (*scoreIt)->guildName;
            guildScore.score = (*scoreIt)->score;
            fightCityInfo.rank_list.push_back(guildScore);

            if (guildScore.guild_name == playerGuildName)
            {
                playerGuildInTopTen = true;
            }
        }

        if (!playerGuildInTopTen)
        {
            // ������ڹ��᲻��top10,������������
            for (; scoreIt != cityIt->second.scoreList.end(); ++scoreIt)
            {
                if ((*scoreIt)->guildName == playerGuildName)
                {
                    guildScore.Clear();
                    guildScore.guild_name = (*scoreIt)->guildName;
                    guildScore.score = (*scoreIt)->score;
                    fightCityInfo.rank_list.push_back(guildScore);
                }
            }
        }

        sendMsg.city_list.push_back(fightCityInfo);
    }

    player->SendToGate(MSG_CITY_FIGHT_INFO_ACK, &sendMsg);
}

// ==================================================
// ��������ս��������
// ==================================================
void CityFightManager::OnCityFightEnterReq(void* data, Player* player)
{
    // ������
    CHANGE_DUNGEON_REQ changeDungeonMsg;
    uint16 errCode = ERR_SUCCEED;

    if (!player)
        return;

    // ��������ս��������ȴʱ��
    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(player->GetCityFightCD() > nowTime)
    {
        player->NotifyErrorCode(ERR_CITY_FIGHT_DUNGEON_CD);
        return;
    }
    player->SetCityFightCD(nowTime + 60);

    CITY_FIGHT_ENTER_REQ* recvMsg = (CITY_FIGHT_ENTER_REQ*)data;
    if (!recvMsg) 
        return;

    errCode = player->CheckEnterActivity(ACTIVITY_TYPE_CITY_FIGHT);
    if(errCode != ERR_SUCCEED)
    {
        player->NotifyErrorCode(errCode);
        return;
    }

    // ����ɫ��������
    errCode = CheckGuildCondition(player);
    if (errCode != ERR_SUCCEED)
    {
        SendCityFightEnterAck(player, errCode, recvMsg->city_id);
        return;
    }

    // �жϳ���״̬
    uint16 cityState = GetCityState(recvMsg->city_id);
    if (cityState != CITY_FIGHT_FLAG_COMBAT)
    {
        // ����δ����
        if (cityState == CITY_FIGHT_FLAG_UNOCCUPY)
        {
            SendCityFightEnterAck(player, ERR_CITY_FIGHT_NO_START, recvMsg->city_id);
            return;
        }

        // ������ռ��
        if (cityState == CITY_FIGHT_FLAG_OCCUPY)
        {
            SendCityFightEnterAck(player, ERR_CITY_FIGHT_OCCUPY, recvMsg->city_id);
            return;
        }

        // δ֪״̬
        SendCityFightEnterAck(player, ERR_FAILED, recvMsg->city_id);
        return;
    }

    // ��ȡ������Ϣ
    const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(recvMsg->city_id);
    if(!cityConfig)
    {
        SendCityFightEnterAck(player, ERR_CHANGE_DUNGEON_CANT_JUMP, recvMsg->city_id);
        return;
    }

    changeDungeonMsg.dungeon_id = cityConfig->dungeon_id;
    player->OnRecv(MSG_CHANGE_DUNGEON_REQ, &changeDungeonMsg);

    SendCityFightEnterAck(player, ERR_SUCCEED, recvMsg->city_id);
}

void CityFightManager::SendCityFightEnterAck(Player* player, uint16 errCode, uint32 cityId)
{
    CITY_FIGHT_ENTER_ACK ack;
    ack.err_code = errCode;
    ack.city_id = cityId;

    player->SendToGate(MSG_CITY_FIGHT_ENTER_ACK, &ack);
}

// ==================================================
// ��������ս��������
// ==================================================
void CityFightManager::OnCityFightRewardReq(void* data, Player* player)
{
    if (!player)
        return;

    CITY_FIGHT_REWARD_REQ* recvMsg = (CITY_FIGHT_REWARD_REQ*)data;
    if (!recvMsg)
        return;

    // ����ɫ��������
    uint16 guildCheckCode = CheckGuildCondition(player);
    if (guildCheckCode != ERR_SUCCEED)
    {
        SendCityFightRewardAck(player, guildCheckCode, recvMsg->city_id);
        return;
    }

    // ��齱������
    uint16 rewardCheckCode = CheckRewardCondition(player, recvMsg->city_id);
    if (rewardCheckCode != ERR_SUCCEED)
    {
        SendCityFightRewardAck(player, rewardCheckCode, recvMsg->city_id);
        return;
    }

    // ������
    const CityFightWarCityConfig*  cityConfig = GuildConfig::Instance()->GetCityFightWarCity(recvMsg->city_id);
    if (!cityConfig)
    {
        SendCityFightRewardAck(player, ERR_FAILED, recvMsg->city_id);
        return;
    }
    //ItemManager::AddRewardTableWithEmail(player->GetCenterSvrID(), player->GetPlayerID(), cityConfig->reward_table);

    // �����콱ʱ��
    time_t nowTime = Crown::SDNow().GetTimeValue();
    player->SetCityFightRewardTimeStamp(recvMsg->city_id, (uint32)nowTime);

    // Ӧ��
    SendCityFightRewardAck(player, ERR_SUCCEED, recvMsg->city_id);
}

void CityFightManager::SendCityFightRewardAck( Player* player, uint16 errCode, uint32 cityId )
{
    CITY_FIGHT_REWARD_ACK ack;
    ack.city_id = cityId;
    ack.err_code = errCode;

    player->SendToGate(MSG_CITY_FIGHT_REWARD_ACK, &ack);
}

void CityFightManager::RecordGuildScore(uint32 cityID, uint64 guildID, uint32 score)
{
    if ((!cityID) || (!guildID) || (score < 0))
        return;

    // �жϳ���״̬
    uint16 cityState = GetCityState(cityID);
    if (cityState != CITY_FIGHT_FLAG_COMBAT)
        return;

    // �����б��
    m_city_info_modify = true;

    CityScoreRecorderMap::iterator cityIt = m_score_recorder.find(cityID);
    if (cityIt == m_score_recorder.end())
    {
        CityScoreRecorder cityScoreRecorder;
        cityScoreRecorder.Clear();
        cityScoreRecorder.cityID = cityID;
        m_score_recorder[cityID] = cityScoreRecorder;

        cityIt = m_score_recorder.find(cityID);
    }

    std::map<uint64, GuildScoreRecorder>::iterator guildIt = cityIt->second.scoreMap.find(guildID);
    if (guildIt != cityIt->second.scoreMap.end())
    {
        guildIt->second.score += score;
        if (guildIt->second.score >= GuildConfig::Instance()->GetCityFightWinScore())
        {
            SetCityState(cityIt->second.cityID, CITY_FIGHT_FLAG_OCCUPY, guildIt->second.guildName, (uint32)Crown::SDNow().GetTimeValue());

            //֪ͨcs��ռ����Ϣ�浵
            m_city_info_need_save = true;

            // �رճ���
            CloseCity(cityIt->second.cityID);

            // ����ȫ������
            SendMarquee(guildIt->second.guildName, cityIt->second.cityID);

            // ���͹����ʼ�
            SendMailToGuild(guildIt->second.guildName, cityIt->second.cityID);
        }
        return;
    }

    GuildScoreRecorder guildScoreRecorder;
    guildScoreRecorder.Clear();
    guildScoreRecorder.guildID = guildID;
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if (guildInfo)
    {
        guildScoreRecorder.guildName = guildInfo->guild_name;
    }
    guildScoreRecorder.score = score;
    cityIt->second.scoreMap[guildID] = guildScoreRecorder;
    cityIt->second.scoreList.push_back(&cityIt->second.scoreMap[guildID]);
}

// ==================================================
// ��������սǰ10�����б�
// ==================================================
void CityFightManager::GetCityFightRankList(uint32 cityID, GUILD_DAMAGE_RANK_LIST& list)
{
    list.Clear();

    CityScoreRecorderMap::iterator cityIt = m_score_recorder.find(cityID);
    if (cityIt == m_score_recorder.end())
        return;

    GUILD_BOSS_DAMAGE_INFO damageInfo;
    cityIt->second.scoreList.sort(ScoreCompareRules);
    std::list<GuildScoreRecorder*>::iterator scoreIt = cityIt->second.scoreList.begin();
    for (uint32 i = 0; (i < 10) && (scoreIt != cityIt->second.scoreList.end()); ++scoreIt, ++i)
    {
        damageInfo.Clear();
        damageInfo.rank = i + 1;
        damageInfo.name = (*scoreIt)->guildName;
        damageInfo.damage = (*scoreIt)->score;
        damageInfo.level = 0;
        list.damage_list.push_back(damageInfo);
    }

}

uint16 CityFightManager::CheckGuildCondition( Player* player )
{
    // ����ɫ�Ƿ���ڹ���
    if (!player->GetGuildID())
    {
        return ERR_HAVENT_GUILD;
    }

    // ��ȡ����
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(player->GetGuildID());
    if (!guildInfo)
    {
        return ERR_UNFOUND_GUILD;
    }

    // ��ȡ����ĵȼ�ģ��
    const GuildLevelTmpl* guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildInfo->level);
    if (!guildLevelTmpl)
    {
        return ERR_FAILED;
    }

    // ����Ƿ��ѿ����ù���
    if (!guildLevelTmpl->HasActivity(GUILD_ACTIVITY_CITY_FIGHT))
    {
        return ERR_FAILED;
    }

    return ERR_SUCCEED;
}

uint16 CityFightManager::CheckRewardCondition( Player* player, uint32 cityId )
{
    CityOccupyInfoMap::iterator cityIter = m_city_occupy_info.find(cityId);
    if (cityIter == m_city_occupy_info.end())
        return ERR_FAILED;

    // �жϳ���״̬
    if (cityIter->second.state != CITY_FIGHT_FLAG_OCCUPY)
        return ERR_FAILED;

    // ռ��Ĺ��᲻����ҹ���
    if (cityIter->second.guildName != player->GetGuildName())
        return ERR_FAILED;

    uint32 lastAcceptTime = player->GetCityFightRewardTimeStamp(cityId);
    time_t nowTime = Crown::SDNow().GetTimeValue();
    // ͬһ�����������
    if (0 == GetDaysBetweenDates(nowTime, lastAcceptTime))
    {
        return ERR_FAILED;
    }

    return ERR_SUCCEED;
}

void CityFightManager::SetCityState( uint32 cityId, int32 state, std::string guildName, uint32 timeStamp )
{
    CityOccupyInfoMap::iterator it = m_city_occupy_info.find(cityId);
    if (it != m_city_occupy_info.end())
    {
        it->second.guildName = guildName;
        it->second.state = state;
        it->second.timeStamp = timeStamp;
    }
}

int32 CityFightManager::GetCityState( uint32 cityId )
{
    CityOccupyInfoMap::iterator it = m_city_occupy_info.find(cityId);
    if (it != m_city_occupy_info.end())
    {
        return it->second.state;
    }

    return 0;
}

void CityFightManager::ResetCityState()
{
    m_city_occupy_info.clear();

    CityOccupyInfo cityOccupyInfo;
    const CityFightWarCityMap* cityMap = GuildConfig::Instance()->GetCityFightCityMap();
    CityFightWarCityMap::const_iterator it = cityMap->begin();
    for (; it != cityMap->end(); it++)
    {
        cityOccupyInfo.Clear();
        cityOccupyInfo.cityID = it->second.city_id;
        cityOccupyInfo.state = CITY_FIGHT_FLAG_UNOCCUPY;

        m_city_occupy_info.insert(std::make_pair(cityOccupyInfo.cityID, cityOccupyInfo));
    }
}

void CityFightManager::Update()
{
    m_cur_time += g_ServerUpdateTime;

    //�ǻ���ڷ�����
    if (!m_activity_server)
        return;

    // �����б��,�Ҵﵽʱ���,��㲥��ǰ��Ϣ
    if ((m_city_info_modify) && (m_cur_time - m_ntf_time > 1000))
    {
        CnDbg("CityFightManager::Update %d %d\n", m_cur_time, m_ntf_time);

        m_city_info_modify = false;
        m_ntf_time = m_cur_time;

        SendCityFightInfoNtf();
    }

    // �ǿ�����
    if (!m_open)
        return;

    // ÿ��30���5��
    if (m_cur_time - m_time_score_time > 30000)
    {
        m_time_score_time = m_cur_time;
        AddTimeScore();
    }
}

void CityFightManager::CloseCity( uint32 cityId )
{
    // ��ȡ������Ϣ
    const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(cityId);
    if(!cityConfig)
        return;

    DungeonManager::Instance()->StopAllRegular(cityConfig->dungeon_id);

    // �����г��ж��������,��رջ
    CityOccupyInfoMap::iterator occupyIter = m_city_occupy_info.begin();
    for (; occupyIter != m_city_occupy_info.end(); occupyIter++)
    {
        if (occupyIter->second.state == CITY_FIGHT_FLAG_COMBAT)
        {
            // ���г�����������
            return;
        }
    }

    // ���г��ж��������,�رջ
    ActivityManager::Instance()->CloseActivityByType(ACTIVITY_TYPE_CITY_FIGHT);
}

std::string CityFightManager::GetCityOccupyGuild( uint32 cityID )
{
    CityOccupyInfoMap::iterator it = m_city_occupy_info.find(cityID);
    if (it == m_city_occupy_info.end())
        return "";

    if (it->second.state != CITY_FIGHT_FLAG_OCCUPY)
        return "";

    return it->second.guildName;
}

uint32 CityFightManager::GetCityOccupyTimeStamp( uint32 cityID )
{
    CityOccupyInfoMap::iterator it = m_city_occupy_info.find(cityID);
    if (it == m_city_occupy_info.end())
        return 0;

    if (it->second.state != CITY_FIGHT_FLAG_OCCUPY)
        return 0;

    return it->second.timeStamp;
}

uint32 CityFightManager::GetGuildScore( uint32 cityID, uint64 guildID )
{
    CityScoreRecorderMap::iterator cityIt = m_score_recorder.find(cityID);
    if (cityIt == m_score_recorder.end())
        return 0;

    std::map<uint64, GuildScoreRecorder>::const_iterator guildScoreIt = cityIt->second.scoreMap.find(guildID);
    if (guildScoreIt == cityIt->second.scoreMap.end())
        return 0;

    return guildScoreIt->second.score;
}

void CityFightManager::OnServerReady()
{
    SendCityFightInfoNtfReq();
}

bool CityFightManager::IsActivityServer()
{
    // ��ȡ������Ϣ
    const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(1);
    if(!cityConfig)
    {
        return false;
    }

    return DungeonManager::Instance()->GetServerIdByDungeonId(cityConfig->dungeon_id) == ServerConfig::Instance()->GetMyID();
}

void CityFightManager::SendMarquee(std::string guildName, uint32 cityId)
{
    char cityIdString[100];
    Crown::SDItoa(cityIdString, 100, cityId);

    SERVER_MARQUEE sendMsg;
    sendMsg.id = MARQUEE_CITY_FIGHT_OCCUPY_CITY;
    sendMsg.parame_list.push_back(guildName);
    sendMsg.parame_list.push_back(cityIdString);

    Player::OnMarQuee(sendMsg);
}

void CityFightManager::SendMailToGuild( std::string guildName, uint32 cityId )
{
    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildName);
    if (!guildInfo)
        return;

    char buff[100];
    memset(buff, 0, 100);
    Crown::SDItoa(buff, 100, cityId);

    std::string cityIdStr = Crown::ToString(cityId);
    std::vector<REWARD_ITEM> list;
    list.clear();

    for(std::vector<GUILD_MEMBER>::iterator memberIt = guildInfo->member_list.begin(); 
        memberIt != guildInfo->member_list.end(); ++memberIt)
    {
        MailManager::SendMessageMail(guildInfo->center_server_id, 0, memberIt->name, list, MAIL_MESSAGE_TYPE_CITY_FIGHT, 1, buff);
    }
}

void CityFightManager::AddTimeScore()
{
    MESSAGE_NTF message;
    message.id = MESSAGE_NTF_CITY_FIGHT_TIME_SCORE;

    CityScoreRecorderMap::iterator cityIt = m_score_recorder.begin();
    for (; cityIt != m_score_recorder.end(); ++cityIt)
    {
        // ��ȡ������Ϣ
        const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(cityIt->second.cityID);
        if(!cityConfig)
        {
            CnAssert(false);
            return;
        }

        // ��ȡ������
        DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(cityConfig->dungeon_id);
        if(!dungeonSet)
        {
            CnAssert(false);
            return;
        }

        // ��ȡ����б�
        std::vector<Player*> playerMap = dungeonSet->GetPlayerMap();
        for(std::vector<Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
        {
            // ÿ���˸��Լ������5��
            RecordGuildScore(cityIt->second.cityID, (*playerIt)->GetGuildID(), 5);

            // ÿ���˷�����
            (*playerIt)->SendMessageNotify(message);
        }
    }
}

void CityFightManager::SendMessageNotifyToCity( uint32 cityId, MESSAGE_NTF& message )
{
    // ��ȡ������Ϣ
    const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(cityId);
    if(!cityConfig)
    {
        CnAssert(false);
        return;
    }

    // ��ȡ������
    DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(cityConfig->dungeon_id);
    if(!dungeonSet)
    {
        CnAssert(false);
        return;
    }

    // ��ȡ����б�
    std::vector<Player*> playerMap = dungeonSet->GetPlayerMap();
    for(std::vector<Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        (*playerIt)->SendMessageNotify(message);
    }
}

void CityFightManager::PlayerEnterCity( Player* player, uint32 cityId )
{
    // û��ʼ,ֱ�ӷ���
    if (!m_open)
        return;

    char cityIdString[100];
    Crown::SDItoa(cityIdString, 100, cityId);
    uint64 guildId = player->GetGuildID();
    std::string guildName = player->GetGuildName();

    CityScoreRecorderMap::iterator cityIt = m_score_recorder.find(cityId);
    if (cityIt != m_score_recorder.end())
    {
        std::map<uint64, GuildScoreRecorder>::const_iterator guildScoreIt = cityIt->second.scoreMap.find(guildId);
        if (guildScoreIt == cityIt->second.scoreMap.end())
        {
            // �����б���û�иù���,��������Ϊ�ù����һ���������,������
            MESSAGE_NTF message;
            message.id = MESSAGE_NTF_GUILD_ENTER_CITY_FIGHT;
            message.parame_list.push_back(guildName);
            message.parame_list.push_back(cityIdString);

            SendMessageNotifyToCity(cityId, message);
        }
    }

    // ����ټӷ�
    RecordGuildScore(cityId, guildId, 0);
}

void CityFightManager::PlayerLeaveCity( Player* player, uint32 cityId )
{
    // û��ʼ,ֱ�ӷ���
    if (!m_open)
        return;

    char cityIdString[100];
    Crown::SDItoa(cityIdString, 100, cityId);
    std::string guildName = player->GetGuildName();

    // ��ȡ������Ϣ
    const CityFightWarCityConfig* cityConfig = GuildConfig::Instance()->GetCityFightWarCity(cityId);
    if(!cityConfig)
    {
        CnAssert(false);
        return;
    }

    // ��ȡ������
    DungeonSet* dungeonSet = DungeonManager::Instance()->GetDungeonSet(cityConfig->dungeon_id);
    if(!dungeonSet)
    {
        CnAssert(false);
        return;
    }

    // ��ȡ����б�
    std::vector<Player*> playerMap = dungeonSet->GetPlayerMap();
    for(std::vector<Player*>::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        if ( ((*playerIt)->GetGuildName() == guildName) &&
            ((*playerIt)->GetPlayerID() != player->GetPlayerID()) )
        {
            // �����л��иù�����������
            return;
        }
    }

    // ������û�иù����������,��������Ϊ�ù������һ���뿪����,������
    MESSAGE_NTF message;
    message.id = MESSAGE_NTF_GUILD_LEAVE_CITY_FIGHT;
    message.parame_list.push_back(guildName);
    message.parame_list.push_back(cityIdString);

    SendMessageNotifyToCity(cityId, message);
}
