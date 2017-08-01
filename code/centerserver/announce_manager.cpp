#include "centerserver_pch.h"
#include "announce_manager.h"
#include "gate_handler.h"
#include "json_helper.h"


IMPLEMENT_SINGLETON(AnnouceManager)

AnnouceManager::AnnouceManager()
{
    m_id = 0;
}

AnnouceManager::~AnnouceManager()
{
}

void AnnouceManager::Update()
{
    uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
    for (std::map<int32, AnnouceStruct>::iterator iter = m_annouceList.begin(); iter != m_annouceList.end(); iter++)
    {
        if(!iter->second.isloop)
            continue;

        if(curtime > iter->second.starttime && curtime < iter->second.endtime)
        {
            if(curtime >= iter->second.nextlooptime)
            {
                // 发送循环公告
                CHAT_MSG sendMsg;
                sendMsg.channel = CHAT_CHANNEL_MARQUEE;
                sendMsg.chat = iter->second.chat;	 
                GateHandler::Instance()->SendToAll(MSG_CHAT_ACK, &sendMsg, BROADCAST_PLAYER_ID);
                CnDbg("loop annouce %d\n", iter->first);
                iter->second.nextlooptime = curtime + iter->second.looptime;
            }
        }
    }
}

void AnnouceManager::OnInsert(CHAT_MSG* recvMsg)
{
    if(!recvMsg) 
        return;

    if(!recvMsg->isloop)
        return;

    m_id += 1;

    AnnouceStruct curStrc;
    curStrc.clear();
    curStrc.chat = recvMsg->chat;
    curStrc.isloop = recvMsg->isloop;
    curStrc.looptime = recvMsg->looptime;
    curStrc.starttime = recvMsg->starttime;
    curStrc.endtime = recvMsg->endtime;

    CnDbg("insert loop annouce time=%d: %s\n", recvMsg->looptime, recvMsg->chat.c_str());

    uint32 nexttime = 0;
    if(recvMsg->isloop && recvMsg->looptime >0 )
    {
        uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
        if(curtime > recvMsg->starttime && curtime > recvMsg->endtime)
        {   //在时间内设置为当前时间
            curStrc.nextlooptime =  curtime + recvMsg->looptime;
        }
        else
        {
            //还没开始设置为开始时间
            curStrc.nextlooptime =  recvMsg->starttime;
        }
    }

    std::map<int32, AnnouceStruct>::iterator iter = m_annouceList.find(m_id);
    if(iter == m_annouceList.end())
    {
        m_annouceList[m_id] = curStrc;
    }
}

bool AnnouceManager::DeleteAnnouceId(int32 id)
{
    for (std::map<int32, AnnouceStruct>::iterator iter=m_annouceList.begin(); iter!=m_annouceList.end(); )
    {
        if (iter->first == id)
        {
            m_annouceList.erase(iter++);
            return true;
        }
        else
        {
            iter++;
        }
    }

    return false;
}

bool AnnouceManager::GetAnnouceList(Json::Value& ackjson)
{
    bool b_exe = false;
    Json::Value str_json;
    for (std::map<int32, AnnouceStruct>::iterator iter = m_annouceList.begin(); iter != m_annouceList.end(); iter++)
    {
        str_json.clear();
        str_json["chat"] = iter->second.chat;
        str_json["id"] = iter->first;
        str_json["isloop"] = iter->second.isloop;
        str_json["looptime"] = iter->second.looptime;
        str_json["starttime"] = iter->second.starttime;
        str_json["endtime"] = iter->second.endtime;
        ackjson["annoucelist"].append(str_json);

        b_exe = true;
    }

    return b_exe;
}



